const im = require("imagickal");
im.dimensions("eye.jpeg").then(function(dim) {
    console.log("dim:",dim);
});


im.commands()
.resize({ width: 40 })
.quality(90)
.strip()
.exec('eye.jpeg', 'result.jpg').then(function () {
  console.log('done');
});
