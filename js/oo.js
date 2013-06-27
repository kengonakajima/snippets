
function Animal() {
    this.age = 1;
}
Animal.prototype.breathe = function() {
    console.log( "breathed" );
}

function Dog() {
    this.numleg = 4;
}
Dog.prototype.__proto__ = Animal.prototype;

Dog.prototype.bark = function() {
    console.log( "barked" );
}

var d = new Dog();

d.breathe();
d.bark();
