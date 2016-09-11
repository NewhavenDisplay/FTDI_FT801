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
* @file                           Font.ino
* @brief                          Sketch to display inbuilt font on display.
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
/* Helper API to convert decimal to ascii - pSrc shall contain NULL terminated string */
int32_t Dec2Ascii(char *pSrc,int32_t value)
{
	int16_t Length;
	char *pdst,charval;
	int32_t CurrVal = value,tmpval,i;
	char tmparray[16],idx = 0;//assumed that output string will not exceed 16 characters including null terminated character

	//get the length of the string
	Length = strlen(pSrc);
	pdst = pSrc + Length;
	
	//cross check whether 0 is sent
	if(0 == value)
	{
		*pdst++ = '0';
		*pdst++ = '\0';
		return 0;
	}
	
	//handling of -ve number
	if(CurrVal < 0)
	{
		*pdst++ = '-';
		CurrVal = - CurrVal;
	}
	/* insert the digits */
	while(CurrVal > 0){
		tmpval = CurrVal;
		CurrVal /= 10;
		tmpval = tmpval - CurrVal*10;
		charval = '0' + tmpval;
		tmparray[idx++] = charval;
	}

	//flip the digits for the normal order
	for(i=0;i<idx;i++)
	{
		*pdst++ = tmparray[idx - i - 1];
	}
	*pdst++ = '\0';

	return 0;
}
/* API to display various inbuilt "Font" on screen */
void Font()
{
	uint8_t i;
	uint16_t xoffset,yoffset;
	/* Change the below string for experimentation */
	char String[10];
	
	xoffset = (FT_DISPLAYWIDTH*1/3) - 50;
	yoffset = FT_DISPLAYHEIGHT*1/9;
	/* Display "Font" string at various locations to demonstrate the inbuilt font */
	/* Totally 16 in built fonts are available, display in two columns */
	FTImpl.DLStart();//start the display list. Note DLStart and DLEnd are helper apis, Cmd_DLStart() and Display() can also be utilized.
	FTImpl.ColorRGB(0xFF,0xFF,0xFF);//set the color of the string to while color
	for(i=0;i<16;i++)
	{
		if(8 == i)
		{
			/* handle xoffset and yoffset modifications */
			xoffset = (FT_DISPLAYWIDTH*2/3) - 50;
			yoffset = FT_DISPLAYHEIGHT*1/9;
		}
		String[0] = '\0';
		Dec2Ascii(String,16+i);//generate string based on font handle
		strcat(String,".Font");
		FTImpl.Cmd_Text(xoffset, yoffset, (16+i), FT_OPT_CENTERY, String);//display "Font" string in 8 rows and 2 columns from bitmap handles 16 to 31
		/* Increment (xoffset,yoffset) coordinates for the next font display */
		yoffset += (FT_DISPLAYHEIGHT*1/9);
	}
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
		Font();
	}
	Serial.println("--End Application--");
}

/* Nothing in loop api */
void loop()
{
}
