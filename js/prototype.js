Prop2D.prototype.idgen=1;

function Prop2D(){
    this.prio = 1;
    this.id = this.idgen;
}

Prop2D.prototype.setIndex = function(ind) {
    this.index = ind;
}
Prop2D.prototype.zzz = function(x) {
    //    setIndex(x); error
    this.setIndex(x);
}
var p = new Prop2D();
p.setIndex(19);
p.zzz(1000);

console.log(p);
    