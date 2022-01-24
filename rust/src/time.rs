//! The ```time``` module provides a struct containing information related to the
//! current time, such as the local timestamp.

use std::error::Error;

use chrono::{DateTime, Local};

use crate::Status;

pub struct Time {
    pub timestamp: DateTime<Local>,
}

impl Time {
    pub fn new() -> Self {
        Time {
            timestamp: Local::now(),
        }
    }
}

impl Status for Time {
    fn update(&mut self) -> Result<(), Box<dyn Error>> {
        self.timestamp = Local::now();
        Ok(())
    }
}
