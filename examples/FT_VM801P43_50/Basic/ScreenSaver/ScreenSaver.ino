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
* @file                           ScreenSaver.ino
* @brief                          This file demonstrates the usage of screen saver widget.
								Tested platform version: Arduino 1.0.4 and later
* @version                        0.1.0
* @date                           2014/21/03
*
*/


/* This application demonstrates the usage of FT801 library on VM801P43 platform */

/* Arduino standard includes */
#include "SPI.h"
#include "Wire.h"

/* Platform specific includes */
#include "FT_VM801P43_50.h"

/* Application specific includes */
#include "Lena_RawData.h"

/* Global object for FT801 Implementation */
FT801IMPL_SPI FTImpl(FT_CS_PIN,FT_PDN_PIN,FT_INT_PIN);

/* Lena face bitmap raw data and respective structure */
typedef struct Bitmap_header
{
	uint8_t Format;
	int16_t Width;
	int16_t Height;
	int16_t Stride;
	int32_t Arrayoffset;
}Bitmap_header_t;

Bitmap_header_t Lena40_Header[] = 
{
	/* format,width,height,stride,arrayoffset */
	{FT_RGB565,	40,	40,	40*2,	0	},
};
/* Macro for size of raw data */
#define Lena40_SIZE (2769)

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
/* API to demonstrate screen saver widget - playing of bitmap via macro0 */
void Screensaver()
{
	/*************************************************************************/
	/* Below code demonstrates the usage of screensaver function. Screen     */
	/* saver modifies macro0 with the vertex2f command.                      */
	/* MCU can display any static display list at the background with the    */
	/* changing bitmap                                                       */
	/*************************************************************************/

	Bitmap_header_t *p_bmhdr;
	int16_t BMoffsetx,BMoffsety;

	p_bmhdr = (Bitmap_header_t *)&Lena40_Header[0];
	
	/* Download the bitmap data */		
	FTImpl.Writefromflash( FT_RAM_G,(uint8_t *)&Lena40_RawData[p_bmhdr->Arrayoffset],p_bmhdr->Stride*p_bmhdr->Height);
	
	/* Send command screen saver */
	FTImpl.Cmd_ScreenSaver();//screen saver command will continuously update the macro0 with vertex2f command
	/* Copy the display list */
	FTImpl.DLStart(); //start the display list. Note DLStart and DLEnd are helper apis, Cmd_DLStart() and Display() can also be utilized.
	FTImpl.ClearColorRGB(0,0,0x80); // set the Clear color to blue
	FTImpl.Clear(1,1,1);
	FTImpl.ColorRGB(0xff,0xff,0xff);//set the color of the bitmap to white
	
	/* lena bitmap */
	FTImpl.Cmd_LoadIdentity();
	FTImpl.Cmd_Scale( 3*65536,3*65536);//scale the bitmap 3 times on both sides
	FTImpl.Cmd_SetMatrix( );
	FTImpl.Begin(FT_BITMAPS); // start drawing bitmaps
	FTImpl.BitmapSource(0); //set the source address of bitmap to starting address of graphis ram
	FTImpl.BitmapLayout(p_bmhdr->Format,p_bmhdr->Stride, p_bmhdr->Height); //set the layout of the bitmap
	FTImpl.BitmapSize(FT_BILINEAR,FT_BORDER,FT_BORDER,p_bmhdr->Width*3,p_bmhdr->Height*3); //set the bitmap size
	FTImpl.Macro(0);
	FTImpl.End();
	
	/* Display the text */
	FTImpl.Cmd_LoadIdentity();
	FTImpl.Cmd_SetMatrix( );
	FTImpl.Cmd_Text((FT_DISPLAYWIDTH/2), (FT_DISPLAYHEIGHT/2), 27, FT_OPT_CENTER, "Screen Saver ...");
	FTImpl.DLEnd(); //end the display list
	FTImpl.Finish(); //render the display list and wait for the completion of the DL
	delay(3*1000); //sleep for 3 seconds
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
		Screensaver();
	}
	Serial.println("--End Application--");
}

/* Nothing in loop api */
void loop()
{
}
