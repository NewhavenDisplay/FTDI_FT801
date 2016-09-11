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
* @file                           BitmapTransform.ino
* @brief                          This file demonstrates the usage of scale, rotate and translate functionality.
								  Tested platform version: Arduino 1.0.4 and later
* @version                        0.1.0
* @date                           2014/21/03
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

/* API to demonstrate scale, rotate and translate functionality */
void BitmapTransform()
{
	/*************************************************************************/
	/* Below code demonstrates the usage of bitmap matrix processing apis.   */
	/* Mainly matrix apis consists if scale, rotate and translate.           */
	/* Units of translation and scale are interms of 1/65536, rotation is in */
	/* degrees and in terms of 1/65536. +ve theta is anticlock wise, and -ve  */
	/* theta is clock wise rotation                                          */
	/*************************************************************************/

	/* Lena image with 40x40 rgb565 is used as an example */	
	{
	int32_t imagewidth,imagestride,imageheight,imagexoffset,imageyoffset;
	Bitmap_header_t *p_bmhdr;
	int16_t BMoffsetx,BMoffsety;

	p_bmhdr = (Bitmap_header_t *)&Lena40_Header[0];
	
	/* Download the bitmap data */		
	FTImpl.Writefromflash(FT_RAM_G,(uint8_t *)&Lena40_RawData[p_bmhdr->Arrayoffset],p_bmhdr->Stride*p_bmhdr->Height);

	
	FTImpl.DLStart(); //start the display list. Note DLStart and DLEnd are helper apis, Cmd_DLStart() and Display() can also be utilized.
	FTImpl.ClearColorRGB(0xff,0xff,0xff); // set the Clear color to white
	FTImpl.Clear(1,1,1); //clear the color component
	FTImpl.ColorRGB(32,32,32); //set the bitmap color
	/*Display the text*/
	FTImpl.Cmd_Text(10, 5, 16, 0, "BM with rotation");
	FTImpl.Cmd_Text(10, 20 + 40 + 5, 16, 0, "BM with scaling");
	FTImpl.Cmd_Text(10, 20 + 40 + 20 + 80 + 5, 16, 0, "BM with flip");

	imagewidth = p_bmhdr->Width;
	imageheight = p_bmhdr->Height;
	imagestride = p_bmhdr->Stride;
	imagexoffset = 10*16;
	imageyoffset = 20*16;

	FTImpl.ColorRGB(0xff,0xff,0xff);  //set the bitmap color to white
	FTImpl.Begin(FT_BITMAPS); // start drawing bitmaps
	FTImpl.BitmapSource(0); //set the source address of bitmap to starting address of graphis ram
	FTImpl.BitmapLayout(p_bmhdr->Format,imagestride,imageheight); //set the layout of the bitmap
	FTImpl.BitmapSize(FT_BILINEAR,FT_BORDER,FT_BORDER,imagewidth,imageheight); //set the bitmap size
	/******************************************* Perform display of plain bitmap ************************************/
	FTImpl.Vertex2f(imagexoffset,imageyoffset); 
		
	/* Perform display of plain bitmap with 45 degrees anti clock wise and the rotation is performed on top left coordinate */
	imagexoffset += (imagewidth + 10)*16;
	FTImpl.Cmd_LoadIdentity();
	FTImpl.Cmd_Rotate( (-45*65536/360));//rotate by 45 degrees anticlock wise
	FTImpl.Cmd_SetMatrix( );
	FTImpl.Vertex2f(imagexoffset,imageyoffset);

	/* Perform display of plain bitmap with 30 degrees clock wise and the rotation is performed on top left coordinate */
	imagexoffset += (imagewidth*1.42 + 10)*16;//add the width*1.41 as diagonal is new width and extra 10 pixels
	FTImpl.Cmd_LoadIdentity();
	FTImpl.Cmd_Rotate( 30*65536/360);//rotate by 33 degrees clock wise
	FTImpl.Cmd_SetMatrix( );
	FTImpl.Vertex2f(imagexoffset,imageyoffset);

	/* Perform display of plain bitmap with 45 degrees anti clock wise and the rotation is performed wrt centre of the bitmap */
	imagexoffset += (imagewidth*1.42 + 10)*16;//add the width*1.41 as diagonal is new width and extra 10 pixels
	FTImpl.Cmd_LoadIdentity();
	FTImpl.Cmd_Translate( 65536*imagewidth/2,65536*imageheight/2);//make the rotation coordinates at the center
	FTImpl.Cmd_Rotate( -45*65536/360);//rotate by 45 degrees anticlock wise
	FTImpl.Cmd_Translate( -65536*imagewidth/2,-65536*imageheight/2);
	FTImpl.Cmd_SetMatrix( );
	FTImpl.Vertex2f(imagexoffset,imageyoffset);

	/* Perform display of plain bitmap with 45 degrees clock wise and the rotation is performed so that whole bitmap is viewable */
	imagexoffset += (imagewidth*1.42 + 10)*16;//add the width*1.41 as diagonal is new width and extra 10 pixels
	FTImpl.Cmd_LoadIdentity();
	FTImpl.Cmd_Translate( 65536*imagewidth/2,65536*imageheight/2);//make the rotation coordinates at the center
	FTImpl.Cmd_Rotate( 45*65536/360);//rotate by 45 degrees clock wise
	FTImpl.Cmd_Translate( -65536*imagewidth/10,-65536*imageheight/2);
	FTImpl.Cmd_SetMatrix( );
	FTImpl.BitmapSize(FT_BILINEAR,FT_BORDER,FT_BORDER,imagewidth*2,imageheight*2);//set the bitmap size
	FTImpl.Vertex2f(imagexoffset,imageyoffset);

	/* Perform display of plain bitmap with 90 degrees anti clock wise and the rotation is performed so that whole bitmap is viewable */
	imagexoffset += (imagewidth*2 + 10)*16;
	FTImpl.Cmd_LoadIdentity();
	FTImpl.Cmd_Translate( 65536*imagewidth/2,65536*imageheight/2);//make the rotation coordinates at the center
	FTImpl.Cmd_Rotate( -90*65536/360);//rotate by 90 degrees anticlock wise
	FTImpl.Cmd_Translate( -65536*imagewidth/2,-65536*imageheight/2);
	FTImpl.Cmd_SetMatrix( );
	FTImpl.BitmapSize(FT_BILINEAR,FT_BORDER,FT_BORDER,imagewidth,imageheight);//set the bitmap size
	FTImpl.Vertex2f(imagexoffset,imageyoffset);

	/* Perform display of plain bitmap with 180 degrees clock wise and the rotation is performed so that whole bitmap is viewable */
	imagexoffset += (imagewidth + 10)*16;
	FTImpl.Cmd_LoadIdentity();
	FTImpl.Cmd_Translate( 65536*imagewidth/2,65536*imageheight/2);//make the rotation coordinates at the center
	FTImpl.Cmd_Rotate( -180*65536/360);//rotate by 180 degrees anticlock wise
	FTImpl.Cmd_Translate( -65536*imagewidth/2,-65536*imageheight/2);
	FTImpl.Cmd_SetMatrix( );
	FTImpl.Vertex2f(imagexoffset,imageyoffset);
	/******************************************* Perform display of bitmap with scale ************************************/
	/* Perform display of plain bitmap with scale factor of 2x2 in x & y direction */
	imagexoffset = (10)*16;
	imageyoffset += (imageheight + 20)*16;
	FTImpl.Cmd_LoadIdentity();
	FTImpl.Cmd_Scale( 2*65536,2*65536);//scale by 2x2
	FTImpl.Cmd_SetMatrix( );
	FTImpl.BitmapSize(FT_BILINEAR,FT_BORDER,FT_BORDER,imagewidth*2,imageheight*2); //set the bitmap size
	FTImpl.Vertex2f(imagexoffset,imageyoffset);

	/* Perform display of plain bitmap with scale factor of .5x.25 in x & y direction, rotate by 45 degrees clock wise wrt top left */
	imagexoffset += (imagewidth*2 + 10)*16;
	FTImpl.Cmd_LoadIdentity();
	FTImpl.Cmd_Translate( 65536*imagewidth/2,65536*imageheight/2);//make the rotation coordinates at the center
	
	FTImpl.Cmd_Rotate( 45*65536/360);//rotate by 45 degrees clock wise
	FTImpl.Cmd_Scale( 65536/2,65536/4);//scale by .5x.25
	FTImpl.Cmd_Translate( -65536*imagewidth/2,-65536*imageheight/2);
	FTImpl.Cmd_SetMatrix( );
	FTImpl.Vertex2f(imagexoffset,imageyoffset);

	/* Perform display of plain bitmap with scale factor of .5x2 in x & y direction, rotate by 75 degrees anticlock wise wrt center of the image */
	imagexoffset += (imagewidth + 10)*16;
	FTImpl.Cmd_LoadIdentity();
	FTImpl.Cmd_Translate( 65536*imagewidth/2,65536*imageheight/2);//make the rotation coordinates at the center	
	FTImpl.Cmd_Rotate( -75*65536/360);//rotate by 75 degrees anticlock wise
	FTImpl.Cmd_Scale( 65536/2,2*65536);//scale by .5x2
	FTImpl.Cmd_Translate( -65536*imagewidth/2,-65536*imageheight/8);
	FTImpl.Cmd_SetMatrix( );
	FTImpl.BitmapSize(FT_BILINEAR,FT_BORDER,FT_BORDER,imagewidth*5/2,imageheight*5/2); //set the bitmap size
	FTImpl.Vertex2f(imagexoffset,imageyoffset);
	/******************************************* Perform display of bitmap flip ************************************/
	/* perform display of plain bitmap with 1x1 and flip right */
	imagexoffset = (10)*16;
	imageyoffset += (imageheight*2 + 20)*16;
	FTImpl.Cmd_LoadIdentity();
	FTImpl.Cmd_Translate( 65536*imagewidth/2,65536*imageheight/2);//make the rotation coordinates at the center
	FTImpl.Cmd_Scale( -1*65536,1*65536);//flip right
	FTImpl.Cmd_Translate( -65536*imagewidth/2,-65536*imageheight/2);
	FTImpl.Cmd_SetMatrix( );
	FTImpl.BitmapSize(FT_BILINEAR,FT_BORDER,FT_BORDER,imagewidth,imageheight); //set the bitmap size
	FTImpl.Vertex2f(imagexoffset,imageyoffset);

	/* Perform display of plain bitmap with 2x2 scaling, flip bottom */
	imagexoffset += (imagewidth + 10)*16;
	FTImpl.Cmd_LoadIdentity();
	FTImpl.Cmd_Translate( 65536*imagewidth/2,65536*imageheight/2);//make the rotation coordinates at the center
	FTImpl.Cmd_Scale( 2*65536,-2*65536);//flip bottom and scale by 2 on both sides
	FTImpl.Cmd_Translate( -65536*imagewidth/4,-65536*imageheight/1.42);
	FTImpl.Cmd_SetMatrix( );
	FTImpl.BitmapSize(FT_BILINEAR,FT_BORDER,FT_BORDER,imagewidth*4,imageheight*4); //set the bitmap size
	FTImpl.Vertex2f(imagexoffset,imageyoffset);

	/* Perform display of plain bitmap with 2x1 scaling, rotation and flip right and make sure whole image is viewable */
	imagexoffset += (imagewidth*2 + 10)*16;
	FTImpl.Cmd_LoadIdentity();
	FTImpl.Cmd_Translate( 65536*imagewidth/2,65536*imageheight/2);//make the rotation coordinates at the center
	
	FTImpl.Cmd_Rotate( -45*65536/360);//rotate by 45 degrees anticlock wise
	FTImpl.Cmd_Scale( -2*65536,1*65536);//flip right and scale by 2 on x axis
	FTImpl.Cmd_Translate( -65536*imagewidth/2,-65536*imageheight/8);
	FTImpl.Cmd_SetMatrix( );
	FTImpl.BitmapSize(FT_BILINEAR,FT_BORDER,FT_BORDER,(imagewidth*5),(imageheight*5)); //set the bitmap size
	FTImpl.Vertex2f(imagexoffset,imageyoffset);

	FTImpl.End();
	FTImpl.DLEnd(); //end the display list

	FTImpl.Finish(); //render the display list and wait for the completion of the DL
    delay(2000); //sleep for 2 seconds

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
		BitmapTransform();
	}
	Serial.println("--End Application--");
}

/* Nothing in loop api */
void loop()
{
}