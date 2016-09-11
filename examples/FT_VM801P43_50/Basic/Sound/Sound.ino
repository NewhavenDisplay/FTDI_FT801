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
* @brief                          This file demonstrates the usage of audio engine of FT801.
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

/* API to demonstrate the usage of sound api of FT801. Play a synthesized sound with respective frequency and volume */
void Sound(uint8_t PlaySound,uint8_t PenDown)
{
	uint8_t Vol,Pitch,Sound;
	char StringArray[40],Tempstr[8];//make sure array is increased in case below string is modified
	
	/* Assign the sound parameters - experiment audio engine by modifying the below parameters */
	Vol = 255;
	Pitch = FT_MIDI_C_6;
	Sound = FT_PIANO;
	
	/* Display list to display informative string on screen */
	FTImpl.DLStart(); //start the display list. Note DLStart and DLEnd are helper apis, Cmd_DLStart() and Display() can also be utilized.
	FTImpl.ColorRGB(0x00FFFFFF);// set the color to white
	
	/* Construct the display string */
	StringArray[0] = '\0';Tempstr[0] = '\0';
	Dec2Ascii(Tempstr,Sound);
	strcat(StringArray,"Sound:");strcat(Tempstr,"  ");
	strcat(StringArray,Tempstr);
	strcat(StringArray,"Pitch:");
	Tempstr[0] = '\0';
	Dec2Ascii(Tempstr,Pitch);strcat(Tempstr,"  ");
	strcat(StringArray,Tempstr);
	strcat(StringArray,"Volume:");
	Tempstr[0] = '\0';
	Dec2Ascii(Tempstr,Vol);
	strcat(StringArray,Tempstr);
	
	FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,FT_DISPLAYHEIGHT/2,27,FT_OPT_CENTER,StringArray);	//display info string at the centre of the screen using bitmap handle 27
	FTImpl.Tag(1);//assign tag value of 1 to the button for tracking the pen down
	FTImpl.Cmd_Button((FT_DISPLAYWIDTH/2) - 40,(FT_DISPLAYHEIGHT/2) + 20,80,40,27,((PenDown?FT_OPT_FLAT:0) | FT_OPT_CENTER),"Play");//display a button on screen & wrt pendown change the options to flat or 3d effect
	FTImpl.DLEnd();//end the display list
	FTImpl.Finish();//render the display list to the screen

	/* Below code demonstrates the usage of audio engine api to play sound */
	if(PlaySound)
		FTImpl.PlaySound(Vol,(uint16_t)((Pitch<<8) | Sound));
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
		Calibrate();//calibrate the touch screen for user inputs
		Sound(1,0);//api to demonstrate the usage of sound engine on FT801 - default values for the first time
	}	
}
//global variables used for sound play
uint8_t gPenDown = 0,gPrevPenDown = 0,gPrevTag = 0;
/* Demonstrate the usage of sound in a loop */
void loop()
{
	/* Get the touch parameters */
	uint8_t FlagPlay = 0;
	sTagXY sTagxy;
	
	FTImpl.GetTagXY(sTagxy);//get the current tag from the touch screen
	gPenDown = FTImpl.IsPendown();
	if((1 == gPrevPenDown) && (0 == gPenDown) && (1 == gPrevTag))
	{
		FlagPlay = 1;//play the sound only during pen up
	}
	Sound(FlagPlay,(1 == sTagxy.tag));
	
	gPrevPenDown = gPenDown;
	gPrevTag = sTagxy.tag;
}