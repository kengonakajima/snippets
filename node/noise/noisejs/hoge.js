var perlinjs=require("./perlin.js");
var noise = perlinjs.noise;
console.log(noise);

var w=130;
for(var y=0;y<w;y++) {
    for(var x=0;x<w;x++) {
        var v_octave0=50;
        var v_octave0 = Math.abs(noise.perlin2(x / v_octave0, y / v_octave0));
        var v_octave1= 3;
        var v_octave1 = Math.abs(noise.perlin2(x / v_octave1, y / v_octave1));
        var value = (v_octave1 + v_octave0)/1.5;
        if(value>0.5) {
            process.stdout.write("*");            
        } else {
            process.stdout.write(".");
        }
    }
    console.log("");
}


