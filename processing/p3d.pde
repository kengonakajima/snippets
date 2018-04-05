float x,y,z;

void setup() {
  size(200,200,P3D);
  x = width/2;
  y = height/2;
  z = 0;
}

void draw() {
  translate(x,y,z);
  rectMode(CENTER);
  rect(0,0,100,100);

  z++; // The rectangle moves forward as z increments.
}


