//! The ```signals``` module provides a helper function for acquiring signals
//! dwmstatus aims to listen for.

use std::io::Error;
use std::sync::{atomic::AtomicBool, Arc};

use signal_hook::consts::signal::*;
use signal_hook::consts::TERM_SIGNALS;
use signal_hook::flag;
use signal_hook::iterator::exfiltrator::origin::WithOrigin;
use signal_hook::iterator::SignalsInfo;

/// Returns the signals that be iterated through in order to
/// persistently listen for them and act in response.
pub fn get_signals() -> Result<SignalsInfo<WithOrigin>, Error> {
    let term_now = Arc::new(AtomicBool::new(false));
    let _ = signal_hook::flag::register(libc::SIGINT, Arc::clone(&term_now));

    for sig in TERM_SIGNALS {
        flag::register_conditional_shutdown(*sig, 1, Arc::clone(&term_now))?;
        flag::register(*sig, Arc::clone(&term_now))?;
    }

    let mut sigs = vec![SIGUSR1];
    sigs.extend(TERM_SIGNALS);
    let signals = SignalsInfo::<WithOrigin>::new(&sigs)?;

    Ok(signals)
}
