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
* @file                           Polygon.ino
* @brief                          Sketch to display polygons on FT801.
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

/* optimized sine table */
PROGMEM  prog_uint16_t sintab[] = {
0, 402, 804, 1206, 1607, 2009, 2410, 2811, 3211, 3611, 4011, 4409, 4807, 5205, 5601, 5997, 6392, 
6786, 7179, 7571, 7961, 8351, 8739, 9126, 9511, 9895, 10278, 10659, 11038, 11416, 11792, 12166, 12539,
12909, 13278, 13645, 14009, 14372, 14732, 15090, 15446, 15799, 16150, 16499, 16845, 17189, 17530, 17868,
18204, 18537, 18867, 19194, 19519, 19840, 20159, 20474, 20787, 21096, 21402, 21705, 22004, 22301, 22594, 
22883, 23169, 23452, 23731, 24006, 24278, 24546, 24811, 25072, 25329, 25582, 25831, 26077, 26318, 26556, 26789, 
27019, 27244, 27466, 27683, 27896, 28105, 28309, 28510, 28706, 28897, 29085, 29268, 29446, 29621, 29790, 29955, 
30116, 30272, 30424, 30571, 30713, 30851, 30984, 31113, 31236, 31356, 31470, 31580, 31684, 31785, 31880, 31970, 
32056, 32137, 32213, 32284, 32350, 32412, 32468, 32520, 32567, 32609, 32646, 32678, 32705, 32727, 32744, 32757, 
32764, 32767, 32764};
/* api to compute sine value, theta from 0 to 360 is between 0 to 65535 */
int16_t SAMAPP_qsin(uint16_t a)
{
  uint8_t f;
  int16_t s0,s1;

  if (a & 32768)
    return -SAMAPP_qsin(a & 32767);
  if (a & 16384)
      a = 32768 - a;
  f = a & 127;
  s0 = pgm_read_word(sintab + (a >> 7));
  s1 = pgm_read_word(sintab + (a >> 7) + 1);
  return (s0 + ((int32_t)f * (s1 - s0) >> 7));
}

/* api to compute cos value */
int16_t SAMAPP_qcos(uint16_t a)
{
  return (SAMAPP_qsin(a + 16384));
}

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

/* API to draw regular polygon based on circle and angle computation */
void PolygonRegular(int16_t edges,int16_t radius,int16_t xoffset,int16_t yoffset)
{
	int16_t i,x[10],y[10];//temp parameters to retain the coordinates - make this array is changed wrt max value of edges
	
	for(i=0;i<edges;i++)
	{
		x[i] = xoffset*16L + ((int32_t)radius*SAMAPP_qcos(65535*i/edges)/2048L);
		y[i] = yoffset*16L - ((int32_t)radius*SAMAPP_qsin(65535*i/edges)/2048L);
	}
	/* Polygons are constructed using stencil and line strips */
	FTImpl.ColorMask(0,0,0,0);//make sure no points are draws on the screen and only stencil values are modified
	FTImpl.Clear(0,1,0);//clear the stencil values to 0	
	FTImpl.StencilOp(FT_INCR,FT_INCR);//increment the stencil values
	FTImpl.Begin(FT_EDGE_STRIP_L);//make sure only one direction is set in order to set the condition
	for(i = 0;i<edges;i++)
	{
		FTImpl.Vertex2f(x[i],y[i]);
	}
	FTImpl.Vertex2f(x[0],y[0]);//first coordinate to complete the polygon	
	FTImpl.End();//end the edge strip primitve
	
	/* now draw a polygon wrt stencil value 1 */
	FTImpl.ColorMask(1,1,1,1);//enable all the colors
	FTImpl.StencilFunc(FT_EQUAL,1,255);
	FTImpl.Begin(FT_EDGE_STRIP_L);//draw a edge strip for the whole frame to make sure all the points are covered
	FTImpl.Vertex2ii(FT_DISPLAYWIDTH,0,0,0);
	FTImpl.Vertex2ii(FT_DISPLAYWIDTH,FT_DISPLAYHEIGHT,0,0);
	FTImpl.End();

	/* below display list demonstrates the usage of line strips to remove the antializaing effect */
	FTImpl.StencilFunc(FT_ALWAYS,0,255);
	FTImpl.LineWidth(2*16);//set the line width to 2 pixels
	FTImpl.Begin(FT_LINE_STRIP);//use line strips to remove the antializing effect from the hardware
	for(i = 0;i<edges;i++)
	{
		FTImpl.Vertex2f(x[i],y[i]);
	}
	FTImpl.Vertex2f(x[0],y[0]);//first coordinate to complete the polygon	
	FTImpl.End();//end the edge strip primitve
	
	
}
/* API to display polygon on screen */
void Polygon()
{
	int16_t radius = FT_DISPLAYWIDTH/8,xoffset,yoffset,i,edges;
	/* change the angle, circle size and coordinates for experimentation */
	/* Display list to display "Hello World" at the centre of display area */
	FTImpl.DLStart();//start the display list. Note DLStart and DLEnd are helper apis, Cmd_DLStart() and Display() can also be utilized.

	/* first draw a triangle, coordinates from Cartesian system can be (X+r*cos(t),Y-r*sin(t)), where r is radius and t is theta */	
	xoffset = (FT_DISPLAYWIDTH/4) - radius;
	yoffset = FT_DISPLAYHEIGHT/2;
	edges = 3;
	FTImpl.ColorRGB(255, 0, 0);
	PolygonRegular(edges,radius,xoffset,yoffset);

	xoffset = FT_DISPLAYWIDTH*2/4;
	edges = 5;
	FTImpl.ColorRGB(0, 255, 0);
	PolygonRegular(edges,radius,xoffset,yoffset);
	
	xoffset = (FT_DISPLAYWIDTH*3/4) + radius;
	edges = 6;
	FTImpl.ColorRGB(0, 0, 255);
	PolygonRegular(edges,radius,xoffset,yoffset);
	
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
		Polygon();
	}
	Serial.println("--End Application--");
}

/* Nothing in loop api */
void loop()
{
}
