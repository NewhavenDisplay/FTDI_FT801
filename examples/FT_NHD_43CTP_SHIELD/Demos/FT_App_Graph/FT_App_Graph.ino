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
* @file                           FT_App_Graph.ino
* @brief                          Sample application to demonstrat FT801's 2-touch functionality
* @version                        0.1.0
* @date                           2014/07/01
*
*/

#include "SPI.h"
#include "Wire.h"
#include "FT_NHD_43CTP_SHIELD.h"

/* Global object for transport */
FT801IMPL_SPI FTImpl(FT_CS_PIN,FT_PDN_PIN,FT_INT_PIN);

#define REPORT_FRAMES   0
#define SUBDIV  4
#define YY      (480 / SUBDIV)
float m_min = 10.0/65536;

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

class Transform {
public:
  float m, c;
  
  void set(int32_t x0, int16_t y0,
           int32_t x1, int16_t y1) {
    int32_t xd = x1 - x0;
    int16_t yd = y1 - y0;
    m = yd / (float)xd;
    if (m < m_min)
       m = m_min;
    c = y0 - m * x0;
  }
  void sset(int32_t x0, int16_t y0) {
    c = y0 - m * x0;
  }
  int16_t m2s(int32_t x) {
    return (int16_t)(m * x + c);
  }
  int32_t s2m(int16_t y) {
    return (int32_t)(y - c) / m;
  }
};
Transform xtf;  

static void read_extended(int16_t sx[5], int16_t sy[5])
{
  uint32_t sxy0, sxyA, sxyB, sxyC;
  sxy0 = FTImpl.Read32(REG_CTOUCH_TOUCH0_XY);
  sxyA = FTImpl.Read32(REG_CTOUCH_TOUCH1_XY);
  sxyB = FTImpl.Read32(REG_CTOUCH_TOUCH2_XY);
  sxyC = FTImpl.Read32(REG_CTOUCH_TOUCH3_XY);

  sx[0] = sxy0 >> 16;
  sy[0] = sxy0;
  sx[1] = sxyA >> 16;
  sy[1] = sxyA;
  sx[2] = sxyB >> 16;
  sy[2] = sxyB;
  sx[3] = sxyC >> 16;
  sy[3] = sxyC;

  sx[4] = FTImpl.Read16(REG_CTOUCH_TOUCH4_X);
  sy[4] = FTImpl.Read16(REG_CTOUCH_TOUCH4_Y);
}

// >>> [int16_t(65535*math.sin(math.pi * 2 * i / 1024)) for i in range(257)]
static const PROGMEM prog_int16_t sintab[257] = {
0, 402, 804, 1206, 1608, 2010, 2412, 2813, 3215, 3617, 4018, 4419, 4821, 5221, 5622, 6023, 6423, 6823, 7223, 7622, 8022, 8421, 8819, 9218, 9615, 10013, 10410, 10807, 11203, 11599, 11995, 12390, 12785, 13179, 13573, 13966, 14358, 14750, 15142, 15533, 15923, 16313, 16702, 17091, 17479, 17866, 18252, 18638, 19023, 19408, 19791, 20174, 20557, 20938, 21319, 21699, 22078, 22456, 22833, 23210, 23585, 23960, 24334, 24707, 25079, 25450, 25820, 26189, 26557, 26924, 27290, 27655, 28019, 28382, 28744, 29105, 29465, 29823, 30181, 30537, 30892, 31247, 31599, 31951, 32302, 32651, 32999, 33346, 33691, 34035, 34378, 34720, 35061, 35400, 35737, 36074, 36409, 36742, 37075, 37406, 37735, 38063, 38390, 38715, 39039, 39361, 39682, 40001, 40319, 40635, 40950, 41263, 41574, 41885, 42193, 42500, 42805, 43109, 43411, 43711, 44010, 44307, 44603, 44896, 45189, 45479, 45768, 46055, 46340, 46623, 46905, 47185, 47463, 47739, 48014, 48287, 48558, 48827, 49094, 49360, 49623, 49885, 50145, 50403, 50659, 50913, 51165, 51415, 51664, 51910, 52155, 52397, 52638, 52876, 53113, 53347, 53580, 53810, 54039, 54265, 54490, 54712, 54933, 55151, 55367, 55581, 55793, 56003, 56211, 56416, 56620, 56821, 57021, 57218, 57413, 57606, 57796, 57985, 58171, 58355, 58537, 58717, 58894, 59069, 59242, 59413, 59582, 59748, 59912, 60074, 60234, 60391, 60546, 60699, 60849, 60997, 61143, 61287, 61428, 61567, 61704, 61838, 61970, 62100, 62227, 62352, 62474, 62595, 62713, 62828, 62941, 63052, 63161, 63267, 63370, 63472, 63570, 63667, 63761, 63853, 63942, 64029, 64114, 64196, 64275, 64353, 64427, 64500, 64570, 64637, 64702, 64765, 64825, 64883, 64938, 64991, 65042, 65090, 65135, 65178, 65219, 65257, 65293, 65326, 65357, 65385, 65411, 65435, 65456, 65474, 65490, 65504, 65515, 65523, 65530, 65533, 65535
};

int16_t rsin(int16_t r, uint16_t th) {
  th >>= 6; // angle 0-123
  // return int16_t(r * sin((2 * M_PI) * th / 1024.));
  int16_t th4 = th & 511;
  if (th4 & 256)
    th4 = 512 - th4; // 256->256 257->255, etc
  uint16_t s = pgm_read_word_near(sintab + th4);
  int16_t p = ((uint32_t)s * r) >> 16;
  if (th & 512)
    p = -p;
  return p;
}

static void plot()
{
	FTImpl.StencilOp(FT_ZERO, FT_ZERO);
	FTImpl.Cmd_Gradient(0, 0, 0x202020, 0, 0x11f, 0x107fff);
	 
    int32_t mm[2];
    mm[0] = xtf.s2m(0);
    mm[1] = xtf.s2m(480);
    int16_t pixels_per_div = xtf.m2s(0x4000) - xtf.m2s(0);
    
    uint8_t fadeout;
    fadeout = min(255, max(0, (pixels_per_div - 32) * 16));  

	FTImpl.LineWidth((max(8, pixels_per_div >> 2)));
	for (int32_t m = mm[0] & ~0x3fff; m <= mm[1]; m += 0x4000) 
	{
		int16_t x = xtf.m2s(m);
		if ((-60 <= x) && (x <= 512)) 
		{
			uint8_t h = 3 * (7 & (m >> 14));

			FTImpl.ColorRGB(0,0,0);
			FTImpl.ColorA((h == 0) ? 192 : 64);
			FTImpl.Begin(FT_LINES);//begin lines primitive
			FTImpl.Vertex2f(x*16, 0);
			FTImpl.Vertex2f(x*16, 272*16);

			if (fadeout) 
			{
			  x -= 1;
			  FTImpl.ColorRGB(0xd0,0xd0,0xd0);
			  FTImpl.ColorA(fadeout);
			  FTImpl.Cmd_Number(x, 0, 26, FT_OPT_RIGHTX | 2, h);
			  FTImpl.Cmd_Text(x, 0, 26, 0, ":00");
			}
		}
	}
	
	FTImpl.ColorA(255);
	int16_t y[YY + 1];
	for (uint8_t i = 0; i < (YY + 1); i++) 
	{
		uint32_t x32 = xtf.s2m(SUBDIV * i);
		uint16_t x = (uint16_t)x32 + rsin(7117, x32);
		y[i] = 130 * 16 + rsin(1200, (217 * x32) >> 8) + rsin(700, 3 * x);
	}
	
	FTImpl.StencilOp(FT_INCR, FT_INCR);
	FTImpl.Begin(FT_EDGE_STRIP_B);
	for (int16_t i = 0; i < (YY + 1); i++)
		FTImpl.Vertex2f(16 * SUBDIV * i, y[i]);
	FTImpl.StencilFunc(FT_EQUAL, 1, 255);
	FTImpl.StencilOp(FT_KEEP, FT_KEEP);
	FTImpl.Cmd_Gradient(0, 0, 0xf1b608, 0, 220, 0x98473a);	

	FTImpl.StencilFunc(FT_ALWAYS, 1, 255);
	FTImpl.ColorRGB(0xE0,0xE0,0xE0);
	FTImpl.LineWidth(24);
	FTImpl.Begin(FT_LINE_STRIP);
	for (int16_t i = 0; i < (YY + 1); i++)
		FTImpl.Vertex2f(16 * SUBDIV * i, y[i]);	
		
	int16_t clock_r = min(24, pixels_per_div >> 2);
    if (clock_r > 4) 
	{
		FTImpl.ColorA(200);
		FTImpl.ColorRGB(0xff,0xff,0xff);
		uint16_t options = FT_OPT_NOSECS | FT_OPT_FLAT;
		if (clock_r < 10)
			options |= FT_OPT_NOTICKS;
		for (int32_t m = mm[0] & ~0x3fff; m <= mm[1]; m += 0x4000) 
		{
			int16_t x = xtf.m2s(m);
			uint8_t h = 3 * (3 & (m >> 14));
			if (x>= -1024)
				FTImpl.Cmd_Clock(x, 270 - 24, clock_r, options, h, 0, 0, 0);
		}
	}  		
}

void Graph()
{
	xtf.set(0, 0, 0x10000, 480);
	FTImpl.SetCTouchMode(FT_CTOUCH_MODE_EXTENDED);	//set mode to extended for FT801
	while(1)
	{
		if (REPORT_FRAMES) 
        {
			int16_t f = FTImpl.Read16(REG_FRAMES);
			static int16_t _f;
			printf("Error :%d" ,f - _f);
			_f = f;
		}
	         
		int16_t sx[5], sy[5];
		read_extended(sx, sy);
		
		static bool down[2]={0,0};
		static int32_t m[2]={0UL,0UL};
		
		for (uint8_t i = 0; i < 2; i++) 
		{
			if (sx[i] > -10 && !down[i]) 
			{
			  down[i] = 1;
			  m[i] = xtf.s2m(sx[i]);
			}
			if (sx[i] < -10)
			  down[i] = 0;
		}
		if (down[0] && down[1]) 
		{
			if (m[0] != m[1])
			  xtf.set(m[0], sx[0], m[1], sx[1]);      
		}
		else if (down[0] && !down[1])
			xtf.sset(m[0], sx[0]);
		else if (!down[0] && down[1])
			xtf.sset(m[1], sx[1]);	
		FTImpl.DLStart();
		plot();
		
		// display touches
		if (0) 
		{
			FTImpl.ColorRGB(0xff,0xff,0xff);
			FTImpl.LineWidth(8);
			FTImpl.Begin(FT_LINES);//begin lines primitive
			for (uint8_t i = 0; i < 2; i++) 
			{
				if (sx[i] > -10) 
				{
					FTImpl.Vertex2ii(sx[i], 0,0,0);
					FTImpl.Vertex2ii(sx[i], 272,0,0);
				}
			}
		}
		FTImpl.DLEnd();//end the display list
		FTImpl.Finish();//render the display list and wait for the completion of the DL	
	}
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
		Graph();
		FTImpl.Exit();
	}
	Serial.println("--End Application--");
}

void loop()
{
	
}
/* Nothing beyond this */

















