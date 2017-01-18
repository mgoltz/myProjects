/**
 * By Michael Goltz, Dec. 2016
 * Processing Perfect Collision Simulator
 * 
 * This sketch contains one dimensional perfect
 * collision simulation using block objects and their scale
 * collisions are detected and new velocities of the
 * blocks are calculated and set upon colliding at the center.
 * Users can set the collision type, velocities of two blocks,
 * and their respective mass velocities.
 */

// blocks, block size, location, and color
Block blockL;
Block blockR;
float blockSize = 25;
float blockLx = 137.5;  // left block starting position
float blockRx = 662.5;  // left block starting position
//float blockRx = 137.5;
float blocky = 250;
color blockLc = color(255, 0, 0); // red
color blockRc = color(0); // black

// button container and size
ArrayList buttons;
float buttonH = 20;
float buttonW = 70;
float buttonX = 100;
float buttonY = 50;

// drop down lists
DropDownList listType;
DropDownList listLeftVel;
DropDownList listRightVel;
DropDownList listMassRatio;

boolean start = false;
boolean collision = false;

float[] avL = { 0.2, 0.3, 0.4, 0.5, 0.6, 0.8, 1.0 };
float[] avR = { -1.0, -0.8, -0.6, -0.5, -0.4, -0.2, 0.0, 0.2, 0.4 };
float[] aM = {0.3, 0.5, 1, 2};
int collisionType = 0; // 1 = inelastic, 0 = elastic
float vL = 0.0;
float vR = 0.0;
float mRat = 0.0;
float pL;
float pR;

void setup() {
  surface.setTitle("Physics - Perfect Collision Simulator, v.1.0"); // set title
  frameRate(50);
  size(800, 400);
  background(255);
  
  // block objects
  blockL = new Block(blockLx, blocky, blockSize, blockLc);
  blockR = new Block(blockRx, blocky, blockSize, blockRc);
  
  // button objects
  buttons = new ArrayList();
  buttons.add( new Button(buttonX, buttonY, buttonW, buttonH, "Start") );
  buttons.add( new Button(buttonX*2, buttonY, buttonW, buttonH, "Reset") );
  
  // list objects
  listType = new DropDownList(buttonX*3, buttonY, buttonW+20, buttonH, "Collision Type", 0);
  //listType.addItem("Elastic");
  //listType.addItem("Inelastic");
  
  // Left Block Settings/Options
  listLeftVel = new DropDownList(buttonX*4, buttonY, buttonW+20, buttonH, "Red Velocity", 1);
  //for (int i = 0; i < avL.length; i++) {
  //  listLeftVel.addItem(str(avL[i]));
  //}

  // Right Block Settings/Options
  listRightVel = new DropDownList(buttonX*5, buttonY, buttonW+20, buttonH, "Black Velocity", 2);
  //for (int i = 0; i < avR.length; i++) {
  //  listRightVel.addItem(str(avR[i]));
  //}
  
  listMassRatio = new DropDownList(buttonX*6, buttonY, buttonW+20, buttonH, "Mass Ratio", 3);
  //for (int i = 0; i < aM.length; i++) {
  //  listMassRatio.addItem(str(aM[i]));
  //}

}

void draw() {
  background(255);
  fill(0);
  textSize(10);
  text("Goltz.Michael@outlook.com", 610, 390);
  textSize(12);
  text("By Michael Goltz, 2016, v.1.0", 610, 380);
  text("INSTRUCTIONS:", 10, 355);
  text("Select a collision type, velocity for the left and right block, and a mass ratio (Right/(Left=1)).", 10, 370);
  text("Press 'Start' to activate the simulator, or 'Reset' to set the blocks back.", 10, 385);
  
  blockL.show();
  blockR.show();
  
  // show the two buttons
  for (int i = buttons.size()-1; i >= 0; i--) {
        Button aButton = (Button) buttons.get(i);
        aButton.show();
  }
  // show the lists
  listType.show();
  listLeftVel.show();
  listRightVel.show();
  listMassRatio.show();
  
  // function to display the scale
  drawScale();
  
  // watch for start button pressed
  if (start) {
    if (!collision) {
      startSim();
    }
    if (collisionDetected() || collision) {
      collision = true;
      resultVelocity();
      endReached();
      blockL.move();
      blockR.move();
      blockL.show();
      blockR.show();
      //stopSim();
    }
  } else {
    stopSim();
    resetSim();
  }
   
}

// draw the scale to size of block
void drawScale() {
  textSize(10);
  line(blockLx-50, blocky+25, blockRx+50, blocky+25); // baseline
  
  line((blockLx+blockSize+blockRx)/2, blocky, 
       (blockLx+blockSize+blockRx)/2, blocky+30);    // midline
  text("0", (blockLx+blockSize+blockRx)/2-4, blocky+45);
  line((blockLx), blocky-10,
       (blockLx), blocky+30);                        // Left endline
  line((blockLx+blockSize*1), blocky+20,
       (blockLx+blockSize*1), blocky+30);
  text("-5", blockLx+blockSize*1-9, blocky+45);
  line((blockLx+blockSize*2), blocky+23,
       (blockLx+blockSize*2), blocky+27);
  //text("-9", blockLx+blockSize*2-9, blocky+45);
  line((blockLx+blockSize*3), blocky+20,
       (blockLx+blockSize*3), blocky+30);
  text("-4", blockLx+blockSize*3-9, blocky+45);
  line((blockLx+blockSize*4), blocky+23,
       (blockLx+blockSize*4), blocky+27);
  //text("-7", blockLx+blockSize*4-9, blocky+45);
  line((blockLx+blockSize*5), blocky+20,
       (blockLx+blockSize*5), blocky+30);
  text("-3", blockLx+blockSize*5-9, blocky+45);
  line((blockLx+blockSize*6), blocky+23,
       (blockLx+blockSize*6), blocky+27);
  //text("-5", blockLx+blockSize*6-9, blocky+45);
  line((blockLx+blockSize*7), blocky+20,
       (blockLx+blockSize*7), blocky+30);     
  text("-2", blockLx+blockSize*7-9, blocky+45);
  line((blockLx+blockSize*8), blocky+23,
       (blockLx+blockSize*8), blocky+27);
  //text("-3", blockLx+blockSize*8-9, blocky+45);
  line((blockLx+blockSize*9), blocky+20,
       (blockLx+blockSize*9), blocky+30);
  text("-1", blockLx+blockSize*9-9, blocky+45);
  line((blockLx+blockSize*10), blocky+23,
       (blockLx+blockSize*10), blocky+27);
  //text("-1", blockLx+blockSize*10-9, blocky+45);
  line((blockRx+blockSize), blocky-10,
       (blockRx+blockSize), blocky+30);            // right endline 
  text("5", blockLx+blockSize*21-3, blocky+45);
  line((blockLx+blockSize*21), blocky+20,
       (blockLx+blockSize*21), blocky+30);       
  //text("9", blockLx+blockSize*20-3, blocky+45);
  line((blockLx+blockSize*20), blocky+23,
       (blockLx+blockSize*20), blocky+27);
  text("4", blockLx+blockSize*19-3, blocky+45);
  line((blockLx+blockSize*19), blocky+20,
       (blockLx+blockSize*19), blocky+30);
  //text("7", blockLx+blockSize*18-3, blocky+45);
  line((blockLx+blockSize*18), blocky+23,
       (blockLx+blockSize*18), blocky+27);
  text("3", blockLx+blockSize*17-3, blocky+45);
  line((blockLx+blockSize*17), blocky+20,
       (blockLx+blockSize*17), blocky+30);
  //text("5", blockLx+blockSize*16-3, blocky+45);
  line((blockLx+blockSize*16), blocky+23,
       (blockLx+blockSize*16), blocky+27);
  text("2", blockLx+blockSize*15-3, blocky+45);
  line((blockLx+blockSize*15), blocky+20,
       (blockLx+blockSize*15), blocky+30);
  //text("3", blockLx+blockSize*14-3, blocky+45);
  line((blockLx+blockSize*14), blocky+23,
       (blockLx+blockSize*14), blocky+27);  
  text("1", blockLx+blockSize*13-3, blocky+45);
  line((blockLx+blockSize*13), blocky+20,
       (blockLx+blockSize*13), blocky+30);
  //text("1", blockLx+blockSize*12-3, blocky+45);
  line((blockLx+blockSize*12), blocky+23,
       (blockLx+blockSize*12), blocky+27); 

}

// watch for mouse click action and check if button or list pressed
void mouseClicked(){
  for (int i = buttons.size()-1; i >= 0; i--) {
    Button aButton = (Button) buttons.get(i);
    int pressed = aButton.clickCheck(mouseX, mouseY);
    if (pressed == 1) {
      println("starting");
      start = true;
      collision = false;
    } else if (pressed == 0) {
      println("stopping");
      start = false;
    }
  }
  
  int selection = listType.clickCheck(mouseX, mouseY);
  if (selection > -1) {
    collisionType = selection;
  }
  selection = listLeftVel.clickCheck(mouseX, mouseY);
  if (selection > -1) {
    vL = avL[selection]; // get index of selection
  }
  selection = listRightVel.clickCheck(mouseX, mouseY);
  if (selection > -1) {
    vR =avR[selection];
  }
  selection = listMassRatio.clickCheck(mouseX, mouseY);
  if (selection > -1) {
    mRat = aM[selection];
  }
  
  println("c: " + collisionType + ", vL: " + vL + ", vR: " + vR + ", M: " + mRat);
}
void startSim() {
  // modify speed and direction of block
  blockL.setSpeed(vL);
  blockR.setSpeed(vR);
  blockL.move();
  blockR.move();
  blockL.show();
  blockR.show();

}
void stopSim() {
  // modify speed and direction of block
  collision = true;
  blockL.setSpeed(0.0);
  blockR.setSpeed(0.0);
  blockL.move();
  blockR.move();
  blockL.show();
  blockR.show();
   
}
void resetSim() {
  // set new speed from list selection
  resetPosition();
  blockL.show();
  blockR.show();
}
boolean collisionDetected() {
  if (blockL.getRightEdge() >= blockR.getLeftEdge()) {
    println("collision detected");
    return true;
  } else return false;
}
void endReached() {
  if (blockL.getRightEdge() <= blockLx) {
    blockL.setSpeed(0.0);
    if (blockR.getRightEdge() <= blockLx+blockSize) {
      blockR.setSpeed(0.0);
    }
  }
  if (blockR.getLeftEdge() >= blockRx+blockSize) {
    blockR.setSpeed(0.0);
    if (blockL.getLeftEdge() >= blockRx) {
      blockL.setSpeed(0.0);
    }
  }
}
void timeBlock() {
 int m = millis();
 if ((blockL.getRightEdge() >= (blockLx+blockSize*2)) && (blockL.getRightEdge() < (blockLx+blockSize*2+1))) {
    println("Start: "+ m);    
 } else if ((blockL.getRightEdge() >= (blockLx+blockSize*3)) && (blockL.getRightEdge() < (blockLx+blockSize*3+1))) {
    println("tick 2: "+ m);    
 } else if ((blockL.getRightEdge() >= (blockLx+blockSize*4)) && (blockL.getRightEdge() < (blockLx+blockSize*4+1))) {
    println("tick 3: "+ m);    
 } else if ((blockL.getRightEdge() >= (blockLx+blockSize*5)) && (blockL.getRightEdge() < (blockLx+blockSize*5+1))) {
    println("tick 4: "+ m);    
 } else if ((blockL.getRightEdge() >= (blockLx+blockSize*6)) && (blockL.getRightEdge() < (blockLx+blockSize*6+1))) {
    println("tick 5: "+ m);    
 } else if ((blockL.getRightEdge() >= (blockLx+blockSize*7)) && (blockL.getRightEdge() < (blockLx+blockSize*7+1))) {
    println("tick 6: "+ m);    
 }
}
void resetPosition() {
  
  float t;
  if (vR < 0) {
    if (-vR > vL) {
      t = -10 / vR;
      pL = blockLx+(blockSize*(10)-blockSize*(vL*t));
      blockL.setX(pL);
      blockR.reset();
    } else if (-vR < vL) {
      t = -10 / vL;
      pR = blockLx+(blockSize*(11)+blockSize*(vR*t));
      blockR.setX(pR);
      blockL.reset();
    } else {
      blockL.reset();
      blockR.reset();
    }
  } else if (vR == 0.0) {
      pR = blockLx+(blockSize*(11));
      blockR.setX(pR);
      blockL.reset();
  } else { // vR is greater than vL, move right block closer
    if (vR < vL) { 
      t = -10 / vL;
      pR = blockLx+(blockSize*(11)+blockSize*(vR*t));
      blockR.setX(pR);
      blockL.reset();
    } else {
      textSize(12);
      text("Left velocity must be greater", 400, 15);
      text("than Right velocity", 400, 30);
      blockL.setX(blockLx+(blockSize*(10)));
      blockR.setX(blockLx+(blockSize*(11)));
    }
  }
  
  blockL.show();
  blockR.show();
}

// calculate new velocities after collision detected
void resultVelocity() {
  float mL = 1.0;
  float mR = mRat / mL;
  float v_L;
  float v_R;
  if (collisionType == 0) { //0 = elastic
    // calculate new velocities for right and left block
    v_R = 2*mL/(mL+mR)*vL - (mL-mR)/(mL+mR)*vR;
    v_L = (mL-mR)/(mL+mR)*vL + 2*mR/(mL+mR)*vR;
    println(v_L + ", " + v_R);
    blockR.setSpeed(v_R);
    blockL.setSpeed(v_L);
    
  } else if (collisionType == 1) {  // 1 = inelastic
    // merge blocks calculate new velocity set equal to both
    v_R = (mL*vL + mR*vR)/(mL+mR);
    println(v_R);
    blockR.setSpeed(v_R);
    blockL.setSpeed(v_R);
  }
  
}

// END OF FILE

// Start of BLOCKS
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

// END of BLOCK

// START of Button
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

// END of Button

// START of DropDownList
/**
 * By Michael Goltz, Dec. 2016
 * Processing DropDownList Object Class
 * 
 * This sketch contains a class for a DropDownList object
 * the variables include x, y cartisian positions,
 * list diminsions and text label positioned above list, a
 * string list for the items in the list and other variables
 * to capture the state of the list selection and actions.
 * Basic functions clickCheck returns true if clickedPressed
 * is within the bounding region. The list will either expand
 * or collapse depending on click region. A additem function
 * adds items to the list container. A constructor and show
 * functions are present as well.
 */

class DropDownList {
  float xpos;
  float ypos;
  float listWidth;
  float listHeight;
  String listLabel;
  //StringList listItems;
  boolean expanded = false;
  String typeSelected = "";

  //float[] avL = { 0.2, 0.3, 0.4, 0.5, 0.6, 0.8, 1.0 };
  //float[] avR = { -1.0, -0.8, -0.6, -0.5, -0.4, -0.2, 0.0, 0.2, 0.4 };
  //float[] aM = {0.3, 0.5, 1, 2};
  
  String[][] allLists = { { "Elastic", "Inelastic" },
                      { "0.2", "0.3", "0.4", "0.5", "0.6", "0.8", "1.0" }, 
                      { "-1.0", "-0.8", "-0.6", "-0.5", "-0.4", "-0.2", "0.0", "0.2", "0.4" }, 
                      {"0.3", "0.5", "1", "2"}
                     };

  int whichList; // 0=type, 1=leftVel, 2=rightVel, 3=massRatio
  
  DropDownList(float x, float y, float w, float h, String t, int listSelection) {
    this.xpos = x;
    this.ypos = y;
    this.listWidth = w;
    this.listHeight = h;
    this.listLabel = t;
    //listItems = new StringList();
    whichList = listSelection;
  }
  
  void show() {
    //rect with down arrow
    stroke(150); fill(200);
    rect(xpos, ypos, listWidth,listHeight);
    stroke(0); fill(0);
    textSize(12);
    text(listLabel, xpos, ypos-2);
    triangle(xpos+listWidth-10, ypos+5, xpos+listWidth, ypos+5, xpos+listWidth-5, ypos+15);
    text(typeSelected, xpos+5, ypos+listHeight-5);
    if (expanded) {
      expand();
    }
    
  }
  void addItem(String item) {
  //  listItems.append(item);
  }
  
  int clickCheck( int _x, int _y ){
    //int i = -1;
    int found = -1;
    if( _x > xpos && _y > ypos && _x < xpos+listWidth && _y < ypos+listHeight ){
      println("expand list");
      if (!expanded) expand();
      else expanded = false;
    } else if (expanded && ( _x > xpos && _y > ypos+listHeight && _x < xpos+listWidth && _y < ypos+listHeight*(allLists[whichList].length+1) )) {
  
      for (int i = 0; i < allLists[whichList].length; i++) {
        if ( (_x > xpos) && (_y > ypos+listHeight*(i+1)) && (_x < xpos+listWidth) && (_y < ypos+listHeight*(i+2)) ) {
          typeSelected = allLists[whichList][i]; //listItems.get(i);
          println(typeSelected);
          expanded = false;
          found = i;
        }
      }
    } else {
        expanded = false;
    }
    return found;
  }
  void expand() {
    expanded = true;
    for (int i = 0; i < allLists[whichList].length; i++) { // listItems.size()
      stroke(150); fill(200);
      rect(xpos,ypos+listHeight*(i+1),listWidth,listHeight);
      stroke(0); fill(0);
      textSize(12);
      text(allLists[whichList][i], xpos+5, ypos+listHeight*(i+2)-5);
    }
  }
}

// END of DropDownList