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
* @file                           KeyBoard.ino
				  This sketch demonstrates the usage of keys function,primitives, widgets and TAG interrupt.
* @brief                          Tested platform version: Arduino 1.0.4 and later
* @version                        0.1.0
* @date                           2014/21/03
*
*/




/* Arduino standard includes */
#include "SPI.h"
#include "Wire.h"

/* Platform specific includes */
#include "FT_VM801B43.h"

/*define interrupt register bits*/
#define TAG_INTERRUPT 0x04
#define TOUCH_INTERRUPT 0x02

/* Global object for FT801 Implementation */
FT801IMPL_SPI FTImpl(FT_CS_PIN,FT_PDN_PIN,FT_INT_PIN);

uint8_t touchedTag=0;

/* API to boot up graphics controller, verify the hardware and configure display/audio pins */
/* Returns 0 in case of success and 1 in case of failure */
int16_t BootupConfigure()
{
	uint32_t chipid = 0;
	FTImpl.Init(FT_DISPLAY_RESOLUTION);//configure the display resolution

	delay(20);//for safer side
	
	/* Identify the chip */
	chipid = FTImpl.Read32(FT_ROM_CHIPID);
	if(FT801_CHIPID != chipid)
	{
		Serial.print("Error in chip id read ");
		Serial.println(chipid,HEX);
		return 1;
	}

	/* Set the Display & audio pins */
	FTImpl.SetDisplayEnablePin(FT_DISPENABLE_PIN);
    FTImpl.SetAudioEnablePin(FT_AUDIOENABLE_PIN); 
	
	/*Turn on display and audio*/
	FTImpl.DisplayOn(); 	
	FTImpl.AudioOn();  	
	return 0;
}

/* API to demonstrate the TAG interrupt*/
void interruptHandler(){
	while(1)
	{
		/* Check the GPIO interrupt pin - 0 or LOW means interrupt(s) have occurred, 1 or HIGH means no interrupt detected*/
		if (0 == digitalRead(FT_INT_PIN))
		{
			interruptService();
			break;
		}
		
		/* Delay or perform some other works*/
		//delay(10);
	}
}

/*Check the interrupt and take the appropriate action*/
void interruptService(){
	uint8_t interruptFlags;
	
	/*Read register REG_INT_FLAGS, the register is clear on read.*/
	interruptFlags = FTImpl.Read(REG_INT_FLAGS);
	
	/*Check the interrupt sources to see if it's the TAG source.  The TAG interrupt will occur two times, one for the initial touch and the other one is after the last touch.*/ 
	if((interruptFlags&TAG_INTERRUPT) == TAG_INTERRUPT){
		touchedTag = FTImpl.Read(REG_TOUCH_TAG);
	}
}


/*************************************************************************/
/* Below code demonstrates the usage of calibrate function. Calibrate    */
/* function will wait till user have pressed all three dots.  Only way to*/
/* come out of this API is to reset the coprocessor.                     */
/*************************************************************************/
void Calibrate()
{
  FTImpl.DLStart(); //start the display list. Note DLStart and DLEnd are helper apis, Cmd_DLStart() and Display() can also be utilized.
  FTImpl.ClearColorRGB(64,64,64); 
  FTImpl.Clear(1,1,1);    
  FTImpl.ColorRGB(0xff, 0xff, 0xff);
  FTImpl.Cmd_Text((FT_DISPLAYWIDTH/2), (FT_DISPLAYHEIGHT/2), 27, FT_OPT_CENTER, "Please Tap on the dot"); //display string at the center of the screen using inbuilt font handle 29 
  FTImpl.Cmd_Calibrate(0);
  FTImpl.Finish(); //render the display list and wait for the completion of the DL
}

void Keys_Interactive()
{
/**************************************************************************/
/* Below code demonstrates the usage of keys function. keys function      */
/* draws buttons with characters given as input parameters. Keys support  */
/* Flat and 3D effects, draw at (x,y) coordinates or center of the display*/
/* , inbuilt or custom fonts can be used for key display                  */
/**************************************************************************/

	int16_t TextFont = 29,ButtonW = 30,ButtonH = 30,yBtnDst = 5,yOffset,xOffset;
#define SAMAPP_COPRO_WIDGET_KEYS_INTERACTIVE_TEXTSIZE (512)
              char DispText[SAMAPP_COPRO_WIDGET_KEYS_INTERACTIVE_TEXTSIZE];
              uint8_t CurrTag = 0, lastTag=0;
              int32_t CurrTextIdx = 0;
	boolean firstTime=true;
    while(1)
    {
	   /*Display the screen then wait for user inputs.*/
	   if(!firstTime)interruptHandler();//blocking call for the interrupt
	   else firstTime=false; 

		/*Assign sensed tag*/
	   if(touchedTag!=0)CurrTag=touchedTag;
	   else CurrTag='|';
	   
	   
		/*Assign current character to the end of the string*/
	   DispText[CurrTextIdx] = CurrTag;
       DispText[CurrTextIdx + 1] = '\0';
	   
		/*Note the character after the touch has been lifted*/
	   if(lastTag!=0 && !FTImpl.IsPendown()){
		DispText[CurrTextIdx] = lastTag;
		DispText[CurrTextIdx+1] = '|';
		DispText[CurrTextIdx+2] = '\0';
		/*Advances the array index for the new character*/
			CurrTextIdx++;
            /* clear all the characters after certain amount of characters have pressed */
            if(CurrTextIdx > 24)
            {
                CurrTextIdx = 0;
				DispText[0] = '\0';
            }
		}

		FTImpl.DLStart(); //start the display list. Note DLStart and DLEnd are helper APIs, Cmd_DLStart() followed by Display() command yield the same result.
		FTImpl.ClearColorRGB(64,64,64); // set the drawing color 
		FTImpl.Clear(1,1,1);    //clear the color, stencil, and tag buffers
		FTImpl.ColorRGB(0xff, 0xff, 0xff); //set the screen color to white


       FTImpl.TagMask(0);
       FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2, 40, TextFont, FT_OPT_CENTER, DispText);//text info
       FTImpl.TagMask(1);


       yOffset = 80 + 10;
       /* Construct a simple keyboard - note that the tags associated with the keys are the character values given in the arguments */
       FTImpl.Cmd_FGColor(0x404080);
       FTImpl.Cmd_GradColor(0x00ff00);
       FTImpl.Cmd_Keys(yBtnDst, yOffset, 10*ButtonW, ButtonH, TextFont, (FT_OPT_CENTER | CurrTag), "qwertyuiop");
       FTImpl.Cmd_GradColor(0x00ffff);
       yOffset += ButtonH + yBtnDst;
       FTImpl.Cmd_Keys(yBtnDst, yOffset, 10*ButtonW, ButtonH, TextFont, (FT_OPT_CENTER | CurrTag), "asdfghjkl");
       FTImpl.Cmd_GradColor(0xffff00);
       yOffset += ButtonH + yBtnDst;
       FTImpl.Cmd_Keys(yBtnDst, yOffset, 10*ButtonW, ButtonH, TextFont, (FT_OPT_CENTER | CurrTag), "zxcvbnm");//highlight the z button
       yOffset += ButtonH + yBtnDst;
       FTImpl.Tag(' ');
       if(' ' == CurrTag)
       {
			FTImpl.Cmd_Button(yBtnDst, yOffset, 10*ButtonW, ButtonH, TextFont, FT_OPT_CENTER | FT_OPT_FLAT, " ");//mandatory to give '\0' at the end to make sure coprocessor understands the end of the string
       }
       else
       {
            FTImpl.Cmd_Button(yBtnDst, yOffset, 10*ButtonW, ButtonH, TextFont, FT_OPT_CENTER, " ");//mandatory to give '\0' at the end to make sure coprocessor understands the end of the string
       }
       yOffset = 80 + 10;
       FTImpl.Cmd_Keys( 11*ButtonW, yOffset, 3*ButtonW, ButtonH, TextFont, (0 | CurrTag), "789");
       yOffset += ButtonH + yBtnDst;
       FTImpl.Cmd_Keys( 11*ButtonW, yOffset, 3*ButtonW, ButtonH, TextFont, (0 | CurrTag), "456");
       yOffset += ButtonH + yBtnDst;
       FTImpl.Cmd_Keys( 11*ButtonW, yOffset, 3*ButtonW, ButtonH, TextFont, (0 | CurrTag), "123");
       yOffset += ButtonH + yBtnDst;
       FTImpl.ColorA(255);
       FTImpl.Cmd_Keys( 11*ButtonW, yOffset, 3*ButtonW, ButtonH, TextFont, (0 | CurrTag), "0.");//highlight button 0
	   FTImpl.DLEnd(); //end the display list
	   FTImpl.Finish(); //render the display list and wait for the completion of the DL
       lastTag=touchedTag;
    } 
}


/* boot up the module and display primitives on screen */
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
		/*Enable interrupt and only allow interrupt sources in the mask where the bit is a 1 to go through*/
		FTImpl.EnableInterrupts(1,TAG_INTERRUPT|TOUCH_INTERRUPT);
			
		Calibrate();
		Keys_Interactive();
	}
	Serial.println("--End Application--");
}

/* Nothing in loop api */
void loop()
{
}