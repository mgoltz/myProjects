/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author m
 */
import java.awt.Graphics;
import javax.swing.JApplet;

public class collisionApplet extends JApplet {
    public void init() {
        
    }
    
    public void paint(Graphics g) {
        super.paint(g);
        g.drawString("(In)elastic Collision Applet", 30, 30);
    }
}
