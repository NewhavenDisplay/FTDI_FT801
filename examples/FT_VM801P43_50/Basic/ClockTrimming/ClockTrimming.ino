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
* @file                           ClockTrimming.ino
* @brief                          Sketch to demonstrate how to choose the internal clock source and "trim" it to the maximum frequency.  
								  Tested platform version: Arduino 1.0.4 and later
* @version                        0.1.0
* @date                           2014/17/05
*
*/

/*
The following steps provide a detailed programming sequence to trim the internal clock:

1. After a hardware reset (power on or toggling PD_N pin), the FT800/FT801 enters STANDBY mode.  The internal relaxation oscillator clock is selected as the clock source of the PLL.  Both the internal clock and the PLL are turned on, but the main clock to the core logic is turned off.

2. Set the chip to ACTIVE mode by doing a dummy read operation through host interface (SPI or I2C).

3. Measure the current clock frequency
	a. Read REG_CLOCK as T0
	b. Delay 1us (1 nano second)
	c. Read REG_CLOCK as t1
	d. Calculate the clock frequency f = (t1-t0) / 1us

4. If the measured frequency f is less than 48MHz - 3%, increase the REG_TRIM register by 1
5. Repeat steps 3 and 4 until the measured frequency is within 48MHz +/- 3%, or the REG_TRIM already reaches the maximum value of 31 (which is unlikely since the trimming range is defined to be able to tune the clock to the target)

*/


/* Arduino standard includes */
#include "SPI.h"
#include "Wire.h"

/* Platform specific includes */
#include "FT_VM801P43_50.h"

/* Good enough frequency boundary*/
#define LOW_FREQ_BOUND  47040000L

/* Global object for FT801 Implementation */
FT801IMPL_SPI FTImpl(FT_CS_PIN,FT_PDN_PIN,FT_INT_PIN);

/* FT_GC enum options */
FT_GCOptions options;


/* Measure the current clock frequency by using the Arduino delayMicroseconds() function.  15625 microseconds was chosen because the frequency can be obtained simply by multiplying the difference of the clock counter reads by 64.  NOTE: This method does not give the most accurate number possible because the time to perform the Read32() function and reading the register was not factored in.*/
uint32_t currentFrequency(void)
{
  uint32_t t0,t1;
  
  t0 = FTImpl.Read32(REG_CLOCK); /* t0 read */
  delayMicroseconds(15625); 
  t1 = FTImpl.Read32(REG_CLOCK); /* t1 read */
  
  return ((t1 - t0) << 6L); /* bitshift 6 places is the same as multiplying 64 */
}


void clockTrimming(void)
{
	uint32_t f;
	uint8_t i;
/* Trim the internal clock by increase the REG_TRIM register till the measured frequency is within the acceptable range.*/	
	for (i=0; (i < 31) && ((f= currentFrequency()) < LOW_FREQ_BOUND); i++)
		FTImpl.Write(REG_TRIM, i);  /* increase the REG_TRIM register value automatically increases the internal clock */ 
	FTImpl.Write32(REG_FREQUENCY,f);  /* Set the final frequency to be used for internal operations */
}


/* API to boot up the graphics controller, verify the hardware and configure the display/audio pins */
/* Returns 0 in case of success and 1 in case of failure */
int16_t BootupConfigure()
{
	uint32_t chipid = 0;
	
	/* Specify to use internal clock source in the optional parameter of the Init() function, default is to use the external clock source.  Options in the FT_GCOptions can be concatenated.*/
	FTImpl.Init(FT_DISPLAY_RESOLUTION,FT_INTERNAL_CLOCK_SOURCE);  
	
	/* trim the internal clock*/
	clockTrimming();
	
	delay(20);//for safer side
	chipid = FTImpl.Read32(FT_ROM_CHIPID);
	
	/* Identify the chip */
	if(FT801_CHIPID != chipid)
	{
		Serial.print("Error in chip id read ");
		Serial.println(chipid,HEX);
		return 1;
	}
	
	FTImpl.SetDisplayEnablePin(FT_DISPENABLE_PIN);
    FTImpl.SetAudioEnablePin(FT_AUDIOENABLE_PIN); 
	FTImpl.DisplayOn(); 
	FTImpl.AudioOn();  	
	
	/* Set the Display & audio pins */
	return 0;
}



void setup()
{
	/* Initialize serial print related functionality */
	Serial.begin(9600);
	
	/* Set the Display Enable pin*/   
	Serial.println("--Start Application--");
	if(BootupConfigure())
	{
		Serial.println("Boot up error.");
	}
  	else
	{		
		FTImpl.DLStart();
		FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,FT_DISPLAYHEIGHT/2 - 20,26,FT_OPT_CENTER,"Trimming finished.");
		FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,FT_DISPLAYHEIGHT/2,26,FT_OPT_CENTER,"Final clock frequency(Hz):");
		FTImpl.Cmd_Number(FT_DISPLAYWIDTH/2,FT_DISPLAYHEIGHT/2 + 20,26,FT_OPT_CENTER,FTImpl.Read32(REG_FREQUENCY));
		FTImpl.DLEnd();
		FTImpl.Finish();
	}
	Serial.println("--End Application--");
}

/* Nothing in loop api */
void loop()
{
}
