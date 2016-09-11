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
* @file                           Blobs.ino
* @brief                          Sample application to demonstrat FT801 multitouch functionality
* @version                        0.1.0
* @date                           2014/04/10
*
*/

#include "SPI.h"
#include "Wire.h"
#include "FT_VM801B43.h"

/* Global object for transport */
FT801IMPL_SPI FTImpl(FT_CS_PIN,FT_PDN_PIN,FT_INT_PIN);

#define NBLOBS 64
#define FT_APP_BLOBS_COUNT  (0x01)
#define FT_APP_BLOBS_NUMTOUCH  (5)
#define OFFSCREEN   -16384

typedef struct Blobs
{
  int16_t x, y;
}Blobs;

typedef struct BlobsInst
{
	Blobs blobs[NBLOBS];
	uint8_t CurrIdx;
}BlobsInst;

int16_t BlobColor( struct BlobsInst *pBInst,int32_t TouchXY)
{
	byte r,g,b;
	uint8_t i=0,j=0;

	// if there is touch store the values 
	if(((TouchXY >> 16) != -32768) )
	{
		pBInst->blobs[pBInst->CurrIdx].x = (TouchXY>>16);	
		pBInst->blobs[pBInst->CurrIdx].y =  (TouchXY & 0xffff);	
	}
	else
	{		
		pBInst->blobs[pBInst->CurrIdx].x = OFFSCREEN;
		pBInst->blobs[pBInst->CurrIdx].y = OFFSCREEN;
	}
	
	//calculate the current index
	pBInst->CurrIdx = (pBInst->CurrIdx + 1) & (NBLOBS - 1);

	FTImpl.Begin(FT_POINTS); //begin points primitive
	FTImpl.ColorRGB(60,166,117); //set the color of points
	for ( i = 0; i < NBLOBS; i++) 
	{
		// Blobs fade away and swell as they age
		FTImpl.ColorA(i<<1);

		FTImpl.PointSize((68)+(i<<3)); //set the size of the point 

		// Random color for each blob, keyed from (blob_i + i)
		j = (pBInst->CurrIdx + i) & (NBLOBS - 1);

		// Draw it!
		if(pBInst->blobs[j].x != OFFSCREEN )
		{
			FTImpl.Vertex2f((pBInst->blobs[j].x)*16, (pBInst->blobs[j].y)*16); //display the point at (x,y) co ordinates
		}
	}
}

void BouncingPoints()
{
	uint32_t val[6];
	uint8_t i=0;
	uint8_t j=0;	
	BlobsInst gBlobsInst[FT_APP_BLOBS_NUMTOUCH],*pBInst;	
	FTImpl.SetCTouchMode(FT_CTOUCH_MODE_EXTENDED);	//set mode to extended for FT801
	pBInst = &gBlobsInst[0];
	
	for(j=0;j<FT_APP_BLOBS_NUMTOUCH;j++)
	{		
		for( i = 0; i < NBLOBS; i++) 
		{
			pBInst->blobs[i].x = OFFSCREEN;
			pBInst->blobs[i].y = OFFSCREEN;
		}
		pBInst++;
	}	
	
	do
	{
		FTImpl.DLStart(); //start the display list. Note DLStart and DLEnd are helper apis, Cmd_DLStart() and Display() can also be utilized.
	#if 1
		  FTImpl.ClearColorRGB(43,73,59);	// set the Clear color	 
		  FTImpl.Clear(1,1,1);  	 //clear the color component	 
		  FTImpl.ColorRGB(0,0,0);
		  FTImpl.BlendFunc(FT_SRC_ALPHA,FT_ONE);
		  FTImpl.ColorMask(1,1,1,0);
		
	#else
		  FTImpl.ClearColorRGB(255,255,255);
		  FTImpl.Clear(1,1,1);  		 
		  FTImpl.ColorRGB(0,0,0);
	#endif

		val[0] = FTImpl.Read32(REG_CTOUCH_TOUCH0_XY); //first touch	 
		val[1] = FTImpl.Read32(REG_CTOUCH_TOUCH1_XY); //second touch
		val[2] = FTImpl.Read32(REG_CTOUCH_TOUCH2_XY); //third touch		
		val[3] = FTImpl.Read32(REG_CTOUCH_TOUCH3_XY); //fourth touch	
		val[4] = FTImpl.Read16(REG_CTOUCH_TOUCH4_X); //fifth touch
		val[4] = val[4] << 16 | FTImpl.Read16(REG_CTOUCH_TOUCH4_Y);	

		// draw blobs according to the number of touches
		for(j=0;j<FT_APP_BLOBS_NUMTOUCH;j++)
		{
			BlobColor(&gBlobsInst[j],(int32_t)val[j]);
		}
		FTImpl.DLEnd();//end the display list
		FTImpl.Finish();//render the display list and wait for the completion of the DL			  
	}while(1);
}
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
			BouncingPoints();		
		}
		FTImpl.Exit();
	}
	Serial.println("--End Application--");
}

void loop()
{
	
}
/* Nothing beyond this */

















