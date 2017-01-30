function N() {
    this.id=1;
}
function G() {
    this.id=2;
}
var n = new N();
var g = new G();

console.log( n.__proto__ == N.prototype );
console.log( g.__proto__ == N.prototype );