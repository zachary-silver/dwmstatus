//! The ```date``` module provides a struct containing information related to the
//! current date, such as the local timestamp.

use std::error::Error;

use chrono::{DateTime, Local};

use crate::Status;

pub struct Date {
    pub timestamp: DateTime<Local>,
}

impl Date {
    pub fn new() -> Self {
        Date {
            timestamp: Local::now(),
        }
    }
}

impl Status for Date {
    fn update(&mut self) -> Result<(), Box<dyn Error>> {
        self.timestamp = Local::now();
        Ok(())
    }
}
