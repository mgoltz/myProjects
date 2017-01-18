/*
Matthew Ro
Peter Trueblood
CDA 4621
Project 4 Part C
*/#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */

void InitializeATD(void);

static float Output[2] = { 0 };
static unsigned char OutputIndex = 0;	//points to the most recent Output location

void main(void)
{
	SYNR = 0x02;
	REFDV = 0x00;		//this code should change the clock to 24MHz
	while (!(CRGFLG & 0x08))
	{ }
	CLKSEL |= 0x80;
	
	InitializeATD();
	
	DDRB = 0xFF;    //enable writing to Port B
	
	// set up MDCU
	MCCTL = 0b11001101; //prescaler is 4
	MCCNT = 1499;		//should cause the interrupt to be triggered every 0.25ms
	
	RTICTL = 0b01101011;	//sets prescaler of 8*2^10 resulting in interrupt every 49.15ms
	CRGINT = 0x80;			//tells that want interrupt given, not flag raised
	
	EnableInterrupts;

	for(;;)
	{	_FEED_COP(); /* feeds the dog */	}
}

//interrupt should trigger once every 0.25ms
#pragma TRAP_PROC
#pragma CODE_SEG __SHORT_SEG NON_BANKED
interrupt VectorNumber_Vtimmdcu void mdcuInterrupt () 
{
	static unsigned char LeftMotorCount = 0;
	static unsigned char RightMotorCount = 0;
	static unsigned char LeftIndex = 7;
	static unsigned char RightIndex = 0;
	static const char LeftSpeed = 11;		//3 is the fastest stable speed
	static char RightSpeed = 11;
	
	static float error = 0;			//represents our distance from desired location
	static float errorDot = 0;		//represents our change in distance from last reading
	
	/*	assigning kp a large value seems to send the bot into confusion, drives in circles and and apparent random motion
	0.1 seems to be a good value.	*/
	static const float kp = 0.005;
	
	/*	assigning kd a larger value seems to cause the bot to overreact to a change in distance
	even at value 4 it starts to go into a snake like motion	*/
	static const float kd = 3;
	
	//LeftStepArray contains half-step config to rotate left motor counter-clockwise the left motor control signals are at the lower 4-bits
	static unsigned char LeftStepArray[] = { 0x0C, 0x08, 0x09, 0x01, 0x03, 0x02, 0x06, 0x04 };//{ 0x05, 0x01, 0x09, 0x08, 0x0A, 0x02, 0x06, 0x04 };

	//RightStepArray contains half-step config to rotate right motor clockwise the right motor control signals are at the upper 4-bits
	static unsigned char RightStepArray[] = { 0xC0, 0x80, 0x90, 0x10, 0x30, 0x20, 0x60, 0x40 };//{ 0x50, 0x10, 0x90, 0x80, 0xA0, 0x20, 0x60, 0x40 };
	
	MCFLG |= 0x80;
	
	error = 80 - Output[OutputIndex];		//represents our error (distance from where we want to be)
	errorDot = Output[OutputIndex] - Output[(OutputIndex - 1 == -1? 1 : 0)];	//represents change in position over last two readings
	
	RightSpeed = 11 + (int)(kp * error - kd * errorDot);
	
	if (RightSpeed > 18)
		RightSpeed = 18;
	else if (RightSpeed < 4)
		RightSpeed = 4;
	
	if (LeftMotorCount > LeftSpeed)
	{
		LeftMotorCount = 0;
		LeftIndex -= 1;
		
		if (LeftIndex == 0)
			LeftIndex = 7;
	}
	else
		LeftMotorCount += 1;
	
	if (RightMotorCount > RightSpeed)
	{
		RightMotorCount = 0;
		RightIndex += 1;
		
		if (RightIndex == 7)
			RightIndex = 0;
		
	}
	else
		RightMotorCount += 1;
	
	PORTB = LeftStepArray[LeftIndex] + RightStepArray[RightIndex];
}

//triggered every 49.15ms
interrupt VectorNumber_Vrti void rtiInterrupt()
{
	static int rti_count = 0;
	static int on = 0;
	// ALWAYS CLEAR THE FLAG FIRST!
		//vars used for IIR filter
	static float A[2] = /*{ -1.561, 0.64135 };*/ { -1.7786, 0.8008}; /*{ -1.9556, 0.95654 };*/
	static float B[3] = /* { 0.020083, 0.040167, 0.020083};*/ { 0.0055427, 0.011085, 0.0055427 }; /* { 0.00024136, 0.00048272, 0.00024136 };*/
	static float D[3] = { 0 };
	
	static unsigned short RunCount = 0;
    
	CRGFLG |= 0x80; 
	
	if (RunCount == 1)
	{
		RunCount = 0;
		
		D[2] = D[1];
		D[1] = D[0];
		
		D[0] = ATD0DR0H;
		
		OutputIndex++;
		if (OutputIndex == 2)
			OutputIndex = 0;
		
		D[0] = D[0] - A[0] * D[1] - A[1] * D[2];
		Output[OutputIndex] = B[0] * D[0] + B[1] * D[1] + B[2] * D[2];
	}
	else
		RunCount += 1;
}

void InitializeATD(void)
{
	ATD0CTL3 = 0b00001000;		//sequence length 8, non-FIFO, 
	ATD0CTL4 |= 0x80;			// 8-bit result

	ATD0CTL5 = 0b00100011;		// left justified, unsigned representation,
								// continuous scan,non-mult, stores result at ATD0DR0H
								// writing to this register starts conversion sequence 
								// We start at 3
	ATD0CTL2 = 0x80; 			// Turn ATD0 on
}