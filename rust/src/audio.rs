use std::{error::Error, ffi::CString, mem::MaybeUninit};

use alsa_sys::*;

use crate::Status;

pub struct Audio {
    pub current_volume: i64,
    pub max_volume: i64,
    pub min_volume: i64,
    pub muted: bool,
    pub percent_volume: f32,
    card_name: CString,
    channel_name: CString,
    handle_ptr: *mut snd_mixer_t,
    selem_id_ptr: *mut snd_mixer_selem_id_t,
}

impl Audio {
    pub fn new(card_name: &str, channel_name: &str) -> Result<Self, Box<dyn Error>> {
        let mut audio = Audio {
            current_volume: 0,
            max_volume: 0,
            min_volume: 0,
            muted: false,
            percent_volume: 0.0,
            card_name: CString::new(card_name)?,
            channel_name: CString::new(channel_name)?,
            handle_ptr: unsafe { MaybeUninit::uninit().assume_init() },
            selem_id_ptr: unsafe { MaybeUninit::uninit().assume_init() },
        };

        audio.setup_selem_id()?;
        audio.validate()?;

        Ok(audio)
    }

    fn validate(&mut self) -> Result<(), Box<dyn Error>> {
        self.update()?;

        Ok(())
    }

    fn setup_selem_id(&mut self) -> Result<(), Box<dyn Error>> {
        unsafe {
            if snd_mixer_selem_id_malloc(&mut self.selem_id_ptr) != 0 {
                return Err("Call to snd_mixer_selem_id_malloc() failed!".into());
            }

            snd_mixer_selem_id_set_index(self.selem_id_ptr, 0);
            snd_mixer_selem_id_set_name(self.selem_id_ptr, self.channel_name.as_ptr());
        }

        Ok(())
    }

    fn tear_down_selem_id(&mut self) {
        unsafe {
            snd_mixer_selem_id_free(self.selem_id_ptr);
        }
    }

    fn setup_handle(&mut self) -> Result<(), Box<dyn Error>> {
        unsafe {
            if snd_mixer_open(&mut self.handle_ptr, 0) != 0 {
                return Err("Call to snd_mixer_open() failed!".into());
            }

            if snd_mixer_attach(self.handle_ptr, self.card_name.as_ptr()) != 0 {
                return Err(format!(
                    "Call to snd_mixer_attach() failed! \
                    Card name '{}' may be invalid.",
                    self.card_name.to_str()?
                )
                .into());
            }

            let regopt_ptr: *mut snd_mixer_selem_regopt = MaybeUninit::zeroed().assume_init();
            let mut mixer_type_ptr: *mut snd_mixer_class_t = MaybeUninit::zeroed().assume_init();

            if snd_mixer_selem_register(self.handle_ptr, regopt_ptr, &mut mixer_type_ptr) != 0 {
                return Err("Call to snd_mixer_selem_register() failed!".into());
            }
        }

        Ok(())
    }

    fn tear_down_handle(&mut self) {
        unsafe {
            snd_mixer_close(self.handle_ptr);
        }
    }
}

impl Drop for Audio {
    fn drop(&mut self) {
        self.tear_down_selem_id();
    }
}

impl Status for Audio {
    fn update(&mut self) -> Result<(), Box<dyn Error>> {
        unsafe {
            self.setup_handle()?;

            snd_mixer_load(self.handle_ptr);

            let selem_ptr = snd_mixer_find_selem(self.handle_ptr, self.selem_id_ptr);
            if selem_ptr == std::ptr::null_mut() {
                return Err(format!(
                    "Call to snd_mixer_find_selem() failed! \
                    Channel name '{}' may be invalid.",
                    self.channel_name.to_str()?
                )
                .into());
            }

            snd_mixer_selem_get_playback_volume_range(
                selem_ptr,
                &mut self.min_volume,
                &mut self.max_volume,
            );
            snd_mixer_selem_get_playback_volume(selem_ptr, 0, &mut self.current_volume);
            let mut muted: i32 = 0;
            snd_mixer_selem_get_playback_switch(selem_ptr, SND_MIXER_SCHN_MONO, &mut muted);

            self.muted = muted == 0;
            self.percent_volume =
                (self.current_volume as f64 / self.max_volume as f64) as f32 * 100.0;

            self.tear_down_handle();
        }

        Ok(())
    }
}

unsafe impl Send for Audio {}
