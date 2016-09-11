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
* @file                           AudioPlayback.ino
* @brief                          This sketch demonstrates the usage of the FT801 audio engine and how the standard Arduino SD library can be used instead of the FTDI's custom SD library.
								  Tested platform version: Arduino 1.0.4 and later
* @version                        0.1.0
* @date                           2014/24/03
*
*/



/* Arduino standard includes */
#include "SPI.h"
#include "Wire.h"
#include <SD.h>

/* Platform specific includes */
#include "FT_VM801P43_50.h"

#define BUFFER_SIZE 16

boolean sd_present = false;

/* Global object for FT801 Implementation */
FT801IMPL_SPI FTImpl(FT_CS_PIN,FT_PDN_PIN,FT_INT_PIN);

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

/* Display an on-screen message if the storage device is not found */
void CheckStorageDevicePrecence(){
	while(!sd_present){
		FTImpl.DLStart();     
		FTImpl.Clear(1,1,1);
		FTImpl.ColorRGB(255,255,255);
		FTImpl.Cmd_Text(FT_DISPLAYWIDTH>>1,FT_DISPLAYHEIGHT>>1,27,FT_OPT_CENTER,"Storage Device Not Found");
		FTImpl.DLEnd();
		FTImpl.Finish();
		delay(5000);
		}
}

/* API to demonstrate the usage of Loading screenshot - display loading informative string and spinner */
void Loading()
{	
	/* Display list to display informative string on screen followed by spinner */
	FTImpl.DLStart(); //start the display list. Note DLStart and DLEnd are helper apis, Cmd_DLStart() and Display() can also be utilized.
	FTImpl.ColorRGB(0x00FFFFFF);// set the color to white

	FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,FT_DISPLAYHEIGHT/6,27,FT_OPT_CENTER,"Loading...");	//display info string at the centre of the screen using bitmap handle 27
	FTImpl.Cmd_Spinner(FT_DISPLAYWIDTH/2,FT_DISPLAYHEIGHT/2,0,1);//display spinner to demonstrate animation kind of effects
	
	//note that co processor engine itself inserts graphics instructions to give out animation effect
	FTImpl.Finish();//render the display list to the screen
}


/* Api to demonstrate display list screen shot construction with informative string and progress using progress bar and font */
void PlaybackStatus(uint8_t Percentage)// 0 to 100%
{
	char String[10];
	/* Display list to display informative string on screen followed by progress */
	FTImpl.DLStart(); //start the display list. Note DLStart and DLEnd are helper apis, Cmd_DLStart() and Display() can also be utilized.
	FTImpl.ColorRGB(0x00FFFFFF);// set the color to white

	FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,FT_DISPLAYHEIGHT/2,27,FT_OPT_CENTER,"DRide.raw");	//display info string at the centre of the screen using bitmap handle 27
	FTImpl.Cmd_Progress((FT_DISPLAYWIDTH/4),(FT_DISPLAYHEIGHT/2) + 10, FT_DISPLAYWIDTH/2, 20, 0, Percentage, 100);//display progress bar to demonstrate analog - range from 0 to 100
	
	/* construct the string for % completion */
	String[0] = '\0';
	Dec2Ascii(String,Percentage);
	strcat(String,"%");
	FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,(FT_DISPLAYHEIGHT/2) + 40 ,27,FT_OPT_CENTER,String);	//display info string for % completeness using bitmap handle 27
	//note that co processor engine itself inserts graphics instructions to give out animation effect
	FTImpl.DLEnd();//end the display list
	FTImpl.Finish();//render the display list to the screen
	
}

/* API to demonstrate audio playback on FT801. Application uses 100KBytes of buffer in gram to playback one shot, downloads the audio data into gram continuously */
void AudioPlayback()
{
	uint8_t Percent = 0;
	uint32_t CurrPlayAddr = 0;
	File file;
	//check whether file is present or not
	file = SD.open("DRide.raw",FILE_READ);  //open DRide.raw in read mode and associate it with file handle.
	if(!file)  //if there is an error opening the file.
	{
		//display list for file not present
		FTImpl.DLStart();     
		FTImpl.Clear(1,1,1);
		FTImpl.ColorRGB(255,255,255);
		FTImpl.Cmd_Text(FT_DISPLAYWIDTH>>1,FT_DISPLAYHEIGHT>>1,27,FT_OPT_CENTER,"File Not Found");
		FTImpl.DLEnd();
		FTImpl.Finish();	
		
		Serial.println("DRide.raw File not present");
	}
	else  //file successfully opened. 
	{
		int32_t filesz,currreadlen=0;
		//uint8_t tbuff[512];//temp buffer for reading sector from sdcard
		uint8_t tbuff[BUFFER_SIZE];
		filesz = file.size();

		Serial.print("Success in opening afile, size ");
		Serial.println(filesz,DEC);

		while(currreadlen<filesz){  //put the whole file to RAM_G
			uint16_t n = min(BUFFER_SIZE, filesz - currreadlen);  //transfer at most BUFFER_SIZE chunk
			file.readBytes((char*)tbuff,n);  //read n bytes 
			FTImpl.Write(currreadlen,tbuff,n);  //write n bytes to address currreadlen of RAM_G
			currreadlen+=n;  //advance file offset
		}
		file.close();
		/* Stop the spinner */
		FTImpl.Cmd_Stop();
		FTImpl.Finish();			
		
		//configure audio and fire the audio playback - note this example is for one shot playback
		FTImpl.PlayAudio(255,FT_LINEAR_SAMPLES,11025,FT_RAM_G,filesz,0);
		
		//display % completeness wrt audio playback status
		while(1)
		{
			if(FT_AE_PLAYBACK_STOPPED == FTImpl.GetAudioStats(CurrPlayAddr))
			{
				PlaybackStatus(100);
				break;//audio playback is completed
			}
			Percent = (uint8_t)((int32_t)CurrPlayAddr*100/filesz);
			//in case of continue, update the completeness
			PlaybackStatus(Percent);
			Serial.print("Playback %  ");
			Serial.println(Percent,DEC);
			
		}
	}
}

/* bootup the module and display primitives on screen */
void setup()
{
	/* Initialize serial print related functionality */
	Serial.begin(9600);
	
	/* Set the Display Enable pin*/   
	Serial.println("--Start Application--");

	/*Initialize the SD object. Screen error message can only be displayed when the FT801 is ready*/
	sd_present = SD.begin(FT_SD_CSPIN);
	
	if(BootupConfigure())
	{
		//error case - do not do any thing
	}
  	else
	{
		CheckStorageDevicePrecence();
		Loading();//show busy kind of animation
		AudioPlayback();//api to demonstrate the usage of audio engine to playback audio PCM data
	}	
}
/* Do nothing in loop */
void loop()
{
}