use std::{error::Error, fs};

use crate::Status;

pub struct Wifi {
    pub active: bool,
    pub strength: f32,
    interface_name: String,
}

impl Wifi {
    pub fn new(interface_name: &str) -> Result<Self, Box<dyn Error>> {
        Ok(Wifi {
            active: false,
            strength: 0.0,
            interface_name: String::from(interface_name),
        })
    }
}

impl Status for Wifi {
    fn update(&mut self) -> Result<(), Box<dyn Error>> {
        let contents = fs::read_to_string("/proc/net/wireless")?;

        for line in contents.lines().skip(2) {
            let mut values = line.split_whitespace();

            if values.next().unwrap().contains(&self.interface_name) {
                let link_quality: f32 = values.skip(1).next().unwrap().parse().unwrap();

                self.strength = link_quality * 100.0 / 70.0;
                self.active = true;

                return Ok(());
            }
        }

        self.strength = 0.0;
        self.active = false;

        Ok(())
    }
}
