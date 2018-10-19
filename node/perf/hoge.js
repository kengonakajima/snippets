class Cell {
    constructor() {
        this.aa=1;
        this.bb=2;
        this.cc=3;
        this.dd=4;
        this.ee=5;
        this.fff=6;
        this.gggg=7;
        this.hh_hh=8;
        this.ii_ii_ii=9;
        this.jj=10;
        this.kk=11;
        this.ll=12;
        this.mm=13;
        this.nn=14;
        this.pp=15;
        this.zz=16;        
    }
};

var num=1024*1024;

class Chunk {
    constructor() {
        this.cells=new Array(num);
        for(var i=0;i<num;i++) {
            this.cells[i]=new Cell();
        }
    }
    get(ind) {
        return this.cells[ind];
    }
};

function initBinCell(tgt,ind) {
    var bi=ind*16;
    for(var i=0;i<16;i++)   tgt[bi+i]=i+1;
}
class BinChunk {
    constructor() {
        this.cell_data=new Uint8Array(16*num);
        for(var i=0;i<num;i++) {
            initBinCell(this.cell_data,i);
        }
    }
    get_aa(ind) {
        return this.cell_data[ind*16+0];
    }
    set_aa(ind,val) {
        return this.cell_data[ind*16+0]=val;        
    }
}

// mk
var t0=new Date().getTime();
var ck=new Chunk();
var t1=new Date().getTime();
var bck=new BinChunk();
var t2=new Date().getTime();

console.log("chk_mk:",t1-t0, "bck_mk:",t2-t1); // 368,28

// update
for(var i=0;i<num;i++) {
    var c=ck.get(i);
    c.aa=i%0xff;
}
var t3=new Date().getTime();
for(var i=0;i<num;i++) {
    bck.set_aa(i,i%0xff);
}
var t4=new Date().getTime();
console.log("ck_upd:",t3-t2, "bck_upd:",t4-t3); // 18, 5