//! The ```dwmstatus``` library is intended to be used in conjunction with
//! the dynamic tiling window manager ```dwm``` in order to set the output
//! in ```dwm```'s status bar.
//!
//! However, the different modules that provide structs that implement
//! the ```Status``` trait may also be easily used in any applications
//! that require retrieving system related information.

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

/// Used to easily reference the different structs that implement
/// the ```Status``` trait in code.
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

/// Should be implemented by any struct that aims to be part of
/// the ```dwmstatus``` output.
pub trait Status: fmt::Display + Send {
    /// Should ensure that all of the implementor's struct fields are
    /// populated with the most up to date values, and return an ```Error```
    /// otherwise.
    fn update(&mut self) -> Result<(), Box<dyn Error>>;
}

/// Helper function that runs the ```update``` method on each status returned
/// by the given ```statuses``` iterator.
pub fn update_statuses<'a, T>(statuses: T)
where
    T: Iterator<Item = &'a mut Box<dyn Status>>,
{
    statuses.for_each(|status| {
        if let Err(err) = status.update() {
            eprintln!("dwmstatus: {:?}", err);
        }
    });
}

/// Calls ```xsetroot``` with the given ```output``` value specified
/// as the ```-name``` parameter in order to set dwm's status output.
pub fn set_status_bar(output: &str) {
    let _ = Command::new("xsetroot").args(["-name", &output]).output();
}
