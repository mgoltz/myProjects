class Snow {
 float x;
 float y;
 float z;

 Snow() {
   x = random(width);
   y = random(3*height/4, height);
   z = random(width);
 }
 
 void show() {
  fill(255);
  noStroke();
 
  float r = map(z, 0, width, 10, 1);
  ellipse(x, y, r, r);
  
 }
}