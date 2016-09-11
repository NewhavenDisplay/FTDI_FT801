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
* @file                           FT_App_Polygon.ino
* @brief                          Sample application to demonstrat FT801 multi touch functionality 
* @version                        0.1.0
* @date                           2014/04/01
*
*/

#include "SPI.h"
#include "Wire.h"
#include "FT_NHD_43CTP_SHIELD.h"

/* Global object for transport */
FT801IMPL_SPI FTImpl(FT_CS_PIN,FT_PDN_PIN,FT_INT_PIN);

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

// >>> [int16_t(round(1024 * math.atan(i / 256.) / math.pi)) for i in range(256)]
static const PROGMEM prog_uchar atan8[] = {
0,1,3,4,5,6,8,9,10,11,13,14,15,17,18,19,20,22,23,24,25,27,28,29,30,32,33,34,36,37,38,39,41,42,43,44,46,47,48,49,51,52,53,54,55,57,58,59,60,62,63,64,65,67,68,69,70,71,73,74,75,76,77,79,80,81,82,83,85,86,87,88,89,91,92,93,94,95,96,98,99,100,101,102,103,104,106,107,108,109,110,111,112,114,115,116,117,118,119,120,121,122,124,125,126,127,128,129,130,131,132,133,134,135,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,177,178,179,180,181,182,183,184,185,186,187,188,188,189,190,191,192,193,194,195,195,196,197,198,199,200,201,201,202,203,204,205,206,206,207,208,209,210,211,211,212,213,214,215,215,216,217,218,219,219,220,221,222,222,223,224,225,225,226,227,228,228,229,230,231,231,232,233,234,234,235,236,236,237,238,239,239,240,241,241,242,243,243,244,245,245,246,247,248,248,249,250,250,251,251,252,253,253,254,255,255
};

uint16_t atan2(int16_t y, int16_t x)
{
  uint16_t a;
  uint16_t xx = 0;
  
  //hack from prashanth
  if(0x8000 == y)
  {
	y++;
  }
  if(0x8000 == x)
  {
	x++;
  }
	
  if ((x <= 0) ^ (y > 0)) {
    int16_t t; t = x; x = y; y = t;
    xx ^= 0x4000;
  }
  if (x <= 0) {
    x = -x;
  } else {
    xx ^= 0x8000;
  }
  y = abs(y);
  if (x > y) {
    int16_t t; t = x; x = y; y = t;
    xx ^= 0x3fff;
  }
  while ((x | y) & 0xff80) {
    x >>= 1;
    y >>= 1;
  }
  if (y == 0) {
    a = 0;
  } else if (x == y) {
    a = 0x2000;
  } else {
    // assert(x <= y);
    int16_t r = ((x << 8) / y);
    // assert(0 <= r);
    // assert(r < 256);
    a = pgm_read_byte(atan8 + r) << 5;
  }
  a ^= xx;
  return a;
}

class Poly {
    int16_t x0, y0, x1, y1;
    int16_t x[8], y[8];
    uint8_t n;
    void restart() {
      n = 0;
      x0 = 16 * 480;
      x1 = 0;
      y0 = 16 * 272;
      y1 = 0;
    }
    void perim() {
      for (uint8_t i = 0; i < n; i++)
        FTImpl.Vertex2f(x[i], y[i]);
      FTImpl.Vertex2f(x[0], y[0]);
    }
  public:
    void begin() {
      restart();

      FTImpl.ColorMask(0,0,0,0);
      FTImpl.StencilOp(FT_KEEP, FT_INVERT);
      FTImpl.StencilFunc(FT_ALWAYS, 255, 255);
    }
    void v(int16_t _x, int16_t _y) {
      x0 = min(x0, _x >> 4);
      x1 = max(x1, _x >> 4);
      y0 = min(y0, _y >> 4);
      y1 = max(y1, _y >> 4);
      x[n] = _x;
      y[n] = _y;
      n++;
    }
    void paint() {
      x0 = max(0, x0);
      y0 = max(0, y0);
      x1 = min(16 * 480, x1);
      y1 = min(16 * 272, y1);
      //FTImpl.ScissorXY(x0, y0);
      //FTImpl.ScissorSize(x1 - x0 + 1, y1 - y0 + 1);
      FTImpl.Begin(FT_EDGE_STRIP_B);
      perim();
    }
    void outline() {
      FTImpl.Begin(FT_LINE_STRIP);
      perim();
    }
};

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

void Polygon()
{
    int16_t xoffset,yoffset;
    xoffset = ((FT_DISPLAYWIDTH - 480)/2);
    yoffset = ((FT_DISPLAYHEIGHT - 272)/2);
	FTImpl.SetCTouchMode(FT_CTOUCH_MODE_EXTENDED);	//set mode to extended for FT801

	while(1)
	{
		FTImpl.DLStart();
		FTImpl.ClearColorRGB(0x50,0,0);
		FTImpl.Clear(1,1,1);
		FTImpl.Cmd_Gradient(0, 0, 0x808080, 0, 272, 0x80ff40);
		FTImpl.SaveContext();
		
		//FTImpl.Finish();//render the display list and wait for the completion of the DL
		FTImpl.ColorRGB(0,0,0x80);//set the color of the string to blue color
		FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2, FT_DISPLAYHEIGHT/2, 24, FT_OPT_CENTER, "MultiTouch Polygon Demo");
		FTImpl.ColorRGB(0x40,0x40,0x40);
		
	    int16_t sx[5], sy[5];
        read_extended(sx, sy);
		
        int16_t cx = 0, cy = 0;
        uint8_t n = 0;
        {
          for (int16_t i = 0; i < 5; i++) {
            if (sx[i] > -10) {
              cx += sx[i];
              cy += sy[i];
              n++;
            }
          }
          cx /= n;
          cy /= n;
        }
  
        uint8_t angles[5];
        for (int16_t i = 0; i < 5; i++) {
          angles[i] = atan2(sx[i] - cx, sy[i] - cy) >> 8;
        }

       Poly po;
        po.begin();
        for (int16_t a = 0; a < 256; a++) {
          for (int16_t i = 0; i < 5; i++) {
            if (angles[i] == a && sx[i]> -10) {
              po.v(16 * sx[i], 16 * sy[i]);
            }
          }
        }
        po.paint();
		FTImpl.ColorA(255);		
		FTImpl.ColorMask(1,1,1,1);
		FTImpl.BlendFunc(FT_SRC_ALPHA,FT_ONE);
		FTImpl.StencilFunc(FT_EQUAL, 255, 255);
		FTImpl.Begin(FT_RECTS);
		FTImpl.Vertex2ii(0,0,0,0);
		FTImpl.Vertex2ii(480,272,0,0);

		FTImpl.RestoreContext();
		FTImpl.ColorRGB(0x80,0,0);
		FTImpl.LineWidth(24);
		po.outline(); 	
		
		FTImpl.DLEnd();//end the display list
		FTImpl.Cmd_LoadIdentity();
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
		Polygon();
		FTImpl.Exit();
	}
	Serial.println("--End Application--");
}

void loop()
{
	
}


/* Nothing beyond this */

















