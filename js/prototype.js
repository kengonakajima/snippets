Prop2D.prototype.idgen=1;

function Prop2D(){
    this.prio = 1;
    this.id = this.idgen;
}

Prop2D.prototype.setIndex = function(ind) {
    this.index = ind;
}

var p = new Prop2D();
p.setIndex(19);


console.log(p);
    