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
* @file                           Snapshot.ino
* @brief                          This file demonstrates the usage of snapshot function.
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

/* API to perform display fadeout effect by changing the display PWM from 100 till 0*/
void fadeout()
{
    int32_t i;
	
	for (i = 100; i >= 0; i -= 3) 
	{
		FTImpl.Write(REG_PWM_DUTY,i);

		delay(2);//sleep for 2 ms
	}
}

/* API to perform display fadein effect by changing the display PWM from 0 till 100 and finally 128 */
void fadein()
{
	int32_t i;
	
	for (i = 0; i <=100 ; i += 3) 
	{
		FTImpl.Write(REG_PWM_DUTY,i);
		delay(2);//sleep for 2 ms
	}
	/* Finally make the PWM 100% */
	i = 128;
	FTImpl.Write(REG_PWM_DUTY,i);
}

void Snapshot()
{
/*************************************************************************/
	/* Below code demonstrates the usage of snapshot function. Snapshot func */
	/* captures the present screen and dumps into bitmap with color formats  */
	/* argb4.                                                                */
	/*************************************************************************/
	{
	uint16_t WriteByte = 0;

	/* fadeout before switching off the pclock */
	fadeout();
	/* Switch off the lcd */
	{
		uint8_t n = 0;
		FTImpl.Write( REG_GPIO, 0x7f);
		delay(100);
	}
	/* Disable the pclock */
	FTImpl.Write( REG_PCLK,WriteByte);
	/* Configure the resolution to 160x120 dimention */
	WriteByte = 160;
	FTImpl.Write16( REG_HSIZE,WriteByte);
	WriteByte = 120;
	FTImpl.Write16( REG_VSIZE,WriteByte);

	/* Construct screen shot for snapshot */
	
	FTImpl.DLStart(); //start the display list. Note DLStart and DLEnd are helper apis, Cmd_DLStart() and Display() can also be utilized.
	FTImpl.ClearColorRGB(0,0,0); // set the Clear color to black
	FTImpl.Clear(1,1,1);    //clear the color component
	FTImpl.ColorRGB(28,20,99); //set the bitmap(points) color to blue
	/* captured snapshot */	
	FTImpl.Begin(FT_POINTS); // start drawing points
	FTImpl.ColorA(128); //set the alpha to 128
	FTImpl.PointSize(20*16);  //set point size
	FTImpl.Vertex2f(0*16,0*16); //set x,y co-ordinates
	FTImpl.PointSize(25*16);//set point size
	FTImpl.Vertex2f(20*16,10*16); //set x,y co-ordinates
	FTImpl.PointSize(30*16);//set point size
	FTImpl.Vertex2f(40*16,20*16); //set x,y co-ordinates
	FTImpl.PointSize(35*16);//set point size
	FTImpl.Vertex2f(60*16,30*16); //set x,y co-ordinates
	FTImpl.PointSize(40*16); //set point size
	FTImpl.Vertex2f(80*16,40*16); //set x,y co-ordinates
	FTImpl.PointSize(45*16); //set point size
	FTImpl.Vertex2f(100*16,50*16); //set x,y co-ordinates
	FTImpl.PointSize(50*16); //set point size
	FTImpl.Vertex2f(120*16,60*16); //set x,y co-ordinates
	FTImpl.PointSize(55*16); //set point size
	FTImpl.Vertex2f(140*16,70*16); //set x,y co-ordinates
	FTImpl.PointSize(60*16); //set point size
	FTImpl.Vertex2f(160*16,80*16); //set x,y co-ordinates
	FTImpl.PointSize(65*16); //set point size
	FTImpl.Vertex2f(0*16,120*16); //set x,y co-ordinates
	FTImpl.PointSize(70*16); //set point size
	FTImpl.Vertex2f(160*16,0*16); //set x,y co-ordinates
	FTImpl.End();//display the bitmap at the center of the display
	FTImpl.ColorA(255); //reset the alpha to 255
	/* Display the text */	
	FTImpl.ColorRGB(32,32,32); 
	FTImpl.Cmd_Text(80, 60, 26, FT_OPT_CENTER, "Points");

	/* Download the commands into fifo */
	FTImpl.DLEnd(); //end the display list
	FTImpl.Finish(); //render the display list and wait for the completion of the DL
	delay(100);//timeout for snapshot to be performed by coprocessor

	/* Take snap shot of the current screen */
	FTImpl.WriteCmd(CMD_SNAPSHOT);	
	FTImpl.WriteCmd( 3200);//store the rgb content at location 3200

	//timeout for snapshot to be performed by coprocessor

	/* Wait till coprocessor completes the operation */
	FTImpl.Finish();

	delay(100);//timeout for snapshot to be performed by coprocessor

	/* reconfigure the resolution wrt configuration */
	WriteByte = FT_DISPLAYWIDTH;
	FTImpl.Write16( REG_HSIZE,WriteByte);
	WriteByte = FT_DISPLAYHEIGHT;
	FTImpl.Write16( REG_VSIZE,WriteByte);
	
	FTImpl.DLStart(); //start the display list. Note DLStart and DLEnd are helper apis, Cmd_DLStart() and Display() can also be utilized.
	FTImpl.ClearColorRGB(0,0,0); // set the Clear color to black
	FTImpl.Clear(1,1,1);    //clear the color component
	FTImpl.ColorRGB(0xff,0xff,0xff); //set color to white
	/* captured snapshot */	
	FTImpl.Begin(FT_BITMAPS);  // start drawing bitmaps
	FTImpl.BitmapSource(3200); //set the source address of bitmap 
	FTImpl.BitmapLayout(FT_ARGB4,160*2,120);  //set the layout of the bitmap
	FTImpl.BitmapSize(FT_BILINEAR,FT_BORDER,FT_BORDER,160,120); //set the bitmap size
	FTImpl.Vertex2f(((FT_DISPLAYWIDTH - 160)/2)*16,((FT_DISPLAYHEIGHT - 120)/2)*16);
	FTImpl.End();//display the bitmap at the center of the display
	/* Display the text info */
	FTImpl.ColorRGB(32,32,32);
	FTImpl.Cmd_Text((FT_DISPLAYWIDTH/2), 40, 27, FT_OPT_CENTER, "Snap shot");

	FTImpl.DLEnd(); //end the display list
	FTImpl.Finish(); //render the display list and wait for the completion of the DL
	
	/* reenable the pclock */
	WriteByte = FT_DISPLAY_PCLK_WQVGA;
	FTImpl.Write( REG_PCLK,WriteByte);
	delay(60);
	/* Power on the LCD */
	{
		uint8_t n = 0x80;
		FTImpl.Write( REG_GPIO,0xff);		
	}

	delay(200);//give some time for the lcd to switchon - hack for one perticular panel
	/* set the display pwm back to 128 */
	{
		fadein();
	}
     delay(1000);
	}
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
		Snapshot();
	}
	Serial.println("--End Application--");
}

/* Nothing in loop api */
void loop()
{
}