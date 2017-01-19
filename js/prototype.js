function Prop2D(){
    this.prio = 1;
}

Prop2D.prototype.setIndex = function(ind) {
    this.index = ind;
}

var p = new Prop2D();
p.setIndex(19);


console.log(p);
    