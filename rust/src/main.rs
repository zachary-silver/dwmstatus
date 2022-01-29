use std::{
    collections::HashMap,
    io::Error,
    sync::{Arc, Mutex},
    sync::mpsc::{Receiver},
    sync::mpsc,
    thread,
};

use signal_hook::consts::signal::*;

use dwmstatus::*;

fn main() -> Result<(), Error> {
    let mut signals = signals::get_signals()?;
    let index_map = get_status_to_index_map();
    let statuses = Arc::new(Mutex::new(get_statuses(&index_map)));

    let cloned_statuses = statuses.clone();
    let (tx, rx) = mpsc::channel();
    let app = thread::spawn(move || run_app(cloned_statuses, rx));

    let audio_status_index = *index_map.get(&StatusType::Audio).unwrap();

    for info in &mut signals {
        match info.signal {
            SIGUSR1 => {
                let mut statuses = statuses.lock().unwrap();

                if let Err(err) = statuses[audio_status_index].update() {
                    eprintln!("{}", err);
                }
                output::output_statuses(statuses.iter());
            },
            _ => break,
        }
    }

    tx.send(0).unwrap();
    app.join().unwrap();

    Ok(())
}

fn run_app(statuses: Arc<Mutex<Vec<Box<dyn Status>>>>, rx: Receiver<u8>) {
    let sleep_time = std::time::Duration::from_millis(1000);

    loop {
        let mut statuses = statuses.lock().unwrap();

        update_statuses(statuses.iter_mut());
        output::output_statuses(statuses.iter());

        drop(statuses);

        thread::sleep(sleep_time);

        match rx.try_recv() {
            Ok(_) => break,
            Err(_) => continue,
        }
    }
}

fn get_status_to_index_map() -> HashMap<StatusType, usize> {
    HashMap::from([
        (StatusType::Wifi, 0),
        (StatusType::Cpu, 1),
        (StatusType::Memory, 2),
        (StatusType::Disk, 3),
        (StatusType::Audio, 4),
        (StatusType::Battery, 5),
        (StatusType::Date, 6),
        (StatusType::Time, 7),
    ])
}

fn get_statuses(index_map: &HashMap<StatusType, usize>) -> Vec<Box<dyn Status>> {
    let mut index_map: Vec<(&StatusType, &usize)> = index_map.iter().collect();

    index_map.sort_by_key(|(_, index)| *index);
    index_map
        .iter()
        .map(|(status_type, _)| -> Box<dyn Status> {
            match status_type {
                StatusType::Audio => Box::new(
                    audio::Audio::new("default", "Master").expect("Failed to create Audio status"),
                ),
                StatusType::Battery => {
                    Box::new(battery::Battery::new().expect("Failed to create Battery status"))
                }
                StatusType::Cpu => Box::new(cpu::Cpu::new()),
                StatusType::Date => Box::new(date::Date::new()),
                StatusType::Disk => {
                    Box::new(disk::Disk::new("/").expect("Failed to create Disk status"))
                }
                StatusType::Memory => Box::new(memory::Memory::new()),
                StatusType::Time => Box::new(time::Time::new()),
                StatusType::Wifi => {
                    Box::new(wifi::Wifi::new("wlp4s0").expect("Failed to create Wifi status"))
                }
            }
        })
        .collect()
}
