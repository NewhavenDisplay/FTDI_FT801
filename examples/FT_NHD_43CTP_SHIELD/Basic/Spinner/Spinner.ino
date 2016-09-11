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
* @file                           Spinner.ino
* @brief                          Sketch to display Spinner functionality on FT801.
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

/* API to display Spinner on the screen with informative string */
void Spinner(int16_t style, int16_t scale)
{
	/*************************************************************************/
	/* Below code demonstrates the usage of spinner function. Spinner func   */
	/* will wait untill stop command is sent from the mcu. Spinner has option*/
	/* for displaying points in circle fashion or in a line fashion.         */
	/*************************************************************************/	
	
	/* Construct the display list to display spinner on the screen with style and scale options */
	FTImpl.DLStart();
	FTImpl.ClearColorRGB(64,64,64);//set the background to grey color
	FTImpl.Clear(1,1,1);    
	FTImpl.ColorRGB(0xff, 0xff, 0xff);	
	FTImpl.Cmd_Text((FT_DISPLAYWIDTH/2), 20, 27, FT_OPT_CENTER, "Spinner.. Please Wait..");//display the informative string
	FTImpl.Cmd_Spinner((FT_DISPLAYWIDTH/2),(FT_DISPLAYHEIGHT/2),style,scale);//style 0 and scale 0
	
	//note that DLEnd() is not required as coprocessor inserts display list at the end and renders by itself in an animated fashion
	
	FTImpl.Finish();//render the commands to FT801
}
int8_t gscale = 0,gstyle = 0,ginit = 0;
/* bootup the module and display spinner on screen */
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
		ginit = 1;
		Serial.println("--Init Done--");
	}
}

/* Perform the spinner in loop */
void loop()
{
	//error handling
	if(1 == ginit)//operate the spinner only if FT801 is initialized
	{	
		/* perform the spinner each itteration */
		Spinner(gscale,gstyle);
		delay(3000);//delay of 3 seconds for the observation
		
		/* Send the stop command */
		FTImpl.WriteCmd( CMD_STOP);
		/* Update the command buffer pointers - both read and write pointers */
		FTImpl.Finish();
		delay(500);//delay of .5 seconds for the observation
		/* change the stype and scale */
		gscale++;
		if(gscale > 3)
		{
			gscale = 0;
			gstyle++;
			if(gstyle >1)
			{
				gstyle = 0;
			}
		}
	}
}
