function Hoge(v) {
  console.log("this:", this);
  this.a = v;
  this.b = function() {
    console.log("x: a:", this.a );
  }
  return this;
}

var h = Hoge(10);


h.b();
