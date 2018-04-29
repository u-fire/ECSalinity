var C = {
  EC_SALINITY: 0x3c,
  EC_MEASURE_EC: 80,
  EC_MEASURE_TEMP: 40,
  EC_CALIBRATE_PROBE: 20,
  EC_CALIBRATE_LOW: 10,
  EC_CALIBRATE_HIGH: 8,
  EC_CALCULATE_K: 2,
  EC_I2C: 1,
  EC_DRY: 82,
  EC_VERSION_REGISTER: 0,
  EC_MS_REGISTER: 1,
  EC_TEMP_REGISTER: 5,
  EC_K_REGISTER: 9,
  EC_SOLUTION_REGISTER: 13,
  EC_TEMPCOEF_REGISTER: 17,
  EC_CALIBRATE_REFHIGH_REGISTER: 21,
  EC_CALIBRATE_REFLOW_REGISTER: 25,
  EC_CALIBRATE_READHIGH_REGISTER: 29,
  EC_CALIBRATE_READLOW_REGISTER: 33,
  EC_CALIBRATE_OFFSET_REGISTER: 37,
  EC_SALINITY_PSU: 41,
  EC_DRY_REGISTER: 45,
  EC_TEMP_COMPENSATION_REGISTER: 49,
  EC_CONFIG_REGISTER: 50,
  EC_TASK_REGISTER: 51,
  EC_EC_MEASUREMENT_TIME: 250,
  EC_TEMP_MEASURE_TIME: 750,
  EC_DUALPOINT_CONFIG_BIT: 0,
  EC_TEMP_COMPENSATION_CONFIG_BIT: 1,
  PSU_TO_PPT_CONVERSION: 1.004715
};

function ECSalinity(_i2c, _address) {
  this.i2c = _i2c;
  this.address = _address;
}

ECSalinity.prototype._change_register = function(reg) {
  this.i2c.writeTo(this.address, reg);
  //for (f = 0; f < 10000; f++) {}
}

ECSalinity.prototype._send_command = function(cmd) {
  this.i2c.writeTo(this.address, [C.EC_TASK_REGISTER, cmd]);
  //for (f = 0; f < 10000; f++) {}
}

ECSalinity.prototype._read_byte = function(reg) {
  this._change_register(reg);
  return this.i2c.readFrom(this.address, 1)[0];
}

ECSalinity.prototype._write_byte = function(reg, val) {
  this.i2c.writeTo(this.address, [reg, val]);
  //for (f = 0; f < 10000; f++) {}
}

ECSalinity.prototype._read_register = function(reg) {
  var data = new Uint8Array(4);

  this._change_register(reg);
  data[0] = this.i2c.readFrom(this.address, 1)[0];
  data[1] = this.i2c.readFrom(this.address, 1)[0];
  data[2] = this.i2c.readFrom(this.address, 1)[0];
  data[3] = this.i2c.readFrom(this.address, 1)[0];

  return new Float32Array(data.buffer)[0];
}

ECSalinity.prototype._write_register = function(reg, val) {
  var data = new Float32Array(1);
  data[0] = val;

  this.i2c.writeTo(this.address, [reg, data.buffer[0], data.buffer[1], data.buffer[2], data.buffer[3]]);
  for (f = 0; f < 3000; f++) {}
}

exports.connect = function(_i2c, _address) {
  return new ECSalinity(_i2c, _address);
}

ECSalinity.prototype.measureTemp = function(callback) {
  var tempC;
  var tempF;
  var self = this;

  this._send_command(C.EC_MEASURE_TEMP);
  var i = setInterval(function() {
    tempC = self._read_register(C.EC_TEMP_REGISTER);
    tempF = ((tempC * 9) / 5) + 32;
    callback({
      tempC: tempC,
      tempF: tempF
    });
    clearInterval(i);
  }, C.EC_TEMP_MEASURE_TIME);
};

ECSalinity.prototype.measureEC = function(tempCoefficient, callback) {
  var self = this;

  this._write_register(C.EC_TEMPCOEF_REGISTER, tempCoefficient);
  this._send_command(C.EC_MEASURE_EC);

  var i = setInterval(function() {
    var mS = self._read_register(C.EC_MS_REGISTER);
    var PSU = self._read_register(C.EC_SALINITY_PSU);
    callback({
      mS: mS,
      uS: mS * 1000,
      S: mS / 1000,
      PPM_500: mS * 500,
      PPM_640: mS * 640,
      PPM_700: mS * 700,
      salinityPSU: self._read_register(C.EC_SALINITY_PSU),
      salinityPPT: PSU * C.PSU_TO_PPT_CONVERSION,
      salinityPPM: self.salinityPPT * 1000 * C.PSU_TO_PPT_CONVERSION
    });
    clearInterval(i);
  }, (C.EC_EC_MEASUREMENT_TIME));
}

ECSalinity.prototype.measureSalinity = function(tempCoefficient, callback) {

}

ECSalinity.prototype.calculateK = function(solutionEC, tempCoefficient, callback) {
  var self = this;
  var dualpoint = this.usingDualPoint();

  this.useDualPoint(false);
  this._write_register(C.EC_TEMPCOEF_REGISTER, tempCoefficient);
  this._write_register(C.EC_SOLUTION_REGISTER, solutionEC);
  this._send_command(C.EC_CALCULATE_K);

  var i = setInterval(function() {
    var K = self._read_register(C.EC_K_REGISTER);
    callback({
      K: K
    });
    clearInterval(i);
  }, (C.EC_EC_MEASUREMENT_TIME));

  this.useDualPoint(dualpoint)
}

ECSalinity.prototype.calibrateProbeLow = function(solutionEC, tempCoef, callback) {
  var self = this;
  var dualpoint = this.usingDualPoint();

  this.useDualPoint(false);
  this._write_register(C.EC_TEMPCOEF_REGISTER, tempCoef);
  this._write_register(C.EC_SOLUTION_REGISTER, solutionEC);
  this._send_command(C.EC_CALIBRATE_LOW);

  var i = setInterval(function() {
    var read = self._read_register(C.EC_CALIBRATE_READLOW_REGISTER);
    var reference = self._read_register(C.EC_CALIBRATE_REFLOW_REGISTER);
    callback({
      read: read,
      reference: reference
    });
    clearInterval(i);
  }, (C.EC_EC_MEASUREMENT_TIME));

  this.useDualPoint(dualpoint)
}

ECSalinity.prototype.calibrateProbeHigh = function(solutionEC, tempCoef, callback) {
  var self = this;
  var dualpoint = this.usingDualPoint();

  this.useDualPoint(false);
  this._write_register(C.EC_TEMPCOEF_REGISTER, tempCoef);
  this._write_register(C.EC_SOLUTION_REGISTER, solutionEC);
  this._send_command(C.EC_CALIBRATE_HIGH);

  var i = setInterval(function() {
    var read = self._read_register(C.EC_CALIBRATE_READHIGH_REGISTER);
    var reference = self._read_register(C.EC_CALIBRATE_REFHIGH_REGISTER);
    callback({
      read: read,
      reference: reference
    });
    clearInterval(i);
  }, (C.EC_EC_MEASUREMENT_TIME));

  this.useDualPoint(dualpoint)
}

ECSalinity.prototype.setDualPointCalibration = function(refLow, refHigh, readLow, readHigh) {
  this._write_register(C.EC_CALIBRATE_REFLOW_REGISTER, refLow);
  this._write_register(C.EC_CALIBRATE_REFHIGH_REGISTER, refHigh);
  this._write_register(C.EC_CALIBRATE_READLOW_REGISTER, readLow);
  this._write_register(C.EC_CALIBRATE_READHIGH_REGISTER, readHigh);
};

ECSalinity.prototype.calibrateDry = function(callback) {
  var self = this;

  this._send_command(C.EC_DRY);

  var i = setInterval(function() {
    var dry = self._read_register(C.EC_DRY_REGISTER);
    callback({
      dry: dry
    });
    clearInterval(i);
  }, (C.EC_EC_MEASUREMENT_TIME));

  this.useDualPoint(dualpoint)
}

ECSalinity.prototype.getVersion = function() {
  return this._read_byte(C.EC_VERSION_REGISTER);
};

ECSalinity.prototype.setK = function(k) {
  this._write_register(C.EC_K_REGISTER, k);
};

ECSalinity.prototype.getK = function() {
  return (Math.round(this._read_register(C.EC_K_REGISTER) * 100.0) / 100.0);
};

ECSalinity.prototype.getCalibrateDry = function() {
  return (Math.round(this._read_register(C.EC_DRY_REGISTER) * 100.0) / 100.0);
};

ECSalinity.prototype.getCalibrateOffset = function() {
  return Math.round(this._read_register(C.EC_CALIBRATE_OFFSET_REGISTER) * 100.0) / 100.0;;
};

ECSalinity.prototype.getCalibrateHigh = function() {
  return Math.round(this._read_register(C.EC_CALIBRATE_REFHIGH_REGISTER) * 100.0) / 100.0;;
};

ECSalinity.prototype.getCalibrateLow = function() {
  return Math.round(this._read_register(C.EC_CALIBRATE_REFLOW_REGISTER) * 100.0) / 100.0;;
};

ECSalinity.prototype.getCalibrateHighReading = function() {
  return Math.round(this._read_register(C.EC_CALIBRATE_READHIGH_REGISTER) * 100.0) / 100.0;;
};

ECSalinity.prototype.getCalibrateLowReading = function() {
  return Math.round(this._read_register(C.EC_CALIBRATE_READLOW_REGISTER) * 100.0) / 100.0;;
};

ECSalinity.prototype.reset = function() {
  this._write_register(C.EC_K_REGISTER, NaN);
  this._write_register(C.EC_CALIBRATE_OFFSET_REGISTER, NaN);
  this._write_register(C.EC_CALIBRATE_REFHIGH_REGISTER, NaN);
  this._write_register(C.EC_CALIBRATE_REFLOW_REGISTER, NaN);
  this._write_register(C.EC_CALIBRATE_READHIGH_REGISTER, NaN);
  this._write_register(C.EC_CALIBRATE_READLOW_REGISTER, NaN);
  this._write_register(C.EC_DRY_REGISTER, NaN);
  this.setTempConstant(0);
  this.useDualPoint(false);
  this.useTemperatureCompensation(false);
};

ECSalinity.prototype.setTempConstant = function(temp) {
  this._write_byte(C.EC_TEMP_COMPENSATION_REGISTER, temp);
};

ECSalinity.prototype.setI2CAddress = function(i2cAddress) {
  this._write_register(C.EC_SOLUTION_REGISTER, i2cAddress);
  this._send_command(C.EC_I2C);
  this.address = i2cAddress;
};

ECSalinity.prototype.getTempConstant = function() {
  return this._read_byte(C.EC_TEMP_COMPENSATION_REGISTER);
};

ECSalinity.prototype.useTemperatureCompensation = function(b) {
  var retval = this._read_byte(C.EC_CONFIG_REGISTER);

  retval = this._bit_set(retval, C.EC_TEMP_COMPENSATION_CONFIG_BIT, b);
  this._write_byte(C.EC_CONFIG_REGISTER, retval);
};

ECSalinity.prototype.useDualPoint = function(b) {
  var retval = this._read_byte(C.EC_CONFIG_REGISTER);

  retval = this._bit_set(retval, C.EC_DUALPOINT_CONFIG_BIT, b);
  this._write_byte(C.EC_CONFIG_REGISTER, retval);
};

ECSalinity.prototype.usingTemperatureCompensation = function() {
  var retval = this._read_byte(C.EC_CONFIG_REGISTER);
  return (retval >> 1) & 0x01;
};

ECSalinity.prototype.usingDualPoint = function() {
  var retval = this._read_byte(C.EC_CONFIG_REGISTER);
  return (retval >> 0) & 0x01;
};

ECSalinity.prototype._bit_set = function(v, index, x) {
  var mask = 1 << index;
  v &= ~mask;
  if (x) {
    v |= mask;
  }
  return v;
};