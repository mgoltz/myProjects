/**
 * By Michael Goltz, Dec. 2016
 * Processing Block Object Class
 * 
 * This sketch contains a class for a block object
 * the variables include x, y cartisian positions,
 * z = size of block, c = color of block in RGB or grey scale
 * xspeed = horizontal speed, and a reset horizontal position.
 * Basic functions as get and set x and xspeed exist as well
 * as a constructor, show, and move functions
 */

class Block {
  float xpos;
  float y;
  float z;
  color c;
  float xspeed;
  float resetX;
  
  Block(float x, float y, float z, color c) {
    this.xpos = x;
    this.resetX = x;
    this.y = y;
    this.z = z;
    this.c = c;
  }
  
  void show() {
    fill(c);
    noStroke();
    rect(this.xpos, this.y, this.z, this.z); 
  }
  
  void move() {
    xpos = xpos + xspeed;
    //if (xpos > width) {
    //  xpos = 0;
    //}
  }
  
  void setX(float x) {
    xpos = x;
  }
  void setSpeed(float s) {
    xspeed = s;
  }
  void reset() {
    //resetX = x;
    xpos = resetX;
  }
  float getLeftEdge() {
    return xpos; 
  }
  float getRightEdge() {
    return xpos+z;
  }
  
}