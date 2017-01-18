Star[] stars = new Star[1000];
Snow[] snow = new Snow[8000];

float merryValue = 0;
float christValue = 0;
float alphaValue = 0;
float reduceValue = 400;
boolean rev = true;

void setup() {
  size(600, 600);
  background(25,25,112);
  
  // snow/stars in sky
  for (int i = 0; i < stars.length; i++) {
     stars[i] = new Star();
     //stars[i].show();
   }
   // snow on ground
   for (int i = 0; i < snow.length; i++) {
     snow[i] = new Snow();
     snow[i].show();
   }
   
   
   
  
}

void draw() {
  //background(25,25,112);
   stroke(25,25,112);
   fill(25,25,112);
   rect(0,0, width, (3*height/4)-5);
   
   for (int i = 0; i < stars.length; i++) {
     stars[i].fall();
     stars[i].show();
   }
   
  // tree trunk
   stroke(139,69,19); fill(139,69,19); // brown
   ellipse(290, 490, 20, 5); // bottom shadow ellipse
   stroke(139,69,19); fill(139,69,19); // brown
   rect(280, 440, 20, 50);
   
   // Christmas tree
   stroke(255,255,255); fill(0, 128, 0);
   // x1, y1 = top, x2, y2 = left, x3, y3 = right
   triangle(290, 380, 220, 460, 360, 460);  // base branch
   triangle(290, 370, 230, 440, 350, 440);  // second branch
   strokeWeight(1.5);
   triangle(290, 360, 240, 420, 340, 420);  // third branch
   strokeWeight(2);
   triangle(290, 350, 250, 400, 330, 400);  // fouth branch
   strokeWeight(2.5);
   triangle(290, 340, 260, 380, 320, 380);  // top
   
   // Snowman
   // x, y, r1, r2
   strokeWeight(2);
   fill(200); stroke(200);
   ellipse(450, 517, 70, 50); // bottom shadow ellipse
   fill(255); stroke(200);
   ellipse(450, 500, 80, 80); // bottom ellipse
   ellipse(450, 450, 60, 60); // middle ellispe
   ellipse(450, 410, 50, 50); // top ellispe
   // face and accessories
   fill(0); stroke(0);  // black
   rect(420,390,60, 3);   // bottom hat
   rect(430,350,40, 40);  // top hat
   ellipse(440, 405, 5, 5);  // left eye
   ellipse(460, 405, 5, 5);  // right eye
   ellipse(440, 420, 3, 3);  // top mouth
   ellipse(444, 423, 3, 3);  // middle mouth
   ellipse(449, 424, 3, 3);  // bottom mouth
   ellipse(450, 445, 5, 5);  // top button
   ellipse(450, 460, 5, 5);  // bottom button
   fill(255,140,0); stroke(255,140,0); // orange
   triangle(450,410,450,414,460,417); // nose
   stroke(165,42,42); // brown
   strokeWeight(3);
   line(472,450,520,430); // right main branch
   line(500,437,515,420); // right twig up
   line(510,435,520,440); // right twig down
   //line(432,450,400,400); // left main branch
   //line(405,405,405,390); // left twig up
   //line(406,410,390,410); // left twig down
   
   if (millis() > 3000) {
     stroke(165,42,42); // brown
     fill(165,42,42, reduceValue);
     line(432,450,400,reduceValue); // left main branch
     line(405,reduceValue+5,405,reduceValue-10); // left twig up
     line(406,reduceValue+8,390,reduceValue+8); // left twig down
     if (rev) {
       if (reduceValue > 400) {
         reduceValue--;
       } else rev = false;
     } else {
       if (reduceValue < 420){
         reduceValue++;
       } else rev = true;
     }
   }
   
   // presents
   stroke(200,0,0); // red
   strokeWeight(2);
   fill(250,0,0); // red
   quad(40,525, 80,530, 80,570, 40,565); // red front panel
   quad(80,530, 100,510, 100,550, 80,570); // red right panel
   quad(65,505, 100,510, 80,530, 40,525); // red top panel
   stroke(25,25,112); // blue
   fill(0, 102, 153);
   quad(101,512, 135,520, 135,560, 101,550); // blue left panel
   quad(135,520, 160,502, 160,540, 135,560); // blue right panel
   quad(130,495, 160,502, 135,520, 101,512); // blue top panel
   stroke(0, 110, 50); // green
   fill(0, 140, 0);
   quad(70,483, 110,490, 110,523, 70,515); // green left panel
   quad(110,490, 135,477, 135,510, 110,523); // green right panel
   quad(98,470, 135,477, 110,490, 70,483);  
   
   // Merry Christmas
   textSize(64);
   if (millis() > 8000 && millis() < 30000){
     fill(0, 102, 153, merryValue);
     text("Merry", 120, 140, -30);  // Specify a z-axis value
     if (merryValue < 255) {
       merryValue++;
     }
   }
   if (millis() > 15000 && millis() < 30000){
     fill(0, 102, 153, christValue);
     text("Christmas!!", 140, 185);  // Default depth, no z-value specified
     if (christValue < 255) {
       christValue++;
     }
   }
   if (millis() > 30000) {
     fill(0, 102, 153, 255);
     text("Merry", 120, 140, -30);  // Specify a z-axis value
     text("Christmas!!", 140, 185);  // Default depth, no z-value specified
   }
   
   
   // signature
   fill(255);
   stroke(255,255,255);
   rect(460,580,140,15);
   textSize(12);
   fill(0);
   text("By Michael Goltz, 2016", 460, 595);  // Default depth, no z-value specified
   fill(0, 102, 153, 80);
   text("By Michael Goltz, 2016", 200, 200);
   text("By Michael Goltz, 2016", 80, 445);
   
   
   saveFrame("frames/frame-######.tiff");

}