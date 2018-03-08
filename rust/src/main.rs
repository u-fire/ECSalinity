extern crate ufire_ec;

use ufire_ec::*;

fn main() {
    let ec = EcProbe::new("/dev/i2c-3", 0x3f).unwrap();

    println!("Config:");
    // println!("	offset: {}", mv.get_calibrate_offset().unwrap());
    // println!("	dual point: {}", mv.using_dual_point().unwrap());
    // println!(
    //     "	low reference / read: {} / {}",
    //     mv.get_calibrate_low_reference().unwrap(),
    //     mv.get_calibrate_low_reading().unwrap()
    // );
    // println!(
    //     "	high reference / read: {} / {}",
    //     mv.get_calibrate_high_reference().unwrap(),
    //     mv.get_calibrate_high_reading().unwrap()
    // );
    // println!(
    //     "	temp. compensation: {}",
    //     mv.using_temperature_compensation().unwrap()
    // );
    // println!("	version: {:x}", mv.get_version().unwrap());
    //
    // loop {
    //     let v = mv.measure_mv().unwrap();
    // let temp = ec.measure_temp().unwrap();
    //
    //     println!("mV: {}", v);
    //     println!("C: {}", temp);
    //     println!("");
    // }
}
