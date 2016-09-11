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
* @file                           FT_App_Signals.ino
* @brief                          Sample application to demonstrat FT801 primitives, widgets and customized screen shots 
* @version                        0.1.0
* @date                           2014/03/21
*
*/

#include "SPI.h"
#include "Wire.h"
#include "FT_NHD_43CTP_SHIELD.h"


/* Global object for transport */
FT801IMPL_SPI FTImpl(FT_CS_PIN,FT_PDN_PIN,FT_INT_PIN);


PROGMEM prog_uchar home_star_icon[] = {0x78,0x9C,0xE5,0x94,0xBF,0x4E,0xC2,0x40,0x1C,0xC7,0x7F,0x2D,0x04,0x8B,0x20,0x45,0x76,0x14,0x67,0xA3,0xF1,0x0D,0x64,0x75,0xD2,0xD5,0x09,0x27,0x17,0x13,0xE1,0x0D,0xE4,0x0D,0x78,0x04,0x98,0x5D,0x30,0x26,0x0E,0x4A,0xA2,0x3E,0x82,0x0E,0x8E,0x82,0xC1,0x38,0x62,0x51,0x0C,0x0A,0x42,0x7F,0xDE,0xB5,0x77,0xB4,0x77,0x17,0x28,0x21,0x26,0x46,0xFD,0x26,0xCD,0xE5,0xD3,0x7C,0xFB,0xBB,0xFB,0xFD,0xB9,0x02,0xCC,0xA4,0xE8,0x99,0x80,0x61,0xC4,0x8A,0x9F,0xCB,0x6F,0x31,0x3B,0xE3,0x61,0x7A,0x98,0x84,0x7C,0x37,0xF6,0xFC,0xC8,0xDD,0x45,0x00,0xDD,0xBA,0xC4,0x77,0xE6,0xEE,0x40,0xEC,0x0E,0xE6,0x91,0xF1,0xD2,0x00,0x42,0x34,0x5E,0xCE,0xE5,0x08,0x16,0xA0,0x84,0x68,0x67,0xB4,0x86,0xC3,0xD5,0x26,0x2C,0x20,0x51,0x17,0xA2,0xB8,0x03,0xB0,0xFE,0x49,0xDD,0x54,0x15,0xD8,0xEE,0x73,0x37,0x95,0x9D,0xD4,0x1A,0xB7,0xA5,0x26,0xC4,0x91,0xA9,0x0B,0x06,0xEE,0x72,0xB7,0xFB,0xC5,0x16,0x80,0xE9,0xF1,0x07,0x8D,0x3F,0x15,0x5F,0x1C,0x0B,0xFC,0x0A,0x90,0xF0,0xF3,0x09,0xA9,0x90,0xC4,0xC6,0x37,0xB0,0x93,0xBF,0xE1,0x71,0xDB,0xA9,0xD7,0x41,0xAD,0x46,0xEA,0x19,0xA9,0xD5,0xCE,0x93,0xB3,0x35,0x73,0x0A,0x69,0x59,0x91,0xC3,0x0F,0x22,0x1B,0x1D,0x91,0x13,0x3D,0x91,0x73,0x43,0xF1,0x6C,0x55,0xDA,0x3A,0x4F,0xBA,0x25,0xCE,0x4F,0x04,0xF1,0xC5,0xCF,0x71,0xDA,0x3C,0xD7,0xB9,0xB2,0x48,0xB4,0x89,0x38,0x20,0x4B,0x2A,0x95,0x0C,0xD5,0xEF,0x5B,0xAD,0x96,0x45,0x8A,0x41,0x96,0x7A,0x1F,0x60,0x0D,0x7D,0x22,0x75,0x82,0x2B,0x0F,0xFB,0xCE,0x51,0x3D,0x2E,0x3A,0x21,0xF3,0x1C,0xD9,0x38,0x86,0x2C,0xC6,0x05,0xB6,0x7B,0x9A,0x8F,0x0F,0x97,0x1B,0x72,0x6F,0x1C,0xEB,0xAE,0xFF,0xDA,0x97,0x0D,0xBA,0x43,0x32,0xCA,0x66,0x34,0x3D,0x54,0xCB,0x24,0x9B,0x43,0xF2,0x70,0x3E,0x42,0xBB,0xA0,0x95,0x11,0x37,0x46,0xE1,0x4F,0x49,0xC5,0x1B,0xFC,0x3C,0x3A,0x3E,0xD1,0x65,0x0E,0x6F,0x58,0xF8,0x9E,0x5B,0xDB,0x55,0xB6,0x41,0x34,0xCB,0xBE,0xDB,0x87,0x5F,0xA9,0xD1,0x85,0x6B,0xB3,0x17,0x9C,0x61,0x0C,0x9B,0xA2,0x5D,0x61,0x10,0xED,0x2A,0x9B,0xA2,0x5D,0x61,0x10,0xED,0x2A,0x9B,0xA2,0x5D,0x61,0x10,0xED,0x2A,0x9B,0xED,0xC9,0xFC,0xDF,0x14,0x54,0x8F,0x80,0x7A,0x06,0xF5,0x23,0xA0,0x9F,0x41,0xF3,0x10,0x30,0x4F,0x41,0xF3,0x18,0x30,0xCF,0xCA,0xFC,0xFF,0x35,0xC9,0x79,0xC9,0x89,0xFA,0x33,0xD7,0x1D,0xF6,0x5E,0x84,0x5C,0x56,0x6E,0xA7,0xDA,0x1E,0xF9,0xFA,0xAB,0xF5,0x97,0xFF,0x2F,0xED,0x89,0x7E,0x29,0x9E,0xB4,0x9F,0x74,0x1E,0x69,0xDA,0xA4,0x9F,0x81,0x94,0xEF,0x4F,0xF6,0xF9,0x0B,0xF4,0x65,0x51,0x08};

char *info[] = { "FT801 Signals Application",
                         "APP to demonstrate drawing Signals,",
                         "using Strips, Points",
                         "& Blend function"
                       }; 
			   
			   
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


/***********************API used to SET the ICON******************************************/
/*Refer the code flow in the flowchart availble in the Application Note */


void home_setup()
{

	
  /*Icon  file is deflated use J1 Command to inflate the file and write into the GRAM*/
  FTImpl.WriteCmd(CMD_INFLATE);
  FTImpl.WriteCmd(250*1024L);
  FTImpl.WriteCmdfromflash(home_star_icon,sizeof(home_star_icon));
  /*Set the Bitmap properties for the ICONS*/ 	
  FTImpl.DLStart( );        // start
  FTImpl.Clear(1,1,1);
  FTImpl.ColorRGB(255, 255, 255);
  FTImpl.BitmapHandle(13);    // handle for background stars
  FTImpl.BitmapSource(250*1024L);      // Starting address in gram
  FTImpl.BitmapLayout(FT_L4, 16, 32);  // format 
  FTImpl.BitmapSize(FT_NEAREST, FT_REPEAT, FT_REPEAT, 512, 512);
  FTImpl.BitmapHandle(14);    // handle for background stars
  FTImpl.BitmapSource(250*1024L);      // Starting address in gram
  FTImpl.BitmapLayout(FT_L4, 16, 32);  // format 
  FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, 32, 32  );
  FTImpl.Display();
  FTImpl.Cmd_Swap();
  FTImpl.Finish();
}
/********API to return the assigned TAG value when penup,for the primitives/widgets******/

static uint8_t keypressed=0;
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
      keypressed = Read_tag;										// Load the Read tag to temp variable	
    }  
  }
  else
  {
    if(temp_tag!=0)
    {
      ret_tag = temp_tag;
	  temp_tag = 0;
    }  
    keypressed = 0;
  }
  return ret_tag;
}

// Info Screen//
void Info()
{
  uint16_t dloffset = 0,z;

  FTImpl.DLStart( ); 
  FTImpl.Clear(1,1,1);
  FTImpl.ColorRGB(255,255,255);
  FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,FT_DISPLAYHEIGHT/2,26,FT_OPT_CENTERX|FT_OPT_CENTERY,"Please tap on a dot");
  FTImpl.Cmd_Calibrate(0);
  FTImpl.Display();
  FTImpl.Cmd_Swap();
  FTImpl.Finish();
// Ftdi Logo animation 
  FTImpl.Cmd_Logo();
	while(FTImpl.CheckLogo() != FT_GE_FINISHED);
  dloffset = FTImpl.Read16(REG_CMD_DL);
  dloffset -=4;
  //Copy the Displaylist from DL RAM to GRAM
  FTImpl.WriteCmd(CMD_MEMCPY);
  FTImpl.WriteCmd(100000L);
  FTImpl.WriteCmd(FT_RAM_DL);
  FTImpl.WriteCmd(dloffset);
  //Enter into Info Screen
  do
  {
    FTImpl.DLStart( );   
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
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,20,28,FT_OPT_CENTERX|FT_OPT_CENTERY,(char*)info[0]);
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,60,26,FT_OPT_CENTERX|FT_OPT_CENTERY,(char*)info[1]);
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,90,26,FT_OPT_CENTERX|FT_OPT_CENTERY,(char*)info[2]);  
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,120,26,FT_OPT_CENTERX|FT_OPT_CENTERY,(char*)info[3]);  
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,FT_DISPLAYHEIGHT-30,26,FT_OPT_CENTERX|FT_OPT_CENTERY,"Click to play");
    //Check if the Play key enter then change the color
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
    FTImpl.Display();
    FTImpl.Cmd_Swap();
	FTImpl.Finish();
  }while(Read_Keys()!='P');  
  /* wait until Play key is not pressed*/ 
} 


static uint16_t add2write = 0;
static uint8_t rate = 1;
static uint16_t x,y,tx;
static uint8_t beats[10];
static int8_t beats_Incr[10] = {-10,10,5,-5,-20,20,12,-12,-5,5};

/* api for signal selection buttons*/

void Ft_Gpu_Radiobutton(uint16_t x,uint16_t y,uint32_t bgcolor,uint32_t fgcolor,uint8_t psize,uint8_t tag,uint8_t option)
{
  uint8_t check_size = psize/2;
  FTImpl.SaveContext();
  FTImpl.ColorRGB((bgcolor >> 16 & 0xff),(bgcolor >> 8 & 0xff),(bgcolor & 0xff));
  FTImpl.Tag(tag);
  FTImpl.PointSize(psize*16);
  FTImpl.Begin(FT_POINTS);
  FTImpl.Vertex2f(x*16,y*16);
  if(tag == option)
  {
    FTImpl.ColorRGB((fgcolor >> 16 & 0xff),(fgcolor >> 8 & 0xff),(fgcolor & 0xff));
    FTImpl.PointSize(check_size*16);
    FTImpl.Vertex2f(x*16,y*16);
  }
  FTImpl.RestoreContext();  
}




/* api to generate the Sine wave*/

void Sine_wave(uint8_t amp)
{
  static uint8_t played = 0,change=0;
  x+=rate;
  if(x>FT_DISPLAYWIDTH) x  = 0;
  y = (FT_DISPLAYHEIGHT/2) + ((int32_t)amp*qsin(-65536*x/(25*rate))/65536);
  if(played==0 &&  change < y){ 
  played = 1;
  FTImpl.PlaySound(255,(108<<8 | 0x10)); }
  if(change > y)
  played = 0;
  change = y;
  FTImpl.Write32(FT_RAM_G+(x/rate)*4,VERTEX2F(x*16,y*16));	
}

/* api to generate the sawtooth wave*/
void Sawtooth_wave(uint8_t amp)
{
  static uint16_t temp=0;
  static uint8_t pk = 0;
  x+=rate;
  if(x>FT_DISPLAYWIDTH){ x  = 0;}
  temp+=2; if(temp>65535L) temp = 0;
  y = (temp % amp);
  pk = y/(amp-2);
  if(pk) FTImpl.PlaySound(255,(108<<8 | 0x10));				 
  y = (FT_DISPLAYHEIGHT/2)-y;
  FTImpl.Write32(FT_RAM_G+(x/rate)*4,VERTEX2F(x*16,y*16));	
}
/* api to generate the Triangle wave*/
void Triangle_wave(uint8_t amp)
{
  static uint16_t temp=0;
 static uint8_t pk = 0,dc=0,p=0;
  x+=rate;
  if(x>FT_DISPLAYWIDTH){ x  = 0;}
  temp+=2; if(temp>65535L) temp = 0;
  y = (temp % amp);
  pk = (y/(amp-2))%2;
  dc = (temp / amp)%2;  
  if(pk) { if(p==0){ p=1; FTImpl.PlaySound(255,(108<<8 | 0x10)); } else  p=0;}
  if(dc) y = (FT_DISPLAYHEIGHT/2) -(amp-y);  else
  y = (FT_DISPLAYHEIGHT/2) - y;
  FTImpl.Write32(FT_RAM_G+(x/rate)*4,VERTEX2F(x*16,y*16));	
}
/* api to generate the ECG*/
static uint16_t temp_x,temp_p,temp_y,en;
void Heartbeat()
{
 
   x+=rate; if(x>FT_DISPLAYWIDTH){ x  = 0;temp_p = 0;temp_y=0;
				y=FT_DISPLAYHEIGHT/2;
				en=0;temp_x=0;}
   tx = 5*rate;
   tx = ((temp_p+1)*tx) + temp_p*temp_x;
   if(tx<=x){ if(0==en)	  en = 1;}
  if(en==1){	
  if(y!=beats[temp_y])
  {
    y += beats_Incr[temp_y] * 5;
    if(y==(FT_DISPLAYHEIGHT/2)+beats_Incr[4] * 5)  
		FTImpl.PlaySound(255,(108<<8 | 0x10));
  }
  else
  {
    temp_y++;
    if(temp_y>9) {	
    temp_y = 0;   temp_p++;
    en = 0;  temp_x = x - tx;}
  }}  
  FTImpl.Write32(FT_RAM_G+(x/rate)*4,VERTEX2F(x*16,y*16));	
 
}
//* waves application*//
void Signals()
{
  uint16_t i,xx=0;
  uint16_t tval,th_cts=0,amp;
  uint8_t  temp[FT_DISPLAYHEIGHT],hide_x=0,tag,opt=3;
// ========  Bg =========================================================
  for(tval=0;tval<=FT_DISPLAYHEIGHT/2;tval++)	
  {
    temp[FT_DISPLAYHEIGHT-tval] = temp[tval] = (tval*0.90);		
  }
  FTImpl.Write(2048L,temp,sizeof(temp));
  
  
  y = FT_DISPLAYHEIGHT/2;
  for(tval=0;tval<10;tval++)
  {
    y = y+(beats_Incr[tval]*5);
    beats[tval] = y;
  }
  
  for(tval=0;tval<FT_DISPLAYWIDTH;tval+=rate)
  {
    FTImpl.Write32(FT_RAM_G+((tval/rate)*4),VERTEX2F(tval*16,y*16));			
  }
  
  FTImpl.Cmd_DLStart(); 
  FTImpl.BitmapSource(2048L);	
  FTImpl.BitmapLayout(FT_L8,1,FT_DISPLAYHEIGHT);	
  FTImpl.BitmapSize(FT_NEAREST, FT_REPEAT, FT_BORDER, FT_DISPLAYWIDTH, FT_DISPLAYHEIGHT);	
  FTImpl.Cmd_Swap();
  FTImpl.Finish();
      
  while(1)
  {
 // ========  Menu  =========================================================
    if(FTImpl.IsPendown()) 
    {
      hide_x = 0;
      th_cts = 0;
    }   
    else 
    { 
      th_cts++;
      if(th_cts > 250)
      {
       if( hide_x < 85) 
       hide_x++;   
       else
       th_cts = 0;     
     }
    }
//==========Option =========================================================    
    tag = Read_Keys();
    if(tag!=0)
    {
      x = 0;   temp_p = 0;en = 0; temp_x = 0; temp_y = 0; //reset
      if(tag>2)  opt = tag;
      if(tag==1)if(rate>1)rate--;	
      if(tag==2)if(rate<6)rate++;			
      y = (FT_DISPLAYHEIGHT/2);
      for(tval=0;tval<FT_DISPLAYWIDTH;tval+=rate)
      {
	FTImpl.Write32(FT_RAM_G+((tval/rate)*4),VERTEX2F(tval*16,y*16));			
      }
    }    
//========= Signals ========================================================
  amp = 100;
  switch(opt)
  {
    case 5:

      Triangle_wave(amp);
    break;
    
    case 4:
        Sawtooth_wave(amp);
    break;
    
    case 3:
          Sine_wave(amp);
    break;
    
    case 6:
      amp = 50;
      Heartbeat();					
    break;
 }
//=========Display list start===================================================    
	FTImpl.StartTransferCmd();
    FTImpl.Cmd_DLStart(); 
    FTImpl.Clear(1,1,1);
    FTImpl.ColorRGB(0x12,0x4A,0x26);    
    FTImpl.TagMask(1);
    FTImpl.LineWidth(2*16);
    FTImpl.Begin(FT_BITMAPS);
    FTImpl.Tag(0);
    FTImpl.Vertex2f(0,0);
    FTImpl.ColorRGB(0x1B,0xE0,0x67);	
    FTImpl.Begin(FT_LINE_STRIP);
// Append first few datas of GRAM data to Displaylist    
    FTImpl.Cmd_Append(FT_RAM_G,(x/rate)*4);
    FTImpl.End();
    FTImpl.Begin(FT_LINE_STRIP);
// Eliminate some bytes of GRAM data to the display list         
    if((x/rate)<(FT_DISPLAYWIDTH/rate)-(50/rate))          
    FTImpl.Cmd_Append(FT_RAM_G+(x/rate)*4+((50/rate)*4),((FT_DISPLAYWIDTH/rate)*4)-((x/rate)*4)-((50/rate)*4));
    FTImpl.PointSize(6*16);
    FTImpl.Begin(FT_POINTS);
    FTImpl.Vertex2f(x*16,y*16);	
    FTImpl.ColorRGB(0xff,0xff,0xff);
    FTImpl.ColorA(100);	
// menu    
    FTImpl.Begin(FT_EDGE_STRIP_R);
    FTImpl.Vertex2f((hide_x+FT_DISPLAYWIDTH-80)*16,0);
    FTImpl.Vertex2f((hide_x+FT_DISPLAYWIDTH-80)*16,FT_DISPLAYHEIGHT*16);
    FTImpl.ColorA(50);
    FTImpl.PointSize(15*16);
    FTImpl.Begin(FT_POINTS);
    if(keypressed==1)
    FTImpl.ColorA(150);
    FTImpl.Tag(1);
    FTImpl.Vertex2f((hide_x+FT_DISPLAYWIDTH-60)*16,20*16);		
    FTImpl.ColorA(50);
    if(keypressed==2)
    FTImpl.ColorA(150);
    FTImpl.Tag(2);
    FTImpl.Vertex2f((hide_x+FT_DISPLAYWIDTH-20)*16,20*16);	
 
    FTImpl.ColorA(255);
    Ft_Gpu_Radiobutton(hide_x+FT_DISPLAYWIDTH-70,FT_DISPLAYHEIGHT-48,0xffffff,0,8,3,opt);		
    Ft_Gpu_Radiobutton(hide_x+FT_DISPLAYWIDTH-70,FT_DISPLAYHEIGHT-28,0xffffff,0,8,4,opt);
    Ft_Gpu_Radiobutton(hide_x+FT_DISPLAYWIDTH-70,FT_DISPLAYHEIGHT-8 ,0xffffff,0,8,5,opt);
    Ft_Gpu_Radiobutton(hide_x+FT_DISPLAYWIDTH-70,FT_DISPLAYHEIGHT-68,0xffffff,0,8,6,opt);
    FTImpl.TagMask(0);    
    FTImpl.Cmd_Text(hide_x+FT_DISPLAYWIDTH-60,FT_DISPLAYHEIGHT-48,26,FT_OPT_CENTERY,"Sine");
    FTImpl.Cmd_Text(hide_x+FT_DISPLAYWIDTH-60,FT_DISPLAYHEIGHT-28,26,FT_OPT_CENTERY,"Sawtooth");
    FTImpl.Cmd_Text(hide_x+FT_DISPLAYWIDTH-60,FT_DISPLAYHEIGHT-8 ,26,FT_OPT_CENTERY,"Triangle");
    FTImpl.Cmd_Text(hide_x+FT_DISPLAYWIDTH-60,FT_DISPLAYHEIGHT-68,26,FT_OPT_CENTERY,"ECG");
    FTImpl.Cmd_Text((hide_x+FT_DISPLAYWIDTH-60),20,30,FT_OPT_CENTERY|FT_OPT_CENTERX,"-");
    FTImpl.Cmd_Text((hide_x+FT_DISPLAYWIDTH-20),20,30,FT_OPT_CENTERY|FT_OPT_CENTERX,"+");
    FTImpl.Cmd_Text((hide_x+FT_DISPLAYWIDTH-80),50,28,0,"Rate:");
    FTImpl.Cmd_Number((hide_x+FT_DISPLAYWIDTH-30),50,28,0,rate);		
    FTImpl.Cmd_Text((hide_x+FT_DISPLAYWIDTH-80),80,28,0,"Pk:");
    FTImpl.Cmd_Number((hide_x+FT_DISPLAYWIDTH-40),80,28,0,amp);	
	
    FTImpl.Display();
    FTImpl.Cmd_Swap();
	FTImpl.Finish();
	FTImpl.EndTransferCmd();
	
//==========================End==================================================    
  }
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
		//Serial.print("Error in chip id read ");
		//Serial.println(chipid,HEX);
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
		Signals();   
		FTImpl.Exit();
	}
	Serial.println("--End Application--");
}

void loop()
{
}





/* Nothing beyond this */







