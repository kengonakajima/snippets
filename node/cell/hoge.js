

var fsz=256;
var cell_num=fsz*fsz*fsz;
var b_per_cell=16;
g_ary=new Uint8Array(cell_num*b_per_cell);

function get_bt(x,y,z) {
    var ind=(x+z*fsz+y*fsz*fsz)*b_per_cell;
    return g_ary[ind+0];    
}
function get_data(x,y,z) {
    var ind=(x+z*fsz+y*fsz*fsz)*b_per_cell;
    return g_ary[ind+1];        
}
function set_bt(x,y,z,bt) {
    var ind=(x+z*fsz+y*fsz*fsz)*b_per_cell;
    g_ary[ind+0]=bt;            
}
function now() {
    return new Date().getTime();
}

for(var i=0;i<10;i++) {
    var st=now();
    for(var y=0;y<fsz;y++){
        for(var z=0;z<fsz;z++) {
            for(var x=0;x<fsz;x++) {
                set_bt(x,y,z,1);
            }
        }
    }
    var et=now();
    console.log("set:",et-st);
    st=now();
    for(var y=0;y<fsz;y++){
        for(var z=0;z<fsz;z++) {
            for(var x=0;x<fsz;x++) {
                get_bt(x,y,z);
            }
        }
    }
    et=now();
    console.log("get:",et-st);
}

console.log("kkkkkk");
g_ary_bt=new Uint8Array(cell_num);

function get_bt2(x,y,z){
    var ind=x+z*fsz+y*fsz*fsz;
    return g_ary_bt[ind];
}
function set_bt2(x,y,z,bt) {
    var ind=x+z*fsz+y*fsz*fsz;
    g_ary_bt[ind]=bt;    
}


for(var i=0;i<10;i++) {
    var st=now();
    for(var y=0;y<fsz;y++){
        for(var z=0;z<fsz;z++) {
            for(var x=0;x<fsz;x++) {
                set_bt2(x,y,z,1);
            }
        }
    }
    var et=now();
    console.log("set2:",et-st);
    st=now();
    for(var y=0;y<fsz;y++){
        for(var z=0;z<fsz;z++) {
            for(var x=0;x<fsz;x++) {
                get_bt2(x,y,z);
            }
        }
    }
    et=now();
    console.log("get2:",et-st);
}