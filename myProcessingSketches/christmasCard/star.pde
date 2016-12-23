class Star {
 float x;
 float y;
 float z;
 
 float pz;
 
 Star() {
   x = random(0, width);
   y = random(0, height);
   z = random(width);
   pz = z;
 }
 
 void show() {
  fill(255);
  noStroke();
  
  float sx = map(x/z, 0, 1, 0, width);
  float sy = map(y/z, 0, 1, 0, height);
  
  float r = map(z, 0, width, 8, 1);
  ellipse(sx, sy, r, r);

 }
}