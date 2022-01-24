//! The ```cpu``` module provides a struct containing information related to the
//! system's cpu, such as total utilization.

use std::{error::Error, fs, ops::Sub};

use crate::Status;

pub struct Cpu {
    pub utilization: f32,
    idle_jiffies: u32,
    load_jiffies: u32,
}

impl Cpu {
    pub fn new() -> Self {
        Cpu {
            utilization: 0.0,
            idle_jiffies: 0,
            load_jiffies: 0,
        }
    }
}

impl Status for Cpu {
    /// The correctness of this update function depends on previous values of
    /// the calling ```Cpu``` struct's properties read from ```/proc/stat```.
    ///
    /// As a result, the first call to update for a ```Cpu``` struct will always
    /// yield incorrect results.
    ///
    /// # Errors
    ///
    /// This method returns an ```Error``` if ```/proc/stat``` cannot be opened
    /// for reading.
    fn update(&mut self) -> Result<(), Box<dyn Error>> {
        let contents = fs::read_to_string("/proc/stat")?;
        let line = contents.lines().next().unwrap();

        let values: Vec<u32> = line
            .split_whitespace()
            .skip(1)
            .map(|value| value.parse().unwrap())
            .collect();

        let idle_jiffies = values[3]; // 4th numeric column contains idle jiffies
        let load_jiffies = values.iter().sum();

        let idle_delta = get_delta(self.idle_jiffies, idle_jiffies);
        let load_delta = get_delta(self.load_jiffies, load_jiffies);

        if load_delta != 0 {
            self.utilization = 100.0 * (load_delta - idle_delta) as f32 / load_delta as f32;

            self.idle_jiffies = idle_jiffies;
            self.load_jiffies = load_jiffies;
        }

        Ok(())
    }
}

fn get_delta<T: PartialOrd + Sub<Output = T>>(a: T, b: T) -> T {
    if a < b {
        b - a
    } else {
        a - b
    }
}
