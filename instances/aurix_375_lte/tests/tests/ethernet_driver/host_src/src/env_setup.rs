use serialport::SerialPort;
use clap::Parser;
use std::time::Duration;

#[derive(Debug, Parser)]
#[command(version, about, long_about = None)]
struct Cli {
    port: String,
    baud: u32,
}

#[derive(Debug)]
pub struct HardwareBridge {
    uart: Box<dyn SerialPort>,
}

impl HardwareBridge {
    pub fn new() -> Self{
        let cli= Cli::parse();
        let port = serialport::new(cli.port.clone(), cli.baud)
            .timeout(Duration::from_millis(10))
            .open().unwrap();
        Self{
            uart: port
        }
    }
    
    pub fn check_hardware_status(mut self, expected: &str) -> bool
    {
        let mut serial_buf: Vec<u8> = vec![0; 1000];
        if self.uart.read(serial_buf.as_mut_slice()).is_ok() {
            expected == String::from_utf8(serial_buf).expect("Our bytes should be valid utf8")
        }else{
            false
        }
    }
}


