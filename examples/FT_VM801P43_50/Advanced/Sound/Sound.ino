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
* @file                           Sound.ino
* @brief                          This file demonstrates the usage of sound engine of FT801.
								  Tested platform version: Arduino 1.0.4 and later
* @version                        0.1.0
* @date                           2014/24/03
*
*/


/* This application demonstrates the usage of FT801 library on VM801P43 platform */

/* Arduino standard includes */
#include "SPI.h"
#include "Wire.h"

/* Platform specific includes */
#include "FT_VM801P43_50.h"

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

int32_t Dec2Ascii(char *pSrc,int32_t value)
{
	int16_t Length;
	char *pdst,charval;
	int32_t CurrVal = value,tmpval,i;
	char tmparray[16],idx = 0;

	Length = strlen(pSrc);
	pdst = pSrc + Length;

	if(0 == value)
	{
		*pdst++ = '0';
		*pdst++ = '\0';
		return 0;
	}

	if(CurrVal < 0)
	{
		*pdst++ = '-';
		CurrVal = - CurrVal;
	}
	/* insert the value */
	while(CurrVal > 0){
		tmpval = CurrVal;
		CurrVal /= 10;
		tmpval = tmpval - CurrVal*10;
		charval = '0' + tmpval;
		tmparray[idx++] = charval;
	}

	for(i=0;i<idx;i++)
	{
		*pdst++ = tmparray[idx - i - 1];
	}
	*pdst++ = '\0';

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
  FTImpl.ClearColorRGB(64,64,64);  // set the Clear color 
  FTImpl.Clear(1,1,1);    //clear the color component
  FTImpl.ColorRGB(0xff, 0xff, 0xff); //set the bitmap color
  FTImpl.Cmd_Text((FT_DISPLAYWIDTH/2), (FT_DISPLAYHEIGHT/2), 27, FT_OPT_CENTER, "Please Tap on the dot"); //display string at the center of the screen using inbuilt font handle 29 
  FTImpl.Cmd_Calibrate(0);
  FTImpl.Finish(); //render the display list and wait for the completion of the DL
}

PROGMEM  prog_uchar Snd_Array[5*58] = "Slce\0Sqrq\0Sinw\0Saww\0Triw\0Beep\0Alrm\0Warb\0Crsl\0Pp01\0Pp02\0Pp03\0Pp04\0Pp05\0Pp06\0Pp07\0Pp08\0Pp09\0Pp10\0Pp11\0Pp12\0Pp13\0Pp14\0Pp15\0Pp16\0Dmf#\0Dmf*\0Dmf0\0Dmf1\0Dmf2\0Dmf3\0Dmf4\0Dmf5\0Dmf6\0Dmf7\0Dmf8\0Dmf9\0Harp\0Xyph\0Tuba\0Glok\0Orgn\0Trmp\0Pian\0Chim\0MBox\0Bell\0Clck\0Swth\0Cowb\0Noth\0Hiht\0Kick\0Pop\0Clak\0Chak\0Mute\0uMut\0";
PROGMEM  prog_uchar Snd_TagArray[58] = {
	0x63,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,
	0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
	0x23,0x2a,
	0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,
	0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,
	0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,
	0x60,0x61
      };

/* API to demonstrate the usage of sound engine of FT801 */
void Sound()
{
	int32_t LoopFlag = 0,wbutton,hbutton,tagval = 0,tagvalsnd = 0,numbtnrow,numbtncol,i,j,prevtag = 0;
	uint32_t freqtrack = 0,currfreq = 0,prevcurrfreq = 0;
	PROGMEM prog_uchar *pString;
	PROGMEM prog_uchar *pTagArray;
	char StringArray[8];
	/*************************************************************************/
	/* Below code demonstrates the usage of sound function. All the supported*/
	/* sounds and respective pitches are put as part of keys/buttons, by     */
	/* choosing particular key/button the sound is played                    */
	/*************************************************************************/
	LoopFlag = 100;
	numbtnrow = 7 /*16*/;//number of rows to be created - note that mute and unmute are not played in this application
	numbtncol = 8 /*13*/;//number of colomns to be created
	wbutton = (FT_DISPLAYWIDTH - 40)/numbtncol;
	hbutton = FT_DISPLAYHEIGHT/numbtnrow;

	/* set the volume to maximum */
	FTImpl.Write(REG_VOL_SOUND,0xFF);
	/* set the tracker to track the slider for frequency */	
	FTImpl.Cmd_Track( FT_DISPLAYWIDTH - 15, 20, 8, (FT_DISPLAYHEIGHT - 40), 100);
	
	FTImpl.Finish();
	while(LoopFlag)
	{
		tagval = FTImpl.Read( REG_TOUCH_TAG);
		freqtrack = FTImpl.Read32( REG_TRACKER);
		
		if(100 == (freqtrack & 0xff))
		{
			currfreq = (uint32_t)(freqtrack >> 16);
			currfreq = (uint32_t)(88*currfreq)/65536;
			if(currfreq > 108)
				currfreq = 108;
		}
		if((tagval > 0))
		{
			if( tagval <= 99)
			{
				tagvalsnd = tagval;
			}
			if(0x63 == tagvalsnd)
			{
				tagvalsnd = 0;
			}
			if((prevtag != tagval) || (prevcurrfreq != currfreq))
			{
				/* Play sound wrt pitch */				
				FTImpl.PlaySound((((currfreq + 21) << 8) | tagvalsnd));				
			}
			if(0 == tagvalsnd)
				tagvalsnd = 99;
		}
		/* start a new display list for construction of screen */
		
		FTImpl.DLStart(); //start the display list. Note DLStart and DLEnd are helper apis, Cmd_DLStart() and Display() can also be utilized.
		FTImpl.ClearColorRGB(64,64,64);// set the Clear color
		FTImpl.Clear(1,1,1);    //clear the color component
		/* line width for the rectangle */
		FTImpl.LineWidth(1*16);

		/* custom keys for sound input */
		pString = Snd_Array;
		pTagArray = Snd_TagArray;
		/* First draw all the rectangles followed by the font */
		/* yellow color for background color */
		FTImpl.ColorRGB(0x80,0x80,0x00);
		FTImpl.Begin(FT_RECTS); //starts drawing rectangles
		for(i=0;i<numbtnrow;i++)
		{
			for(j=0;j<numbtncol;j++)
			{
				FTImpl.Tag(pgm_read_byte(pTagArray));
				FTImpl.Vertex2ii((j*wbutton + 2),(hbutton*i + 2),0,0);
				FTImpl.Vertex2ii(((j*wbutton) + wbutton - 2),((hbutton*i) + hbutton - 2),0,0);
				pTagArray++;
			}
		}
		FTImpl.End();
		FTImpl.ColorRGB(0xff,0xff,0xff);
		/* draw the highlight rectangle and text info */
		pTagArray = Snd_TagArray;
		pString = Snd_Array;
		for(i=0;i<numbtnrow;i++)
		{
			for(j=0;j<numbtncol;j++)
			{
				FTImpl.Tag(pgm_read_byte(pTagArray));
				if(tagvalsnd == pgm_read_byte(pTagArray))
				{
					/* red color for highlight effect */
					FTImpl.ColorRGB(0x80,0x00,0x00);
					FTImpl.Begin(FT_RECTS);
					FTImpl.Tag(pgm_read_byte(pTagArray));
					FTImpl.Vertex2ii((j*wbutton + 2),(hbutton*i + 2),0,0);
					FTImpl.Vertex2ii(((j*wbutton) + wbutton - 2),((hbutton*i) + hbutton - 2),0,0);
					FTImpl.End();
					/* reset the color to make sure font doesnt get impacted */
					FTImpl.ColorRGB(0xff,0xff,0xff);
				}
				/* to make sure that highlight rectangle as well as font to take the same tag values */
	

                strcpy_P(StringArray,(const PROGMEM prog_char*)pString);	                          
                FTImpl.Cmd_Text((wbutton/2) + j*wbutton,(hbutton/2) + hbutton*i,26,FT_OPT_CENTER,StringArray);
                pString += (strlen(StringArray) + 1);
				pTagArray++;
			}
		}

		/* Draw vertical slider bar for frequency control */
		StringArray[0] = '\0';
		strcat(StringArray,"Pt ");
		Dec2Ascii(StringArray,(int32_t)(currfreq + 21));
		FTImpl.TagMask(0);
		FTImpl.Cmd_Text(FT_DISPLAYWIDTH - 20,10,26,FT_OPT_CENTER,StringArray);
		FTImpl.TagMask(1);
		FTImpl.Tag(100);
		FTImpl.Cmd_Slider( FT_DISPLAYWIDTH - 15, 20, 8, (FT_DISPLAYHEIGHT - 40), 0, currfreq, 88);
		prevtag = tagval;
		prevcurrfreq = currfreq;
		
		/* Download the commands into fifo */
		FTImpl.DLEnd(); //end the display list
		FTImpl.Finish(); //render the display list and wait for the completion of the DL
	}
	FTImpl.PlaySound(0);
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
		FTImpl.Write( REG_VOL_SOUND,255);
		Sound();
		
	}
	Serial.println("--End Application--");
}

/* Nothing in loop api */
void loop()
{
	
}