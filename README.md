# dwmstatus
Prints the status information on the top right of the status bar.

![Screenshot](https://github.com/ZmanSilver/scripts/blob/master/screen.png)
![gif](dwmstatus.gif)

## Installation and Execution Instructions
dwmstatus requires dwm window manager from suckless (https://dwm.suckless.org/) running on Linux to work.

From the cloned dwmstatus directory...

### C
Run the `make` command to compile, then `./dwmstatus` to run the program.

### Rust
Run the `cargo build --release` command to compile, then `./target/release/dwmstatus` to run the program.

Enter the `dwmstatus &` command to run the program in the background.
I suggest adding this command to your .xprofile to launch at startup.
