/*****************************************************************************
* Copyright (c) Future Technology Devices International 2014
* propriety of Future Technology devices International.
*
* Software License Agreement
*
* This code is provided as an example only and is not guaranteed by FTDI. 
* FTDI accept no responsibility for any issues resulting from its use. 
* The developer of the final application incorporating any parts of this 
* sample project is responsible for ensuring its safe and correct operation 
* and for any consequences resulting from its use.
*****************************************************************************/
/**
* @file                           Primitives.ino
* @brief                          Sketch to display graphics primitives on FT801.
								  Tested platform version: Arduino 1.0.4 and later
* @version                        0.1.0
* @date                           2014/17/05
*
*/


/* This application demonstrates the usage of FT801 library on VM801B43 platform */

/* Arduino standard includes */
#include "SPI.h"
#include "Wire.h"

/* Platform specific includes */
#include "FT_VM801B43.h"

/* Global object for FT801 Implementation */
FT801IMPL_SPI FTImpl(FT_CS_PIN,FT_PDN_PIN,FT_INT_PIN);

/* Api to bootup ft801, verify FT801 hardware and configure display/audio pins */
/* Returns 0 in case of success and 1 in case of failure */
int16_t BootupConfigure()
{
	uint32_t chipid = 0;
	FTImpl.Init(FT_DISPLAY_RESOLUTION);//configure the display to the WQVGA

	delay(20);//for safer side
	chipid = FTImpl.Read32(FT_ROM_CHIPID);
	
	/* Identify the chip */
	if(FT801_CHIPID != chipid)
	{
		Serial.print("Error in chip id read ");
		Serial.println(chipid,HEX);
		return 1;
	}
	
	/* Set the Display & audio pins */
	FTImpl.SetDisplayEnablePin(FT_DISPENABLE_PIN);
    FTImpl.SetAudioEnablePin(FT_AUDIOENABLE_PIN); 
	FTImpl.DisplayOn(); 
	FTImpl.AudioOn();  			
	return 0;
}

/* API to display points, lines and rectangles on the screen */
void Primitives()
{
	uint16_t LineHeight,RectWidth,RectHeight;
	/* Change the below display list for experimentation */
	
	/* Display list to display "Hello World" at the centre of display area */
	FTImpl.DLStart();//start the display list. Note DLStart and DLEnd are helper apis, Cmd_DLStart() and Display() can also be utilized.
	
	/* display list for drawing point */
	FTImpl.ColorRGB(0xFF,0x00,0x00);//set the color of the point to red
	FTImpl.PointSize(15 * 16);//set the size of the point to 15 pixels
	FTImpl.Begin(FT_POINTS);//begin points primitive
	FTImpl.Vertex2f((FT_DISPLAYWIDTH/5) * 16, (FT_DISPLAYHEIGHT/3) * 16);//display the point at (x,y) co ordinates
	FTImpl.End();//end points primitive
	
	/* display list for drawing line */
	FTImpl.ColorRGB(0x00,0xFF,0x00);//set the color of the line to green
	FTImpl.LineWidth(7 * 16);//set the line width to 7 pixels - effective line width will be 7*2 = 14 pixles
	FTImpl.Begin(FT_LINES);//begin lines primitive
	LineHeight = 40;//set the line height to be 40 pixels - effective line height will be 40 + linewidth*2
	FTImpl.Vertex2f((FT_DISPLAYWIDTH*2/5) * 16, ((FT_DISPLAYHEIGHT - LineHeight)/3) * 16);	//starting coordinates
	FTImpl.Vertex2f((FT_DISPLAYWIDTH*2/5) * 16, ((FT_DISPLAYHEIGHT + LineHeight)/3) * 16);  //ending coordinates	
	FTImpl.End();//end lines primitive

	/* display list for drawing rectangle */
	FTImpl.ColorRGB(0x00,0x00,0xFF);//set the color of the rectangle to blue
	FTImpl.LineWidth(1 * 16);//set the line width to 1 pixels - corner are 1 pixel radius
	FTImpl.Begin(FT_RECTS);//begin lines primitive	
	RectWidth = 40;RectHeight = 10;//set the rectangle width and height to 40x10 - effective dimension will be 42x12
	FTImpl.Vertex2f(((FT_DISPLAYWIDTH*3/5)-(RectWidth/2)) * 16, ((FT_DISPLAYHEIGHT-RectHeight)/3) * 16);	//top left coordinates
	FTImpl.Vertex2f(((FT_DISPLAYWIDTH*3/5)+(RectWidth/2)) * 16, ((FT_DISPLAYHEIGHT+RectHeight)/3) * 16);	//bottom right coordinates
	FTImpl.End();//end rectangle primitive

	/* display list for edge strip */
	FTImpl.ColorRGB(0x00,0xFF,0xFF);//set the color of the edge strip
	FTImpl.Begin(FT_EDGE_STRIP_R);//begin lines primitive	
	FTImpl.Vertex2f(((FT_DISPLAYWIDTH*4/5)-(LineHeight/2)) * 16, ((FT_DISPLAYHEIGHT-LineHeight)/3) * 16);	
	FTImpl.Vertex2f(((FT_DISPLAYWIDTH*4/5)+(LineHeight/2)) * 16, ((FT_DISPLAYHEIGHT+LineHeight)/3) * 16);
	FTImpl.End();//end edge strip primitive

	/* display list for line strip */
	FTImpl.ColorRGB(0xFF,0xFF,0xFF);//set the color of the line strip to white
	FTImpl.Begin(FT_LINE_STRIP);//begin lines primitive	
	FTImpl.Vertex2f(((FT_DISPLAYWIDTH/5)-(LineHeight/2)) * 16, ((FT_DISPLAYHEIGHT-LineHeight)*2/3) * 16);	//starting coordinates
	FTImpl.Vertex2f(((FT_DISPLAYWIDTH/5)-(LineHeight/2)) * 16, ((FT_DISPLAYHEIGHT+LineHeight)*2/3) * 16);
	FTImpl.Vertex2f(((FT_DISPLAYWIDTH/5)+(LineHeight/2)) * 16, ((FT_DISPLAYHEIGHT-LineHeight)*2/3) * 16);	
	FTImpl.Vertex2f(((FT_DISPLAYWIDTH/5)+(LineHeight/2)) * 16, ((FT_DISPLAYHEIGHT+LineHeight)*2/3) * 16);	//ending coordinates
	FTImpl.End();//end line strip primitive

	/* display list for character with font handle 30 */
	FTImpl.ColorRGB(0xFF,0xFF,0xFF);//set the color of the character to white
	FTImpl.Begin(FT_BITMAPS);//begin bitmaps primitive	
	FTImpl.Vertex2ii((FT_DISPLAYWIDTH*2/5), (FT_DISPLAYHEIGHT*2/3),30,'A');	//coordinates to display 'A' font 30
	FTImpl.End();//end line bitmaps primitive
	
	/* Display list for scissor */
	FTImpl.ScissorXY((FT_DISPLAYWIDTH*3/5)-(LineHeight/2), ((FT_DISPLAYHEIGHT-LineHeight)*2/3)); // top left co ordinates of Scissor
	FTImpl.ScissorSize(LineHeight*2, LineHeight); // Scissor rectangle is 80 x 40 pixels
	FTImpl.ClearColorRGB(0xFF, 0xFF, 0);// set the Clear color to yellow	
	FTImpl.Clear(1,0,0);//clear the color components of the background
	
	FTImpl.DLEnd();//end the display list
	FTImpl.Finish();//render the display list and wait for the completion of the DL
}

/* bootup the module and display primitives on screen */
void setup()
{
	/* Initialize serial print related functionality */
	Serial.begin(9600);
	
	/* Set the Display Enable pin*/   
	Serial.println("--Start Application--");
	if(BootupConfigure())
	{
		//error case - do not do any thing
	}
  	else
	{
		Primitives();
	}
	Serial.println("--End Application--");
}

/* Nothing in loop api */
void loop()
{
}
