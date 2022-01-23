use std::{error::Error, ffi::CString, fs, mem::MaybeUninit};

use libc::statvfs;

use crate::Status;

struct Statvfs(statvfs);

impl Statvfs {
    pub fn new(path: &CString) -> Self {
        unsafe {
            let mut stat: Statvfs = MaybeUninit::uninit().assume_init();

            statvfs(path.as_ptr(), &mut stat.0);

            stat
        }
    }

    pub fn block_size(&self) -> u64 {
        self.0.f_bsize
    }

    pub fn blocks_available(&self) -> u64 {
        self.0.f_blocks
    }

    pub fn blocks_free(&self) -> u64 {
        self.0.f_bfree
    }
}

pub struct Disk {
    pub free_bytes: u64,
    pub total_bytes: u64,
    path: CString,
}

impl Disk {
    pub fn new(path: &str) -> Result<Self, Box<dyn Error>> {
        validate(path)?;
        let path = CString::new(path)?;

        Ok(Disk {
            free_bytes: 0,
            total_bytes: 0,
            path,
        })
    }
}

impl Status for Disk {
    fn update(&mut self) -> Result<(), Box<dyn Error>> {
        let stat = Statvfs::new(&self.path);

        self.free_bytes = stat.blocks_free() * stat.block_size();
        self.total_bytes = stat.blocks_available() * stat.block_size();

        Ok(())
    }
}

fn validate(path: &str) -> Result<(), Box<dyn Error>> {
    fs::metadata(path)?;

    Ok(())
}
