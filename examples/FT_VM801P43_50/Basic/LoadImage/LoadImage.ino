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
* @file                           LoadImage.ino
* @brief                          Sketch to demonstrate useage of jpg decode functionality and display decoded bitmaps.
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

/* Macro for size of raw data */
#define GOLF_JPEGDATA_SIZE 	(4187)
#define FT_NUM_BITMAPCELLS 		(4)
/* Application specific includes */
#include "Golf_JpegData.h"

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

Bitmap_header_t Golf_Header[] = 
{
	/* format,width,height,stride,arrayoffset */
	{FT_RGB565,	68,	86,	68*2,	0	},
};


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

/* API to demonstrate the usage of Loadimage command - compression done via jpeg baseline profile */
void LoadImage()
{
	const Bitmap_header_t *pBitmapHdr = NULL;
	int16_t xoffset,yoffset;
	
	/*************************************************************************/
	/* Below code demonstrates the usage of LoadImage functionality          */
	/* Download the jpeg encoded data into command buffer and in turn 		 */
	/* coprocessor decodes the jpeg data and output at 0 location			 */
	/*************************************************************************/

	pBitmapHdr = &Golf_Header[0];
	
	/* Calculate the screen centre offset based on image dimension */
	xoffset = ((FT_DISPLAYWIDTH - pBitmapHdr->Width*FT_NUM_BITMAPCELLS)/2);
	yoffset = ((FT_DISPLAYHEIGHT - pBitmapHdr->Height)/2);

	/* Clear the memory at location 0 - any previous junk or bitmap data */	
	FTImpl.Cmd_Memset(0L,255L,1L*pBitmapHdr->Stride*pBitmapHdr->Height*FT_NUM_BITMAPCELLS);

	/* Construction of display list to display background and set of golf bitmaps on the foreground */
	FTImpl.DLStart();//start the display list. Note DLStart and DLEnd are helper apis, Cmd_DLStart() and Display() can also be utilized.
	FTImpl.ClearColorRGB(0,255,255);//clear the background color
	FTImpl.Clear(1,0,0);//clear the color component
	FTImpl.ColorRGB(255,255,255);//set the bitmap color to White
	
	/* Assign the bitmap parameters and display the bitmap at (xoffset,yoffset) coordinates */
	FTImpl.Begin(FT_BITMAPS);
	FTImpl.BitmapSource(0);
	FTImpl.BitmapLayout(pBitmapHdr->Format,pBitmapHdr->Stride,pBitmapHdr->Height);
	FTImpl.BitmapSize(FT_BILINEAR,FT_BORDER,FT_BORDER,pBitmapHdr->Width,pBitmapHdr->Height);
	FTImpl.Vertex2ii(xoffset,yoffset,0,0);//display Lena at (xoffset,yoffset) pixel coordinates
	xoffset += pBitmapHdr->Width;
	yoffset += 10;
	FTImpl.Vertex2ii(xoffset,yoffset,0,1);//display other cells
	xoffset += pBitmapHdr->Width;
	yoffset += 10;
	FTImpl.Vertex2ii(xoffset,yoffset,0,2);
	xoffset += pBitmapHdr->Width;
	yoffset += 10;
	FTImpl.Vertex2ii(xoffset,yoffset,0,3);
	FTImpl.End();

	/* Display of information string at (xoffset,yoffset) pixel coordinates */
	xoffset = (FT_DISPLAYWIDTH/2);
	yoffset = ((FT_DISPLAYHEIGHT - pBitmapHdr->Height - 20)/2);
	FTImpl.ColorRGB(0x000000);//set the color of the informative text to black
	FTImpl.Cmd_Text(xoffset, yoffset, 26, FT_OPT_CENTER, "Display bitmap by LoadImage");
	FTImpl.DLEnd();
	
	/* Wait till coprocessor completes the operation */
	FTImpl.Finish();//after this White color rectangle will be visible on the screen

	/* decode jpeg data into location 0 */
	FTImpl.Cmd_LoadImage(0,FT_OPT_NODL);//using option not to generate DL for jpeg image
	/* Copy the jpeg data into coprocessor fifo */
	FTImpl.WriteCmdfromflash(Golf_JpegData,1L*GOLF_JPEGDATA_SIZE);	
}
/* bootup the module and display golf bitmaps on screen using jpeg decode command */
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
		LoadImage();
	}
	Serial.println("--End Application--");
}

/* Nothing in loop api */
void loop()
{
}
