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
* @file                           FT_App_MainMenu.ino
* @brief                          Sample application to demonstrate FT801 primitives, widgets and customized screen shots. 
* @version                        0.2.0
* @date                           Jun/30/2014
*
version 0.1.0 - Mar/21/2014 - Initial version
version 0.2.0 - Jun/30/2014 - Added multiple animated backgrounds
*/

#include "SPI.h"
#include "Wire.h"
#include "FT_NHD_43CTP_SHIELD.h"


#define NOTOUCH		-32768
#define F16(s)        ((int32_t)((s) * 65536))
#define SQ(v) (v*v)



#define MAX_MENUS 12
#define THUMBNAIL_ADDRESS (50*1024L)
#define MENU_POINTSIZE  5 // 16bit prec 


#define LOOPBACK_METHOD	 
//#define ANDROID_METHOD  
//#define WIN8_METHOD


//#define BACKGROUND_ANIMATION_1
//#define BACKGROUND_ANIMATION_2
#define BACKGROUND_ANIMATION_3
//#define BACKGROUND_ANIMATION_4
//#define BACKGROUND_ANIMATION_5
//#define BACKGROUND_ANIMATION_6

/* SD card object*/
FT_SD FtSd(FT_SD_CSPIN);

/* sd_present holds error values during initialization.  0 means no error and all other errors are non zero value */
FT_SDStatus sd_present; 
FT_SDFile Imagefile;

/* Global object for transport */
FT801IMPL_SPI FTImpl(FT_CS_PIN,FT_PDN_PIN,FT_INT_PIN);

uint32_t coprostatus = 0;
PROGMEM prog_uchar home_star_icon[] = {0x78,0x9C,0xE5,0x94,0xBF,0x4E,0xC2,0x40,0x1C,0xC7,0x7F,0x2D,0x04,0x8B,0x20,0x45,0x76,0x14,0x67,0xA3,0xF1,0x0D,0x64,0x75,0xD2,0xD5,0x09,0x27,0x17,0x13,0xE1,0x0D,0xE4,0x0D,0x78,0x04,0x98,0x5D,0x30,0x26,0x0E,0x4A,0xA2,0x3E,0x82,0x0E,0x8E,0x82,0xC1,0x38,0x62,0x51,0x0C,0x0A,0x42,0x7F,0xDE,0xB5,0x77,0xB4,0x77,0x17,0x28,0x21,0x26,0x46,0xFD,0x26,0xCD,0xE5,0xD3,0x7C,0xFB,0xBB,0xFB,0xFD,0xB9,0x02,0xCC,0xA4,0xE8,0x99,0x80,0x61,0xC4,0x8A,0x9F,0xCB,0x6F,0x31,0x3B,0xE3,0x61,0x7A,0x98,0x84,0x7C,0x37,0xF6,0xFC,0xC8,0xDD,0x45,0x00,0xDD,0xBA,0xC4,0x77,0xE6,0xEE,0x40,0xEC,0x0E,0xE6,0x91,0xF1,0xD2,0x00,0x42,0x34,0x5E,0xCE,0xE5,0x08,0x16,0xA0,0x84,0x68,0x67,0xB4,0x86,0xC3,0xD5,0x26,0x2C,0x20,0x51,0x17,0xA2,0xB8,0x03,0xB0,0xFE,0x49,0xDD,0x54,0x15,0xD8,0xEE,0x73,0x37,0x95,0x9D,0xD4,0x1A,0xB7,0xA5,0x26,0xC4,0x91,0xA9,0x0B,0x06,0xEE,0x72,0xB7,0xFB,0xC5,0x16,0x80,0xE9,0xF1,0x07,0x8D,0x3F,0x15,0x5F,0x1C,0x0B,0xFC,0x0A,0x90,0xF0,0xF3,0x09,0xA9,0x90,0xC4,0xC6,0x37,0xB0,0x93,0xBF,0xE1,0x71,0xDB,0xA9,0xD7,0x41,0xAD,0x46,0xEA,0x19,0xA9,0xD5,0xCE,0x93,0xB3,0x35,0x73,0x0A,0x69,0x59,0x91,0xC3,0x0F,0x22,0x1B,0x1D,0x91,0x13,0x3D,0x91,0x73,0x43,0xF1,0x6C,0x55,0xDA,0x3A,0x4F,0xBA,0x25,0xCE,0x4F,0x04,0xF1,0xC5,0xCF,0x71,0xDA,0x3C,0xD7,0xB9,0xB2,0x48,0xB4,0x89,0x38,0x20,0x4B,0x2A,0x95,0x0C,0xD5,0xEF,0x5B,0xAD,0x96,0x45,0x8A,0x41,0x96,0x7A,0x1F,0x60,0x0D,0x7D,0x22,0x75,0x82,0x2B,0x0F,0xFB,0xCE,0x51,0x3D,0x2E,0x3A,0x21,0xF3,0x1C,0xD9,0x38,0x86,0x2C,0xC6,0x05,0xB6,0x7B,0x9A,0x8F,0x0F,0x97,0x1B,0x72,0x6F,0x1C,0xEB,0xAE,0xFF,0xDA,0x97,0x0D,0xBA,0x43,0x32,0xCA,0x66,0x34,0x3D,0x54,0xCB,0x24,0x9B,0x43,0xF2,0x70,0x3E,0x42,0xBB,0xA0,0x95,0x11,0x37,0x46,0xE1,0x4F,0x49,0xC5,0x1B,0xFC,0x3C,0x3A,0x3E,0xD1,0x65,0x0E,0x6F,0x58,0xF8,0x9E,0x5B,0xDB,0x55,0xB6,0x41,0x34,0xCB,0xBE,0xDB,0x87,0x5F,0xA9,0xD1,0x85,0x6B,0xB3,0x17,0x9C,0x61,0x0C,0x9B,0xA2,0x5D,0x61,0x10,0xED,0x2A,0x9B,0xA2,0x5D,0x61,0x10,0xED,0x2A,0x9B,0xA2,0x5D,0x61,0x10,0xED,0x2A,0x9B,0xED,0xC9,0xFC,0xDF,0x14,0x54,0x8F,0x80,0x7A,0x06,0xF5,0x23,0xA0,0x9F,0x41,0xF3,0x10,0x30,0x4F,0x41,0xF3,0x18,0x30,0xCF,0xCA,0xFC,0xFF,0x35,0xC9,0x79,0xC9,0x89,0xFA,0x33,0xD7,0x1D,0xF6,0x5E,0x84,0x5C,0x56,0x6E,0xA7,0xDA,0x1E,0xF9,0xFA,0xAB,0xF5,0x97,0xFF,0x2F,0xED,0x89,0x7E,0x29,0x9E,0xB4,0x9F,0x74,0x1E,0x69,0xDA,0xA4,0x9F,0x81,0x94,0xEF,0x4F,0xF6,0xF9,0x0B,0xF4,0x65,0x51,0x08};
prog_char info_1[] PROGMEM =  "FT801 MainMenu Application";
prog_char info_2[] PROGMEM =  "APP to demonstrate interactive menus,";
prog_char info_3[] PROGMEM =  "using Jpeg decode,";
prog_char info_4[] PROGMEM =  "Rectangle & Points.";

PROGMEM const char *information[] = 	{   
											info_1,
											info_2,
											info_3,
											info_4,
										};




/* Boot up for FT801 followed by graphics primitive sample cases */
/* Initial boot up DL - make the back ground green color */
const uint8_t FT_DLCODE_BOOTUP[12] = 
{
  255,255,255,2,//GPU instruction ClearColorRGB
  7,0,0,38, //GPU instruction Clear
  0,0,0,0,  //GPU instruction Display
};
// deflated  icon

PROGMEM prog_uint16_t sintab[] = {
0, 402, 804, 1206, 1607, 2009, 2410, 2811, 3211, 3611, 4011, 4409, 4807, 5205, 5601, 5997, 6392,
6786, 7179, 7571, 7961, 8351, 8739, 9126, 9511, 9895, 10278, 10659, 11038, 11416, 11792, 12166, 12539,
12909, 13278, 13645, 14009, 14372, 14732, 15090, 15446, 15799, 16150, 16499, 16845, 17189, 17530, 17868,
18204, 18537, 18867, 19194, 19519, 19840, 20159, 20474, 20787, 21096, 21402, 21705, 22004, 22301, 22594,
22883, 23169, 23452, 23731, 24006, 24278, 24546, 24811, 25072, 25329, 25582, 25831, 26077, 26318, 26556, 26789,
27019, 27244, 27466, 27683, 27896, 28105, 28309, 28510, 28706, 28897, 29085, 29268, 29446, 29621, 29790, 29955,
30116, 30272, 30424, 30571, 30713, 30851, 30984, 31113, 31236, 31356, 31470, 31580, 31684, 31785, 31880, 31970,
32056, 32137, 32213, 32284, 32350, 32412, 32468, 32520, 32567, 32609, 32646, 32678, 32705, 32727, 32744, 32757,
32764, 32767, 32764};


// API used to play the KEYsound
void Ft_Play_Sound(uint8_t sound,uint8_t vol,uint8_t midi)
{
  uint16_t val = (midi << 8) | sound; 
  FTImpl.Write(REG_SOUND,val);
  FTImpl.Write(REG_PLAY,1); 
}

static struct {
  signed short dragprev;
  int vel;      // velocity
  long base;    // screen x coordinate, in 1/16ths pixel
  long limit;
} scroller;

/* Api to Intialise the scroller*/
static void scroller_init(uint32_t limit)
{
  scroller.dragprev = -32768;
  scroller.vel = 0;      // velocity
  scroller.base = 0;     // screen x coordinate, in 1/16ths pixel
  scroller.limit = limit;
}
/* Api used to scroll the screen horizontally*/
static void scroller_run()
{
 signed short sx;
  static uint16_t _delay = 0;

  static uint32_t prev_time = 0;
  uint32_t time  = millis();
  if(prev_time!=0)
  _delay += (time-prev_time); 
  prev_time = time;
  if(_delay<30)
  {
	scroller.base += scroller.vel;
	scroller.base = max(0, min(scroller.base, scroller.limit));
	return; 
  }		

  sx = FTImpl.Read16(REG_TOUCH_SCREEN_XY + 2);
  if ((sx != -32768) & (scroller.dragprev != -32768)) {
    scroller.vel = (scroller.dragprev - sx) << 4;
  } else {
    int change = max(1, abs(scroller.vel) >> 5);
    if (scroller.vel < 0)
      scroller.vel += change;
    if (scroller.vel > 0)
      scroller.vel -= change;
  }
  scroller.dragprev = sx;
  scroller.base += scroller.vel;
  scroller.base = max(0, min(scroller.base, scroller.limit));
  _delay = 0;
}
/********API to return the assigned Tag value when penup,for the primitives/widgets******/
uint8_t keypressed = 0;
uint8_t keyin_cts = 0;
#define KEYIN_COUNTS	10

uint8_t Read_keys()
{	
	static uint8_t temp_key = 0,prev_keyin = 0; 
	keypressed = FTImpl.Read(REG_TOUCH_TAG);
	if(keypressed!=NULL && temp_key!=keypressed)
	temp_key = keypressed;
	if(keypressed==NULL)
	{
		if(temp_key!=NULL)
		{
			Ft_Play_Sound(0x51,100,108);	
			prev_keyin = temp_key;
			temp_key = NULL;
			return prev_keyin;
		}
		temp_key = 	keypressed;	
	}	
	return 0;
}


/* Load the icons of the menu to GRAM*/ 
void Load_Thumbnails()
{ 
  String stringOne;  
  if(sd_present)
  {
    for(int i=0;i<12;i++)
    {
	  FTImpl.DLStart(); 
      FTImpl.Cmd_Memset(THUMBNAIL_ADDRESS+(i*10*1024L),random(150),50*2*100);
	  FTImpl.Finish();
    }  
    return;
  }
  int image_no = 1;
  do
  {	
	stringOne = 0;
	stringOne+=image_no;
	stringOne+=".jpg";	
	char apps[stringOne.length()+1];
	stringOne.toCharArray(apps,stringOne.length()+1);
	FtSd.OpenFile(Imagefile,apps);
    int16_t fsize  = Imagefile.Size;
    if(fsize==NULL)
    {
		FTImpl.WriteCmd(CMD_MEMSET);
		FTImpl.WriteCmd((image_no-1)*10*1024L);
		FTImpl.WriteCmd(random(150));
		FTImpl.WriteCmd(random(10*1024L));
    }
    else
    {
		FTImpl.WriteCmd(CMD_LOADIMAGE);
		FTImpl.WriteCmd(THUMBNAIL_ADDRESS+((image_no-1)*10000));
		FTImpl.WriteCmd(0);
    }
	uint8_t *buff = (uint8_t*)malloc(512);  
	while (Imagefile.Offset < Imagefile.Size)
	{
		uint16_t n = min(512, Imagefile.Size - Imagefile.Offset);
		n = (n + 3) & ~3;   // force 32-bit alignment
		Imagefile.ReadSector(buff);
		FTImpl.WriteCmd(buff, n);//alignment is already taken care by this api
	}
	free(buff);
    image_no++;
  }while(image_no<=MAX_MENUS);
  FTImpl.DLStart();    
  FTImpl.BitmapHandle(0);	
  FTImpl.BitmapSource(THUMBNAIL_ADDRESS);	
  FTImpl.BitmapLayout(FT_RGB565,100*2,50);	
  FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, 100, 50);	
  FTImpl.Display();
  FTImpl.Cmd_Swap();
  FTImpl.Finish();
}

/***********************API used to SET the ICON******************************************/
/*Refer the code flow in the flowchart availble in the Application Note */


void home_setup()
{
/*Icon  file is deflated use J1 Command to inflate the file and write into the GRAM*/
  FTImpl.Cmd_Inflate(250*1024L);
  FTImpl.WriteCmdfromflash(home_star_icon,sizeof(home_star_icon));
  /*Set the Bitmap properties for the ICONS*/
  FTImpl.DLStart();        // start
  FTImpl.Clear(1,1,1);  
  FTImpl.BitmapHandle(14);    // handle for background stars
  FTImpl.BitmapSource(250*1024L);      // Starting address in gram
  FTImpl.BitmapLayout(FT_L4, 16, 32);  // format 
  FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, 32, 32  );
  FTImpl.DLEnd();
  FTImpl.Finish();
}
//==============================================Information Screen =======================================
void Info()
{
  uint16_t dloffset = 0,z;
// Touch Screen Calibration
  FTImpl.DLStart(); 
  FTImpl.Clear(1,1,1);
  FTImpl.ColorRGB(255,255,255);
  FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,FT_DISPLAYHEIGHT/2,26,FT_OPT_CENTERX|FT_OPT_CENTERY,"Please tap on a dot");
  FTImpl.Cmd_Calibrate(0);
  FTImpl.DLEnd();
  FTImpl.Finish();
//Ftdi Logo animation 
  FTImpl.Cmd_Logo();
  while(FTImpl.CheckLogo() != FT_GE_FINISHED); 
  dloffset = FTImpl.Read16(REG_CMD_DL);
  dloffset -=4;
//Copy the Displaylist from DL RAM to GRAM
  FTImpl.Cmd_Memcpy(100000L,FT_RAM_DL,dloffset);
  do
  {	
    FTImpl.DLStart();   
    FTImpl.Cmd_Append(100000L,dloffset);
//Reset the BITMAP properties used during Logo animation
    FTImpl.BitmapTransformA(256);
    FTImpl.BitmapTransformB(0);
    FTImpl.BitmapTransformC(0);
    FTImpl.BitmapTransformD(0);
    FTImpl.BitmapTransformE(256);
    FTImpl.BitmapTransformF(0);  
    FTImpl.SaveContext();	
//Display the information with transparent Logo using Edge Strip 
    FTImpl.ColorRGB(219,180,150);
    FTImpl.ColorA(220);
    FTImpl.Begin(FT_EDGE_STRIP_A);
    FTImpl.Vertex2f(0,FT_DISPLAYHEIGHT*16);
    FTImpl.Vertex2f(FT_DISPLAYWIDTH*16,FT_DISPLAYHEIGHT*16);
    FTImpl.ColorA(255);
    FTImpl.RestoreContext();	
    FTImpl.ColorRGB(0,0,0);
// INFORMATION 
    char buffer[40];
	strcpy_P(buffer, (char*)pgm_read_word(&(information[0])));
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,20,28,FT_OPT_CENTERX|FT_OPT_CENTERY,buffer);
	strcpy_P(buffer, (char*)pgm_read_word(&(information[1])));
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,60,26,FT_OPT_CENTERX|FT_OPT_CENTERY,buffer);
	strcpy_P(buffer, (char*)pgm_read_word(&(information[2])));
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,90,26,FT_OPT_CENTERX|FT_OPT_CENTERY,buffer);  
	strcpy_P(buffer, (char*)pgm_read_word(&(information[3])));
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,120,26,FT_OPT_CENTERX|FT_OPT_CENTERY,buffer);  
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,FT_DISPLAYHEIGHT-30,26,FT_OPT_CENTERX|FT_OPT_CENTERY,"Click to play");
//Check if the Play key and change the color
    if(keypressed!='P')
    FTImpl.ColorRGB(255,255,255);
    else
    FTImpl.ColorRGB(100,100,100);
    FTImpl.Begin(FT_POINTS);   
    FTImpl.PointSize(20*16);
    FTImpl.Tag('P');
    FTImpl.Vertex2f((FT_DISPLAYWIDTH/2)*16,(FT_DISPLAYHEIGHT-60)*16);
    FTImpl.ColorRGB(180,35,35);
    FTImpl.Begin(FT_BITMAPS);
    FTImpl.Vertex2ii((FT_DISPLAYWIDTH/2)-14,(FT_DISPLAYHEIGHT-75),14,4);
    FTImpl.DLEnd();
    FTImpl.Finish();
  }while(Read_keys()!='P');
/* wait until Play key is not pressed*/
}

static uint8_t istouch()
{
  return FTImpl.IsPendown();
}

void Sine_wave(uint8_t amp,uint16_t address,uint16_t yoffset)
{
  uint16_t x = 0,y=0; 

  for(x=0;x<FT_DISPLAYWIDTH+100;x+=10)
  {		
	y = (yoffset) + ((int32_t)amp*qsin(-65536*x/(25*10))/65536);
	FTImpl.Write32(address+(x/10)*4,VERTEX2F(x*16,y*16));	
  }	
}

float linear(float p1,float p2,float t,uint16_t rate)
{
	float st  = (float)t/rate;
	return p1+(st*(p2-p1));
}
float acceleration(float p1,float p2,uint16_t t,uint16_t rate)
{
	float dst  = (float)t/rate;
	float st = SQ(dst);
	return p1+(st*(p2-p1));
}

float deceleration(float p1,float p2,uint16_t t,uint16_t rate)
{
	float st,dst  = (float)t/rate;
	dst = 1-dst;
	st = 1-SQ(dst);
	return p1+(st*(p2-p1));
}

int16_t qsin(uint16_t a)
{
  uint8_t f;
  int16_t s0,s1;

  if (a & 32768)
    return -qsin(a & 32767);
  if (a & 16384)
      a = 32768 - a;
  f = a & 127;
  s0 = pgm_read_word(sintab + (a >> 7));
  s1 = pgm_read_word(sintab + (a >> 7) + 1);
  return (s0 + ((int32_t)f * (s1 - s0) >> 7));
}

/* cos funtion */
int16_t qcos(uint16_t a)
{
  return (qsin(a + 16384));
}

static void polar(int32_t r, float th,uint16_t ox,uint16_t oy)
{
  int32_t x, y;
  th = (th * 32768L / 180);
  x = (16 * ox + (((long)r * qsin(th)) >> 11) + 16);
  y = (16 * oy - (((long)r * qcos(th)) >> 11));
  FTImpl.Vertex2f(x,y);
}


int16_t ftsize;
#define rotation_rate 2


void Load_file2gram(uint32_t add,uint8_t sectors,FT_SDFile &r)
{
  uint8_t pbuff[512];
  while(r.Offset < r.Size)
  {
    r.ReadSector(pbuff);
    FTImpl.Write(add,pbuff,512L);
    add+=512;
    ftsize-=512;
  }  
}

#ifdef BACKGROUND_ANIMATION_1

uint16_t xoffset_array[30],dx_pts[20];
uint8_t yoffset_array[30],BitmapHandle[30],dy_pts[20];
uint8_t rate_cts[30],iteration_cts[30];

void Backgroundanimation_1()
{
	static uint16_t bg_cts  = 0,fg = 0,wave_cts = 490;
	static uint8_t init = 0,_cell = 0,VEL;
	static uint8_t cts = 0;
	int16_t i = 0,j=0,xoff,yoff;
	float _xoff = 0;
	wave_cts = 490;
	if(cts>=5){cts = 0;if(_cell>0)_cell--; else _cell = 7;}cts++;
	_cell = 0;
	if(wave_cts<580)wave_cts+=10; else wave_cts = 0;
	if(istouch()) VEL = 2; else VEL = 1;

	if(!init)
	{
      	init = 1;
		Sine_wave(15,FT_RAM_G,FT_DISPLAYHEIGHT/2);		
		Sine_wave(12,FT_RAM_G+232,16+(FT_DISPLAYHEIGHT/2));
		Sine_wave(9,FT_RAM_G+2*232,32+(FT_DISPLAYHEIGHT/2));
		Sine_wave(6,FT_RAM_G+3*232,48+(FT_DISPLAYHEIGHT/2));
		for(i=0;i<30;i++)
		{		
			yoffset_array[i] = random(255);
			BitmapHandle[i] = 4+random(4);
			rate_cts[i] = 100+random(155);
			iteration_cts[i] = random(200);
		}
		FtSd.OpenFile(Imagefile,"nts1.raw");
		Load_file2gram(220*1024L,1,Imagefile);
		
		FtSd.OpenFile(Imagefile,"nts2.raw");               
		Load_file2gram(222*1024L,1,Imagefile);
		
		FtSd.OpenFile(Imagefile,"nts3.raw");               		      
		Load_file2gram(256506L,1,Imagefile);
		
		FtSd.OpenFile(Imagefile,"nts4.raw");               
		
		Load_file2gram(257306L,1,Imagefile);		
		
		FtSd.OpenFile(Imagefile,"hline.raw");               	
		Load_file2gram(255*1024L,1,Imagefile);
                
		FTImpl.BitmapHandle(4);	
		FTImpl.BitmapSource(220*1024L);	
		FTImpl.BitmapLayout(FT_L4,10,50);	
		FTImpl.BitmapSize(FT_BILINEAR, FT_BORDER, FT_BORDER, 40, 100);		
		FTImpl.BitmapHandle(5);	
		FTImpl.BitmapSource(222*1024L);	
		FTImpl.BitmapLayout(FT_L4,25,60);	
		FTImpl.BitmapSize(FT_BILINEAR, FT_BORDER, FT_BORDER, 100, 120);		
		FTImpl.BitmapHandle(6);	
		FTImpl.BitmapSource(256506L);	
		FTImpl.BitmapLayout(FT_L4,10,40);	
		FTImpl.BitmapSize(FT_BILINEAR, FT_BORDER, FT_BORDER, 40, 80);			
		FTImpl.BitmapHandle(7);	
		FTImpl.BitmapSource(257306L);	
		FTImpl.BitmapLayout(FT_L4,10,24);	
		FTImpl.BitmapSize(FT_BILINEAR, FT_BORDER, FT_BORDER, 40, 48);		
		FTImpl.BitmapHandle(8);	
		FTImpl.BitmapSource(255*1024L);	
		FTImpl.BitmapLayout(FT_L8,512,1);	
		FTImpl.BitmapSize(FT_NEAREST, FT_REPEAT, FT_REPEAT, 512, 272);		
	}
//	
    FTImpl.SaveContext();
	FTImpl.ClearColorRGB(30,30,30);
	FTImpl.Clear(1,1,1);
	FTImpl.ColorA(170);
	
	
	FTImpl.Begin(FT_BITMAPS);
	FTImpl.BitmapHandle(8);	
	FTImpl.Vertex2f((int16_t)0,0);	
	
	FTImpl.ColorRGB(30,30,30);
	FTImpl.ColorA(255);
	


	for(j=0;j<3;j++)
	{
	  switch(j)
	  {
		case 0:
		  FTImpl.BitmapTransformA(384);
		  FTImpl.BitmapTransformE(384);
		break;

		case 1:
		  FTImpl.BitmapTransformA(256);
		  FTImpl.BitmapTransformE(256);
		break;

		case 2:			
		  FTImpl.BitmapTransformA(200);
		  FTImpl.BitmapTransformE(200);
		break;
	  }
	  for(i = 0;i<10;i++)
	  {
		FTImpl.BitmapHandle(BitmapHandle[j*10+i]);
		if(BitmapHandle[j*10+i]==5)
		FTImpl.Cell(_cell);
		else 
		{
			switch(j)
			{
				case 0:
				FTImpl.Cell(0);
				break;

				case 1:
				FTImpl.Cell(0);
				break;

				case 2:
				FTImpl.Cell(1);				
				break;
			}
		}
		xoff = linear(FT_DISPLAYWIDTH,-50,iteration_cts[j*10+i],rate_cts[j*10+i]);
		yoff = linear(FT_DISPLAYHEIGHT/2,yoffset_array[j*10+i],iteration_cts[j*10+i],rate_cts[j*10+i]);
		FTImpl.Vertex2f(xoff*16,yoff*16);
	  }
	}
	for(i = 0; i<30 ; i++)
	{
	    if(iteration_cts[i]==0)
	    {
			yoffset_array[i] = random(255);
			BitmapHandle[i] = 4+random(4);
	    }
	    if(iteration_cts[i]<rate_cts[i])iteration_cts[i]+=VEL; else{ iteration_cts[i] = 0;}
	}
	
	wave_cts = 490;	
	FTImpl.LineWidth(1*16);
	FTImpl.ColorA(255);
	
	FTImpl.Begin(FT_LINE_STRIP);
	FTImpl.Cmd_Append(FT_RAM_G,(wave_cts/10)*4);    
	FTImpl.End();
	
	FTImpl.Begin(FT_LINE_STRIP);
	FTImpl.Cmd_Append(FT_RAM_G+232,(wave_cts/10)*4);
	FTImpl.End();
	
	FTImpl.Begin(FT_LINE_STRIP);
	FTImpl.Cmd_Append(FT_RAM_G+2*232,(wave_cts/10)*4);
	FTImpl.End();
	
	FTImpl.Begin(FT_LINE_STRIP);
	FTImpl.Cmd_Append(FT_RAM_G+3*232,(wave_cts/10)*4);
	FTImpl.End();
	
	FTImpl.RestoreContext();
}
#endif

#ifdef BACKGROUND_ANIMATION_2

uint16_t point_size[20];
uint16_t xoffset_array[20],yoffset_array[20],dx_pts[20],dy_pts[20];
uint8_t color[20];

void Backgroundanimation_2()
{
	static uint8_t init = 0;	
	uint8_t ptclrarray[6][3] = {
		{0xb9,0xba,0xde},
		{0x0c,0x61,0xb7},
		{0x01,0x18,0x4e},
		{0xbf,0x25,0xbd},
		{0x29,0x07,0x3a},
		{0xc9,0x61,0x22}};
	int32_t i,ptradius,colorindex;
	static uint8_t fg = 0;
	float vel = 2;
	static float t=0,t1=0;	
	int32_t xoffset,yoffset,x1offset,y1offset,rate = 2000;
	uint8_t pts;
	if(!init)
	{
		init = 1;
		for(pts=0;pts<20;pts++)
		{
			point_size[pts] = 136 *16 +random(375*16); 
			xoffset_array[pts] = random(512)*16;
			yoffset_array[pts] = random(512)*16;
			color[pts] = random(5);
			dx_pts[pts] = 240*16+random(240*16);
			dy_pts[pts] = 130*16+random(142*16);	
		}
	}
	if(istouch())	
	{ t1 = 0; vel  = 5; } else {
		vel = linear(5,2,t1,100); 
		if(t1<100) t1++; }
	FTImpl.SaveContext();
	FTImpl.ColorMask(1,1,1,1);
	FTImpl.ColorA(255);


	//draw 20 points with various radius and additive blending
	FTImpl.Begin(FT_POINTS);
	FTImpl.BlendFunc(FT_SRC_ALPHA,FT_ONE);
	FTImpl.ColorMask(1,1,1,0);
	FTImpl.ColorA(50);

	/* compute points on top */
	for(i=0;i<5;i++)
	{
		ptradius = point_size[i]; 
		colorindex = color[i+0];
		FTImpl.ColorRGB(ptclrarray[colorindex][0],ptclrarray[colorindex][1],ptclrarray[colorindex][2]);
		yoffset = linear(dy_pts[i+0],yoffset_array[i+0],t,1000);		
		xoffset = xoffset_array[i+0];
		FTImpl.PointSize(ptradius);
		FTImpl.Vertex2f(xoffset,yoffset);
	}
	/* compute points on right */
	for(i=0;i<5;i++)
	{
		ptradius = point_size[5+i];
		colorindex = color[i+5];	
		FTImpl.ColorRGB(ptclrarray[colorindex][0],ptclrarray[colorindex][1],ptclrarray[colorindex][2]);				
		yoffset = linear(dy_pts[i+5],yoffset_array[i+5],t,1000);			
	
		yoffset = yoffset_array[i+5];		
		FTImpl.PointSize(ptradius);
		FTImpl.Vertex2f(xoffset,yoffset);
	}
	///* compute points on left */
	for(i=0;i<5;i++)
	{
		ptradius = point_size[10+i];
		colorindex = color[i+10];		
		
		FTImpl.ColorRGB(ptclrarray[colorindex][0],ptclrarray[colorindex][1],ptclrarray[colorindex][2]);		
		xoffset = linear(dx_pts[i+0],xoffset_array[i+0],t,1000);		
		yoffset = yoffset_array[i+5];
		FTImpl.PointSize(ptradius);
		FTImpl.Vertex2f(xoffset,yoffset);
	}
	/* compute points on bottom */
	for(i=0;i<5;i++)
	{
		ptradius = point_size[15+i];
		colorindex = color[i+15];		
		FTImpl.ColorRGB(ptclrarray[colorindex][0],ptclrarray[colorindex][1],ptclrarray[colorindex][2]);
		yoffset = linear(dy_pts[i+15],yoffset_array[i+15],t,1000);
		xoffset = xoffset_array[i+15];		
		FTImpl.PointSize(ptradius);
		FTImpl.Vertex2f(xoffset,yoffset);
	}
	//
	////draw additive blend lines diagonally
	FTImpl.ColorA(100);
	FTImpl.ColorRGB(32,32,32);
	FTImpl.Begin(FT_LINES);
	for(i=-1;i<4;i++)
	{
		xoffset = i*136;
		yoffset = 0;
		x1offset = 136*(2 + i);
		y1offset = 272;
		if(x1offset > 480)
		{
			y1offset = 272 - (x1offset - 480);
			x1offset = 480;
		}
		if(xoffset < 0)
		{
			yoffset = -xoffset;
			xoffset = 0;
		}
		FTImpl.Vertex2ii(xoffset,yoffset,0,0);
		FTImpl.Vertex2ii(x1offset,y1offset,0,0);
	}
	for(i=1;i<6;i++)
	{
		xoffset = i*136;
		yoffset = 0;
		x1offset = 136*(i - 2);
		y1offset = 272;
		if(x1offset < 0)
		{
			y1offset = 272 + x1offset;
			x1offset = 0;
		}
		if(xoffset > 480)
		{
			yoffset = (xoffset - 480);
			xoffset = 480;
		}
		FTImpl.Vertex2ii(xoffset,yoffset,0,0);
		FTImpl.Vertex2ii(x1offset,y1offset,0,0);
	}
	if(!fg)
	{
		if(t<rate) t+=vel; else
		fg = 1;   		
	}else
	{
		if(t>0) t-=vel;
		else
		fg=0;
	}
	FTImpl.RestoreContext();
	return;
}
#endif

#ifdef BACKGROUND_ANIMATION_3
uint16_t xoffset_array[30],dx_pts[20];
uint8_t yoffset_array[30],BitmapHandle[30],dy_pts[20];
uint8_t rate_cts[30],iteration_cts[30];

void Backgroundanimation_3()
{
	int32_t i=0,rate = 1000;
	uint8_t alpha  = 0,VEL = 0;
	static uint16_t t1 = 0;
	int16_t xoff = 0 ,yoff = 0;
	if(istouch())	
	{ 
		t1 = 0; VEL  = 2; 
	} 
	else {
		VEL = linear(2,1,t1,100); 
		if(t1<100) t1++; 
	}
	//Clear the background color
	FTImpl.SaveContext();
	FTImpl.ClearColorRGB(0,0,0);
	FTImpl.ClearColorA(0);
	FTImpl.Clear(1,1,1);
	FTImpl.ColorMask(1,1,1,0);
	//draw the cmd gradient with scissors
	FTImpl.ScissorSize(480,136);
	FTImpl.ScissorXY(0,0);
	FTImpl.Cmd_Gradient(0,0,0x708fa1,0,136,0xc4cdd2);
	FTImpl.ScissorXY(0,136);
	FTImpl.Cmd_Gradient(0,136,0xc4cdd2,0,272,0x4f7588);
	
	FTImpl.ScissorXY(0,0);
	FTImpl.ScissorSize(512,512);//reprogram with  default values

	//draw 20 points with various radious with additive blending
	FTImpl.Begin(FT_POINTS);	
	FTImpl.ColorMask(1,1,1,0);
	FTImpl.ColorRGB(255,255,255);
	for(i=0;i<20;i++)
	{
		//if(0 == i%4)		
		alpha  = linear(80,0,iteration_cts[i],rate_cts[i]);
		if(alpha<75)
		{
			FTImpl.PointSize(16*(30 + (3*i/2)));			
			FTImpl.ColorA(alpha);
			xoff = linear(xoffset_array[i],dx_pts[i],iteration_cts[i],rate_cts[i]);
			yoff = linear(yoffset_array[i],dy_pts[i],iteration_cts[i],rate_cts[i]);
			FTImpl.Vertex2f(xoff*16,yoff*16);
		}
	}
//	
	FTImpl.RestoreContext();
	for(i = 0; i<20 ; i++)
	{
		if(iteration_cts[i]==0)
		{
			xoffset_array[i] = random(FT_DISPLAYWIDTH);
			yoffset_array[i] = 100+random(FT_DISPLAYHEIGHT/4);
			dx_pts[i] = random(FT_DISPLAYWIDTH);
			dy_pts[i] = random(255);				
			rate_cts[i] = 100+random(155);
		}
		if(iteration_cts[i]<rate_cts[i])iteration_cts[i]+=VEL; else{ iteration_cts[i] = 0;}
	}

	return;
}
#endif

#ifdef BACKGROUND_ANIMATION_4

uint16_t xoffset_array[30],dx_pts[20];
uint8_t yoffset_array[30],BitmapHandle[30],dy_pts[20];
uint8_t rate_cts[30],iteration_cts[30];

void Backgroundanimation_4()
{
	static char init = 0;
	int32_t i,linesize;	
	int16_t xoff = 0 ,yoff = 0;
	uint8_t tval = 0,numBlobs=20,VEL = 0,alpha = 0;
	if(!init)
	{	
		/*Load background raw data*/
		FtSd.OpenFile(Imagefile,"hline.raw");	
		Load_file2gram(255*1024L,1,Imagefile);
		init = 1;
	}

	if(istouch())	
		VEL = 2; else VEL = 1;

	for(i = 0; i<numBlobs ; i++)
	{
		if(iteration_cts[i]==0)
		{
			xoffset_array[i] = random(FT_DISPLAYWIDTH);
			yoffset_array[i] = random(FT_DISPLAYHEIGHT);
			dx_pts[i] = random(FT_DISPLAYWIDTH);
			dy_pts[i] = random(FT_DISPLAYHEIGHT);	
			rate_cts[i] = 500+random(500);
		}
		if(iteration_cts[i]<rate_cts[i])iteration_cts[i]+=VEL; else{ iteration_cts[i] = 0;}
	}

	//draw the bitmap at location 0 with FT_RGB565
	FTImpl.SaveContext();
	FTImpl.BitmapHandle(1);//bitmap handle 2 is used for background balls
	FTImpl.BitmapSource(255*1024L);
	FTImpl.BitmapLayout(FT_L8, 512, 1);
	FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_REPEAT, 512, 512);

	//Clear the background color
	FTImpl.ClearColorRGB(255,255,255);
	FTImpl.ClearColorA(0);
	FTImpl.Clear(1,1,1);
	FTImpl.ColorMask(1,1,1,1);


	FTImpl.Cmd_LoadIdentity();
	FTImpl.Cmd_Rotate( 60*65536/360);//rotate by 30 degrees clock wise

	FTImpl.Cmd_SetMatrix();

	FTImpl.ColorMask(0,0,0,1);
	FTImpl.BlendFunc(FT_ONE,FT_ZERO);
	FTImpl.Begin(FT_BITMAPS);
	FTImpl.ColorRGB(0xff,0xb4,0x00);
	FTImpl.ColorRGB(255,255,255);
	FTImpl.Vertex2ii(0,0,1,0);	

	FTImpl.ColorMask(1,1,1,1);
	FTImpl.BlendFunc(FT_DST_ALPHA,FT_ONE_MINUS_DST_ALPHA);
	FTImpl.ColorRGB(0xff,0xb4,0x00);
	FTImpl.Begin(FT_RECTS);
	FTImpl.Vertex2ii(0,0,0,0);
	FTImpl.Vertex2ii(FT_DISPLAYWIDTH,FT_DISPLAYHEIGHT,0,0);

	FTImpl.Begin(FT_RECTS);
	FTImpl.BlendFunc(FT_SRC_ALPHA,FT_ONE_MINUS_DST_ALPHA);
	FTImpl.ColorMask(1,1,1,0);
	FTImpl.ColorRGB(0x96,0x6e,0x0d);
	FTImpl.LineWidth(16*1);
	for(i=0;i<numBlobs;i++)
	{
		int32_t xoffset,yoffset;
		if(0 == i%4)
		{
			linesize = 16*(25 + (3*i/4));
		}
		alpha  = linear(80,0,iteration_cts[i],rate_cts[i]);
		if(alpha<75)
		{
			xoff = linear(xoffset_array[i],dx_pts[i],iteration_cts[i],rate_cts[i]);
			yoff = linear(yoffset_array[i],dy_pts[i],iteration_cts[i],rate_cts[i]);
			FTImpl.Vertex2f(xoff*16,yoff*16);
			FTImpl.Vertex2f(xoff*16+linesize,yoff*16+linesize);
		}
	}
	FTImpl.RestoreContext();
}
#endif

#ifdef BACKGROUND_ANIMATION_5
static struct 
{
	uint8_t init;
	uint16_t xoffset_array[12];
	uint16_t yoffset_array[12];
	int16_t yoffset_array_source[12];
	uint16_t iteration_cts[12];
	uint8_t disable_cts[20][12];
	uint8_t radius_a[12];
	uint8_t radius_b[12];
	uint16_t angle[12];
	uint8_t number_of_firebubbles;
}firebubbles;


void draw_bubbles(uint8_t inc)
{
	int16_t i,j,yoff,xoff;
	for(j=0;j<3;j++)
	{
		FTImpl.Cell(j);
		if(j==2) 
			FTImpl.ColorA(150);
		else
			FTImpl.ColorA(255);
		for(i=0;i<firebubbles.number_of_firebubbles;i++)
		{
			if(firebubbles.iteration_cts[j*5+i] < firebubbles.yoffset_array[j*5+i]+0)
			{
				yoff = acceleration(firebubbles.yoffset_array_source[j*5+i],firebubbles.yoffset_array[j*5+i],firebubbles.iteration_cts[j*5+i],firebubbles.yoffset_array[j*5+i]*1);
				FTImpl.Vertex2f(firebubbles.xoffset_array[j*5+i]*16,yoff*16);

				if(inc){
				if(firebubbles.iteration_cts[j*5+i]<firebubbles.yoffset_array[j*5+i]*1)  firebubbles.iteration_cts[j*5+i]+=1;  					
				}
			}
		}
	}
}

void collaid_bubbles(uint8_t inc)
{
	int16_t i,j,k,yoff,xoff,temp;
	static uint8_t rate = 50;
	FTImpl.Cell(3);
	for(j=0;j<3;j++)
	{
		for(i=0;i<firebubbles.number_of_firebubbles;i++)
		{		
			if(firebubbles.iteration_cts[j*5+i]>=firebubbles.yoffset_array[j*5+i])
			{
				for(k=0;k<12;k++)	
				{
					FTImpl.ColorA(200-firebubbles.disable_cts[j*5+i][k]*10);		
					temp = (uint8_t)deceleration(0,firebubbles.radius_a[k],firebubbles.disable_cts[j*5+i][k],20);
					xoff = firebubbles.xoffset_array[j*5+i]+10 + (temp)*cos(firebubbles.angle[k]*0.01744);  //3.14/180=0.01744		
					temp = (uint8_t)deceleration(0,firebubbles.radius_b[k],firebubbles.disable_cts[j*5+i][k],20);
					yoff = firebubbles.yoffset_array[j*5+i]+10 + (temp)*sin(firebubbles.angle[k]*0.01744);  //3.14/180=0.01744
					yoff = firebubbles.yoffset_array[j*5+i]+10 + (temp)*sin(firebubbles.angle[k]*0.01744);  //3.14/180=0.01744
					FTImpl.Vertex2f(xoff*16,yoff*16);
					if(inc)
					{
						temp =  j*5+i;
						if(firebubbles.disable_cts[temp][k]<20)
						firebubbles.disable_cts[temp][k]++;
						else
						{
							firebubbles.disable_cts[temp][k] = 0;
							firebubbles.iteration_cts[temp] = 0;							
							firebubbles.xoffset_array[temp] = random(FT_DISPLAYWIDTH);
							firebubbles.yoffset_array_source[temp] = -50-random(100);
							if(j==0)
							firebubbles.yoffset_array[temp] = random(20)+(FT_DISPLAYHEIGHT-50); 		
							else if(j==1)
							firebubbles.yoffset_array[temp] = random(20)+(FT_DISPLAYHEIGHT-75); 		
							else if(j==2)
							firebubbles.yoffset_array[temp] = random(20)+(FT_DISPLAYHEIGHT-95); 		
							
						}
					}
				}
			}
		}
	}
}

void Backgroundanimation_5()
{
	int16_t i,j,yoff,xoff;
	firebubbles.number_of_firebubbles = 3;
	
	if(!firebubbles.init)
	{
		firebubbles.init = 1;		
		for(i=0;i<12;i++)
		{
			firebubbles.xoffset_array[i] = random(FT_DISPLAYWIDTH);
			firebubbles.yoffset_array[i] = random(50) + (FT_DISPLAYHEIGHT-80);
			firebubbles.yoffset_array_source[i] =  -50 - random(100);		
		}		
		for(i = 0;i<12;i++)
		{
				firebubbles.radius_a[i] = random(70);
				firebubbles.radius_b[i] = random(10);
				firebubbles.angle[i] =  random(360);
		}
		
		FtSd.OpenFile(Imagefile,"fire.raw");
		Load_file2gram(0,1,Imagefile);
		
		FtSd.OpenFile(Imagefile,"floor.raw");		
		Load_file2gram(7*1024L,1,Imagefile);
		
		FtSd.OpenFile(Imagefile,"grad.raw");		
		Load_file2gram(17*1024L,1,Imagefile);
		
		
		FTImpl.BitmapHandle(5);	
		FTImpl.BitmapSource(0);	
		FTImpl.BitmapLayout(FT_L8,40,40);
		FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, 40, 40);
		
		FTImpl.BitmapHandle(6);	
		FTImpl.BitmapSource(7*1024);	
		FTImpl.BitmapLayout(FT_L8,240,40);
		FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, 480, 80);
		
		FTImpl.BitmapHandle(8);	
		FTImpl.BitmapSource(17*1024);	
		FTImpl.BitmapLayout(FT_L8,60,30);
		FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, 120, 60);
		for(i=0;i<80;i++)
		{
			FTImpl.Write(19*1024L+i,255-(i*2));	
		}
		FTImpl.BitmapHandle(7);	
		FTImpl.BitmapSource(19*1024);	
		FTImpl.BitmapLayout(FT_L8,1,80);
		FTImpl.BitmapSize(FT_NEAREST, FT_REPEAT, FT_BORDER, 480, 80);
	}	
	FTImpl.SaveContext();
	FTImpl.Begin(FT_BITMAPS);
	FTImpl.ColorRGB(139,92,50);	
	
	FTImpl.BitmapTransformA(128);		
	FTImpl.BitmapTransformE(128);			
	FTImpl.Vertex2ii(0,FT_DISPLAYHEIGHT-80,6,0);	
	FTImpl.BitmapTransformA(170);		
	FTImpl.BitmapTransformE(200);
	FTImpl.BlendFunc(FT_SRC_ALPHA,FT_ONE);
	FTImpl.BitmapTransformA(256);		
	FTImpl.BitmapTransformE(256);
	FTImpl.BlendFunc(FT_SRC_ALPHA,FT_ONE_MINUS_SRC_ALPHA);
	FTImpl.ColorRGB(0,0,0);	
	FTImpl.Vertex2ii(0,FT_DISPLAYHEIGHT-80,7,0);
	FTImpl.BlendFunc(FT_SRC_ALPHA,FT_ONE);
	FTImpl.BitmapHandle(5);	
	FTImpl.ColorRGB(200,10,10);	
	FTImpl.ColorA(200);	
	draw_bubbles(0);	
	FTImpl.ColorRGB(255,220,3);	
	FTImpl.ColorA(255);	
	draw_bubbles(1);
	FTImpl.ColorRGB(200,10,10);	
	collaid_bubbles(0);	
	FTImpl.ColorRGB(255,220,3);	
	collaid_bubbles(1);		
	FTImpl.RestoreContext();
}
#endif

#ifdef BACKGROUND_ANIMATION_6

#define ANGLE 18 
int16_t xoffset_array[20],dx_pts[20],dy_pts[20];
uint8_t yoffset_array[20],point_size[20];
uint8_t rate_cts[20],iteration_cts[20];

void Backgroundanimation_6()
{
	static float move = 0;
	static uint8_t init = 0;
	int z= 0,x=0,y=0;
	int16_t Ox=0, Oy=0;
	uint8_t VEL = 0;
	if(istouch()) VEL = 2; else VEL = 1;
	if(!init)
	{
		for(z = 0; z<20 ; z++)
		{
			point_size[z] = 20 +random(20); 		
			dx_pts[z] = random(FT_DISPLAYWIDTH+120);
			if(dx_pts[z]<FT_DISPLAYWIDTH)
			dy_pts[z] = -random(FT_DISPLAYHEIGHT);
			else
			dy_pts[z] = random(FT_DISPLAYHEIGHT);				
			rate_cts[z] = 100+random(155);
		}
		init = 1;
	}
	
	move+=0.1; 
	if(move>=90)
	move = 0;
	FTImpl.SaveContext();
	FTImpl.Cmd_Gradient(0, 0, 0x183c78, 340, 0,0x4560dd);
	FTImpl.ColorRGB(255,255,255);
	FTImpl.ColorA(120);
	FTImpl.LineWidth(15*16);
	FTImpl.Begin(FT_LINES);
	for(z=0;z<20;z++)
	{
		polar((int32_t)(0),move+z*ANGLE,0,272);
		polar((int32_t)(600),move+z*ANGLE,0,272);
	}	
	FTImpl.ColorA(60);
	FTImpl.Begin(FT_POINTS);
	FTImpl.PointSize(95*16);
	FTImpl.Vertex2f(0,272*16);	
	FTImpl.ColorA(200);
	for(z=0;z<20;z++)
	{
		FTImpl.PointSize(point_size[z]*16);
		Ox = linear(0,dx_pts[z],iteration_cts[z],rate_cts[z]);
		Oy = linear(272,dy_pts[z],iteration_cts[z],rate_cts[z]);
		FTImpl.Vertex2f(Ox*16,Oy*16);	
	}	
	FTImpl.RestoreContext();
	for(z = 0; z<20 ; z++)
	{		
		if(iteration_cts[z]<rate_cts[z])iteration_cts[z]+=VEL; else{ iteration_cts[z] = 0;}
	}	
}
#endif
/* API for android method*/

void show_icon(uint8_t iconno)
{
	Ft_Play_Sound(0x51,100,108);
	do
	{
		 FTImpl.DLStart( );      
		FTImpl.Clear(1,1,1);
		#ifdef BACKGROUND_ANIMATION_1		
			Backgroundanimation_1();
		#endif
		#ifdef BACKGROUND_ANIMATION_2		
			Backgroundanimation_2();
		#endif
		#ifdef BACKGROUND_ANIMATION_3		
			Backgroundanimation_3();
		#endif
		#ifdef BACKGROUND_ANIMATION_4		
			Backgroundanimation_4();
		#endif
		#ifdef BACKGROUND_ANIMATION_5		
			Backgroundanimation_5();
		#endif		
		#ifdef BACKGROUND_ANIMATION_6		
			Backgroundanimation_6();
		#endif	
		FTImpl.Begin(FT_BITMAPS);
		FTImpl.SaveContext();
		FTImpl.BitmapHandle(0);		  
		FTImpl.BitmapSize(FT_NEAREST,FT_BORDER,FT_BORDER,200,100); 
		FTImpl.Cell(iconno-1);
		FTImpl.BitmapTransformA(128);
		FTImpl.BitmapTransformE(128);
		FTImpl.Vertex2f(((FT_DISPLAYWIDTH-200)/2)*16,((FT_DISPLAYHEIGHT-100)/2)*16);	
		FTImpl.RestoreContext();
		#ifdef BACKGROUND_ANIMATION_4
		FTImpl.ColorRGB(0,0,0);
		#endif
		FTImpl.Tag('H');
		FTImpl.Vertex2ii(5,5,14,0);	
		FTImpl.Display();
		FTImpl.Cmd_Swap();
		FTImpl.Finish();
	}while(Read_keys()!='H');
	Ft_Play_Sound(0x51,100,108);
	scroller.vel = 0;
}

void android_menu()
{
	uint8_t image_height = 50,image_width = 100;
	uint8_t dt = 30,dx,dy;
	uint8_t col,row,per_frame,noof_frame,current_frame=0;
	uint8_t i,key_in=0;

	int16_t Ox,Oy,sx,drag=0,prev=0,drag_dt=30,dragth=0;

// for points

	uint8_t  point_offset,point_dt =15;

 
	dx = (dt*2)+image_width;	
	dy = (10*2)+image_height;
	col = FT_DISPLAYWIDTH/dx;
	row = 2;
	per_frame = col*row;
	noof_frame = (MAX_MENUS-1)/per_frame;

	point_offset = (FT_DISPLAYWIDTH-(noof_frame+1)*(MENU_POINTSIZE+point_dt))/2;

	scroller_init((FT_DISPLAYWIDTH*noof_frame)*16);

	while(1)
	{
		/*Read touch screen x varaiation and tag in*/
		sx =  FTImpl.Read16(REG_TOUCH_SCREEN_XY + 2);
		key_in =  Read_keys();
		
		/*Check if any tag in*/    
		if(sx!=NOTOUCH)	
		keyin_cts++;     	
		
		/*Move into the particular frame based on dragdt now 30pixels*/
		if(sx==NOTOUCH)
		{
			keyin_cts = 0; 		
			if(drag>((current_frame*FT_DISPLAYWIDTH)+drag_dt)) drag = min((current_frame+1)*FT_DISPLAYWIDTH,drag+15); 
			if(drag<((current_frame*FT_DISPLAYWIDTH)-drag_dt)) drag = max((current_frame-1)*FT_DISPLAYWIDTH,drag-15); 
			if(dragth==drag) current_frame = drag/FT_DISPLAYWIDTH;
			dragth = drag;
			scroller.vel = 0; 
			scroller.base = dragth*16;				// 16bit pre
		}
		/*if tag in but still pendown take a scroller basevalue*/
		else if(keyin_cts>KEYIN_COUNTS)
		{
			key_in = 0;
			drag = scroller.base>>4;
		}
		if(key_in==0)  scroller_run();

		/*Display list start*/   
		 FTImpl.DLStart( );      
		FTImpl.Clear(1,1,1);
		#ifdef BACKGROUND_ANIMATION_1		
			Backgroundanimation_1();
		#endif
		#ifdef BACKGROUND_ANIMATION_2		
			Backgroundanimation_2();
		#endif
		#ifdef BACKGROUND_ANIMATION_3		
			Backgroundanimation_3();
		#endif
		#ifdef BACKGROUND_ANIMATION_4		
			Backgroundanimation_4();
		#endif
		#ifdef BACKGROUND_ANIMATION_5		
			Backgroundanimation_5();
		#endif		
		#ifdef BACKGROUND_ANIMATION_6		
			Backgroundanimation_6();
		#endif	
		FTImpl.BitmapHandle(0);
		FTImpl.BitmapSize(FT_NEAREST,FT_BORDER,FT_BORDER,100,50);
		FTImpl.TagMask(1);
		FTImpl.ColorRGB(255,255,255);
		FTImpl.LineWidth(25);				// for rect	
		FTImpl.Begin(FT_RECTS);	
		FTImpl.ColorA(150);
		FTImpl.ColorRGB(100,106,156);
		Oy = 10;
		for(i=0;i<=noof_frame;i++)
		{
			Ox = 10;  
			Ox+=(i*FT_DISPLAYWIDTH);
			Ox-=drag;
			if(i==0) FTImpl.ColorRGB(156,100,128);
			if(i==1) FTImpl.ColorRGB(100,106,156);
			if(i==2) FTImpl.ColorRGB(156,152,100);   
			FTImpl.Vertex2f((Ox)*16,(Oy)*16);	
			FTImpl.Vertex2f((Ox+FT_DISPLAYWIDTH-20)*16,(int16_t)(FT_DISPLAYHEIGHT*0.75)*16);					// i pixels wide than image width +1 
		}
		FTImpl.ColorA(255);
		FTImpl.ColorRGB(255,255,255); 
		for(i=0;i<MAX_MENUS;i++)
		{
			Ox = dt+dx*(i%col);                                          // Calculate the xoffsets
			Ox +=((i/per_frame)*FT_DISPLAYWIDTH);       
			Ox -= drag;        
			Oy = dt+(dy*((i/col)%row));
			if(Ox > (FT_DISPLAYWIDTH+dt)) 0;	
			else
			{
				FTImpl.Vertex2f((Ox-1)*16,(Oy-1)*16);	
				FTImpl.Vertex2f((image_width+Ox+1)*16,(image_height+Oy+1)*16);
			}					// i pixels wide than image width +1
		} 
		FTImpl.TagMask(1); 
		FTImpl.Begin(FT_BITMAPS);		
		FTImpl.BitmapHandle(0);
		for(i=0;i<MAX_MENUS;i++)
		{
			Ox = dt+dx*(i%col);                                          // Calculate the xoffsets
			Ox +=((i/per_frame)*FT_DISPLAYWIDTH);       
			Ox -= drag;        
			Oy = dt+(dy*((i/col)%row));	
			if(Ox > (FT_DISPLAYWIDTH+dt) || Ox < -dx) 0;	
			else
			{
				FTImpl.Cell(i);
				FTImpl.Tag(i+1);	
				FTImpl.Vertex2f(Ox*16,Oy*16);	
			}
		}
		FTImpl.TagMask(0);

		// frame_no_points

		FTImpl.PointSize(MENU_POINTSIZE*16);
		FTImpl.Begin(FT_POINTS);				
		FTImpl.ColorA(50);	
		Oy = FT_DISPLAYHEIGHT - 20;
		for(i=0;i<=noof_frame;i++)
		{
		  Ox = point_offset+(i*(MENU_POINTSIZE+point_dt));
		  FTImpl.Vertex2f(Ox*16,Oy*16);		
		}

		Ox = point_offset+(current_frame*(MENU_POINTSIZE+point_dt));
		FTImpl.ColorA(255);
		FTImpl.Vertex2f(Ox*16,Oy*16);	

		FTImpl.Display();
		FTImpl.Cmd_Swap();
		FTImpl.Finish();
		if(key_in>0 && key_in<=12 && !istouch())
		show_icon(key_in);
	}
}

// Api to Single Row//
void menu_loopback()
{
	uint8_t keyin_cts = 0,temp_keyin;
	uint8_t image_height = 50,image_width = 100; 
	uint8_t dt = 30,dx,dy;
	uint8_t per_frame,no_frames,key_in,current_frame; 
	int16_t sx,drag,Oy,Ox,dragth,i;
	dx = (dt*2)+image_width;	
	dy = (10*2)+image_height;
	per_frame = FT_DISPLAYWIDTH/dx;
	no_frames = (MAX_MENUS-1)/per_frame;

	scroller_init((FT_DISPLAYWIDTH*no_frames)*16);
	while(1)
	{
		/*Read touch screen x varaiation and tag in*/
		sx =  FTImpl.Read16(REG_TOUCH_SCREEN_XY + 2);
		key_in =  Read_keys();

		/*Check if any tag in*/    
		if(sx!=NOTOUCH)	
		keyin_cts++;     	
		
		/*Move into the particular frame based on dragdt now 30pixels*/
		if(sx==NOTOUCH)
		{
			keyin_cts = 0;
			drag = scroller.base>>4; 	
			drag = scroller.base>>4; 	
		}
		/*if tag in but still pendown take a scroller basevalue*/
		else if(keyin_cts>KEYIN_COUNTS)
		{
			key_in = 0;		
			drag = scroller.base>>4;
		}
		if(key_in==0)scroller_run();
		
		FTImpl.DLStart( );      
		FTImpl.Clear(1,1,1);
		#ifdef BACKGROUND_ANIMATION_1		
			Backgroundanimation_1();
		#endif
		#ifdef BACKGROUND_ANIMATION_2		
			Backgroundanimation_2();
		#endif
		#ifdef BACKGROUND_ANIMATION_3		
			Backgroundanimation_3();
		#endif
		#ifdef BACKGROUND_ANIMATION_4		
			Backgroundanimation_4();
		#endif
		#ifdef BACKGROUND_ANIMATION_5		
			Backgroundanimation_5();
		#endif		
		#ifdef BACKGROUND_ANIMATION_6		
			Backgroundanimation_6();
		#endif			
		FTImpl.BitmapHandle(0);	 
		FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, 100, 50);	
		FTImpl.TagMask(1);
		FTImpl.ColorRGB(255,255,255);
		FTImpl.LineWidth(1*16); // for rect	

		Oy = (FT_DISPLAYHEIGHT-image_width)/2;					//dt+(dy*((i/col)%row));
		current_frame = drag/dx;                       // noof items moved in +/- directions
		dragth = drag%dx;

		FTImpl.Begin(FT_RECTS);
		for(i=-1;i<(per_frame+1);i++)
		{
			Ox = dt+dx*i;
			Ox-=dragth;
			if(Ox > (FT_DISPLAYWIDTH+dt) || Ox < -dx) 0;						
			else
			{      
				FTImpl.Vertex2f((Ox-1)*16,(Oy-1)*16);	
				FTImpl.Vertex2f((image_width+Ox+1)*16,(image_height+Oy+1)*16);					// i pixels wide than image width +1  
			}
		}

		FTImpl.Begin(FT_BITMAPS);											// draw the bitmap
		FTImpl.BitmapHandle(0);
		for(i=-1;i<(per_frame+1);i++)
		{
			Ox = dt+dx*i;
			Ox-=dragth;
			if(Ox > (FT_DISPLAYWIDTH+dt) || Ox < -dx) 0;						
			else
			{ 
				FTImpl.Cell((MAX_MENUS+i+current_frame)%12);
				FTImpl.Tag((1+i+current_frame)%(MAX_MENUS+1));	
				FTImpl.Vertex2f(Ox*16,Oy*16);	
			}
		}
		
		FTImpl.Display();
		FTImpl.Cmd_Swap();
		FTImpl.Finish();
		if(key_in>0 && key_in<=12 && !istouch())
		show_icon(key_in);
	}  
}  

// Api to tiles type menu*/
void menu_win8()
{
	uint8_t touch_detect_cts = 0,current_frame = 0,total_frames = 0,key_in=0;	
	int16_t frame_xoffset = 0,frame_xoffset_th= 0; 
	uint8_t frame_xoffset_dt = 30,menus_per_frame = 0;
	uint8_t col = 3,row = 2,option;
	uint8_t image_height = 50,image_width = 100,rectangle_width,rectangle_height; 
	int16_t Ox,Oy,i,sx;
	
	uint8_t color[12][3] = 
	{  
		0xE0,0x01B,0xA2,
		0x1B,0xE0,0xA8,
		0x9E,0x9E,0x73,
		0xE0,0x8E,0x1B,
		0xB8,0x91,0xB3,
		0x6E,0x96,0x8e,
		0x1B,0x60,0xE0,
		0xC7,0xE3,0x7B,
		0x8B,0x1B,0xE0,
		0xE3,0x91,0xC1,
		0xE0,0x8E,0x1B,
		0xAC,0x91,0xE3,
	};

	char *menudetails[20]=	{  "Music",  "Gauges ",  "Gradient",  "Photo",  "Metaball",  "Notepad",  "Signature",  "Sketch","Swiss","Waves","Player","Clocks"};

// for points
	uint8_t  point_offset,frame_point_dt =15;
	uint16_t dx = (frame_xoffset_dt*2)+image_width;	
	uint16_t dy = (10*2)+image_height;
	col = FT_DISPLAYWIDTH/dx;
	menus_per_frame = col*row;
	total_frames = (MAX_MENUS-1)/menus_per_frame;

	point_offset = (FT_DISPLAYWIDTH-(total_frames+1)*(MENU_POINTSIZE+frame_point_dt))/2;	
	/*Load menu Thumbnails*/
	Load_Thumbnails();
	/*Intilaize the scroller*/
	scroller_init((FT_DISPLAYWIDTH*total_frames)*16);
	while(1)
	{
		/*Read touch screen x varaiation and tag in*/
		sx =  FTImpl.Read16(REG_TOUCH_SCREEN_XY + 2);
		key_in =  Read_keys();

		/*Check if any tag in*/    
		if(sx!=NOTOUCH)	
		keyin_cts++;     	
			   
		
		/*Move into the particular frame based on dragdt now 30pixels*/
		if(sx==NOTOUCH)
		{
			keyin_cts = 0;
			frame_xoffset = scroller.base>>4; 	
		}
		/*if tag in but still pendown take a scroller basevalue*/
		else if(keyin_cts>KEYIN_COUNTS)
		{
			key_in = 0;
			frame_xoffset = scroller.base>>4;
		}
		if(key_in==0)scroller_run();
		
		FTImpl.DLStart( );      
	    FTImpl.Clear(1,1,1);
		FTImpl.BitmapHandle(0);		  
		FTImpl.BitmapSize(FT_NEAREST,FT_BORDER,FT_BORDER,100,50); 
	    FTImpl.SaveContext();
		#ifdef BACKGROUND_ANIMATION_1
				Backgroundanimation_1();
		#endif

		#ifdef BACKGROUND_ANIMATION_2
				Backgroundanimation_2();
		#endif
		#ifdef BACKGROUND_ANIMATION_3
				Backgroundanimation_3();
		#endif
		#ifdef BACKGROUND_ANIMATION_4
				Backgroundanimation_4();
		#endif

		#ifdef BACKGROUND_ANIMATION_5
				Backgroundanimation_5();
		#endif
		#ifdef BACKGROUND_ANIMATION_6		
				Backgroundanimation_6();
		#endif
	    FTImpl.RestoreContext();
	   	   
// Draw the backround Rectangles for the icon 	
#if (FT_DISPLAY_RESOLUTION == FT_DISPLAY_WQVGA_480x272)
		for(option=0;option<3;option++)
		{
		   switch(option)
		   {
			  case 0:
				 FTImpl.LineWidth(1*16);
				 FTImpl.Begin(FT_RECTS);
			  break;
			  case 1:
				  FTImpl.Begin(FT_BITMAPS);
				  FTImpl.ColorRGB(255,255,255);
			  break;
			  case 2:	  
				  FTImpl.ColorRGB(255,255,255);
			  break;
		   }
		   rectangle_width = 220;
		   rectangle_height = 100;
		   for(i=0;i<4;i+=1)
		   {
				if(i<2)
				{
					Ox = 10+FT_DISPLAYWIDTH*i;
					Oy = 10;
				}else
				{	
					Ox = 250+FT_DISPLAYWIDTH*(i%2);
					Oy = 120;
				}
				Ox -= frame_xoffset;		
				if(Ox > (FT_DISPLAYWIDTH+frame_xoffset_dt) || Ox < -512) 0;	
				else
				{	
					FTImpl.Tag(i+1);	
					switch(option)
					{
						case 0:
							FTImpl.ColorRGB(color[i][0],color[i][1],color[i][2]);					
							FTImpl.Vertex2f((Ox)*16,(Oy)*16);	
							FTImpl.Vertex2f((rectangle_width+Ox)*16,(rectangle_height+Oy)*16);					// i pixels wide than image width +1
						break;

						case 1:
							FTImpl.Cell(i);	
							FTImpl.Vertex2f((55+Ox)*16,(25+Oy)*16);	
						break;

						case 2:
							FTImpl.Cmd_Text(Ox+10,Oy+80,26,0,menudetails[i]);
						break;

					}
					
				}
		   }
		   rectangle_width = 100;
		   rectangle_height = 100;		
		   if(option==1)  FTImpl.BitmapTransformA(512);
		  
		   for(i=0;i<8;i+=1)
		   {
				if(i<4)
				{
					Ox = 250+FT_DISPLAYWIDTH*(i/2)+(image_width*(i%2))+(20*(i%2));	// 20 is space between two icon
					Oy = 10;
				}
				else
				{	
					Ox = 10+FT_DISPLAYWIDTH*(i/6)+ (((i-4)%2)*image_width)+(((i-4)%2)*20);
					Oy = 120;
				}
				Ox -= frame_xoffset;		
				if(Ox > (FT_DISPLAYWIDTH+frame_xoffset_dt) || Ox < -512) 0;	
				else
				{
					FTImpl.Tag(i+5);	
					switch(option)
					{	
						case 0:
								FTImpl.ColorRGB(color[i+5][0],color[i+5][1],color[i+5][2]);					
								FTImpl.Vertex2f((Ox)*16,(Oy)*16);	
								FTImpl.Vertex2f((rectangle_width+Ox)*16,(rectangle_height+Oy)*16);					// i pixels wide than image width +1
						break;

						case 1:							
								FTImpl.Cell(i+4);														
								FTImpl.Vertex2f((25+Ox)*16,(25+Oy)*16);	
						break;

						case 2:
								FTImpl.Cmd_Text(Ox+10,Oy+80,26,0,menudetails[i+4]);
						break;
					}								// i pixels wide than image width +1
				}
		   }
		   if(option==1)  FTImpl.BitmapTransformA(256);
		}			

#elif (FT_DISPLAY_RESOLUTION == FT_DISPLAY_WQVGA_320x240)
		rectangle_width = 170;
		rectangle_height = 100;	
		FTImpl.Begin(FT_RECTS);
		for(i=0;i<12;i+=2)
		{
			Ox = 10+FT_DISPLAYWIDTH*(i/4);
			Ox -= frame_xoffset;
			Oy = 10+(110*((i/col)%row));
			FTImpl.ColorRGB(color[i][0],color[i][1],color[i][2]);
			FTImpl.Tag(i+1);	
			FTImpl.Vertex2f((Ox)*16,(Oy)*16);	
			FTImpl.Vertex2f((rectangle_width+Ox)*16,(rectangle_height+Oy)*16);					// i pixels wide than image width +1
		}

		rectangle_width = 110;
		rectangle_height = 100;	

		for(i=1;i<12;i+=2)
		{
			Ox = 200+FT_DISPLAYWIDTH*(i/4);
			Ox -= frame_xoffset;
			Oy = 10+(110*((i/col)%row));
			FTImpl.ColorRGB(color[i][0],color[i][1],color[i][2]);
			FTImpl.Tag(i+1);	
			FTImpl.Vertex2f((Ox)*16,(Oy)*16);	
			FTImpl.Vertex2f((rectangle_width+Ox)*16,(rectangle_height+Oy)*16);					// i pixels wide than image width +1
		}
		FTImpl.TagMask(0);

		FTImpl.ColorRGB(255,255,255);
		for(i=0;i<12;i+=2)
		{
			Ox = 10+FT_DISPLAYWIDTH*(i/4);
			Ox -= frame_xoffset;
			Oy = 10+(110*((i/col)%row));
			FTImpl.Cmd_Text(Ox+10,Oy+80,26,0,menudetails[i]);
		}
		for(i=1;i<12;i+=2)
		{
			Ox = 200+FT_DISPLAYWIDTH*(i/4);
			Ox -= frame_xoffset;
			Oy = 10+(110*((i/col)%row));
			FTImpl.Cmd_Text(Ox+10,Oy+80,26,0,menudetails[i]);
		}
			
		rectangle_height = 100;
		rectangle_height = 50;
		FTImpl.Begin(FT_BITMAPS);											// draw the bitmap
		FTImpl.BitmapHandle(0);
		for(i=0;i<12;i+=2)
		{
			Ox = 75+FT_DISPLAYWIDTH*(i/4);
			Ox -= frame_xoffset;
			Oy = 20+(110*((i/col)%row));
			FTImpl.Cell(i);
			FTImpl.Tag(i+1);	
			FTImpl.Vertex2f(Ox*16,Oy*16);	
		}
		FTImpl.BitmapTransformA(512);
		for(i=1;i<12;i+=2)
		{
			Ox = 230+FT_DISPLAYWIDTH*(i/4);
			Ox -= frame_xoffset;
			Oy = 20+(110*((i/col)%row));
			FTImpl.Cell(i);    
			FTImpl.Tag(i+1);	
			FTImpl.Vertex2f(Ox*16,Oy*16);	
		}
		FTImpl.BitmapTransformA(256);
  
#endif
		FTImpl.Display();
		FTImpl.Cmd_Swap();
		FTImpl.Finish();
		if(key_in>0 && key_in<=12 && !istouch())
		show_icon(key_in);
	}
}

/* Api to bootup ft801, verify FT801 hardware and configure display/audio pins */
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

	/*Initialize the SD object. Screen error message can only be displayed when the FT801 is ready*/
	sd_present = FtSd.Init();
	
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
		Load_Thumbnails();
		#ifdef ANDROID_METHOD
		android_menu();
		#endif 
		#ifdef LOOPBACK_METHOD
		menu_loopback();
		#endif
		#ifdef WIN8_METHOD
		menu_win8();
		#endif
		FTImpl.Exit();
	}
	Serial.println("--End Application--");
}

void loop()
{
}













  








