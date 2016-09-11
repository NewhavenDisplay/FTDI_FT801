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
* @file                           Sketch.ino
* @brief                          This file demonstrates the usage of sketch function.
								  Tested platform version: Arduino 1.0.4 and later
* @version                        0.1.0
* @date                           2014/21/03
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

/*************************************************************************/
/* Below code demonstrates the usage of calibrate function. Calibrate    */
/* function will wait untill user presses all the three dots. Only way to*/
/* come out of this api is to reset the coprocessor bit.                 */
/*************************************************************************/
void Calibrate()
{
  FTImpl.DLStart(); //start the display list. Note DLStart and DLEnd are helper apis, Cmd_DLStart() and Display() can also be utilized.
  FTImpl.ClearColorRGB(64,64,64); 
  FTImpl.Clear(1,1,1);    
  FTImpl.ColorRGB(0xff, 0xff, 0xff);
  FTImpl.Cmd_Text((FT_DISPLAYWIDTH/2), (FT_DISPLAYHEIGHT/2), 27, FT_OPT_CENTER, "Please Tap on the dot"); //display string at the center of the screen using inbuilt font handle 29 
  FTImpl.Cmd_Calibrate(0);
  FTImpl.Finish(); //render the display list and wait for the completion of the DL
}

/*************************************************************************/
/* Below code demonstrates the usage of sketch function. Sketch function */
/* draws line for pen movement. Skecth supports L1 and L8 output formats */
/* Sketch draws till stop command is executed.                           */
/*************************************************************************/
void Sketch()
{
	int16_t BorderSz = 40;
    uint32_t MemZeroSz;
	
	/* Send command sketch */
	MemZeroSz = 1L*(FT_DISPLAYWIDTH - 2*BorderSz)*(FT_DISPLAYHEIGHT - 2*BorderSz);
	FTImpl.Cmd_Memzero(FT_RAM_G,MemZeroSz);        
	FTImpl.Cmd_CSketch( BorderSz,BorderSz,(FT_DISPLAYWIDTH - 2*BorderSz),(FT_DISPLAYHEIGHT - 2*BorderSz),0,FT_L1,1500);//sketch in L1 format
	/* Display the sketch */
	FTImpl.DLStart(); //start the display list. Note DLStart and DLEnd are helper apis, Cmd_DLStart() and Display() can also be utilized.
	FTImpl.ClearColorRGB(0x80,0,0); // set the Clear color to red
	FTImpl.Clear(1,1,1);    
	FTImpl.ColorRGB(0xff,0xff,0xff); //set the color of the rectangle to white
	FTImpl.ScissorSize((FT_DISPLAYWIDTH - 2*BorderSz),(FT_DISPLAYHEIGHT - 2*BorderSz)); // Scissor rectangle 
	FTImpl.ScissorXY(BorderSz,BorderSz);
	FTImpl.ClearColorRGB(0xff,0xff,0xff); // set the Clear color to white
	FTImpl.Clear(1,1,1);

	FTImpl.ScissorSize(512,512);
	FTImpl.ScissorXY(0,0);
	FTImpl.ColorRGB(0,0,0); //set the color of the drawing to black
	/* L1 bitmap display */
	FTImpl.Begin(FT_BITMAPS);// start drawing bitmaps
	FTImpl.BitmapSource(0); //set the source address of bitmap to starting address of graphis ram
	FTImpl.BitmapLayout(FT_L1,(FT_DISPLAYWIDTH - 2*BorderSz)/8,(FT_DISPLAYHEIGHT - 2*BorderSz)); //set the layout of the bitmap
	FTImpl.BitmapSize(FT_BILINEAR,FT_BORDER,FT_BORDER,(FT_DISPLAYWIDTH - 2*BorderSz),(FT_DISPLAYHEIGHT - 2*BorderSz)); //set the bitmap size
	FTImpl.Vertex2f(BorderSz*16,BorderSz*16);
	FTImpl.End();
	/* Display the text */	
	FTImpl.ColorRGB(0xff,0xff,0xff); 
	FTImpl.Cmd_Text((FT_DISPLAYWIDTH/2), 20, 27, FT_OPT_CENTER, "Sketch L1");
	FTImpl.DLEnd(); //end the display list
	FTImpl.Finish(); //render the display list and wait for the completion of the DL
	delay(5000);//sleep for 5 seconds

	/* Send the stop command */
	FTImpl.WriteCmd(CMD_STOP);
	/* Update the command buffer pointers - both read and write pointers */
	FTImpl.Finish();	

	/* Sketch L8 format */
	
	/* Send command sketch */
	
	FTImpl.Cmd_Memzero(FT_RAM_G,MemZeroSz);
	FTImpl.Cmd_CSketch( BorderSz,BorderSz,(FT_DISPLAYWIDTH - 2*BorderSz),(FT_DISPLAYHEIGHT - 2*BorderSz),0,FT_L8,1500);//sketch in L8 format
	/* Display the sketch */
	FTImpl.DLStart(); //start the display list. Note DLStart and DLEnd are helper apis, Cmd_DLStart() and Display() can also be utilized.
	FTImpl.ClearColorRGB(0x00,0,0x80); // set the Clear color to blue
	FTImpl.Clear(1,1,1);
	FTImpl.ColorRGB(0xff,0xff,0xff); //set the color of the rectangle to white
	FTImpl.ScissorSize((FT_DISPLAYWIDTH - 2*BorderSz),(FT_DISPLAYHEIGHT - 2*BorderSz));  // Scissor rectangle 
	FTImpl.ScissorXY(BorderSz,BorderSz);
	FTImpl.ClearColorRGB(0xff,0xff,0xff);
	FTImpl.Clear(1,1,1);

	FTImpl.ScissorSize(512,512);
	FTImpl.ScissorXY(0,0);
	FTImpl.ColorRGB(0,0,0); //set the color of the drawing to black
	/* L8 bitmap display */
	FTImpl.Begin(FT_BITMAPS); // start drawing bitmaps
	FTImpl.BitmapSource(0); //set the source address of bitmap to starting address of graphis ram
	FTImpl.BitmapLayout(FT_L8,(FT_DISPLAYWIDTH - 2*BorderSz),(FT_DISPLAYHEIGHT - 2*BorderSz)); //set the layout of the bitmap
	FTImpl.BitmapSize(FT_BILINEAR,FT_BORDER,FT_BORDER,(FT_DISPLAYWIDTH - 2*BorderSz),(FT_DISPLAYHEIGHT - 2*BorderSz)); //set the bitmap size
	FTImpl.Vertex2f(BorderSz*16,BorderSz*16);
	FTImpl.End();
	/* Display the text */	
	FTImpl.ColorRGB(0xff,0xff,0xff);
	FTImpl.Cmd_Text((FT_DISPLAYWIDTH/2), 20, 27, FT_OPT_CENTER, "Sketch L8");
	
	FTImpl.DLEnd(); //end the display list
	FTImpl.Finish(); //render the display list and wait for the completion of the DL

	delay(5000);//sleep for 5 seconds

	/* Send the stop command */
	FTImpl.WriteCmd(  CMD_STOP);
	/* Update the command buffer pointers - both read and write pointers */
	FTImpl.Finish(); 
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
		Calibrate();
	}
	Serial.println("--End Application--");
}

/* Nothing in loop api */
void loop()
{
	Sketch();
}