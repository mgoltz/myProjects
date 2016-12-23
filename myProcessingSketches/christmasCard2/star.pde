class Star {
 float x;
 float y;
 float sx;
 float sy;
 float yspeed;
 float z;
 
 float pz;
 
 Star() {
   x = random(0, width);
   y = random(-height, 0);
   yspeed = 1;
   z = random(width);
 }
 
 void fall() {
   y = y + yspeed;
   if (sy > (3*height/4)) {
     x = random(0, width);
     y = random(-height, 0);
   }
 }
 
 void show() {
  fill(255);
  noStroke();
  
  sx = map(x/z, 0, 1, 0, width);
  sy = map(y/z, 0, 1, -height, 0);
  
  float r = map(z, 0, width, 8, 1);
  ellipse(sx, sy, r, r);

 }
}