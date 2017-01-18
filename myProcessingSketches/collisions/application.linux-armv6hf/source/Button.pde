/**
 * By Michael Goltz, Dec. 2016
 * Processing Button Object Class
 * 
 * This sketch contains a class for a button object
 * the variables include x, y cartisian positions,
 * button diminsions and text label centered within button
 * Basic functions clickCheck returns true if clickedPressed
 * is within the bounding region. A constructor and show
 * functions are present as well.
 */

class Button {
  float xpos;
  float ypos;
  float buttonWidth;
  float buttonHeight;
  String buttonText;
  
  Button(float x, float y, float w, float h, String t) {
    this.xpos = x;
    this.ypos = y;
    this.buttonWidth = w;
    this.buttonHeight = h;
    this.buttonText = t;
  }
  
  void show() {
    stroke(150); fill(200);
    rect(xpos, ypos, buttonWidth, buttonHeight);
    stroke(0); fill(0);
    textSize(12);
    text(buttonText, xpos+buttonWidth/3, ypos+3*buttonHeight/4);
  }
  
  // check button functionality
  int clickCheck( int _x, int _y ){
    if( _x > xpos && _y > ypos && _x < xpos+buttonWidth && _y < ypos+buttonHeight ){
      if ( buttonText.equals("Start") ) {
        println(buttonText + " clicked");
        return 1;  // start pressed
      } else {
        println(buttonText + " clicked");
        return 0; // reset pressed
      }
    } else return -1;
  }
  
}