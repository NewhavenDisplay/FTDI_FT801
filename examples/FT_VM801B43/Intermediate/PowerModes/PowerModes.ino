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
* @file                           PowerModes.ino
* @brief                          Sketch to demonstrate various power modes supported in FT801.
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
//api to verify chip id and enable audio and display
int16_t CheckChipEnableDispAud()
{
	uint32_t chipid = 0;
	
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


/* API to perform display fadeout effect by changing the display PWM from 100 till 0*/
void fadeout()
{
    int32_t i;
	
	for (i = 100; i >= 0; i -= 3) 
	{
		FTImpl.Write(REG_PWM_DUTY,i);

		delay(2);//delay for 2 ms
	}
}

/* API to perform display fadein effect by changing the display PWM from 0 till 100 and finally 128 */
void fadein()
{
	int32_t i;
	
	for (i = 0; i <=100 ; i += 3) 
	{
		FTImpl.Write(REG_PWM_DUTY,i);
		delay(2);//delay for 2 ms
	}
	/* Finally make the PWM 100% */
	i = 128;
	FTImpl.Write(REG_PWM_DUTY,i);
}
/* API for displaying informative string on screen */
void InfoScreen(char *str)
{
	//construct display list for informative string on screen
	FTImpl.DLStart();//start the display list. Note DLStart and DLEnd are helper apis, Cmd_DLStart() and Display() can also be utilized.
	FTImpl.ColorRGB(0xFF,0xFF,0xFF);//set the color of the string to while color
	FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2, FT_DISPLAYHEIGHT/2, 27, FT_OPT_CENTER, str);//display *str at the center of the screen using inbuilt font handle 29 "
	FTImpl.DLEnd();//end the display list
	FTImpl.Finish();//render the display list and wait for the completion of the DL
}
/* API to demonstrate various power modes of FT801. Also display string on the screen for informative purposes */
void PowerModes()
{
	/*************************************************
	Senario1:  Transition from Active mode to Standby mode.
	           Transition from Standby mode to Active Mode
	**************************************************/

    InfoScreen("Active-standby-active");
	
	//Switch FT801 from Active to Standby mode
	fadeout();
	FTImpl.PlaySound(0,FT_MUTE);//play mute sound - feature in FT801 to avoid audio glitch during power down stage. also note that audio engine plays unmute sound by default after hardware reset
	delay(25);
	FTImpl.HostCommand(FT_STANDBY);//send standby command to FT801 - power to PLL and crystal is enabled, rest all cut off, registers contents are retained. Instantaneous wakeup, 1.5mA current consumed

	//Wake up from Standby first before accessing FT801 registers.
	FTImpl.HostCommand(FT_ACTIVE);//send active command to FT801
	FTImpl.PlaySound(0,FT_UNMUTE);
	InfoScreen("Active-standby-active");
	fadein();
	delay(3000);

	/*************************************************
	Senario2:  Transition from Active mode to delay mode.
	           Transition from delay mode to Active Mode
	**************************************************/
    //Switch FT801 from Active to delay mode
	InfoScreen("Active-sleep-active");
    fadeout();
	FTImpl.PlaySound(0,FT_MUTE);//play mute sound - feature in FT801 to avoid audio glitch during power down stage. also note that audio engine plays unmute sound by default after hardware reset
    delay(25);//to make sure mute is played back by the hardware
	FTImpl.HostCommand(FT_SLEEP);//send delay command - from active to sleep

	//Wake up from sleep 
	FTImpl.ActiveInternalClock();
	FTImpl.DisplayConfigExternalClock(FT_DISPLAY_RESOLUTION);
	CheckChipEnableDispAud();
	FTImpl.PlaySound(0,FT_UNMUTE);
	InfoScreen("Active-sleep-active");
	fadein();
	delay(3000);

	/*************************************************
	Senario3:  Transition from Active mode to PowerDown mode.
	           Transition from PowerDown mode to Active Mode via Standby mode.
	**************************************************/
    //Switch FT801 from Active to PowerDown mode by sending command
	InfoScreen("Active-powerdown-active");
    fadeout();
	FTImpl.PlaySound(0,FT_MUTE);//play mute sound - feature in FT801 to avoid audio glitch during power down stage. also note that audio engine plays unmute sound by default after hardware reset
    delay(25);
	FTImpl.HostCommand(FT_PWRDOWN);//power down from active
	
    FTImpl.Reset();//do a reset followed by disp config
	FTImpl.DisplayConfigExternalClock(FT_DISPLAY_RESOLUTION);
	CheckChipEnableDispAud();
	//Need download display list again because power down mode lost all registers and memory
	InfoScreen("Active-powerdown-active");
	
	fadein();
	delay(3000);

	/*************************************************
	Senario4:  Transition from Active mode to PowerDown mode by toggling PDN from high to low.
	           Transition from PowerDown mode to Active Mode via Standby mode.
	**************************************************/
    //Switch FT801 from Active to PowerDown mode by toggling PDN
	InfoScreen("Active-pdnpgio-active");
    fadeout();
	FTImpl.PlaySound(0,FT_MUTE);//play mute sound - feature in FT801 to avoid audio glitch during power down stage. also note that audio engine plays unmute sound by default after hardware reset
    delay(25);
	FTImpl.PDN_Cycle();//do a power down cycle using PDN gpio pin
	
    FTImpl.Reset();//do a reset followed by disp config
	FTImpl.DisplayConfigExternalClock(FT_DISPLAY_RESOLUTION);
	CheckChipEnableDispAud();
	//Need download display list again because power down mode lost all registers and memory
	InfoScreen("Active-pdnpgio-active");
	fadein();
	delay(3000);


	/*************************************************
	Senario5:  Transition from Active mode to PowerDown mode via Standby mode.
	           Transition from PowerDown mode to Active mode via Standby mode.
	**************************************************/
    //Switch FT801 from Active to standby mode
	InfoScreen("Active-standby-pdngpio-active");
    fadeout();
	FTImpl.PlaySound(0,FT_MUTE);//play mute sound - feature in FT801 to avoid audio glitch during power down stage. also note that audio engine plays unmute sound by default after hardware reset
	delay(25);
	FTImpl.HostCommand(FT_STANDBY);//from active to standby
	FTImpl.PDN_Cycle();//do a power down cycle using PDN gpio pin
	
    FTImpl.Reset();//do a reset followed by disp config
	FTImpl.DisplayConfigExternalClock(FT_DISPLAY_RESOLUTION);
	CheckChipEnableDispAud();
	//Need download display list again because power down mode lost all registers and memory
	InfoScreen("Active-standby-pdngpio-active");
	fadein();
	delay(3000);


	/*************************************************
	Senario6:  Transition from Active mode to PowerDown mode via delay mode.
	           Transition from PowerDown mode to Active mode via Standby mode.
	**************************************************/
    //Switch FT801 from Active to standby mode
	InfoScreen("Active-sleep-pdngpio-active");
    fadeout();
    FTImpl.PlaySound(0,FT_MUTE);//play mute sound - feature in FT801 to avoid audio glitch during power down stage. also note that audio engine plays unmute sound by default after hardware reset
	delay(25);
	FTImpl.HostCommand(FT_SLEEP);//active to sleep mode
	FTImpl.PDN_Cycle();//do a power down cycle using PDN gpio pin
	
    FTImpl.Reset();//do a reset followed by disp config
	FTImpl.DisplayConfigExternalClock(FT_DISPLAY_RESOLUTION);
	CheckChipEnableDispAud();
	//Need download display list again because power down mode lost all registers and memory
	InfoScreen("Active-sleep-pdngpio-active");
	fadein();
	delay(3000);
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
	}
}

/* Perform power modes test continuously */
void loop()
{
	PowerModes();
}
