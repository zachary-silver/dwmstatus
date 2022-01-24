//! The ```memory``` module provides a struct containing information related to
//! the system's RAM modules, such as the number of free and total kibibytes.

use std::{error::Error, fs};

use crate::Status;

pub struct Memory {
    pub free_kibibytes: u32,
    pub total_kibibytes: u32,
}

impl Memory {
    pub fn new() -> Self {
        Memory {
            free_kibibytes: 0,
            total_kibibytes: 0,
        }
    }
}

impl Status for Memory {
    /// # Errors
    ///
    /// This method will return an ```Error``` if ```/proc/meminfo``` can't
    /// be opened for reading.
    fn update(&mut self) -> Result<(), Box<dyn Error>> {
        let contents = fs::read_to_string("/proc/meminfo")?;
        let mut lines = contents.lines();

        self.total_kibibytes = lines
            .next()
            .unwrap()
            .split_whitespace()
            .skip(1)
            .next()
            .unwrap()
            .parse()
            .unwrap();
        self.free_kibibytes = lines
            .next()
            .unwrap()
            .split_whitespace()
            .skip(1)
            .next()
            .unwrap()
            .parse()
            .unwrap();

        Ok(())
    }
}
