var code=
    "void setup() {\n"+
    "size(200, 200);\n"+
    "background(125);\n"+
    "fill(255);\n"+
    "noLoop();\n"+
    "}\n"+
    "void draw() {\n"+
    "text(\"Hello World!\", 20, 20);\n"+
    "}\n";

var sketch=Processing.compile(code);

var canvas = document.getElementById("canvas1");
// attaching the sketchProc function to the canvas
var processingInstance = new Processing(canvas, sketch);
