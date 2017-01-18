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
  StringList listItems;
  boolean expanded = false;
  String typeSelected = "";
  
  DropDownList(float x, float y, float w, float h, String t) {
    this.xpos = x;
    this.ypos = y;
    this.listWidth = w;
    this.listHeight = h;
    this.listLabel = t;
    listItems = new StringList();
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
    listItems.append(item);
  }
  
  int clickCheck( int _x, int _y ){
    //int i = -1;
    int found = -1;
    if( _x > xpos && _y > ypos && _x < xpos+listWidth && _y < ypos+listHeight ){
      println("expand list");
      if (!expanded) expand();
      else expanded = false;
    } else if (expanded && ( _x > xpos && _y > ypos+listHeight && _x < xpos+listWidth && _y < ypos+listHeight*(listItems.size()+1) )) {
      for (int i = 0; i < listItems.size(); i++) {
        if ( (_x > xpos) && (_y > ypos+listHeight*(i+1)) && (_x < xpos+listWidth) && (_y < ypos+listHeight*(i+2)) ) {
          typeSelected = listItems.get(i);
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
    for (int i = 0; i < listItems.size(); i++) {
      stroke(150); fill(200);
      rect(xpos,ypos+listHeight*(i+1),listWidth,listHeight);
      stroke(0); fill(0);
      textSize(12);
      text(listItems.get(i), xpos+5, ypos+listHeight*(i+2)-5);
    }
  }
}