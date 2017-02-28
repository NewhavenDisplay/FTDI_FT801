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
* @file                           HelloWorld.ino
* @brief                          Sketch to display points based on stencil on FT801.
								  Tested platform version: Arduino 1.0.4 and later
* @version                        0.1.0
* @date                           2014/17/05
*
*/


/* This application demonstrates the usage of FT801 library on NHD_43CTP_SHIELD platform */

/* Arduino standard includes */
#include "SPI.h"
#include "Wire.h"

/* Platform specific includes */
#include "FT_NHD_43CTP_SHIELD.h"

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

/* API to display 2 points and draw third point using stencil feature on the screen */
void Stencil()
{
	/* Change the below string for experimentation */
	int16_t PointSize = FT_DISPLAYWIDTH/6,DispBtwPoints = FT_DISPLAYWIDTH/5;
	
	/* Display list to display 2 points directly and 1 point based on stencil check */
	FTImpl.DLStart();//start the display list. Note DLStart and DLEnd are helper apis, Cmd_DLStart() and Display() can also be utilized.
	
	FTImpl.StencilOp(FT_INCR,FT_INCR);//change the stencil operation to increment index based on spass or sfail	
	
	/* Draw 2 points to change the stencil buffer value */
	/* Note that stencil and alpha blending doesnt go through anti aliasing functionality */
	FTImpl.Begin(FT_POINTS);
	FTImpl.PointSize(PointSize * 16);
	FTImpl.Vertex2ii(((FT_DISPLAYWIDTH - DispBtwPoints)/2), FT_DISPLAYHEIGHT/2, 0, 0);
	FTImpl.Vertex2ii(((FT_DISPLAYWIDTH + DispBtwPoints)/2), FT_DISPLAYHEIGHT/2, 0, 0);
	FTImpl.Vertex2ii(FT_DISPLAYWIDTH/2, ((FT_DISPLAYHEIGHT - DispBtwPoints)/2), 0, 0);
	FTImpl.Vertex2ii(FT_DISPLAYWIDTH/2, ((FT_DISPLAYHEIGHT + DispBtwPoints)/2), 0, 0);
	
	FTImpl.PointSize(PointSize * 16 * 2);
	FTImpl.StencilOp(FT_KEEP,FT_KEEP);//change the stencil operation to increment index based on spass or sfail	
	FTImpl.StencilFunc(FT_EQUAL, 2, 255);//stencil condition check equal to 2	
	FTImpl.ColorRGB(128, 0, 0);
	FTImpl.Vertex2ii((FT_DISPLAYWIDTH/2), (FT_DISPLAYHEIGHT/2), 0, 0);//draw this point only if stencil value is equal to 2
	FTImpl.StencilFunc(FT_EQUAL, 3, 255);//stencil condition check equal to 2	
	FTImpl.ColorRGB(0, 128, 0);
	FTImpl.Vertex2ii((FT_DISPLAYWIDTH/2), (FT_DISPLAYHEIGHT/2), 0, 0);//draw this point only if stencil value is equal to 2
	FTImpl.StencilFunc(FT_EQUAL, 4, 255);//stencil condition check equal to 2	
	FTImpl.ColorRGB(0, 0, 128);
	FTImpl.Vertex2ii((FT_DISPLAYWIDTH/2), (FT_DISPLAYHEIGHT/2), 0, 0);//draw this point only if stencil value is equal to 2
	
	FTImpl.DLEnd();//end the display list
	FTImpl.Finish();//render the display list and wait for the completion of the DL
}
/* bootup the module and display "Hello World" on screen */
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
		Stencil();
	}
	Serial.println("--End Application--");
}

/* Nothing in loop api */
void loop()
{
}