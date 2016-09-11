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
* @file                           Circles.ino
* @brief                          Sample application to demonstrat FT801 multitouch functionality
* @version                        0.1.0
* @date                           2014/04/14
*
*/

#include "SPI.h"
#include "Wire.h"
#include "FT_VM801B43.h"

/* Global object for transport */
FT801IMPL_SPI FTImpl(FT_CS_PIN,FT_PDN_PIN,FT_INT_PIN);

#define NO_OF_CIRCLE (5)

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
double Tsq1[NO_OF_CIRCLE];
double C1X[NO_OF_CIRCLE];
double C1Y[NO_OF_CIRCLE];
double TouchX[NO_OF_CIRCLE];double TouchY[NO_OF_CIRCLE];


typedef  struct 
{
	uint8_t F[NO_OF_CIRCLE];
}TouchNoAndCircleNo;

TouchNoAndCircleNo TN[NO_OF_CIRCLE];

uint16_t ConcentricCircles(double C1,uint16_t R,uint16_t G,uint16_t B)
{
	int16_t C1X=0,Total=0;
	C1X = C1;
	FTImpl.ClearColorA(0);  
		
	/* Using ColorMask to disable color buffer updates, and
	set the BlendFunc to a value that writes 	incoming alpha 
	directly into the alpha buffer, by specifying a source blend factor
	of ONE */

	FTImpl.ColorMask(0,0,0,1);		 
	FTImpl.BlendFunc(FT_ONE, FT_ONE_MINUS_SRC_ALPHA);

	/*Draw the Outer circle */
	FTImpl.Begin(FT_POINTS) ;
	FTImpl.PointSize((uint16_t)(C1) * 16) ;		//outer circle
	FTImpl.Vertex2ii(240,136,0,0);		

	/* Draw the inner circle in a blend mode that clears any drawn 
	pixels to zero, so the source blend factor is ZERO */
	FTImpl.BlendFunc(FT_ZERO, FT_ONE_MINUS_SRC_ALPHA);

	FTImpl.PointSize((uint16_t)(C1-2) * 16);//inner circle
	FTImpl.Vertex2ii(240,136,0,0);		

	/*Enable the color Mask and the source blend factor is set to DST ALPHA, so the
	transparency values come from the alpha buffer*/
	FTImpl.ColorMask(1,1,1,0);
	FTImpl.BlendFunc(FT_DST_ALPHA, FT_ONE);

	/* draw the outer circle again with the color mask enabled and the blend factor 
	is set to SRC_ALPHA */
	FTImpl.ColorRGB(R ,G ,B); 
	FTImpl.PointSize((uint16_t)(C1) * 16) ;		
	FTImpl.Vertex2ii(240,136,0,0);		

	FTImpl.BlendFunc(FT_SRC_ALPHA, FT_ONE_MINUS_SRC_ALPHA);
	FTImpl.End();//end the edge strip primitve
}

void TouchPoints(int16_t C1X,int16_t C1Y,uint8_t i)
{
	/* Draw the five white circles for the Touch areas with their rescpective numbers*/
	FTImpl.Begin(FT_POINTS) ;
	FTImpl.PointSize((14) * 16) ;
	FTImpl.ColorRGB(255 ,255 ,255); 
	FTImpl.Vertex2ii(C1X,C1Y,0,0);	
	FTImpl.ColorRGB(155 ,155 ,0); 
	FTImpl.Cmd_Number(C1X,C1Y,29,FT_OPT_CENTERX|FT_OPT_CENTERY,i);
}

void PlotXY()
{

	uint8_t i=0;
	uint16_t PlotHt=0,PlotWth=0,X=0,Y=0;

	PlotHt = FT_DISPLAYHEIGHT/10;
	PlotWth = FT_DISPLAYWIDTH/10;

	FTImpl.ColorRGB(36 ,54 ,125); 
	/* Horizontal Lines */
	for(i=1;i<11;i++)
	{
		Y = i* PlotHt;
		FTImpl.Begin(FT_LINES);
		FTImpl.LineWidth(1 * 16) ;		
		FTImpl.Vertex2f(0,Y*16);
		FTImpl.Vertex2f(FT_DISPLAYWIDTH*16,Y*16);
	}
	/* Vertical Lines */
	for(i=1;i<11;i++)
	{
		X = i* PlotWth;
		FTImpl.Begin(FT_LINES);
		FTImpl.LineWidth(1 * 16) ;		
		FTImpl.Vertex2f(X*16,0);
		FTImpl.Vertex2f(X*16,FT_DISPLAYHEIGHT*16);
	}
	FTImpl.End();//end the lines primitve
}

uint8_t CheckCircleTouchCood(int32_t val,uint8_t TouchNum,uint8_t i)
{
	double CX=0;
	double CY=0;
	uint8_t j=0,AllClear=0;

	if((val >> 16) != -32768)
	{
		CX = (val>>16);		  
		CY = (val & 0xffff);

		for(j=0; j<NO_OF_CIRCLE; j++)
		{
			if( TN[TouchNum].F[j] == 0)
			{
				if(AllClear!= 10)
					AllClear =j;
			}
			else 				
				AllClear =10;			
		}
		if(AllClear!=10) 
			AllClear = 1;

		if(AllClear == 1)
		{
			if( TN[TouchNum].F[i] !=1)
			{
				/* check which circle being touched falls according to its coordinates and set its flag*/
				if((CX > (C1X[i]-15))&&(CX < (C1X[i]+15) ) )
				{
					if((CY > (C1Y[i] -30))&&(CY < C1Y[i]+30))
					{
						C1X[i] = CX;
						C1Y[i] = CY;
						TN[TouchNum].F[i] =1;						
				  }
				}
			}
			AllClear =0;
		}		 			
		if(TN[TouchNum].F[i] == 1) 
		{
			C1X[i] = CX;
			C1Y[i] = CY;
		}
	}
	else 
		TN[TouchNum].F[i]=0;
}

uint16_t CirclePlot(uint16_t X,uint16_t Y,uint8_t Val)
{
	double Xsq1[NO_OF_CIRCLE],Ysq1[NO_OF_CIRCLE];
	/* calculate the radius of the  circles*/
	Xsq1[Val]= (X - (FT_DISPLAYWIDTH/2))*(X- (FT_DISPLAYWIDTH/2));
	Ysq1[Val] = (Y - (FT_DISPLAYHEIGHT/2))*(Y - (FT_DISPLAYHEIGHT/2));
	Tsq1[Val] = (Xsq1[Val] + Ysq1[Val]);
	Tsq1[Val] = sqrt(Tsq1[Val]);
	return Tsq1[Val];
}

void StoreTouch(int32_t Touchval,uint8_t TouchNo)
{
	 if(Touchval >> 16 != -32768)
	 {
			TouchX[TouchNo] = Touchval>>16;
			TouchY[TouchNo] = Touchval&0xffff;
	 }
	
}
void Circles()
{
	uint8_t CircleNo[NO_OF_CIRCLE];
	uint8_t i=0,j=0;
	int32_t val[NO_OF_CIRCLE];

	FTImpl.SetCTouchMode(FT_CTOUCH_MODE_EXTENDED);	//set mode to extended for FT801
	/* calculate the intital radius of the circles before the touch happens*/
	Tsq1[0] = 50;C1X[0] = 190;C1Y[0] = 136;
	for(i = 1;i<NO_OF_CIRCLE;i++)
	{
		Tsq1[i] = Tsq1[i-1] +30;
		C1X[i] = C1X[i-1] - 30;
		C1Y[i] = 136;
	}
	do
	{
		FTImpl.DLStart(); //start the display list. Note DLStart and DLEnd are helper apis, Cmd_DLStart() and Display() can also be utilized.
		FTImpl.Clear(1,1,1);  	 //clear the color component	
		FTImpl.ClearColorRGB(255,255,255);// set the Clear color	

		/* values of the five touches are stored here */
		val[0] = FTImpl.Read32(REG_CTOUCH_TOUCH0_XY);	 //first touch
		val[1] = FTImpl.Read32(REG_CTOUCH_TOUCH1_XY);	 //second touch
		val[2] = FTImpl.Read32(REG_CTOUCH_TOUCH2_XY);	//third touch	
		val[3] = FTImpl.Read32(REG_CTOUCH_TOUCH3_XY);	//fourth touch
		val[4] = FTImpl.Read16(REG_CTOUCH_TOUCH4_X);	//fifth touch
		val[4] = val[4] << 16 | FTImpl.Read16(REG_CTOUCH_TOUCH4_Y);			  	
		
		for(i=0;i<NO_OF_CIRCLE;i++)
		{
			StoreTouch(val[i],i);
		}
		  
		/* The plot is drawn here */
		 PlotXY();

		 /* check which circle has been touched based on the coordinates and store the number of the circle touched*/
		 for(i=0;i<NO_OF_CIRCLE;i++)
		 {
			CheckCircleTouchCood(val[0],0,i);				
			CheckCircleTouchCood(val[1],1,i);				
			CheckCircleTouchCood(val[2],2,i);				
			CheckCircleTouchCood(val[3],3,i);				
			CheckCircleTouchCood(val[4],4,i);	
		 }

		/* calculate the radius of each circle according to the touch of each individual circle */
		for(i =0;i<NO_OF_CIRCLE;i++)
		{
			Tsq1[i] = CirclePlot(C1X[i],C1Y[i],i);
		}

		/* with the calculated radius draw the circles as well as the Touch points */
		for(i =0;i<NO_OF_CIRCLE;i++)
		{
			ConcentricCircles(Tsq1[i],255,0,0);
			TouchPoints(C1X[i],C1Y[i],i+1);
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
			Circles();	
		}
		FTImpl.Exit();
	}
	Serial.println("--End Application--");
}

void loop()
{
	
}
/* Nothing beyond this */

















