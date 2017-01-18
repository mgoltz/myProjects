/*
 * Copyright (c) 2010-2016 William Bittle  http://www.dyn4j.org/
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification, are permitted 
 * provided that the following conditions are met:
 * 
 *   * Redistributions of source code must retain the above copyright notice, this list of conditions 
 *     and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright notice, this list of conditions 
 *     and the following disclaimer in the documentation and/or other materials provided with the 
 *     distribution.
 *   * Neither the name of dyn4j nor the names of its contributors may be used to endorse or 
 *     promote products derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR 
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER 
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
package exampleDyn4j;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.FlowLayout;
import java.awt.Font;
import java.awt.Graphics2D;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JPanel;

import org.dyn4j.dynamics.World;
import org.dyn4j.geometry.Geometry;
import org.dyn4j.geometry.MassType;

/**
 * A simple scene of two billiard balls colliding with one another
 * and a wall.
 * <p>
 * Primarily used to illustrate the computation of the mass and size
 * of the balls.  See the {@link Billiards#initializeWorld()} method.
 * @author William Bittle
 * @version 3.2.1
 * @since 3.2.0
 */
public final class Billiards extends SimulationFrame {
	/** The serial version id */
	private static final long serialVersionUID = -8518496343422955267L;
        private double CollsionType;
        private static double SquareSize = 0.132;
        private static double Sqaurem3 = SquareSize * SquareSize * SquareSize;
        private static double RedMass = 1.0; // kg
        private double BlackMass;
        private double BlackDensity;
        private static double RedDensity = RedMass / Sqaurem3;
        private double RedVel;
        private double BlackVel;
        private static Billiards simulation;
        private static JButton jbtStart;
        private static JButton jbtReset;
	/**
	 * Default constructor.
	 */
	public Billiards() {
		super("Collision Simulator", 300.0);      //pixels per meter scale factor
	}
	
	/* (non-Javadoc)
	 * @see org.dyn4j.samples.SimulationFrame#initializeWorld()
	 */
	@Override
	protected void initializeWorld() {
            
                // create all your bodies/joints
		Color whtColor = new Color(255,255,255);
                Color blkColor = new Color(0,0,0);
                Color redColor = new Color(255,0,0);
                
                SimulationBody squareL = new SimulationBody(redColor);
                SimulationBody squareR = new SimulationBody(blkColor);
                
            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            //- - Labels, Buttons, and Combo Boxes
            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                Font TitleFont = new Font("TimesRoman", Font.BOLD, 20);
                Font ControlFont = new Font("TimesRoman", Font.BOLD, 14);
                JPanel panelButtons = new JPanel();
                JPanel panelControl = new JPanel(new FlowLayout(FlowLayout.LEFT, 10, 20)); 
                JPanel panelTitle = new JPanel();

                JLabel lblTitle = new JLabel("Collision Simulator");
                JLabel lblCollType = new JLabel("Collision Type");
                JLabel lblMassRatio = new JLabel("Mass Ratio");
                JLabel lblBlackSpeed = new JLabel("Black Speed");
                JLabel lblRedSpeed = new JLabel("Red Speed");
                lblCollType.setFont(ControlFont);
                lblMassRatio.setFont(ControlFont);
                lblBlackSpeed.setFont(ControlFont);
                lblRedSpeed.setFont(ControlFont);

                lblTitle.setFont(TitleFont);
                jbtStart = new JButton("Start");
                jbtReset = new JButton("Reset");
                jbtStart.setEnabled(false);

                String[] vRedOptions = {"0.10", "0.20", "0.30", "0.40", "0.50" };
                String[] vBlackOptions = {"-0.50", "-0.40", "-0.30", "-0.20", "-0.10", 
                    "0.00", "0.10", "0.20", "0.30", "0.40", "0.50" };
                String[] massRatioOptions = {"0.20", "0.30", "0.40", "0.50",
                    "0.60", "0.70", "0.80", "0.90", "1.00", "2.00" };
                String[] collisionOptions = {"Perfectly Inelastic", "Perfectly Elastic"};

                JComboBox jcbCollType = new JComboBox(collisionOptions);
                JComboBox jcbMassRatio = new JComboBox(massRatioOptions);
                JComboBox jcbBlackSpeed = new JComboBox(vBlackOptions);
                JComboBox jcbRedSpeed = new JComboBox(vRedOptions);

                jcbCollType.setSelectedIndex(0);
                int CollsionTypeIndex = jcbCollType.getSelectedIndex();
                CollsionType = (double)CollsionTypeIndex;
                System.out.println("Collision Type selected: " + collisionOptions[CollsionTypeIndex] + " = " + CollsionType);
                
                jcbMassRatio.setSelectedIndex(0);
                int MassRatio = jcbMassRatio.getSelectedIndex();
                BlackMass = Double.parseDouble(massRatioOptions[MassRatio]);
                BlackDensity = BlackMass / Sqaurem3;
                System.out.println("Mass Ratio selected: " + massRatioOptions[MassRatio] + " = Red: " + RedMass + ", Black: " + BlackMass);                
                
                jcbBlackSpeed.setSelectedIndex(0);
                int BlackVelIndex = jcbBlackSpeed.getSelectedIndex();
                BlackVel = Double.parseDouble(vBlackOptions[BlackVelIndex]);
                System.out.println("Black Velocity selected: " + vBlackOptions[BlackVelIndex] + " = " + BlackVel);
                        
                jcbRedSpeed.setSelectedIndex(0);
                int RedVelIndex = jcbRedSpeed.getSelectedIndex();
                RedVel = Double.parseDouble(vRedOptions[RedVelIndex]);
                System.out.println("Red Velocity selected: " + vRedOptions[RedVelIndex] + " = " + RedVel);
                
                
                System.out.println("Red Density: " + RedDensity + ", Black Density: " + BlackDensity);

                panelTitle.add(lblTitle);
                panelButtons.add(jbtStart);
                panelButtons.add(jbtReset);
                panelControl.add(lblCollType);      // collision type
                panelControl.add(jcbCollType);      // 
                panelControl.add(lblMassRatio);     // mass ratio
                panelControl.add(jcbMassRatio);
                panelControl.add(lblRedSpeed);      // red velocity
                panelControl.add(jcbRedSpeed);
                panelControl.add(lblBlackSpeed);    // black velocity
                panelControl.add(jcbBlackSpeed);

                this.add(panelButtons, BorderLayout.EAST);
                this.add(panelTitle, BorderLayout.NORTH);
                this.add(panelControl, BorderLayout.SOUTH);      
          
                //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                //- - Action Listener to get the user selections
                //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                jcbCollType.addActionListener(new ActionListener() {
                    public void actionPerformed(ActionEvent e) {
                        JComboBox cb = (JComboBox)e.getSource();
                        String collisionSelected = (String)cb.getSelectedItem();
                        System.out.println(collisionSelected);
                    }
                });
                jcbMassRatio.addActionListener(new ActionListener() {
                    public void actionPerformed(ActionEvent e) {
                        JComboBox cb = (JComboBox)e.getSource();
                        //String massRatioSeleted = (String)cb.getSelectedItem();
                        
                        int MassRatio = cb.getSelectedIndex();
                        BlackMass = Double.parseDouble(massRatioOptions[MassRatio]);
                        BlackDensity = BlackMass / Sqaurem3;
                        System.out.println("Mass Ratio selected: " + massRatioOptions[MassRatio] + " = Red: " + RedMass + ", Black: " + BlackMass);
                        System.out.println("Red Density: " + RedDensity + ", Black Density: " + BlackDensity);
                    }
                });
                jcbRedSpeed.addActionListener(new ActionListener() {
                    public void actionPerformed(ActionEvent e) {
                        JComboBox cb = (JComboBox)e.getSource();
                        String redSpeedSelected = (String)cb.getSelectedItem();
                        RedVel = Double.parseDouble(redSpeedSelected);
                        System.out.println(redSpeedSelected);
                    }
                });
                jcbBlackSpeed.addActionListener(new ActionListener() {
                    public void actionPerformed(ActionEvent e) {
                        JComboBox cb = (JComboBox)e.getSource();
                        String blackSpeedSelected = (String)cb.getSelectedItem();
                        System.out.println(blackSpeedSelected);
                    }
                });
                
                 jbtStart.addActionListener(new ActionListener() {
                    public void actionPerformed(ActionEvent e) {
                        // start simulation
                        //if (!simulation.isStopped())
                        //    simulation.run();
                        //else 
                            
                        simulation.resume();
                        
                        jbtStart.setEnabled(false);
                        System.out.println("Collision: Start");
                    }
                });
                
                jbtReset.addActionListener(new ActionListener() {
                    public void actionPerformed(ActionEvent e) {
                        
                        
                        //simulation = new Billiards();
                        //simulation.run();
                        //simulation.pause();
                        
                        // stop simulation
                        //if (!simulation.isStopped()) {
                        //    simulation.stop();
                        //    System.out.println("reset: sim set to stop");
                       // }
                        //else { // re-render
                        //    simulation.reset();
                        //    simulation.run();
                        //    simulation.pause();
                       // }
                        
                        jbtStart.setEnabled(true);
                        System.out.println("Collision: Reset");
                    }
                });
                
                //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            
		// no gravity on a top-down view of a billiards game
		this.world.setGravity(World.EARTH_GRAVITY);
                
		SimulationBody wallr = new SimulationBody(blkColor);
		wallr.addFixture(Geometry.createRectangle(0.1, 1)); // width , height
		wallr.translate(1.0, 0);
		wallr.setMass(MassType.INFINITE);
		world.addBody(wallr);
                
                SimulationBody walll = new SimulationBody(blkColor);
		walll.addFixture(Geometry.createRectangle(0.1, 1)); // width , height
		walll.translate(-1.0, 0);
		walll.setMass(MassType.INFINITE);
		world.addBody(walll);
                
                SimulationBody floor = new SimulationBody(blkColor);
		floor.addFixture(Geometry.createRectangle(2, 0.1)); // width , height
		floor.translate(0.0, -0.12);
		floor.setMass(MassType.INFINITE);
		world.addBody(floor);
                
                SimulationBody roof = new SimulationBody(whtColor);
		roof.addFixture(Geometry.createRectangle(2, 0.1)); // width , height
		roof.translate(0.0, 0.11);
		roof.setMass(MassType.INFINITE);
		world.addBody(roof);
		
                
		// Left Red Square
		squareL.addFixture(Geometry.createSquare(SquareSize), //  2.25 in diameter = 0.028575 m radius .createSquare
				RedDensity, 			//  0.126 oz/in^3 = 217.97925 kg/m^3
				0.0,                            //  friction 0-1
				CollsionType);                  //  restitution=elasticity 0.0 - 1.0, 0=inelastic, 1=elastic
		squareL.translate(-0.5, 0.0);                   // from center
		//ball1.setLinearVelocity(5.36448, 0.0);        // 12 mph = 5.36448 m/s
		squareL.setLinearVelocity(1.5, 0);      	//  so we can see the bouncing
		squareL.setMass(MassType.NORMAL);
		this.world.addBody(squareL);
		
		// Right Black Square
		squareR.addFixture(Geometry.createSquare(SquareSize), BlackDensity, 0.0, CollsionType); // convex, density, friction=0.08, restitution=0.9
		squareR.translate(0.5, 0.0); // from center
                squareR.setLinearVelocity(-1.5, 0);
		squareR.setMass(MassType.NORMAL);
		this.world.addBody(squareR);
                
           
	}
	
	/* (non-Javadoc)
	 * @see org.dyn4j.samples.SimulationFrame#render(java.awt.Graphics2D, double)
	 */
	@Override
	protected void render(Graphics2D g, double elapsedTime) {
		// move the view a bit
		g.translate(0, 0);  // -200
		
		super.render(g, elapsedTime);
	}
	
	/**
	 * Entry point for the example application.
	 * @param args command line arguments
	 */
	public static void main(String[] args) {
                
                // get input from selection menues
                    // pass to Billiards initial constructor
                    // constructor sets blocks to location
		simulation = new Billiards();
                           
                // rerender any changes
                // see reset button action
                //simulation.setupRendering();
                
                // wait until start button pressed to run simulation
                // see start button action
		simulation.run();
                simulation.pause();
                
               
	}
}
