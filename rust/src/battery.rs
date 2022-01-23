use std::{error::Error, fs, path::Path};

use crate::Status;

pub struct Battery {
    pub capacity_watt_hours: u64,
    pub current_watt_hours: u64,
    pub charging: bool,
    pub percent: f32,
    capacity_battery_files: Vec<String>,
    current_battery_files: Vec<String>,
    status_battery_files: Vec<String>,
}

impl Battery {
    pub fn new() -> Result<Self, Box<dyn Error>> {
        let mut battery = Battery {
            capacity_watt_hours: 0,
            current_watt_hours: 0,
            charging: false,
            percent: 0.0,
            capacity_battery_files: Vec::new(),
            current_battery_files: Vec::new(),
            status_battery_files: Vec::new(),
        };

        battery.set_battery_files()?;

        if battery.capacity_battery_files.len() == 0 {
            return Err("No battery files found!".into());
        }

        Ok(battery)
    }

    fn set_battery_files(&mut self) -> Result<(), Box<dyn Error>> {
        let dir = Path::new("/sys/class/power_supply");

        for entry in fs::read_dir(dir)? {
            let path = entry?.path();
            let path_name = path.to_str().unwrap();

            if path_name.contains("BAT") {
                self.current_battery_files
                    .push(format!("{}/energy_now", path_name));
                self.capacity_battery_files
                    .push(format!("{}/energy_full", path_name));
                self.status_battery_files
                    .push(format!("{}/status", path_name));
            }
        }

        Ok(())
    }
}

impl Status for Battery {
    fn update(&mut self) -> Result<(), Box<dyn Error>> {
        self.current_watt_hours = 0;
        self.capacity_watt_hours = 0;
        self.charging = false;

        for file_name in &self.current_battery_files {
            self.current_watt_hours += get_watt_hours(file_name)?;
        }
        for file_name in &self.capacity_battery_files {
            self.capacity_watt_hours += get_watt_hours(file_name)?;
        }

        for file_name in &self.status_battery_files {
            if battery_charging(file_name)? {
                self.charging = true;
                break;
            }
        }

        self.percent =
            ((self.current_watt_hours as f64) / (self.capacity_watt_hours as f64) * 100.0) as f32;

        Ok(())
    }
}

fn get_watt_hours(file_name: &str) -> Result<u64, Box<dyn Error>> {
    let contents = fs::read_to_string(file_name)?;
    let watt_hours: u64 = contents.trim().parse()?;

    Ok(watt_hours)
}

fn battery_charging(file_name: &str) -> Result<bool, Box<dyn Error>> {
    let contents = fs::read_to_string(file_name)?;

    Ok(contents.lines().next().unwrap().eq("Charging"))
}
