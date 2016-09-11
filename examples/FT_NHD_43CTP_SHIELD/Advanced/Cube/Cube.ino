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
* @file                           Cube.ino
* @brief                          Tested platform version: Arduino 1.0.4 and later
* @version                        0.1.0
* @date                           2014/21/03
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

void Cube()
{
    uint32_t points[6*5],x,y,i,z;
    int16_t xoffset,yoffset,CubeEdgeSz;

// color vertices
    uint16_t colors[6][3] = { 255,0,0,
                                255,0,150,
                                0,255,0,
                                110,120,110,
                                0,0,255,
                                128,128,0
	                            };

// Cube dimention is of 100*100*100
	CubeEdgeSz = 100;
	xoffset = (FT_DISPLAYWIDTH/2 - CubeEdgeSz);
	yoffset=((FT_DISPLAYHEIGHT - CubeEdgeSz)/2);	


// xy plane(front)
       points[0] = VERTEX2F(xoffset*16,yoffset*16);
       points[1] = VERTEX2F((xoffset+CubeEdgeSz)*16,yoffset*16);
       points[2] = VERTEX2F((xoffset+CubeEdgeSz)*16,(yoffset+CubeEdgeSz)*16);
       points[3] = VERTEX2F(xoffset*16,(yoffset+CubeEdgeSz)*16);
       points[4] = points[0];

//yz plane (left)
       x = (xoffset+(CubeEdgeSz/2));            //     xoff+w/2
       y = (yoffset-(CubeEdgeSz/2));            //     yoff-h/2

       points[5] = points[0];     
       points[6] = VERTEX2F(x*16,y*16);
       points[7] = VERTEX2F(x*16,(y+CubeEdgeSz)*16);
       points[8] = points[3];
       points[9] = points[5];

//xz plane(top)
       points[10] = points[0];
       points[11] = points[1];  
       points[12] = VERTEX2F((x+CubeEdgeSz)*16,(y)*16);
       points[13] = points[6];
       points[14] = points[10];

//xz plane(bottom)
       points[15] = points[3];
       points[16] = points[2];
       points[17] = VERTEX2F((x+CubeEdgeSz)*16,(y+CubeEdgeSz)*16);
       points[18] = points[7];
       points[19] = points[15];

//yz plane (right)
       points[20] = points[2];
       points[21] = points[17];
       points[22] = points[12];
       points[23] = points[1];
       points[24] = points[20];

//yz plane (back)
       points[25] = points[6];
       points[26] = points[7];
       points[27] = points[17];
       points[28] = points[12];
       points[29] = points[25];

       
    FTImpl.DLStart();	//start the display list.Note DLStart and DLEnd are helper apis, Cmd_DLStart() and Display() can also be utilized.	
    FTImpl.ClearColorRGB(255,255,255); // set the Clear color to white
    FTImpl.Clear(1,1,1); //clear the color component
    FTImpl.LineWidth(16); //drawing lines
    FTImpl.ColorRGB(255,255,255);   //set bitmap color

// Draw a cube 
    FTImpl.StencilOp(FT_INCR,FT_INCR);
    FTImpl.ColorA(192); //set alpha to 192
    for(z=0;z<6;z++)
    {      
		FTImpl.Clear(0,1,1);
		FTImpl.ColorMask(0,0,0,0);//mask all the colors and draw one surface
		FTImpl.StencilFunc(FT_ALWAYS,0,255);//stencil function to increment all the values
        FTImpl.Begin(FT_EDGE_STRIP_L); //start drawing edge strip L
        for(i = 0;i<5;i++)
        {
			FTImpl.WriteCmd(points[z*5 + i]);
        }
        FTImpl.End();
		/* set the color and draw a strip */
		FTImpl.ColorMask(1,1,1,1);
		FTImpl.StencilFunc(FT_EQUAL,1,255);		
        FTImpl.ColorRGB(colors[z][0],colors[z][1],colors[z][2]);
		FTImpl.Begin(FT_RECTS); //start drawing rectangle
		FTImpl.Vertex2ii(xoffset,0,0,0);
		FTImpl.Vertex2ii(xoffset + CubeEdgeSz*2,yoffset + CubeEdgeSz*2,0,0);
		FTImpl.End();
    }
	/* Download the DL into DL RAM */
	FTImpl.DLEnd();  //end the display list
    FTImpl.Finish(); //render the display list and wait for the completion of the DL
	delay(1000);
}
/* bootup the module and display primitives on screen */
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
		Cube();
	}
	Serial.println("--End Application--");
}

/* Nothing in loop api */
void loop()
{
}