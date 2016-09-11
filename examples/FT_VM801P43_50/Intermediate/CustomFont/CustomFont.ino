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
* @file                           CustomFont.ino
* @brief                          Sketch to display custom font on display.
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

/* Font specific macros */
#define Roboto_BoldCondensed_Size (19348)

#include "Roboto_BoldCondensed.h"

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

/* API to display custom Font on screen */
/* Fonts are generated using FT801 specific font conversion tools */
void CustomFont()
{
	uint32_t fontaddr = FT_RAM_G;
	uint16_t blocklen = FT_FONT_TABLE_SIZE;	
	uint8_t FontIdxTable[FT_FONT_TABLE_SIZE],*pbuff;//font index table
	
    pbuff = FontIdxTable;
	
    /* Copy the header from starting of the array into FT_RAM_G */
	FTImpl.Writefromflash(FT_RAM_G,Roboto_BoldCondensed,FT_FONT_TABLE_SIZE);
	/* update the address of the font data - last 4 bytes of the index table contains the font data address */
	FTImpl.Write32(FT_RAM_G + FT_FONT_TABLE_SIZE - 4,1024);//FT_RAM_G + 1024 is the starting address of the font raw data, out of which first 32 characters are dummy
	/* download the custom font data - note that first 32 characters in ascii table are control commands and hence need to take care of offset */
	
	/* Next download the data at location 32*8*25 - skip the first 32 characters */
	/* each character is 8x25 bytes */
	fontaddr = (FT_RAM_G + 1024+32*8*25);//make sure space is left at the starting of the buffer for first 32 characters	
	FTImpl.Writefromflash( fontaddr,&Roboto_BoldCondensed[FT_FONT_TABLE_SIZE],1L*(Roboto_BoldCondensed_Size - FT_FONT_TABLE_SIZE));
	
	FTImpl.DLStart();//start the display list. Note DLStart and DLEnd are helper apis, Cmd_DLStart() and Display() can also be utilized.
	FTImpl.ColorRGB(0xFF,0xFF,0xFF);//set the color of the string to while color

	/* Display of information string on screen - using inbuilt font */
	FTImpl.Cmd_Text((FT_DISPLAYWIDTH/2), 20, 27, FT_OPT_CENTER, "SetFont - format FT_L4");
	
	/* Configure the custom font to coprocessor */
	FTImpl.WriteCmd(BITMAP_HANDLE(6));//give index table 6
	FTImpl.WriteCmd(BITMAP_SOURCE(1024));//make the address to 0
	FTImpl.WriteCmd(BITMAP_LAYOUT(FT_L4,8,25));//stride is 8 and height is 25
	FTImpl.WriteCmd(BITMAP_SIZE(FT_NEAREST,FT_BORDER,FT_BORDER,16,25));//width is 16 and height is 25

	FTImpl.Cmd_SetFont(6,0);//set the font table to the coprocessor engine, bitmap handle used is 6 and the starting address of the table is 0
	/* Display string */
	FTImpl.Cmd_Text((FT_DISPLAYWIDTH/2), 80, 6, FT_OPT_CENTER, "The quick brown fox jumps");
	FTImpl.Cmd_Text((FT_DISPLAYWIDTH/2), 120, 6, FT_OPT_CENTER, "over the lazy dog.");
	FTImpl.Cmd_Text((FT_DISPLAYWIDTH/2), 160, 6, FT_OPT_CENTER, "1234567890");
	
	FTImpl.DLEnd();//end the display list
	FTImpl.Finish();//render the display list and wait for the completion of the DL
}

/* bootup the module and display custom font on screen */
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
		CustomFont();
	}
	Serial.println("--End Application--");
}

/* Nothing in loop api */
void loop()
{
}
