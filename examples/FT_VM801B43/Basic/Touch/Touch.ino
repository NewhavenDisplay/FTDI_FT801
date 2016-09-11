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
* @file                           Touch.ino
* @brief                          Sketch to demonstrate touch engine usage on FT801.
								  Tested platform version: Arduino 1.0.4 and later
* @version                        0.1.0
* @date                           2014/26/03
*
*/




/* Arduino standard includes */
#include "SPI.h"
#include "Wire.h"

/* Platform specific includes */
#include "FT_VM801B43.h"

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
/* API for calibration on FT801 */
void Calibrate()
{  
	/*************************************************************************/
	/* Below code demonstrates the usage of calibrate function. Calibrate    */
	/* function will wait untill user presses all the three dots. Only way to*/
	/* come out of this api is to reset the coprocessor bit.                 */
	/*************************************************************************/

	/* Construct the display list with grey as background color, informative string "Please Tap on the dot" followed by inbuilt calibration command */
	FTImpl.DLStart();
	FTImpl.ClearColorRGB(64,64,64);
	FTImpl.Clear(1,1,1);    
	FTImpl.ColorRGB(0xff, 0xff, 0xff);
	FTImpl.Cmd_Text((FT_DISPLAYWIDTH/2), (FT_DISPLAYHEIGHT/2), 27, FT_OPT_CENTER, "Please Tap on the dot");
	FTImpl.Cmd_Calibrate(0);
	
	/* Wait for the completion of calibration - either finish can be used for flush and check can be used */
	FTImpl.Finish();
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

void Touch()
{
	int32_t  wbutton,hbutton,tagval,tagoption;
	char StringArray[100],StringArray1[100];
	uint32_t ReadWord;
	int16_t xvalue,yvalue,pendown;
	sCTouchXY cTouchXY;
	/*************************************************************************/
	/* Below code demonstrates the usage of touch function. Display info     */
	/* touch raw, touch screen, touch tag, raw adc and resistance values     */
	/*************************************************************************/
	/* Perform the calibration */
	Calibrate();
	
	wbutton = FT_DISPLAYWIDTH/8;
	hbutton = FT_DISPLAYHEIGHT/8;
	FTImpl.SetCTouchMode(FT_CTOUCH_MODE_EXTENDED);	//set mode to extended for FT801
	while(1)
	{
		/* Read the touch screen xy and tag from GetCTouchXY API */
		FTImpl.GetCTouchXY(cTouchXY);
		/* Construct a screen shot with grey color as background, check constantly the touch registers,
		   form the infromative string for the coordinates of the touch, check for tag */		   
		FTImpl.DLStart(); //start the display list. Note DLStart and DLEnd are helper apis, Cmd_DLStart() and Display() can also be utilized.
		FTImpl.ClearColorRGB(64,64,64);  
		FTImpl.Clear(1,1,1);    //clear color component
		FTImpl.ColorRGB(0xff,0xff,0xff);
		FTImpl.TagMask(0);

		/* Draw informative text "Touch Screen XY0" with respective touch register values */
		StringArray[0] = '\0';
		strcat(StringArray,"Touch Screen XY0 (");
		yvalue = cTouchXY.y0;
		xvalue = cTouchXY.x0;
		Dec2Ascii(StringArray,(int32_t)xvalue);
		strcat(StringArray,",");
		Dec2Ascii(StringArray,(int32_t)yvalue);
		strcat(StringArray,")");
		FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2, 50, 26, FT_OPT_CENTER, StringArray);

		/* Draw informative text "Touch Screen XY1" with respective touch register values */
		StringArray[0] = '\0';
		strcat(StringArray,"Touch Screen XY1 (");
		xvalue=yvalue=0;
		yvalue = cTouchXY.y1;
		xvalue = cTouchXY.x1;
		Dec2Ascii(StringArray,(int32_t)xvalue);
		strcat(StringArray,",");
		Dec2Ascii(StringArray,(int32_t)yvalue);
		strcat(StringArray,")");
		FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2, 70, 26, FT_OPT_CENTER, StringArray);

		/* Draw informative text "Touch Screen XY2" with respective touch register values */
		StringArray[0] = '\0';
		strcat(StringArray,"Touch Screen XY2 (");
		yvalue = cTouchXY.y2;
		xvalue = cTouchXY.x2;		
		Dec2Ascii(StringArray,(int32_t)xvalue);
		strcat(StringArray,",");
		Dec2Ascii(StringArray,(int32_t)yvalue);
		strcat(StringArray,")");
		FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2, 90, 26, FT_OPT_CENTER, StringArray);

		/* Draw informative text "Touch Screen XY3" with respective touch register values */
		StringArray[0] = '\0';
		strcat(StringArray,"Touch Screen XY3 (");
		yvalue = cTouchXY.y3;
		xvalue = cTouchXY.x3;		
		Dec2Ascii(StringArray,(int32_t)xvalue);
		strcat(StringArray,",");
		Dec2Ascii(StringArray,(int32_t)yvalue);
		strcat(StringArray,")");
		FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2, 110, 26, FT_OPT_CENTER, StringArray);

		/* Draw informative text "Touch Screen XY4" with respective touch register values */
		StringArray[0] = '\0';
		StringArray1[0] = '\0';
		strcat(StringArray,"Touch Screen XY4 (");
		yvalue = cTouchXY.y4;
		xvalue = cTouchXY.x4;		
		Dec2Ascii(StringArray,(int32_t)xvalue);
		strcat(StringArray,",");
		Dec2Ascii(StringArray1,(int32_t)yvalue);
		strcat(StringArray1,")");
		strcat(StringArray,StringArray1);
		FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2, 130, 26, FT_OPT_CENTER, StringArray);

		StringArray[0] = '\0';
		strcat(StringArray,"Touch TAG (");
		ReadWord = FTImpl.Read( REG_TOUCH_TAG);
		Dec2Ascii(StringArray,ReadWord);
		strcat(StringArray,")");
		FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2, 170, 26, FT_OPT_CENTER, StringArray);
		tagval = ReadWord;

		/* demonstration of tag assignment and based on the tag report from FT801 change the button properties */
		FTImpl.Cmd_FGColor(0x008000);
		FTImpl.TagMask(1);

		FTImpl.Tag(13); 		//assign tag value 13 to the button
		tagoption = 0;
		if(13 == tagval)
		{
			tagoption = FT_OPT_FLAT;
		}
		FTImpl.Cmd_Button((FT_DISPLAYWIDTH/2) - (wbutton/2),(FT_DISPLAYHEIGHT*3/4) - (hbutton/2),wbutton,hbutton,26,tagoption,"Tag13");
		FTImpl.DLEnd();
		FTImpl.Finish();
	}
}
/* bootup the module and demonstrate touch functionality on screen */
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
		Touch();
		
	}	
	Serial.println("--End Application--");
}

/* Do nothing */
void loop()
{
}
