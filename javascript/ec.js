var i2c = new I2C();
i2c.setup({
  scl: D17,
  sda: D16,
  bitrate: 100000
});
var ec = require("http://localhost:8000/ECSalinity.js").connect(i2c, 0x3c);
ec.getVersion().toString(16);


function temp() {
  ec.measureTemp(function(d) {
    console.log("tempC: " + d.tempC);
  });
}