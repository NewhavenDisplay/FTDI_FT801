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
* @file                           MovingPoints.ino
* @brief                          Sketch to demonstrate multi-touch MovingPoints on FT801 platform.
                                                                                                                                  Tested platform version: Arduino 1.0.4 and later
* @version                        0.1.0
* @date                           2014/17/05
*
*/




/* Arduino standard includes */
#include "SPI.h"
#include "Wire.h"

/* Platform specific includes */
#include "FT_NHD_43CTP_SHIELD.h"

#define NO_OF_POINTS (64)
//#define NO_OF_POINTS (24)
int16_t SmallX[6], SmallY;
int16_t X[(NO_OF_POINTS)*4],Y[(NO_OF_POINTS)*4];
uint8_t Flag=1,t[NO_OF_POINTS*4];
int32_t val[5];


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
	delay(220);  // touch controller delay
	return 0;
}

void ColorSelection(int16_t k,int16_t i)
{
    if(k ==0)
    {
        if(i&1)
            FTImpl.ColorRGB(128,0,255);//purple
        else
            FTImpl.ColorRGB(255,43,149);//pink
    }
    else if(k ==1)
    {
        if(i&1)
            FTImpl.ColorRGB(255,0,0);//red
        else
            FTImpl.ColorRGB(0,255,0);//green
    }
    else if(k == 2)
    {
        if(i&1)
            FTImpl.ColorRGB(255,128,64);//orange
        else
            FTImpl.ColorRGB(0,255,255);//blue
    }
    else if(k == 3)
    {
        if(i&1)
            FTImpl.ColorRGB(128,0,0);//orange
        else
            FTImpl.ColorRGB(255,255,128);//blue
    }

}

int16_t linear(float p1,float p2,uint16_t t,uint16_t rate)
{
    float st  = (float)t/rate;
    return (int16_t)(p1+(st*(p2-p1)));
}

void PointsCalc(uint8_t TouchNo)
{
                                
    int16_t alpha=255,k=0,j=0,pointset = 0; 

    FTImpl.ColorRGB(0,0,0);
    FTImpl.PointSize(20*16);                              
    FTImpl.ColorA(120);        
    /* For total number of points calculate the offsets of movement */
    for(k=0;k<NO_OF_POINTS*4L;k++)
    {                                              
        {
            pointset = k/NO_OF_POINTS;
            if((t[k] > NO_OF_POINTS))
            {
            t[k]=0;
            X[k] = (val[pointset]>>16)&0xffff;
            Y[k] = (val[pointset] & 0xffff);
            }

            ColorSelection(pointset,k);

            if(X[k] != -32768)
            {
                int16_t tempdeltax = linear(X[k],SmallX[pointset], t[k],NO_OF_POINTS);
                int16_t tempdeltay  = linear(Y[k],SmallY, t[k],NO_OF_POINTS);
                FTImpl.Vertex2f(tempdeltax*16L,tempdeltay*16L);
            }                                                                              
            t[k]++;
        }
    }
}

void MovingPoints()
{
    
    uint16_t i=0,j=0,tn=0;
    
    /* Set the mode to extended */
    FTImpl.SetCTouchMode(FT_CTOUCH_MODE_EXTENDED);
    /* Initialization of all the points */
    
    /* initialize all the parameters */
    for(j=0;j<4;j++)
    {
        for(i=0;i<NO_OF_POINTS;i++)
        {
            t[i+j*NO_OF_POINTS] = i;
            X[i+j*NO_OF_POINTS] = -32768;
        }
    }
    
    /* Destination points */
    SmallX[1] = 180;SmallY = 20;
    for(i = 0; i<6;i++)
    {
        SmallX[i] = 130+i*50L;
    }
                    
      do
      {
          /* Read the 4 touch co ordinates */
          val[0] = FTImpl.Read32(REG_CTOUCH_TOUCH0_XY);      
          val[1] = FTImpl.Read32(REG_CTOUCH_TOUCH1_XY);
          val[2] = FTImpl.Read32(REG_CTOUCH_TOUCH2_XY);      
          val[3] = FTImpl.Read32(REG_CTOUCH_TOUCH3_XY);
          
          /* Screen shot construction */
          /* White background with moving points from touch coordinates to a fixed offset */
          FTImpl.StartTransferCmd();
          FTImpl.DLStart();
          FTImpl.ClearColorRGB(255,255,255);
          FTImpl.Clear(1,1,1);
                          
          FTImpl.Begin(FT_POINTS);
          
                      FTImpl.ColorRGB(255,0,0);           
                      PointsCalc(tn);
                      FTImpl.DLEnd();
                      FTImpl.Finish();
                      FTImpl.EndTransferCmd();
        
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
		FTImpl.DLStart();
		FTImpl.Cmd_Calibrate(0);
		FTImpl.DLEnd();
		FTImpl.Finish();
        MovingPoints();
    }
    Serial.println("--End Application--");
}

/* Nothing in loop api */
void loop()
{
}
