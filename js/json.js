var sys = require("sys");

var v = {}
v["method"]="aho";
v["params"]=[1,2,"hoge"];

var s = JSON.stringify(v);

sys.print(s+"\n");
