use std::{error::Error, fmt, process::Command};

pub mod audio;
pub mod battery;
pub mod cpu;
pub mod date;
pub mod disk;
pub mod memory;
pub mod time;
pub mod wifi;

pub mod signals;

pub mod output;

#[derive(PartialEq, Eq, Hash)]
pub enum StatusType {
    Audio,
    Battery,
    Cpu,
    Date,
    Disk,
    Memory,
    Time,
    Wifi,
}

#[derive(Debug)]
struct StatusUpdateNotImplemented;

impl Error for StatusUpdateNotImplemented {}

impl fmt::Display for StatusUpdateNotImplemented {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(
            f,
            "This type has not properly implemented the \
            update() method for the Status trait!"
        )
    }
}

pub trait Status: fmt::Display + Send {
    fn update(&mut self) -> Result<(), Box<dyn Error>> {
        Err(StatusUpdateNotImplemented.into())
    }
}

pub fn update_statuses<'a, T>(statuses: T)
where
    T: Iterator<Item = &'a mut Box<dyn Status>>,
{
    for status in statuses {
        if let Err(err) = status.update() {
            eprintln!("dwmstatus: {:?}", err);
        }
    }
}

pub fn set_status_bar(output: &str) {
    let _ = Command::new("xsetroot").args(["-name", &output]).output();
}
