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
* @file                           FT_App_Gauges.ino
* @brief                          Sample application to demonstrate FT800 primitives, widgets and customized screen shots
* @version                        0.1.1
* @date                           Jun/21/2014

version 0.1.0 - Mar/21/2014 - initial version
version 0.1.1 - Jun/30/2014 - FT801 platform now uses x position as the input.
*
*/


#include "SPI.h"
#include "Wire.h"
#include "FT_NHD_43CTP_SHIELD.h"
#include "digit.h"




/* Global object for transport */
FT801IMPL_SPI FTImpl(FT_CS_PIN,FT_PDN_PIN,FT_INT_PIN);



/* Optimized implementation of sin and cos table - precision is 16 bit */
uint16_t sintab[] = {
  0, 402, 804, 1206, 1607, 2009, 2410, 2811, 3211, 3611, 4011, 4409, 4807, 5205, 5601, 5997, 6392, 
  6786, 7179, 7571, 7961, 8351, 8739, 9126, 9511, 9895, 10278, 10659, 11038, 11416, 11792, 12166, 12539,
  12909, 13278, 13645, 14009, 14372, 14732, 15090, 15446, 15799, 16150, 16499, 16845, 17189, 17530, 17868,
  18204, 18537, 18867, 19194, 19519, 19840, 20159, 20474, 20787, 21096, 21402, 21705, 22004, 22301, 22594, 
  22883, 23169, 23452, 23731, 24006, 24278, 24546, 24811, 25072, 25329, 25582, 25831, 26077, 26318, 26556, 26789, 
  27019, 27244, 27466, 27683, 27896, 28105, 28309, 28510, 28706, 28897, 29085, 29268, 29446, 29621, 29790, 29955, 
  30116, 30272, 30424, 30571, 30713, 30851, 30984, 31113, 31236, 31356, 31470, 31580, 31684, 31785, 31880, 31970, 
  32056, 32137, 32213, 32284, 32350, 32412, 32468, 32520, 32567, 32609, 32646, 32678, 32705, 32727, 32744, 32757, 
  32764, 32767, 32764};

int16_t qsin(uint16_t a)
{
  uint8_t f;
  int16_t s0,s1;

  if (a & 32768)
    return -qsin(a & 32767);
  if (a & 16384)
    a = 32768 - a;
  f = a & 127;
  s0 = sintab[a >> 7];
  s1 = sintab[(a >> 7) + 1];
  return (s0 + ((int32_t)f * (s1 - s0) >> 7));
}

/* cos funtion */
int16_t qcos(uint16_t a)
{
  return (qsin(a + 16384));
}


static int32_t ox;
static uint8_t noofch=0;

/*deflated icons*/
PROGMEM prog_uchar home_star_icon[] = {0x78,0x9C,0xE5,0x94,0xBF,0x4E,0xC2,0x40,0x1C,0xC7,0x7F,0x2D,0x04,0x8B,0x20,0x45,0x76,0x14,0x67,0xA3,0xF1,0x0D,0x64,0x75,0xD2,0xD5,0x09,0x27,0x17,0x13,0xE1,0x0D,0xE4,0x0D,0x78,0x04,0x98,0x5D,0x30,0x26,0x0E,0x4A,0xA2,0x3E,0x82,0x0E,0x8E,0x82,0xC1,0x38,0x62,0x51,0x0C,0x0A,0x42,0x7F,0xDE,0xB5,0x77,0xB4,0x77,0x17,0x28,0x21,0x26,0x46,0xFD,0x26,0xCD,0xE5,0xD3,0x7C,0xFB,0xBB,0xFB,0xFD,0xB9,0x02,0xCC,0xA4,0xE8,0x99,0x80,0x61,0xC4,0x8A,0x9F,0xCB,0x6F,0x31,0x3B,0xE3,0x61,0x7A,0x98,0x84,0x7C,0x37,0xF6,0xFC,0xC8,0xDD,0x45,0x00,0xDD,0xBA,0xC4,0x77,0xE6,0xEE,0x40,0xEC,0x0E,0xE6,0x91,0xF1,0xD2,0x00,0x42,0x34,0x5E,0xCE,0xE5,0x08,0x16,0xA0,0x84,0x68,0x67,0xB4,0x86,0xC3,0xD5,0x26,0x2C,0x20,0x51,0x17,0xA2,0xB8,0x03,0xB0,0xFE,0x49,0xDD,0x54,0x15,0xD8,0xEE,0x73,0x37,0x95,0x9D,0xD4,0x1A,0xB7,0xA5,0x26,0xC4,0x91,0xA9,0x0B,0x06,0xEE,0x72,0xB7,0xFB,0xC5,0x16,0x80,0xE9,0xF1,0x07,0x8D,0x3F,0x15,0x5F,0x1C,0x0B,0xFC,0x0A,0x90,0xF0,0xF3,0x09,0xA9,0x90,0xC4,0xC6,0x37,0xB0,0x93,0xBF,0xE1,0x71,0xDB,0xA9,0xD7,0x41,0xAD,0x46,0xEA,0x19,0xA9,0xD5,0xCE,0x93,0xB3,0x35,0x73,0x0A,0x69,0x59,0x91,0xC3,0x0F,0x22,0x1B,0x1D,0x91,0x13,0x3D,0x91,0x73,0x43,0xF1,0x6C,0x55,0xDA,0x3A,0x4F,0xBA,0x25,0xCE,0x4F,0x04,0xF1,0xC5,0xCF,0x71,0xDA,0x3C,0xD7,0xB9,0xB2,0x48,0xB4,0x89,0x38,0x20,0x4B,0x2A,0x95,0x0C,0xD5,0xEF,0x5B,0xAD,0x96,0x45,0x8A,0x41,0x96,0x7A,0x1F,0x60,0x0D,0x7D,0x22,0x75,0x82,0x2B,0x0F,0xFB,0xCE,0x51,0x3D,0x2E,0x3A,0x21,0xF3,0x1C,0xD9,0x38,0x86,0x2C,0xC6,0x05,0xB6,0x7B,0x9A,0x8F,0x0F,0x97,0x1B,0x72,0x6F,0x1C,0xEB,0xAE,0xFF,0xDA,0x97,0x0D,0xBA,0x43,0x32,0xCA,0x66,0x34,0x3D,0x54,0xCB,0x24,0x9B,0x43,0xF2,0x70,0x3E,0x42,0xBB,0xA0,0x95,0x11,0x37,0x46,0xE1,0x4F,0x49,0xC5,0x1B,0xFC,0x3C,0x3A,0x3E,0xD1,0x65,0x0E,0x6F,0x58,0xF8,0x9E,0x5B,0xDB,0x55,0xB6,0x41,0x34,0xCB,0xBE,0xDB,0x87,0x5F,0xA9,0xD1,0x85,0x6B,0xB3,0x17,0x9C,0x61,0x0C,0x9B,0xA2,0x5D,0x61,0x10,0xED,0x2A,0x9B,0xA2,0x5D,0x61,0x10,0xED,0x2A,0x9B,0xA2,0x5D,0x61,0x10,0xED,0x2A,0x9B,0xED,0xC9,0xFC,0xDF,0x14,0x54,0x8F,0x80,0x7A,0x06,0xF5,0x23,0xA0,0x9F,0x41,0xF3,0x10,0x30,0x4F,0x41,0xF3,0x18,0x30,0xCF,0xCA,0xFC,0xFF,0x35,0xC9,0x79,0xC9,0x89,0xFA,0x33,0xD7,0x1D,0xF6,0x5E,0x84,0x5C,0x56,0x6E,0xA7,0xDA,0x1E,0xF9,0xFA,0xAB,0xF5,0x97,0xFF,0x2F,0xED,0x89,0x7E,0x29,0x9E,0xB4,0x9F,0x74,0x1E,0x69,0xDA,0xA4,0x9F,0x81,0x94,0xEF,0x4F,0xF6,0xF9,0x0B,0xF4,0x65,0x51,0x08};


static void polarxy(int32_t r, uint16_t th, int32_t *x, int32_t *y)
{
  *x = (16 * (FT_DISPLAYWIDTH/(2*noofch)) + (((long)r * qsin(th)) >> 11) + 16 * ox);
  *y = (16 * 300 - (((long)r * qcos(th)) >> 11));
}


void vertex(int32_t x, int32_t y)
{
  FTImpl.Vertex2f(x,y);
}


static void polar(int32_t r, uint16_t th)
{
  int32_t x, y;
  polarxy(r, th, &x, &y);
  vertex(x, y);
}

uint16_t da(int32_t i)
{
  return (i - 45) * 32768L / 360;
}

static void cs(uint8_t i)
{
  switch (i)
  {
    case  0: FTImpl.ColorRGB(200,255,200); break;
    case 60: FTImpl.ColorRGB(255,255,0); break;
    case 80: FTImpl.ColorRGB(255,0,0); break;
  }
}

/// draw the gauges

void Gauges()
{
  uint16_t w,h,n, val=0,a,th,size_d,tval,lastXPos=0;
  int32_t rval,tgt;
  uint8_t i,bi,z;
  int32_t x,y,tx,ty,o,dt;
  w = 220; h = 130;
	sTagXY sTagxy;
	
#if (FT_DISPLAY_RESOLUTION == FT_DISPLAY_QVGA_320x240)
	noofch = 1;
	dt = 50;
#elif (FT_DISPLAY_RESOLUTION == FT_DISPLAY_WQVGA_480x272)
	noofch = 2;
	dt = 10;
#endif

// clear GRAM  
  FTImpl.DLStart();  
  FTImpl.Cmd_Memset(0,0,10*1024);	
  FTImpl.Finish();
//**********load the deflated icons to GRAM via J1
  FTImpl.Cmd_Inflate(0);
  FTImpl.WriteCmdfromflash(digits,sizeof(digits));

 // Set the bitmap properties
  FTImpl.DLStart();  	
  FTImpl.Cmd_SetFont(13,0);
  FTImpl.BitmapHandle(13);
  FTImpl.BitmapSource(144 - (32L*(54/2)*87));
  FTImpl.BitmapLayout(FT_L4, 54/2, 87);
  FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, 54, 87);  
  FTImpl.DLEnd();
  FTImpl.Finish();
  
// Construct the Gauge background	

 FTImpl.DLStart();  							// dl = 0;
 FTImpl.ClearColorRGB(55,55,55);
 FTImpl.Clear(1,1,1);
 FTImpl.ClearColorRGB(0,0,0);
 y = 10;
 for(z=0;z<(FT_DISPLAYWIDTH/w);z++)
 {
   ox = 240*z;  
   FTImpl.ScissorXY(ox+dt,y);
   FTImpl.ScissorSize(w,h);
   
   FTImpl.Clear(1,1,1);			
   FTImpl.Begin(FT_LINES);
   FTImpl.LineWidth(10);
   for (bi = 0; bi < 81; bi += 10) 
   {
     cs(bi);
     for ( i = 2; i < 10; i += 2) 
     {
       a = da(bi + i);
       polar(220, a);
       polar(240, a);
     }
   }
    
   FTImpl.LineWidth(16);
   for (i = 0; i < 91; i += 10)
   {
     cs(i);
     a = da(i);
     polar(220, a);
     polar(250, a);
   }
   FTImpl.ColorRGB(255,255,255);
   for (i = 0; i < 91; i += 10)
   {
     a = da(i);
     polarxy(260, a, &tx, &ty);
     FTImpl.Cmd_Number(tx >> 4, ty >> 4,26,FT_OPT_CENTER, i);
   }
   ox = (FT_DISPLAYWIDTH/(2*noofch))+(z*(FT_DISPLAYWIDTH/2));
   if(z==1)FTImpl.Cmd_Text(ox,h-10,28,FT_OPT_CENTERX,"Random"); 
#if defined(FT801_CHIPID)
   if(z==0)FTImpl.Cmd_Text(ox,h-10,28,FT_OPT_CENTERX,"X Position");
#else
	if(z==0)FTImpl.Cmd_Text(ox,h-10,28,FT_OPT_CENTERX,"Resistance");
#endif
 }
  FTImpl.Finish();
// copy the displaylist from DLRAM to GRAM 
 uint16_t dloffset;
 dloffset =  FTImpl.Read16(REG_CMD_DL); 

 FTImpl.Cmd_Memcpy(100000L,FT_RAM_DL,dloffset);
 y = 10+120+20;
 rval=0,tgt = 4500;
 do
 {
   // Start the New Displaylist
   FTImpl.DLStart();  
   FTImpl.Cmd_Append(100000L,dloffset);	
   for(z=0;z<(FT_DISPLAYWIDTH/w);z++)
   {
     ox = (FT_DISPLAYWIDTH/2)*z;
     if(z==0)
     {
	#if defined(FT800_CHIPID)
		val = FTImpl.Read16(REG_TOUCH_RZ);			
		val = 10*min(899,val);	
	#else
		if(FTImpl.IsPendown()){
			FTImpl.GetTagXY(sTagxy);
			if(sTagxy.x >=0 && sTagxy.x <= FT_DISPLAYWIDTH && sTagxy.x != lastXPos){
				val = (uint16_t)((double)sTagxy.x * 899 / FT_DISPLAYWIDTH)*10;
				lastXPos = sTagxy.x;
			}
			else{
				val = (uint16_t)((double)lastXPos * 899 / FT_DISPLAYWIDTH)*10;
			}
		}
		else{
				val = (uint16_t)((double)lastXPos * 899 / FT_DISPLAYWIDTH)*10;
			}
	#endif
     }else   
     if(z==1)
     {
	int d = (tgt - rval) / 16;
	rval += d;
	if (random(60) == 0)
	tgt = random(9000L);
	val = rval;
     }
    // Scissor the Value display position 
    FTImpl.ScissorXY(ox+dt,10);
	FTImpl.ScissorSize(w,120);
    
    FTImpl.ColorRGB(255,255,255);
    FTImpl.Begin(FT_LINES);
    FTImpl.LineWidth(10);
    th = (val - 4500L) * 32768L / 36000L;
    for (o = -5; o < 6; o++) 
    {
      polar(170, th + (o << 5));
      polar(235, th);
    }
    FTImpl.ScissorXY(ox+dt,y);
    FTImpl.ScissorSize(w,(uint16_t)(FT_DISPLAYHEIGHT*0.36));
    FTImpl.Clear(1,1,1);
    FTImpl.ColorRGB(255,0,0);
    FTImpl.Cmd_Number(ox+dt+10,160,13,2,val/100);	
    FTImpl.Cmd_Text(ox+dt+96,160,13,0,".");
    FTImpl.Cmd_Number(ox+dt+106,160,13,2,val%100);
   }
	FTImpl.DLEnd();
	FTImpl.Finish();
 }
 while(1);
}

/********API to return the assigned TAG value when penup,for the primitives/widgets******/
static uint8_t sk=0;
uint8_t Read_Keys()
{
  static uint8_t Read_tag=0,temp_tag=0,ret_tag=0;	
  Read_tag = FTImpl.Read(REG_TOUCH_TAG);
  ret_tag = NULL;
  if(Read_tag!=NULL)								// Allow if the Key is released
  {
    if(temp_tag!=Read_tag)
    {
      temp_tag = Read_tag;	
      sk = Read_tag;										// Load the Read tag to temp variable	
    }  
  }
  else
  {
    if(temp_tag!=0)
    {
      ret_tag = temp_tag;
    }  
    sk = 0;
  }
  return ret_tag;
}
/***********************API used to SET the ICON******************************************/
/*Refer the code flow in the flowchart availble in the Application Note */

void home_setup()
{
  /*Icon  file is deflated use J1 Command to inflate the file and write into the GRAM*/
  FTImpl.Cmd_Inflate(250*1024L);
  FTImpl.WriteCmdfromflash(home_star_icon,sizeof(home_star_icon));

  FTImpl.DLStart();       
  FTImpl.Clear(1,1,1);
 
  /*Set the Bitmap properties for the ICONS*/ 
  
  FTImpl.BitmapHandle(13);    // handle for background stars
  FTImpl.BitmapSource(250*1024L);      // Starting address in gram
  FTImpl.BitmapLayout(FT_L4, 16, 32);  // format 
  FTImpl.BitmapSize(FT_NEAREST, FT_REPEAT, FT_REPEAT, 512, 512);
  FTImpl.BitmapHandle(14);    // handle for background stars
  FTImpl.BitmapSource(250*1024L);      // Starting address in gram
  FTImpl.BitmapLayout(FT_L4, 16, 32);  // format 
  FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, 32, 32  );
  FTImpl.DLEnd();
  FTImpl.Finish();
}


const char *info[] = {  "FT801 Gauges Application",
                        "APP to demonstrate interactive Gauges,",
                        "using FT_LINES & custom Font."
                     }; 



void Info()
{

  uint16_t dloffset = 0,z;
  uint32_t status;
  FTImpl.DLStart();
  FTImpl.ClearColorRGB(0,0,0);
  FTImpl.Clear(1,1,1);
  FTImpl.ColorRGB(255,255,255);
  FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,FT_DISPLAYHEIGHT/2,26,FT_OPT_CENTER,"Please tap on a dot");
  FTImpl.Cmd_Calibrate(0);
  FTImpl.DLEnd();
  FTImpl.Finish();
 
  FTImpl.Cmd_Logo();
  while(FTImpl.CheckLogo() != FT_GE_FINISHED);
  dloffset = FTImpl.Read16(REG_CMD_DL);
  dloffset -=4;
  FTImpl.WriteCmd(CMD_MEMCPY);
  FTImpl.WriteCmd(100000L);
  FTImpl.WriteCmd(FT_RAM_DL);
  FTImpl.WriteCmd(dloffset);
  do
  {
    FTImpl.DLStart();  
    FTImpl.Cmd_Append(100000L,dloffset);
    //Reset the BITMAP properties used during Logo animation
    FTImpl.BitmapTransformA((256));
    FTImpl.BitmapTransformB((0));
    FTImpl.BitmapTransformC((0));
    FTImpl.BitmapTransformD((0));
    FTImpl.BitmapTransformE((256));
    FTImpl.BitmapTransformF((0));  
    FTImpl.SaveContext();	
    //Display the information with transparent Logo using Edge Strip  
    FTImpl.ColorRGB(219,180,150);
    FTImpl.ColorA((220));
    FTImpl.Begin((FT_EDGE_STRIP_A));
    FTImpl.Vertex2f(0,FT_DISPLAYHEIGHT*16);
    FTImpl.Vertex2f(FT_DISPLAYWIDTH*16,FT_DISPLAYHEIGHT*16);
    FTImpl.ColorA((255));
    FTImpl.RestoreContext();	
    FTImpl.ColorRGB(0,0,0);
    //INFORMATION 
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,20,28,FT_OPT_CENTER,info[0]);
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,60,26,FT_OPT_CENTER,info[1]);
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,90,26,FT_OPT_CENTER,info[2]);  
    //FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,120,26,FT_OPT_CENTER,info[3]);  
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,FT_DISPLAYHEIGHT-30,26,FT_OPT_CENTER,"Click to play");
    //Check the Play key and change the color
    if(sk!='P')
    FTImpl.ColorRGB(255,255,255);
    else
    FTImpl.ColorRGB(100,100,100);
    FTImpl.Begin((FT_POINTS));   
    FTImpl.PointSize(20*16);
    FTImpl.Tag('P');
    FTImpl.Vertex2f((FT_DISPLAYWIDTH/2)*16,(FT_DISPLAYHEIGHT-60)*16);
    FTImpl.ColorRGB(180,35,35);
    FTImpl.Begin((FT_BITMAPS));
    FTImpl.Vertex2ii((FT_DISPLAYWIDTH/2)-14,(FT_DISPLAYHEIGHT-75),14,4);
    FTImpl.DLEnd();
  FTImpl.Finish();
  }while(Read_Keys()!='P');
  /* wait until Play key is not pressed*/ 
}



/* Api to bootup FT801, verify FT801 hardware and configure display/audio pins */
/* Returns 0 in case of success and 1 in case of failure */
int16_t BootupConfigure(){
	uint32_t chipid = 0;
	FTImpl.Init(FT_DISPLAY_RESOLUTION);

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
		home_setup();
		Info();
		Gauges();  
		FTImpl.Exit();
	}
	Serial.println("--End Application--");
}

void loop()
{
}


/* Nothing beyond this */
















