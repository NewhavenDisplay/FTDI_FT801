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
* @file                           Track.ino
* @brief                          This file demonstrates the usage of track function.
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

/* Sample app api to demonstrate track widget funtionality */
void CoPro_Track()
{
	uint8_t *pbuff;
	uint32_t NumBytesGen = 0;
	uint16_t CurrWriteOffset = 0;

	/*************************************************************************/
	/* Below code demonstrates the usage of track function. Track function   */
	/* tracks the pen touch on any specific object. Track function supports  */
	/* rotary and horizontal/vertical tracks. Rotary is given by rotation    */
	/* angle and horizontal/vertucal track is offset position.               */
	/*************************************************************************/
	{
	int32_t LoopFlag = 0;
	uint32_t TrackRegisterVal = 0;
	uint16_t angleval = 0,slideval = 0,scrollval = 0;

	/* Set the tracker for 3 bojects */
	FTImpl.Cmd_Track( FT_DISPLAYWIDTH/2, FT_DISPLAYHEIGHT/2, 1,1, 10);
	FTImpl.Cmd_Track( 40, (FT_DISPLAYHEIGHT - 40), (FT_DISPLAYWIDTH - 80),8, 11);
	FTImpl.Cmd_Track( (FT_DISPLAYWIDTH - 40), 40, 8,(FT_DISPLAYHEIGHT - 80), 12);
	/* Download the commands into fifo */
	FTImpl.Finish();

	LoopFlag = 600;
	/* update the background color continuously for the color change in any of the trackers */
	while(LoopFlag--)
	{
		uint8_t tagval = 0;
		TrackRegisterVal = FTImpl.Read32(REG_TRACKER);
		tagval = TrackRegisterVal & 0xff;
		if(0 != tagval)
		{
			if(10 == tagval)
			{
				angleval = TrackRegisterVal>>16;
			}
			else if(11 == tagval)
			{
				slideval = TrackRegisterVal>>16;
			}
			else if(12 == tagval)
			{
				scrollval = TrackRegisterVal>>16;				
				if((scrollval + 65535/10) > (9*65535/10))
				{
					scrollval = (8*65535/10);
				}
				else if(scrollval < (1*65535/10))
				{
					scrollval = 0;
				}
				else
				{
					scrollval -= (1*65535/10);
				}
			}
		}
		/* Display a rotary dial, horizontal slider and vertical scroll */
		FTImpl.DLStart();	//start the display list. Note DLStart and DLEnd are helper apis, Cmd_DLStart() and Display() can also be utilized.
		{
                int32_t tmpval0,tmpval1,tmpval2;
				uint8_t angval,sldval,scrlval;

                tmpval0 = (int32_t)angleval*255/65536;
                tmpval1 = (int32_t)slideval*255/65536;
                tmpval2 = (int32_t)scrollval*255/65536;
                        
                angval = tmpval0&0xff;
                sldval = tmpval1&0xff;
                scrlval = tmpval2&0xff;
				FTImpl.ClearColorRGB(angval,sldval,scrlval); // set the Clear color 
		}
		
		FTImpl.Clear(1,1,1);
		FTImpl.ColorRGB(0xff,0xff,0xff); // set the color to white

		/* Draw dial with 3d effect */
		FTImpl.Cmd_FGColor(0x00ff00);
		FTImpl.Cmd_BGColor(0x800000);
		FTImpl.Tag(10);
		FTImpl.Cmd_Dial((FT_DISPLAYWIDTH/2), (FT_DISPLAYHEIGHT/2), (FT_DISPLAYWIDTH/8), 0, angleval);
		
		/* Draw slider with 3d effect */		
		FTImpl.Cmd_FGColor(0x00a000);
		FTImpl.Cmd_BGColor(0x800000);
		FTImpl.Tag(11);
		FTImpl.Cmd_Slider( 40, (FT_DISPLAYHEIGHT - 40), (FT_DISPLAYWIDTH - 80),8, 0, slideval, 65535);

		/* Draw scroll with 3d effect */		
		FTImpl.Cmd_FGColor(0x00a000);
		FTImpl.Cmd_BGColor( 0x000080);
		FTImpl.Tag(12);
		FTImpl.Cmd_Scrollbar((FT_DISPLAYWIDTH - 40), 40, 8, (FT_DISPLAYHEIGHT - 80), 0, scrollval, (65535*0.2), 65535);
		
		FTImpl.TagMask(0);
		FTImpl.ColorRGB(0xff,0xff,0xff);
		FTImpl.Cmd_Text((FT_DISPLAYWIDTH/2), ((FT_DISPLAYHEIGHT/2) + (FT_DISPLAYWIDTH/8) + 8), 26, FT_OPT_CENTER, "Rotary track");
		FTImpl.Cmd_Text(((FT_DISPLAYWIDTH/2)), ((FT_DISPLAYHEIGHT - 40) + 8 + 8), 26, FT_OPT_CENTER, "Horizontal track");
		FTImpl.Cmd_Text((FT_DISPLAYWIDTH - 40), 20, 26, FT_OPT_CENTER, "Vertical track");
		FTImpl.DLEnd();
		FTImpl.Finish();
		delay(10);//sleep for 3 seconds
	}

	/* Set the tracker for 3 bojects */
	
	FTImpl.Cmd_Track( 240, 136, 0,0, 10);
	FTImpl.Cmd_Track( 40, 232, 0,0, 11);
	FTImpl.Cmd_Track( 400, 40, 0,0, 12);

	/* Download the commands into fifo */
	FTImpl.Finish();
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
		Calibrate();
		
	}
	Serial.println("--End Application--");
}

/* Nothing in loop api */
void loop()
{
	CoPro_Track();
}