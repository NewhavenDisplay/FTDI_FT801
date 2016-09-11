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
* @file                           Bars.ino
* @brief                          Sample application to demonstrat FT801 multitouch functionality
* @version                        0.1.0
* @date                           2014/04/11
*
*/

#include "SPI.h"
#include "Wire.h"
#include "FT_NHD_43CTP_SHIELD.h"

/* Global object for transport */
FT801IMPL_SPI FTImpl(FT_CS_PIN,FT_PDN_PIN,FT_INT_PIN);
#define NO_OF_RECTS (5)
//*************************************************************************/
/* Below code demonstrates the usage of calibrate function. Calibrate    */
/* function will wait untill user presses all the three dots. Only way to*/
/* come out of this api is to reset the coprocessor bit.                 */
/*************************************************************************/
void Calibrate()
{
  FTImpl.DLStart(); //start the display list. Note DLStart and DLEnd are helper apis, Cmd_Start() and Display() can also be utilized.
  FTImpl.ClearColorRGB(64,64,64); 
  FTImpl.Clear(1,1,1);    
  FTImpl.ColorRGB(0xff, 0xff, 0xff);
  FTImpl.Cmd_Text((FT_DISPLAYWIDTH/2), (FT_DISPLAYHEIGHT/2), 27, FT_OPT_CENTER, "Please Tap on the dot"); //display string at the center of the screen using inbuilt font handle 29 
  FTImpl.Cmd_Calibrate(0);
  FTImpl.Finish(); //render the display list and wait for the completion of the DL
}
/* Api to bootup FT801, verify FT801 hardware and configure display/audio pins */
/* Returns 0 in case of success and 1 in case of failure */
int16_t BootupConfigure(){
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

int16_t BRy[5],BRx[5],My[5];
uint8_t E[5];
static uint8_t RectNo[5];

void BouncingSquaresCall(int16_t BRx,int16_t BRy,int16_t MovingRy,uint8_t SqNumber)
{
	int16_t MovingRx;
	int16_t R1,G1,B_1,R2,G2,B2;

	MovingRx = BRx;

	if(BRy<=60)BRy=60;
	if(BRy >= 260)BRy = 260;

	//different colours are set for the different rectangles
	if(SqNumber==0)
	{
		R1 = 63; G1 =72;  B_1=204;
		R2 = 0;  G2= 255; B2 = 255;
	}

	if(SqNumber==1)
	{
		R1 = 255; G1 =255;  B_1=0;
		R2 = 246;  G2= 89; B2 = 12;
	}

	if(SqNumber==2)
	{
		R1 = 255; G1 =0;  B_1=0;
		R2 = 237;  G2= 28; B2 = 36;
	}

	if(SqNumber==3)
	{
		R1 = 131; G1 = 171;  B_1=9;
		R2 = 8;  G2= 145; B2 = 76;
	}

	if(SqNumber == 4)
	{
		R1 = 141; G1 = 4;  B_1=143;
		R2 = 176;  G2= 3; B2 = 89;
	}

	// Draw the rectanles here
	FTImpl.Begin(FT_RECTS); //begin RECTS primitives
	FTImpl.ColorRGB(R1, G1, B_1); //set the color
	FTImpl.LineWidth(10 * 16);

	FTImpl.Vertex2f(BRx * 16, BRy * 16);
	FTImpl.Vertex2f((BRx+45) * 16, (260) * 16);

	FTImpl.ColorRGB(R2, G2, B2);
	FTImpl.LineWidth(5 * 16);
	FTImpl.Vertex2f(MovingRx * 16, MovingRy * 16);
	FTImpl.Vertex2f((MovingRx+45) * 16, (MovingRy+10) * 16);
}

int16_t MovingRect(int16_t BRy,int16_t MovingRy,uint8_t EndPtReach)
{	
	if(MovingRy <=0) 
	{
		EndPtReach =0;
		MovingRy =1;
	}
	if(EndPtReach == 1&& MovingRy >0)
	{
		MovingRy-=1;//the smaller rectangles are moved behind		
	}
	else if(EndPtReach == 0)
	{
		MovingRy+=2;//the smaller rectangles are moved forward slightly faster		
	}
	return MovingRy;
}

void RectangleCalc(uint8_t Arrayno)
{
	uint8_t Arr;
	int16_t MovingRy1,leap=0;
	if(RectNo[Arrayno]  ==1)
	{
		Arr = Arrayno;
		//the limits for the smaller rectangles forward and backward movement is set here
		if(My[Arr]  == 0 && My[Arr] +25 < BRy[Arr])
			E[Arr]=0;//inc
		else if(My[Arr]+25 >= BRy[Arr])
			E[Arr]=1;//dec
			
		// the smaller rectangles are moved accordingly according to the flags set above ion this function call
		delay(1);
		MovingRy1 = MovingRect(BRy[Arr],My[Arr],E[Arr]);
		if(BRy[Arr] == 0)MovingRy1 = 4;
			My[Arr] = MovingRy1;
		
		if(My[Arr] > (BRy[Arr]-15))
		{			
			leap = My[Arr] - BRy[Arr];
			My[Arr] = My[Arr] - (leap+25);				
		}			
	}
}
void CheckTouch(int16_t Tx1,int32_t val1)
{
	int16_t MovingRy1,i=0;
	uint8_t Arrayno=-1;

	// Check which rectangle is being touched according to the coordinates 
	if(Tx1 >= 60 && Tx1 <= 105)
		Arrayno =0;
	if(Tx1 >= 140 && Tx1 <= 185)
		Arrayno = 1;	
	if(Tx1 >= 220 && Tx1 <= 265)			
		Arrayno = 2;		
	if(Tx1 >= 300 && Tx1 <= 345)			
		Arrayno = 3;
	if(Tx1 >= 380 && Tx1 <= 425)
		Arrayno=4;

	//Set the flag for the rectangle being touched
	RectNo[Arrayno] =1;

	//store the vertices of the rectangle selected according to the flag
	if((val1>>16) != -32768)
	{
		BRx[Arrayno] = (val1>>16);
		BRy[Arrayno] = (val1 & 0xffff);
	}
	//limit the Bigger rectangle's height 
	if(BRy[Arrayno]<=60)
		BRy[Arrayno]=60;		
		
	//According to the bigger rectangle values move the smaller rectangles
	for( i=0;i<NO_OF_RECTS;i++)
	{
		RectangleCalc(i);
	}		
}


void  BouncingSquares()
{
	uint8_t i=0;
	int16_t RectX[5];
	int32_t val[5];

	//Calculate the X vertices where the five rectangles have to be placed
	for(i =1;i<5;i++)
	{	
		RectX[0] = 60; 		
		RectX[i] = RectX[i-1] + 80;
	}
	FTImpl.SetCTouchMode(FT_CTOUCH_MODE_EXTENDED);	//set mode to extended for FT801
	do
	{
		FTImpl.DLStart(); //start the display list. Note DLStart and DLEnd are helper apis, Cmd_DLStart() and Display() can also be utilized.
		FTImpl.ClearColorRGB(0,0,0);// set the Clear color	 
		FTImpl.Clear(1,1,1);  	 //clear the color component	 

		val[0] = FTImpl.Read32(REG_CTOUCH_TOUCH0_XY);	 //first touch
		val[1] = FTImpl.Read32(REG_CTOUCH_TOUCH1_XY);	 //second touch
		val[2] = FTImpl.Read32(REG_CTOUCH_TOUCH2_XY);	//third touch	
		val[3] = FTImpl.Read32(REG_CTOUCH_TOUCH3_XY);	//fourth touch
		val[4] = FTImpl.Read16(REG_CTOUCH_TOUCH4_X);	//fifth touch
		val[4] = val[4] << 16 | FTImpl.Read16(REG_CTOUCH_TOUCH4_Y);	

		//Check which rectangle is being touched using the coordinates and move the respective smaller rectangle
		for(i =0;i<NO_OF_RECTS;i++)
		{
			CheckTouch( val[i]>>16,val[i]);
			BouncingSquaresCall(RectX[i],BRy[i],My[i],i);
		}
		FTImpl.DLEnd();//end the display list
		FTImpl.Finish();//render the display list and wait for the completion of the DL			
	}while(1);
}

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
		while(1)
		{
			BouncingSquares();	
		}
		FTImpl.Exit();
	}
	Serial.println("--End Application--");
}

void loop()
{
	
}
/* Nothing beyond this */

















