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
* @file                           Bitmaps.ino
* @brief                          Sketch to display bitmaps on FT801.
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

/* Display lena face at different locations with various colors, -ve offsets, alpha blend etc */
void Bitmaps()
{
	Bitmap_header_t *p_bmhdr;
	int16_t BMoffsetx,BMoffsety;

	p_bmhdr = (Bitmap_header_t *)&Lena40_Header[0];
	/* Copy raw data into address 0 followed by generation of bitmap */
	FTImpl.Writefromflash(FT_RAM_G,&Lena40_RawData[p_bmhdr->Arrayoffset], p_bmhdr->Stride*p_bmhdr->Height);

	/* Display list to demonstrate the display of lena bitmap at various co ordinates and various colors */
	FTImpl.DLStart();//start the display list	
	FTImpl.BitmapSource(FT_RAM_G);//set the source address of bitmap to starting address of graphis ram
	FTImpl.BitmapLayout(p_bmhdr->Format, p_bmhdr->Stride, p_bmhdr->Height);//set the layout of the bitmap
	FTImpl.BitmapSize(FT_NEAREST,FT_BORDER,FT_BORDER, p_bmhdr->Width, p_bmhdr->Height);//set the bitmap size
	FTImpl.Begin(FT_BITMAPS); // start drawing bitmaps
	
	FTImpl.ColorRGB(255,255,255);//set the color of the bitmap to white
	BMoffsetx = ((FT_DISPLAYWIDTH/4) - (p_bmhdr->Width/2));
	BMoffsety = ((FT_DISPLAYHEIGHT/2) - (p_bmhdr->Height/2));
	FTImpl.Vertex2ii(BMoffsetx, BMoffsety, 0, 0);
	
	FTImpl.ColorRGB(255, 64, 64); // set the color of the bitmap
	BMoffsetx = ((FT_DISPLAYWIDTH*2/4) - (p_bmhdr->Width/2));
	BMoffsety = ((FT_DISPLAYHEIGHT/2) - (p_bmhdr->Height/2));
	FTImpl.Vertex2ii(BMoffsetx, BMoffsety, 0, 0);
	
	FTImpl.ColorRGB(64, 180, 64); // set the color of the bitmap
	BMoffsetx += (p_bmhdr->Width/2);
	BMoffsety += (p_bmhdr->Height/2);
	FTImpl.Vertex2ii(BMoffsetx, BMoffsety, 0, 0);
	
	FTImpl.ColorRGB(255, 255, 64); // transparent yellow at (232, 152)
	FTImpl.ColorA(150);
	BMoffsetx += (p_bmhdr->Width/2);
	BMoffsety += (p_bmhdr->Height/2);
	FTImpl.Vertex2ii(BMoffsetx, BMoffsety, 0, 0);
	
	FTImpl.ColorA(255);//reset the alpha to 255
	FTImpl.ColorRGB(255,255,255);
	FTImpl.Vertex2f(-10*16, -10*16);//for -ve coordinates use vertex2f instruction
	FTImpl.End();
	
	FTImpl.DLEnd();//end the display list
	FTImpl.Finish();//render the display list and wait for the completion of the DL
}

/* bootup the module and display bitmaps on screen */
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
		Bitmaps();
	}
	Serial.println("--End Application--");
}

/* Nothing in loop api */
void loop()
{
}
