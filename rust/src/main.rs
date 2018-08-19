extern crate ufire_ec;

use ufire_ec::*;

fn main() {
    let mut ec = EcProbe::new("/dev/i2c-3", 0x3c).unwrap();

    println!("Config:");
    println!("	K: {}", ec.get_k().unwrap());
    println!("	offset: {}", ec.get_calibrate_offset().unwrap());
    println!("	dual point: {}", ec.using_dual_point().unwrap());
    println!(
        "	low reference / read: {} / {}",
        ec.get_calibrate_low_reference().unwrap(),
        ec.get_calibrate_low_reading().unwrap()
    );
    println!(
        "	high reference / read: {} / {}",
        ec.get_calibrate_high_reference().unwrap(),
        ec.get_calibrate_high_reading().unwrap()
    );
    println!(
        "	temp. compensation: {}",
        ec.using_temperature_compensation().unwrap()
    );
    println!("	temp. constant: {}", ec.get_temp_constant().unwrap());
    println!("	version: {:x}", ec.get_version().unwrap());

    loop {
        let ms = ec.measure_ec(0.019).unwrap();
        let temp = ec.measure_temp().unwrap();

        println!("mS: {}", ms);
        println!("C: {}", temp);
        println!("");
    }
}
