

var sys = require("sys");

function ExampleClass() {
}

ExampleClass.prototype.print = function(message) {
  sys.print("ExampleClass.prototype.print: " + message)
}

ExampleClass.prototype.__noSuchMethod__ = function() {
  sys.print("No such method, maybe you should try reading the docs?");
}

example = new ExampleClass();
example.eatPizza();