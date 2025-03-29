use self::env_setup::HardwareBridge;

mod env_setup;


fn main() {
    let hw = HardwareBridge::new();
    
    hw.check_hardware_status("ready");

    //INFO: add here your test using hw.check_hardware_status
}
