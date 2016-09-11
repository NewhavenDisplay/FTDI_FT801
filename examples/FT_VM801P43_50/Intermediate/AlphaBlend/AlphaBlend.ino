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
* @file                           AlphaBlend.ino
* @brief                          Sketch to display hello world on FT801.
								  Tested platform version: Arduino 1.0.4 and later
* @version                        0.1.0
* @date                           2014/17/05
*
*/


/* This application demonstrates the usage of FT801 library on VM801P43 platform */

/* Arduino standard includes */
#include "SPI.h"
#include "Wire.h"

/* Platform specific includes */
#include "FT_VM801P43_50.h"

/* Global object for FT801 Implementation */
FT801IMPL_SPI FTImpl(FT_CS_PIN,FT_PDN_PIN,FT_INT_PIN);

/* Api to bootup FT801, verify FT801 hardware and configure display/audio pins */
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

/* API to display characters and points with alpha blend functionality */
void AlphaBlend()
{
	/* Change the below string for experimentation */
	const char Display_string[12] = "Hello World";
	uint8_t i;
	int16_t hoffset,voffset;
	
	/* Display list to display "Hello World" at the centre of display area */
	FTImpl.DLStart();//start the display list. Note DLStart and DLEnd are helper apis, Cmd_DLStart() and Display() can also be utilized.
	FTImpl.ColorRGB(0xFF,0xFF,0xFF);//set the color of the string to while color

	/* display list that demonstrates transparency */
	FTImpl.Begin(FT_BITMAPS);
	FTImpl.Vertex2ii(50, 30, 31, 'G');//character G with alpha transparency being 255
	FTImpl.ColorA(128);
	FTImpl.Vertex2ii(58, 38, 31, 'G');//character G with alpha transparency being 128
	FTImpl.ColorA(64);
	FTImpl.Vertex2ii(66, 46, 31, 'G');//character G with alpha transparency being 64
	FTImpl.End();//end the bitmap primitive

	/* display list that demonstrates alpha blend functionality - additive blending */
	/* draw 20 points at random locations within second half of the screen */
	FTImpl.ColorRGB(20, 91,20); // green color for additive blending
	FTImpl.BlendFunc(FT_SRC_ALPHA,FT_ONE);//input is source alpha and destination is whole color
	FTImpl.PointSize(30*16);	//set the point radius to be 30 pixels
	FTImpl.Begin(FT_POINTS);
	/* compute the coordinates of the points randomly */
	for(i=0;i<20;i++)
	{
		hoffset = FT_DISPLAYWIDTH/2 + random(FT_DISPLAYWIDTH/2);
		voffset = random(FT_DISPLAYHEIGHT);
		FTImpl.Vertex2f(hoffset*16,voffset*16);
	}
	FTImpl.End();//end the points primitive
	
	FTImpl.DLEnd();//end the display list
	FTImpl.Finish();//render the display list and wait for the completion of the DL
}

/* bootup the module and display transparency and additive blending of points on screen */
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
		AlphaBlend();
	}
	Serial.println("--End Application--");
}

/* Nothing in loop api */
void loop()
{
}
