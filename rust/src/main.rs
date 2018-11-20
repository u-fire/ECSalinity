extern crate ufire_ec;

use ufire_ec::*;

fn main() {
    let mut ec = EcProbe::new("/dev/i2c-3", 0x3c).unwrap();

    println!("Config:");
    println!("	calibration:");
    println!(
        "	EC: {}",
        ec.get_calibration_ec().unwrap()
    );
    println!(
        "	SW: {}",
        ec.get_calibration_sw().unwrap()
    );
    println!(
        "	temp. compensation: {}",
        ec.using_temperature_compensation().unwrap()
    );
    println!("	temp. constant: {}", ec.get_temp_constant().unwrap());
    println!("	version: {}.{}", ec.get_version().unwrap(),
            ec.get_firmware().unwrap()
    );

    loop {
        let ms = ec.measure_ec(true).unwrap();
        let temp = ec.measure_temp().unwrap();

        println!("mS: {}", ms);
        println!("C: {}", temp);
        println!("");
    }
}
