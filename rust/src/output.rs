//! The ```output``` module provides default ```fmt::Display``` implementations
//! for the different ```Status``` structs in the crate as well as
//! the ```output_statuses``` function for convenience.

use std::fmt;

use crate::*;

static LIGHT_BLUE_COLOR_FORMAT: &'static str = "^c#68a7d4^";
static DEFAULT_COLOR_FORMAT: &'static str = "^d^";

/// This function will call ```dwmstatus::set_status_bar`` with the outputs of all
/// the ```Status``` implementors found in the given ```statuses``` iterator.
pub fn output_statuses<'a, T>(statuses: T)
where
    T: Iterator<Item = &'a Box<dyn Status>>,
{
    let status_outputs: Vec<String> = statuses.map(|status| status.to_string()).collect();

    let output = format!(
        "{left_padding}{status_output}{right_padding}",
        left_padding = " ".repeat(8),
        status_output = status_outputs.join("  "),
        right_padding = " ".repeat(5)
    );

    set_status_bar(&output);
}

impl fmt::Display for audio::Audio {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(
            f,
            "{0}{icon} {1}{value:.0}%",
            LIGHT_BLUE_COLOR_FORMAT,
            DEFAULT_COLOR_FORMAT,
            icon = if self.muted { '' } else { '' },
            value = self.current_volume as f64 / self.max_volume as f64 * 100.0,
        )
    }
}

impl fmt::Display for battery::Battery {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        let percent = self.current_watt_hours as f64 / self.capacity_watt_hours as f64 * 100.0;
        let icon = if self.charging {
            ''
        } else {
            match percent as u16 {
                90..=100 => '',
                60..=89 => '',
                30..=59 => '',
                10..=29 => '',
                _ => '',
            }
        };

        write!(
            f,
            "{0}{icon} {1}{value}%",
            LIGHT_BLUE_COLOR_FORMAT,
            DEFAULT_COLOR_FORMAT,
            icon = icon,
            value = percent as u16,
        )
    }
}

impl fmt::Display for cpu::Cpu {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(
            f,
            "{0}{icon} {1}{value:.1}%",
            LIGHT_BLUE_COLOR_FORMAT,
            DEFAULT_COLOR_FORMAT,
            icon = '',
            value = self.utilization,
        )
    }
}

impl fmt::Display for date::Date {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(
            f,
            "{0}{icon} {1}{value}",
            LIGHT_BLUE_COLOR_FORMAT,
            DEFAULT_COLOR_FORMAT,
            icon = '',
            value = self.timestamp.format("%A %x"),
        )
    }
}

impl fmt::Display for disk::Disk {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(
            f,
            "{0}{icon} {1}{value:.0}%",
            LIGHT_BLUE_COLOR_FORMAT,
            DEFAULT_COLOR_FORMAT,
            icon = '',
            value = (1.0 - self.free_bytes as f32 / self.total_bytes as f32) * 100.0,
        )
    }
}

impl fmt::Display for memory::Memory {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(
            f,
            "{0}{icon} {1}{value:.0}%",
            LIGHT_BLUE_COLOR_FORMAT,
            DEFAULT_COLOR_FORMAT,
            icon = '',
            value = (1.0 - self.available_kibibytes as f32 / self.total_kibibytes as f32) * 100.0,
        )
    }
}

impl fmt::Display for time::Time {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(
            f,
            "{0}{icon} {1}{value}",
            LIGHT_BLUE_COLOR_FORMAT,
            DEFAULT_COLOR_FORMAT,
            icon = '',
            value = self.timestamp.format("%I:%M %p"),
        )
    }
}

impl fmt::Display for wifi::Wifi {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        if self.active {
            write!(
                f,
                "{0}{icon} {1}{value:.1}%",
                LIGHT_BLUE_COLOR_FORMAT,
                DEFAULT_COLOR_FORMAT,
                icon = '',
                value = self.strength,
            )
        } else {
            write!(f, "")
        }
    }
}
