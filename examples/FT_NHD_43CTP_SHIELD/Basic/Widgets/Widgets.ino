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
* @file                           Widgets.ino
* @brief                          Sketch to display various widgets on FT801.
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

/* API to display various widgets at various locations on the screen */
void Widgets()
{
	int16_t xOffset,yOffset,cRadius,xDistBtwClocks;
	int16_t wScissor,hScissor,bWidth,bHeight;

	wScissor = ((FT_DISPLAYWIDTH - 4*10)/3);
	hScissor = ((FT_DISPLAYHEIGHT - 3*20)/2);
	bWidth = 80;
	bHeight = 30;

	xDistBtwClocks = FT_DISPLAYWIDTH/5;
	cRadius = xDistBtwClocks/2 - FT_DISPLAYWIDTH/64;

	/* Display list to display various inbuilt widgets at various locations */
	FTImpl.DLStart();//start the display list. Note DLStart and DLEnd are helper apis, Cmd_DLStart() and Display() can also be utilized.
	FTImpl.ClearColorRGB(64,64,64);//set the background color to grey
	FTImpl.Clear(1,1,1);
	FTImpl.ColorRGB(255,255,255);
	
	xOffset = xDistBtwClocks/2;
	yOffset = cRadius + 5;
	
	//display cock with blue as background color
	FTImpl.Cmd_BGColor( 0x0000ff);
	FTImpl.ColorRGB(0xff,0x00,0x00);
	FTImpl.Cmd_Clock( xOffset,yOffset,cRadius,0,10,10,5,10);//display clock with default 3d effect and the time being 10:10:05
	FTImpl.Cmd_Text(xOffset, (yOffset + cRadius + 6), 26, FT_OPT_CENTER, "Clock");//text info
	
	/* display gauge with green as background color and red as foreground color */
	FTImpl.Cmd_BGColor( 0x00ff00);
	FTImpl.ColorRGB(0xff,0x00,0x00);
	FTImpl.Cmd_FGColor(0xff0000);
	xOffset += xDistBtwClocks;
	FTImpl.Cmd_Gauge( xOffset,yOffset,cRadius,0,5,1,60,100);//draw gauge with range from 0 to 100 and current value being 60
	FTImpl.Cmd_Text(xOffset, (yOffset + cRadius + 6), 26, FT_OPT_CENTER, "Gauge");//text info
	
	/* Draw dial with 3d effect and the knob at 45 degrees from bottom down*/
	FTImpl.Cmd_FGColor(0x00ff00);
	FTImpl.Cmd_BGColor( 0x800000);
	xOffset += xDistBtwClocks;
	FTImpl.Cmd_Dial( xOffset, yOffset, cRadius, 0, 0.45*65535);//45% rotation from bottom
	FTImpl.Cmd_Text(xOffset, (yOffset + cRadius + 6), 26, FT_OPT_CENTER, "Dial");//text info
	
	/* Draw progress bar with 3d effect */
	FTImpl.ColorRGB(0xff, 0x00, 0x00);
	FTImpl.Cmd_BGColor( 0x000080);//set the background color to blue
	xOffset += xDistBtwClocks;
	yOffset -= cRadius;
	FTImpl.Cmd_Progress( xOffset, yOffset, cRadius*2.5, 10, 0, 19660, 65535);//progress bar with cRadious*2 width and 10 height, 19660/65535 % complete

	/* Draw scroll bar with 3d effect */
	FTImpl.Cmd_FGColor(0x00ff00);
	FTImpl.Cmd_BGColor( 0x800000);
	FTImpl.Cmd_Scrollbar( xOffset, yOffset + 20, cRadius*2.5, 10, 0, 20, 30, 100);//scroll bar with cRadious*2 width and 10 height, 30% status

	/* Draw slider with 3d effect */
	FTImpl.Cmd_FGColor(0x00ff00);
	FTImpl.Cmd_BGColor( 0x800000);
	FTImpl.Cmd_Slider( xOffset, yOffset + 40, cRadius*2.5, 10, 0, 50, 100);//slider bar with cRadious*2 width and 10 height, 50% status
	
	FTImpl.Cmd_FGColor(0x00ff00);
	FTImpl.Cmd_BGColor( 0x800000);
	FTImpl.Cmd_Toggle( xOffset, yOffset + 80, cRadius*2.5, 27, 0, 0.75*65535,"Start""\xff""Stop");//toggle with cRadious*2 width and 10 height, 75% status
	
	/* display gradient - diagonal gradient effect */
	xOffset = 10;
	yOffset = 20 + hScissor + 20;
	FTImpl.ScissorSize(wScissor,30);//use the scissor to cull and get the rectangle from gradient
	FTImpl.ScissorXY(xOffset,(yOffset + hScissor/2 - 15));//clip the display 
	FTImpl.Cmd_Gradient(xOffset,(yOffset + hScissor/2 - 15),0x606060,(xOffset + wScissor),(yOffset + hScissor/2 + 15),0x404080);//draw diagonal gradient
	FTImpl.Cmd_Text((xOffset + wScissor/2), (yOffset + hScissor/2), 28, FT_OPT_CENTER, "Heading 1");//text info

	/* revert back the scissor for the following widgets */
	FTImpl.ScissorSize(FT_MAX_DISPLAYWIDTH,FT_MAX_DISPLAYHEIGHT);//use the scissor to cull and get the rectangle from gradient
	FTImpl.ScissorXY(0,0);//set the starting coordinates of the scissor to default values

	/* button with 3d effect with gradient color */
	FTImpl.Cmd_FGColor(0x101010);
	FTImpl.Cmd_GradColor(0xff0000);
	xOffset += wScissor + 15;
	yOffset += (hScissor/2) - 15;
	FTImpl.Cmd_Button(xOffset,yOffset,bWidth,bHeight,27,0,"BUTTON");//draw button with 3d effect and gradient color
	
	/* display signed number with center option */
	FTImpl.ColorRGB(0x0b,0x07,0x21);
	xOffset += bWidth + 40;
	yOffset += 15;
	FTImpl.Cmd_Number(xOffset, yOffset, 29, FT_OPT_CENTER | FT_OPT_SIGNED, -1234);//display signed number wit center option
	
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
		Widgets();
	}
	Serial.println("--End Application--");
}

/* Nothing in loop api */
void loop()
{
}
