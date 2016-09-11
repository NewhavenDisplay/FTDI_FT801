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
* @file                           FTDILogo.ino
* @brief                          Sketch to display FTDI logo on FT801.
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



int8_t ginit = 0;
/* bootup the module and display spinner on screen */
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
		ginit = 1;
		Serial.println("--Init Done--");
	}	
}
/* API to display FTDI logo on the screen */
int16_t FTDILogo()
{
  uint32_t coprostatus;
  FTImpl.Cmd_Logo();
  
  //make sure finish is not used here as coprocessor will take over both the command read and write pointers
  
  delay(2500);//delay of 2.5 seconds for the completion of logo
  Serial.print("Logo app: ");
  do
  {  
    coprostatus = FTImpl.CheckLogo();
  } 
  while(coprostatus == FT_GE_BUSY);
  
  Serial.println(coprostatus,HEX);
  return coprostatus;
}

/* Perform the FTDI logo in loop */
void loop()
{
	//error handling
	if(1 == ginit)//operate the logo only if FT801 is initialized
	{	
		/* perform the logo each itteration */
		FTDILogo();		
	}
}
