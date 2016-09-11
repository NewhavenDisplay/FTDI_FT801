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
* @file                           TextVGA8x8.ino
* @brief                          Sketch to display TextVGA and Text8x8 on display.
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

/* API to display Hello World string on the screen for text8x8 and random string for text VGA */
void TestVGA8x8()
{
	/* Change the below string for experimentation */
	int32_t String_size,hoffset = 16,voffset = 32,i;
	uint16_t VGAText_Array[160];	//text vga data
	const char Text_Array[9] = "abcdefgh";	//text 8x8 data
	
	/* Download data into FT_RAM_G */
	/* Download character values into graphics ram - only ascii 8x8 L1 format is supported */
    String_size = sizeof(Text_Array);
    FTImpl.Write(FT_RAM_G,(uint8_t *)Text_Array, String_size);
	
	/* Download textvga characters into graphics ram - characters are computed randomly for simplicity */
	for(i=0;i<160;i++)
	{
	  VGAText_Array[i] = random(65536);//160 vga characters with random values
	}	
	FTImpl.Write(FT_RAM_G + 8,(uint8_t *)VGAText_Array, 160*2);
	
	/* Display list to display text in 8x8 and vga format at the centre of display area */
	FTImpl.DLStart();//start the display list. Note DLStart and DLEnd are helper apis, Cmd_DLStart() and Display() can also be utilized.
	FTImpl.ColorRGB(0xFF,0xFF,0xFF);//set the color of the string to while color
	
	//display informative string 
	FTImpl.Cmd_Text(FT_DISPLAYWIDTH/4, 8, 27, FT_OPT_CENTER, "Text 8x8");//display informative string "Text 8x8" with handle 27
	
	/* text 8x8 implementation */
	//configure the bitmap for text 8x8 display
    FTImpl.BitmapSource(FT_RAM_G);
    //draw 8 characters in horizontal repeated in 2 line
	FTImpl.BitmapLayout(FT_TEXT8X8, 1*8,1);//L1 format, each input data element is in 1 byte size
    FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_REPEAT, 8*8, 8*2);//output is 8x8 format - draw 8 characters in horizontal repeated in 2 line
    FTImpl.Begin(FT_BITMAPS);
	/* Display text 8x8 at hoffset, voffset location */
    FTImpl.Vertex2f(hoffset*16,voffset*16);
	
	//draw 32 characters in horizontal and 2 characters in vertical
    FTImpl.BitmapLayout(FT_TEXT8X8, 1*4,2);//L1 format and each datatype is 1 byte size
    FTImpl.BitmapSize(FT_NEAREST, FT_REPEAT, FT_BORDER, 8*16, 8*2);//each character is 8x8 in size -  so draw 32 characters in horizontal and 2 characters in vertical
    hoffset = 16;
    voffset = FT_DISPLAYHEIGHT/2;
    FTImpl.Vertex2f(hoffset*16,voffset*16);
	
	//display informative string 
	FTImpl.Cmd_Text(FT_DISPLAYWIDTH*3/4, 8, 27, FT_OPT_CENTER, "Text VGA");//display informative string "Text 8x8" with handle 27
	/* Text vga implementation */
	/* mandatory for textvga as background color is also one of the parameter in textvga format */
	FTImpl.BlendFunc(FT_ONE,FT_ZERO);

	//draw textvga 8 characters in horizontal and 8 vertical
	FTImpl.BitmapSource(FT_RAM_G + 8);//update the starting address of the textvga raw data
	FTImpl.BitmapLayout(FT_TEXTVGA, 2*4,8);//L1 format, but each input data element is of 2 bytes in size
	FTImpl.BitmapSize(FT_NEAREST,FT_BORDER,FT_BORDER, 8*8, 8*8);//output is 8x8 format - draw 8 characters in horizontal and 8 vertical
	FTImpl.Begin(FT_BITMAPS);
	hoffset = (FT_DISPLAYWIDTH/2) + 16;
	voffset = 32;
	FTImpl.Vertex2f(hoffset*16,voffset*16);
	
	//draw textvga 8 pixels per character and 32 rows/colomns
	FTImpl.BitmapLayout(FT_TEXTVGA, 2*16,8);//L1 format but each datatype is 16bit size
	FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_REPEAT, 8*32, 8*32);//8 pixels per character and 32 rows/colomns
	FTImpl.Begin(FT_BITMAPS);
	hoffset = (FT_DISPLAYWIDTH/2) + 16;
	voffset = FT_DISPLAYHEIGHT/2;
	FTImpl.Vertex2f(hoffset*16,voffset*16);
	
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
		TestVGA8x8();
	}
	Serial.println("--End Application--");
}

/* Nothing in loop api */
void loop()
{
}
