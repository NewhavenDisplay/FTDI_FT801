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
* @file                           FT_App_Logo.ino
* @brief                          Sample application to demonstrat FT801 primitives, widgets and customized screen shots
* @version                        0.1.0
* @date                           2014/03/21
*
*/


#include "SPI.h"
#include "Wire.h"
#include "FT_NHD_43CTP_SHIELD.h"



#define MEMCMD(a) FTImpl.WriteCmdfromflash((prog_uchar*)(a), sizeof(a))

#define SQ(v) (v*v)
#define AUDIO_ADDRESS 102400L



/* SD card object*/
FT_SD FtSd(FT_SD_CSPIN);
/* sd_present holds error values during initialization.  0 means no error and all other errors are non zero value */
FT_SDStatus sd_present; 

/* Global object for transport */
FT801IMPL_SPI FTImpl(FT_CS_PIN,FT_PDN_PIN,FT_INT_PIN);


static uint8_t home_star_icon[] = {0x78,0x9C,0xE5,0x94,0xBF,0x4E,0xC2,0x40,0x1C,0xC7,0x7F,0x2D,0x04,0x8B,0x20,0x45,0x76,0x14,0x67,0xA3,0xF1,0x0D,0x64,0x75,0xD2,0xD5,0x09,0x27,0x17,0x13,0xE1,0x0D,0xE4,0x0D,0x78,0x04,0x98,0x5D,0x30,0x26,0x0E,0x4A,0xA2,0x3E,0x82,0x0E,0x8E,0x82,0xC1,0x38,0x62,0x51,0x0C,0x0A,0x42,0x7F,0xDE,0xB5,0x77,0xB4,0x77,0x17,0x28,0x21,0x26,0x46,0xFD,0x26,0xCD,0xE5,0xD3,0x7C,0xFB,0xBB,0xFB,0xFD,0xB9,0x02,0xCC,0xA4,0xE8,0x99,0x80,0x61,0xC4,0x8A,0x9F,0xCB,0x6F,0x31,0x3B,0xE3,0x61,0x7A,0x98,0x84,0x7C,0x37,0xF6,0xFC,0xC8,0xDD,0x45,0x00,0xDD,0xBA,0xC4,0x77,0xE6,0xEE,0x40,0xEC,0x0E,0xE6,0x91,0xF1,0xD2,0x00,0x42,0x34,0x5E,0xCE,0xE5,0x08,0x16,0xA0,0x84,0x68,0x67,0xB4,0x86,0xC3,0xD5,0x26,0x2C,0x20,0x51,0x17,0xA2,0xB8,0x03,0xB0,0xFE,0x49,0xDD,0x54,0x15,0xD8,0xEE,0x73,0x37,0x95,0x9D,0xD4,0x1A,0xB7,0xA5,0x26,0xC4,0x91,0xA9,0x0B,0x06,0xEE,0x72,0xB7,0xFB,0xC5,0x16,0x80,0xE9,0xF1,0x07,0x8D,0x3F,0x15,0x5F,0x1C,0x0B,0xFC,0x0A,0x90,0xF0,0xF3,0x09,0xA9,0x90,0xC4,0xC6,0x37,0xB0,0x93,0xBF,0xE1,0x71,0xDB,0xA9,0xD7,0x41,0xAD,0x46,0xEA,0x19,0xA9,0xD5,0xCE,0x93,0xB3,0x35,0x73,0x0A,0x69,0x59,0x91,0xC3,0x0F,0x22,0x1B,0x1D,0x91,0x13,0x3D,0x91,0x73,0x43,0xF1,0x6C,0x55,0xDA,0x3A,0x4F,0xBA,0x25,0xCE,0x4F,0x04,0xF1,0xC5,0xCF,0x71,0xDA,0x3C,0xD7,0xB9,0xB2,0x48,0xB4,0x89,0x38,0x20,0x4B,0x2A,0x95,0x0C,0xD5,0xEF,0x5B,0xAD,0x96,0x45,0x8A,0x41,0x96,0x7A,0x1F,0x60,0x0D,0x7D,0x22,0x75,0x82,0x2B,0x0F,0xFB,0xCE,0x51,0x3D,0x2E,0x3A,0x21,0xF3,0x1C,0xD9,0x38,0x86,0x2C,0xC6,0x05,0xB6,0x7B,0x9A,0x8F,0x0F,0x97,0x1B,0x72,0x6F,0x1C,0xEB,0xAE,0xFF,0xDA,0x97,0x0D,0xBA,0x43,0x32,0xCA,0x66,0x34,0x3D,0x54,0xCB,0x24,0x9B,0x43,0xF2,0x70,0x3E,0x42,0xBB,0xA0,0x95,0x11,0x37,0x46,0xE1,0x4F,0x49,0xC5,0x1B,0xFC,0x3C,0x3A,0x3E,0xD1,0x65,0x0E,0x6F,0x58,0xF8,0x9E,0x5B,0xDB,0x55,0xB6,0x41,0x34,0xCB,0xBE,0xDB,0x87,0x5F,0xA9,0xD1,0x85,0x6B,0xB3,0x17,0x9C,0x61,0x0C,0x9B,0xA2,0x5D,0x61,0x10,0xED,0x2A,0x9B,0xA2,0x5D,0x61,0x10,0xED,0x2A,0x9B,0xA2,0x5D,0x61,0x10,0xED,0x2A,0x9B,0xED,0xC9,0xFC,0xDF,0x14,0x54,0x8F,0x80,0x7A,0x06,0xF5,0x23,0xA0,0x9F,0x41,0xF3,0x10,0x30,0x4F,0x41,0xF3,0x18,0x30,0xCF,0xCA,0xFC,0xFF,0x35,0xC9,0x79,0xC9,0x89,0xFA,0x33,0xD7,0x1D,0xF6,0x5E,0x84,0x5C,0x56,0x6E,0xA7,0xDA,0x1E,0xF9,0xFA,0xAB,0xF5,0x97,0xFF,0x2F,0xED,0x89,0x7E,0x29,0x9E,0xB4,0x9F,0x74,0x1E,0x69,0xDA,0xA4,0x9F,0x81,0x94,0xEF,0x4F,0xF6,0xF9,0x0B,0xF4,0x65,0x51,0x08};


typedef struct
{
  uint8_t sd_present:1;
}ft_bitfield;

ft_bitfield ft_flags;

/* Logo Setups*/

typedef struct logoim
{
  char name[7];
  uint16_t image_height;
  uint8_t image_format:5; 
  uint8_t filter:1;
  uint16_t sizex;
  uint16_t sizey;
  uint16_t linestride:10;
  uint8_t gram_address;
}t_imageprp;

t_imageprp sym_prp[6] = {
                          {"S1.BIN",136,FT_RGB565,FT_BILINEAR, 512, 512,480,0},      // handle 0
              	          {"S2.BIN", 77,    FT_L8,FT_BILINEAR,2*77,2*77, 77,65},      // handle 1
                          {"S3.BIN", 28,    FT_L8, FT_NEAREST,3*50,3*28, 50,77},      // handle 2
                          {"S4.BIN", 50,    FT_L8, FT_NEAREST,  50,  50, 50,80},
                          {"S5.BIN", 77,    FT_L8,FT_BILINEAR,2*77,2*77, 77,83},
                          {"S6.BIN", 77,    FT_L8, FT_NEAREST,  84,  77, 84,95}
			},
          target_prp[4] = {  
                            {"T1.BIN", 68,    FT_L8,FT_BILINEAR, 136,136,68,64},      // handle 0
                  	    {"T2.BIN", 55,    FT_L4,FT_BILINEAR, 120, 55,60,70},      // handle 1
                            {"T3.BIN", 60,    FT_L4,FT_BILINEAR, 240,180,60,78},      // handle 2
                            {"T4.BIN", 150,   FT_L8,FT_BILINEAR, 480,300,240,82},
                          },
          hbo_prp[7]= {
                          {"H1.BIN",136,FT_RGB565,FT_BILINEAR,512,512,480,95},
                          {"H2.BIN",120,    FT_L8,FT_BILINEAR,512,512,120,32},
                          {"H3.BIN",3,    FT_L8, FT_NEAREST,360,3,360,55},
                          {"H4.BIN",136,   FT_L8, FT_NEAREST,512,512,30,57},
                          {"H5.BIN",72,   FT_L8, FT_NEAREST,512,512,72,65},
                          {"H6.BIN",64,   FT_L8, FT_NEAREST,512,512,60,71},
                          {"H7.BIN",63,   FT_L8, FT_NEAREST,512,512,258,79}, 
                        },
          fuse_prp[2] = { {"F1.BIN",128,    FT_L8, FT_NEAREST,512,512, 128,1},
                          {"F2.BIN", 94,    FT_L4, FT_NEAREST, 88, 94, 44,20}
                         },
          music_prp[3]= {
                          {"M1.BIN", 57,FT_RGB565,FT_BILINEAR,110,114,110,0},
                          {"M2.BIN",127,    FT_L8, FT_NEAREST,180,127,180,30},
                          {"M3.BIN",66,    FT_L8, FT_NEAREST,149,66,149,100},
                        },
         menu_prp[3]=   {
                          {"ML.BIN", 120,FT_L8,FT_BILINEAR,120,240,60,180},
                          {"MG.BIN", 272,FT_L8,FT_NEAREST,480,272,1,177},
                          {"ME.BIN", 136,FT_L8,FT_BILINEAR,480,272,240,217},
                        }; 
                        
PROGMEM prog_uint16_t  sx[20] = {0x017d,0x0011,0x00f8,0x001b,231,0x01a1,0x01ad,0x00ee,0x0004,0x011f,0x003b,0x000e,0x00dc,0x016d,0x0152,0x01da,0x00f7,0x0091,0x0045,0x0162};
PROGMEM prog_uchar  sy[20] = {0x3d,0x1d,0x18,0x7f,101,0x60,0x18,0x1e,0x5f,0x9f,0x43,0x77,0x8a,0x6d,0x60,0x25,0x38,0x17,0x5b,0x94};
PROGMEM prog_uchar  ss[20] = {0x04,0x05,0x06,0x07,0x05,0x05,0x08,0x08,0x0A,0x05,0x07,0x05,0x06,0x05,0x04,0x05,0x07,0x07,0x05,0x04};

PROGMEM prog_uint16_t hyp[48] = {72,144,216,102,161,228,161,204,260,228,260,305,72,144,216,72,144,216,102,161,228,161,204,260,228,260,305,102,161,228,161,204,260,228,260,305,72,144,216,102,161,228,161,204,260,228,260,305};
const PROGMEM float t_can[48] = {1.0,1.0,1.0,0.707388,0.891115,0.951106,0.454487,0.707388,0.829206,0.309623,0.559604,0.707388,0.000796,0.000796,0.000796,-0.999999,-0.999999,-0.999999,-0.706262,-0.453068,-0.308108,-0.890391,-0.706262,-0.558283,-0.950613,-0.828314,-0.706262,-0.708513,-0.891837,-0.951596,-0.455905,-0.708513,-0.830095,-0.311137,-0.560923,-0.575278,-0.002389,-0.002389,-0.002389,0.705133,0.451647,0.306592,0.889665,0.705133,0.556961,0.950117,0.827421,-0.708513};
const PROGMEM float t_san[48] = {0.0,0.0,0.0,0.706825,0.453778,0.308866,0.890753,0.706825,0.558943,0.950859,0.828760,0.706825,1.000000,1.000000,1.000000,0.001593,0.001593,0.001593,0.707951,0.891476,0.951351,0.455196,0.707951,0.829651,0.310380,0.560263,0.707951,-0.705698,-0.452358,-0.307350,-0.890028,-0.705698,-0.557622,-0.950365,-0.827868,-0.817958,-0.999997,-0.999997,-0.999997,-0.709075,-0.892196,-0.951841,-0.456614,-0.709075,-0.830539,-0.311894,-0.561582,-0.705698};


/*************************************************************************/
float linear(float p1,float p2,uint16_t t,uint16_t rate)
{
	float st  = (float)t/rate;
	return p1+(st*(p2-p1));
}

int16_t smoothstep(int16_t p1,int16_t p2,uint16_t t,uint16_t rate)
{
	float dst  = (float)t/rate;
	float st = SQ(dst)*(3-2*dst);
	return p1+(st*(p2-p1));
}

int16_t acceleration(int16_t p1,int16_t p2,uint16_t t,uint16_t rate)
{
	float dst  = (float)t/rate;
	float st = SQ(dst);
	return p1+(st*(p2-p1));
}

float deceleration(int16_t p1,int16_t p2,uint16_t t,uint16_t rate)
{
	float st,dst  = (float)t/rate;
	dst = 1-dst;
	st = 1-SQ(dst);
	return p1+(st*(p2-p1));
}

    

void Use_Rgb()
{
	FTImpl.ColorA(255);
	FTImpl.ColorMask(1,1,1,1);	
}
void Use_Alpha()
{
	FTImpl.ColorMask(0,0,0,1);
	FTImpl.ColorA(255);
}

void Clear_Alpha()
{
	FTImpl.ColorMask(0,0,0,1);
	FTImpl.ColorA(0);
}

    
void load_inflate_image(uint8_t address,char* name)
{
    uint8_t ibuff[512];
    FT_SDFile imfile;
    if(FtSd.OpenFile(imfile,name)){
		//Serial.print("Error Loading: ");
		//Serial.println(name);
	}
    int32_t fsize = imfile.Size; 
    FTImpl.WriteCmd(CMD_INFLATE);
     FTImpl.WriteCmd(address*1024L);
    while(fsize > 0)
    {
      uint16_t n = fsize > 512L ? 512L :fsize;
      imfile.ReadSector(ibuff);     
	  FTImpl.WriteCmd(ibuff,n);				/* copy data continuously into command memory */
      fsize -= n;
    }
}

void Load_file2gram(uint32_t add,uint8_t sectors,FT_SDFile &r)
{
  uint8_t pbuff[512];
  for(uint8_t z=0;z<sectors;z++)
  {
    r.ReadSector(pbuff);
    FTImpl.Write(add,pbuff,512L);
    add+=512;
  }  
}



void Play_Sound(uint8_t sound,uint8_t volume)
{
  FTImpl.Write(REG_VOL_SOUND,volume);
  FTImpl.Write(REG_SOUND,sound);
  FTImpl.Write(REG_PLAY,1);
}	

uint8_t Menu_prp = 0;
void Logo_Intial_setup(struct logoim sptr[],uint8_t num)
{
   for(uint8_t z=0;z<num;z++)
   {
     load_inflate_image(sptr[z].gram_address,sptr[z].name);
   }     
   
   FTImpl.DLStart();        // start
   FTImpl.Clear(1,1,1);
   for(uint8_t z=0;z<num;z++)
   {
     FTImpl.BitmapHandle(z);					// bg             
     FTImpl.BitmapSource(sptr[z].gram_address*1024L);      
     FTImpl.BitmapLayout(sptr[z].image_format,sptr[z].linestride,sptr[z].image_height);      
     if(Menu_prp==1 && z==1)          
     FTImpl.BitmapSize(sptr[z].filter,FT_REPEAT,FT_BORDER,sptr[z].sizex,sptr[z].sizey); 
     else
     FTImpl.BitmapSize(sptr[z].filter,FT_BORDER, FT_BORDER, sptr[z].sizex,sptr[z].sizey); 
   }
   FTImpl.DLEnd();
   FTImpl.Finish();    
}




void end_frame()
{
  static PROGMEM prog_uint32_t std2[] = {
    DISPLAY(),
    CMD_SWAP
  };  MEMCMD(std2); 
}

uint8_t Logo_Example5()
{
    uint16_t zt=0,zt_s=0,zt1=0,zt_s1=0;
    uint16_t ft_iteration_cts = 0;
    int16_t ft_xoffset=0,ft_yoffset=0; 
    int16_t x,y,r;
    uint16_t zin;
    float inout = 0.01;
    uint8_t hcell=2,f=25;
    ft_xoffset = 0; 
    ft_yoffset = 0; 
    ft_iteration_cts = 0;
    zt = 0; zt1= 0;
    Logo_Intial_setup(music_prp,3);
    do
    {
      if(FTImpl.IsPendown())
      return 1;
     
      
      x = smoothstep(480,90,zt,200);
      y = smoothstep(0,124,zt,200);
		
      if(inout<1.2)
      inout = linear(0.1,1.2,zt,200);		
      zin = (int16_t)(256/inout);	
      r = smoothstep(50,8,zt,200);
      ft_yoffset = y;
      ft_xoffset = x-(r*sin(2*3.14*f*y));
      
      static PROGMEM prog_uint32_t std[] = {
      CMD_DLSTART,
      CLEAR_COLOR_RGB(255,255,255),
      CLEAR(1,1,1),
      COLOR_RGB(255,255,255),
 //----------------------------------use alpha-----------------------------------------        
      COLOR_MASK(0,0,0,1),      
      COLOR_A(255),
 //------------------------------------------------------------------------------------        
      BEGIN(FT_POINTS),
      SAVE_CONTEXT(),
      CMD_LOADIDENTITY,
    };
    MEMCMD(std);
    FTImpl.PointSize((uint16_t)(25*inout)*16);
    FTImpl.Vertex2f((ft_xoffset)*16,(ft_yoffset)*16);
    Use_Rgb();
    FTImpl.BlendFunc(FT_DST_ALPHA,FT_ONE_MINUS_DST_ALPHA);
    FTImpl.Begin(FT_BITMAPS);
    FTImpl.BitmapTransformA(zin);
    FTImpl.BitmapTransformE(zin);
    ft_yoffset -=(25*inout);
    ft_xoffset -=(27*inout); 
    FTImpl.Vertex2ii(ft_xoffset,ft_yoffset,0,1);
    FTImpl.BlendFunc(FT_SRC_ALPHA,FT_ONE_MINUS_SRC_ALPHA);
    Clear_Alpha();
    FTImpl.BlendFunc(FT_DST_ALPHA,FT_ONE_MINUS_DST_ALPHA);
    FTImpl.Begin(FT_POINTS);
    ft_yoffset +=(26*inout);
    ft_xoffset +=(27*inout);
    FTImpl.Vertex2f((ft_xoffset)*16,(ft_yoffset)*16);
    FTImpl.BlendFunc(FT_SRC_ALPHA,FT_ONE_MINUS_SRC_ALPHA);
    Use_Alpha();
    x = smoothstep(480,100,zt,200);
    y = smoothstep(0,130,zt,200);
    r = smoothstep(150,75,zt,200);
    ft_xoffset = x+(r*sin(2*3.14*f*y));   
    if(r<=75)
    {
      ft_xoffset = smoothstep(ft_xoffset,120,zt1,100);
    }
    ft_yoffset = y;	
    FTImpl.Vertex2f((ft_xoffset)*16,(ft_yoffset)*16);
    FTImpl.End();
    FTImpl.BlendFunc(FT_DST_ALPHA,FT_ONE_MINUS_DST_ALPHA);
    Use_Rgb();
    ft_yoffset -=(25*inout);
    ft_xoffset -=(27*inout);
    FTImpl.Begin(FT_BITMAPS);
    FTImpl.Vertex2ii(ft_xoffset,ft_yoffset,0,2);
    	FTImpl.BlendFunc(FT_SRC_ALPHA,FT_ONE_MINUS_SRC_ALPHA);
    Clear_Alpha();
    FTImpl.BlendFunc(FT_DST_ALPHA,FT_ONE_MINUS_DST_ALPHA);
    FTImpl.Begin(FT_POINTS);
    ft_yoffset +=(26*inout);
    ft_xoffset +=(27*inout);
    FTImpl.Vertex2f((ft_xoffset)*16,(ft_yoffset)*16);
    	FTImpl.BlendFunc(FT_SRC_ALPHA,FT_ONE_MINUS_SRC_ALPHA);
    Use_Alpha();
    r = smoothstep(250,75,zt,200);
    ft_yoffset = y+(r*sin(2*3.14*f*x));	
    if(r<=75)
    {
      ft_yoffset = smoothstep(ft_yoffset,90,zt1,100);
      if(zt1<100)zt1+=5;
    }
    ft_xoffset = x;			
    FTImpl.Vertex2f((ft_xoffset)*16,(ft_yoffset)*16);
    FTImpl.End();
    FTImpl.BlendFunc(FT_DST_ALPHA,FT_ONE_MINUS_DST_ALPHA);
    Use_Rgb();
    ft_yoffset -=(26*inout);
    ft_xoffset -=(27*inout);
    FTImpl.Begin(FT_BITMAPS);
    FTImpl.Vertex2ii(ft_xoffset,ft_yoffset,0,0);
	FTImpl.BlendFunc(FT_SRC_ALPHA,FT_ONE_MINUS_SRC_ALPHA);
    Clear_Alpha();
    FTImpl.BlendFunc(FT_DST_ALPHA,FT_ONE_MINUS_DST_ALPHA);
    ft_yoffset +=(26*inout);
    ft_xoffset +=(27*inout);
    FTImpl.Begin(FT_POINTS);
    FTImpl.Vertex2f((ft_xoffset)*16,(ft_yoffset)*16);
    FTImpl.End();
    	FTImpl.BlendFunc(FT_SRC_ALPHA,FT_ONE_MINUS_SRC_ALPHA);
    if(r<=75)	
    {
      Use_Rgb();
      FTImpl.BitmapTransformA(256);
      FTImpl.BitmapTransformE(256);
      FTImpl.Begin(FT_BITMAPS);
      FTImpl.BitmapHandle(1);
      ft_yoffset = smoothstep(-127,43,zt1,100);
      if(zt1<=100)
      {
	FTImpl.Cell(hcell);
	ft_xoffset = 15;
      }
      else
      {	
	ft_xoffset = 11;
	if(hcell>0)hcell--;
	FTImpl.Cell(hcell);
	FTImpl.ColorRGB(255,255,255);			
	FTImpl.Vertex2f(ft_xoffset*16,ft_yoffset*16);
	FTImpl.Vertex2f(ft_xoffset*16,ft_yoffset*16);
      }
      FTImpl.ColorRGB(0,0,0);			
      FTImpl.Vertex2f(ft_xoffset*16,ft_yoffset*16);			
      inout = linear(0.1,1,zt_s1,100);		
      zin = (int16_t)(256/inout);	
      FTImpl.BitmapTransformA(zin);
      FTImpl.BitmapTransformE(zin);
      r = smoothstep(100,8,zt_s1,100);
      x = smoothstep(512,200,zt_s1,100);
      y = smoothstep(-50,43,zt_s1,100);
      ft_xoffset = x;
      ft_yoffset = y-(r*sin(94.2*x));
      FTImpl.Vertex2ii(ft_xoffset,ft_yoffset,2,0);			
      ft_yoffset = 145-ft_yoffset;
      FTImpl.Vertex2ii(ft_xoffset,ft_yoffset,2,1);	
      if(zt_s1<100) zt_s1+=2;		
      if(zt1<100)zt1+=2;
    }
    if(zt<200)zt+=2;
    end_frame();
    FTImpl.Finish();  	
    ft_iteration_cts++;
  }  while(ft_iteration_cts<300);
  return 0;
}

uint8_t Logo_Example4()
{
  uint16_t zt=0,zt_s=0,zt1=0,zt_s1=0;
  uint16_t ft_iteration_cts = 0;
 int16_t ft_xoffset=0,ft_yoffset=0; 
 uint8_t max_dots = 15;
 float zinout = 4;
 uint8_t exp_p = 0;
 uint16_t sts_rad=0;  

 uint8_t   as[20];
 uint8_t alpha = 255,rad=30; 
 Logo_Intial_setup(sym_prp,6);
 ft_iteration_cts = 0;
 ft_xoffset = 0;
 ft_yoffset = 0;
 zt = zt1 = 0;
 exp_p = 0;
 zt_s = zt_s1 = 0;
 do
 {
    if(FTImpl.IsPendown())     return 1;
     
    static PROGMEM prog_uint32_t std[] = {
     CMD_DLSTART,
     CLEAR(1,1,1),
     BEGIN(FT_BITMAPS),
     SAVE_CONTEXT(),
     CMD_LOADIDENTITY,
   };MEMCMD(std);
    FTImpl.Cmd_Scale(zinout*65536L,zinout*65536L);
    FTImpl.Cmd_SetMatrix();
    
    FTImpl.Vertex2ii(0,0,0,0);	 
    FTImpl.RestoreContext();
    if(!exp_p)
    {
      zinout = linear(rad,2,zt,200);
      zinout = (rad<=1)?linear(2,1,zt_s,200):(zinout);
      FTImpl.BitmapTransformA((uint16_t)(256*zinout));
      FTImpl.BitmapTransformE((uint16_t)(256*zinout));  
      FTImpl.ColorA(alpha);
      FTImpl.Vertex2ii(80,60,1,0);
      FTImpl.ColorA(255-alpha);
      FTImpl.Vertex2ii(80,60,1,1);      
    }    
    else
    {
    //    ft_xoffset = acceleration(80,190,zt1,100);
	FTImpl.Vertex2ii(190,60,5,0);			
	ft_xoffset = acceleration(80,108,zt1,100);
	FTImpl.Vertex2ii(ft_xoffset,60,4,0);
	ft_xoffset = acceleration(80,52,zt1,100);
	FTImpl.Vertex2ii(ft_xoffset,60,4,1);
        FTImpl.BitmapHandle(2);
	for(uint8_t r=1;r<3;r++)
	{
	  int16_t radius = r*10;
	  if(r==1) zinout = deceleration(1,10,zt_s1,300);		
	  if(r==2) zinout = deceleration(1,.3,zt_s1,300);
	  FTImpl.BitmapTransformA((uint16_t)(256*zinout));
	  FTImpl.BitmapTransformE((uint16_t)(256*zinout));
	  for(uint8_t r1=1;r1<10;r1++)
	  {
            uint16_t angle = r1*(r*30);
	    zinout = (radius+sts_rad)*cos(angle*0.01744);  //3.14/180=0.01744
	    ft_xoffset = 90+zinout;
	    zinout = (radius+sts_rad)*sin(angle*0.01744);
	    ft_yoffset = 90+zinout;
	    FTImpl.Vertex2f(ft_xoffset*16,ft_yoffset*16);
	  }			
	}
  	if(sts_rad<512)sts_rad+=4;	//if(rad<=0) rad = 480;
	if(zt_s1<300) zt_s1+=1;
	if(zt1<100) zt1+=5;
      }
    FTImpl.ColorA(alpha);	
    FTImpl.Begin(FT_RECTS);
    FTImpl.LineWidth(1*16);	
    FTImpl.RestoreContext();
    FTImpl.Begin(FT_BITMAPS);
    for(uint8_t z=0;z<20;z++)
    {
      if(random(20)==z)
      FTImpl.ColorA(0);	
      else
      FTImpl.ColorA(255);	
      FTImpl.BitmapTransformA(256*pgm_read_byte(ss+z));
      FTImpl.BitmapTransformE(256*pgm_read_byte(ss+z));
      FTImpl.Vertex2ii(pgm_read_word(sx+z),pgm_read_byte(sy+z),3,0);
    }    
    end_frame();
    FTImpl.Finish();

    rad = linear(30,1,zt,200);
    alpha = linear(255,0,zt,200);
    zinout = linear(4,2,zt,200);  
    if(zt_s<200)zt_s +=2; else exp_p = 1;  
    if(zt<200){zt+=2; zt_s=0;}     
    ft_iteration_cts++; 
 }while(ft_iteration_cts<300);  
 return 0;
}
uint8_t  Logo_Example3()
{
  uint16_t ft_iteration_cts = 0;
  int16_t ft_xoffset=0,ft_yoffset=0,temp_xoffset,temp_yoffset,temp1_xoffset,temp2_xoffset; 
  uint8_t alpha=255;
  uint8_t rate=100,dst=0,src=210;
  uint16_t resize = 1;
  int16_t zt=0,zt1=0,r=30,cts=0,zt2=0,zt3=0,zt4=0,zt5=0;
  float zinout = .5,r2=0.01,r3=0;
  Logo_Intial_setup(hbo_prp,7);
  do
  { 
     if(FTImpl.IsPendown())
      return 1;
     
    static PROGMEM prog_uint32_t std[] =
    {
      CMD_DLSTART,
      CLEAR(1,1,1),
      SAVE_CONTEXT(),
      BEGIN(FT_BITMAPS),
      BITMAP_HANDLE(0),
      COLOR_A(180),
    };MEMCMD(std);
    zinout = linear(5,0.5,zt,rate);	      // 
    ft_xoffset = 210;
    ft_yoffset = 104;
    temp2_xoffset = 0; 
    temp1_xoffset =  temp_xoffset = ft_xoffset;
    temp_yoffset = ft_yoffset;
    FTImpl.Cmd_LoadIdentity();
    if(zinout<=1.3 && ft_iteration_cts>150)
    {
      ft_xoffset = acceleration(480,269,zt2,50);
      if(zt2<50)zt2+=2;   
      if(ft_xoffset<=269)
      {
        ft_xoffset = smoothstep(269,155,zt3,100);
        if(zt3<100)zt3+=2; 
        if(zt4<50)zt4+=2; 
      }
      temp_xoffset = ft_xoffset;
      temp_yoffset = ft_yoffset;  
      ft_xoffset = acceleration(210,0,zt4,50);//  ball
      temp1_xoffset = ft_xoffset;
      if(ft_xoffset<=0)
      {
        ft_xoffset = deceleration(0,90,zt5,100); // ball
        temp2_xoffset = ft_xoffset;
        if(zt5<100) zt5+=2;
      }
  
    }
    ft_xoffset =min(210,ft_xoffset);
    FTImpl.Cmd_Translate(-1*ft_xoffset*65536,0);
    FTImpl.Cmd_Scale(3*65536,3*65536);
    FTImpl.Cmd_SetMatrix();
    FTImpl.Vertex2f(0,0);
    FTImpl.RestoreContext();
    alpha = linear(255,0,zt,100);
    FTImpl.ColorRGB(255,255,255);
    FTImpl.ColorA(alpha);
    ft_xoffset = 60; //(FT_DISPLAYWIDTH - 360)/2;
    ft_yoffset = 134+2;//(FT_DISPLAYHEIGHT- 4)/2;
    FTImpl.Vertex2ii(ft_xoffset,ft_yoffset,2,0);  // Center line
    r2 = linear(0.01,1,zt,rate);	
    ft_xoffset = smoothstep(239,400,zt,rate);	
    ft_yoffset = (FT_DISPLAYHEIGHT- 136*r2)/2;
    FTImpl.BitmapTransformA((uint16_t)(256/r2));  
    FTImpl.BitmapTransformE((uint16_t)(256/r2));
    FTImpl.Vertex2ii(ft_xoffset,ft_yoffset,3,0);   // side curvature
    ft_xoffset = smoothstep(239,60,zt,rate);
    FTImpl.Vertex2ii(ft_xoffset,ft_yoffset,3,1);   // side curvature
    alpha = linear(255,0,zt1,300);		
    FTImpl.ColorA(alpha);
    ft_xoffset = 168;//(FT_DISPLAYWIDTH - 72*2)/2;
    ft_yoffset = 64;//(FT_DISPLAYHEIGHT- 72*2)/2;
    FTImpl.BitmapTransformA((uint16_t)(128));    // center  lighting
    FTImpl.BitmapTransformE((uint16_t)(128));
    FTImpl.Vertex2ii(ft_xoffset,ft_yoffset,4,0); 	
    if(zinout>0.5)
    FTImpl.ColorA(255);
    FTImpl.BitmapTransformA((uint16_t)(256/zinout));
    FTImpl.BitmapTransformE((uint16_t)(256/zinout));
    ft_xoffset = (uint16_t)(FT_DISPLAYWIDTH - 120*zinout)/2;
    ft_yoffset = (uint16_t)(FT_DISPLAYHEIGHT- 120*zinout)/2;
    FTImpl.BitmapHandle(1);
    FTImpl.Vertex2f(ft_xoffset*16,ft_yoffset*16); 
    FTImpl.RestoreContext(); 	
    if(zinout<=1.3)
    {      
      ft_xoffset = temp_xoffset;
      ft_yoffset = temp_yoffset;
      if( ft_iteration_cts>150)
      FTImpl.Vertex2ii(ft_xoffset,ft_yoffset+3,6,0);      // ftdi string
      ft_xoffset = temp1_xoffset;//acceleration(210,0,zt4,50);	
      if(ft_xoffset<=0)
      {
        ft_xoffset = temp2_xoffset;//deceleration(0,90,zt5,100);
        if(ft_xoffset>=48)
	FTImpl.Vertex2ii(48,ft_yoffset,5,0);			// tail
      }  
      FTImpl.Vertex2ii(ft_xoffset,ft_yoffset,5,1);	// ftdiball*/
    }
    end_frame();
    FTImpl.Finish();  
    if(zt<rate) zt+=2; 
    if(zt1<300) zt1+=2; 
    ft_iteration_cts++;
  }while(ft_iteration_cts<300);
  return 0;
}

uint8_t Logo_Example2()
{
  uint16_t zt=0,zt_s=0,zt1=0,zt_s1=0;
  uint16_t ft_iteration_cts = 0;
  int16_t ft_xoffset=0,ft_yoffset=0,h,adj=0; 
  uint8_t ch=0,handle=0,rangle=30,pausects=0;
  uint8_t zoom_iteration_cts = 160,noofzinout=0,rate=8,tempitcts=0;  
  uint8_t  img_width = 68;
  uint8_t  img_height = 68;
  float z_dst=2,z_src=1;
  ft_iteration_cts = 0;
  Logo_Intial_setup(target_prp,4);
  do
  {
     if(FTImpl.IsPendown())
      return 1;
     
  //----------------------------------------------------zoominout properties-------------------------------------   
    if(zt1>=zoom_iteration_cts && handle==0)
    {
      if(noofzinout<3)
      {
        noofzinout+=1;
        zt1 = 0;
      }else
      {
        handle=3;
        tempitcts = ft_iteration_cts+20;
        zt1 = 0;   

      }
      if(noofzinout==1)        
      {  
        zoom_iteration_cts = 200;
        z_src = 2;z_dst = 1; rate=10;
      }
      if(noofzinout==2)
      {  
        zoom_iteration_cts = 160;
        z_src = 1;z_dst = 2; rate=8;
      }
      if(noofzinout==3)
      {  
        zoom_iteration_cts = 200;
        z_src = 2;z_dst = 0.1; rate=10;
      }
    }
  //------------------------------------------------------------------------------------------------------------   
    float zinout = linear(z_src,z_dst,zt1,zoom_iteration_cts);  
    ft_xoffset = (FT_DISPLAYWIDTH  - zinout*68)/2;
    ft_yoffset = (FT_DISPLAYHEIGHT - zinout*68)/2; 
    
    static PROGMEM prog_uint32_t std[] = {
    CMD_DLSTART,
    CLEAR_COLOR_RGB(255,255,255),
    CLEAR(1,1,1),
    BEGIN(FT_BITMAPS),
    };MEMCMD(std); 
    FTImpl.ColorRGB(197,0,15);
    FTImpl.SaveContext();
    if(handle==3 && tempitcts<ft_iteration_cts && rangle>0)
    {    
     zinout = linear(0.1,1,zt1,200);	
     ft_xoffset = (FT_DISPLAYWIDTH  - zinout*120)/2;
     ft_yoffset = (FT_DISPLAYHEIGHT - zinout*60)/2;
     FTImpl.Cmd_LoadIdentity();
     FTImpl.Cmd_Scale(zinout*65536,zinout*65536);
     FTImpl.Cmd_Translate(60*65536,30*65536);
     FTImpl.Cmd_Rotate(-rangle*65536/360);
     FTImpl.Cmd_Translate(-60*65536,-30*65536);
     FTImpl.Cmd_SetMatrix();      
     FTImpl.ColorRGB(0,0,0);
     FTImpl.Vertex2ii(ft_xoffset+70-rangle,ft_yoffset+20,2,0);
     zinout = linear(0.1,.5,zt1,200);	
     ft_xoffset = (FT_DISPLAYWIDTH  - zinout*240)/2;
     ft_yoffset = (FT_DISPLAYHEIGHT - zinout*150)/2;
     FTImpl.Cmd_LoadIdentity();
     FTImpl.Cmd_Scale(zinout*65536,zinout*65536);
     FTImpl.Cmd_Translate(240*65536,150*65536);
     FTImpl.Cmd_Rotate(-rangle*65536/360);
     FTImpl.Cmd_Translate(-240*65536,-150*65536);
     FTImpl.Cmd_SetMatrix();			
     FTImpl.ColorRGB(197,0,15);
     FTImpl.Vertex2ii(ft_xoffset-100,ft_yoffset,3,0);
     if(rangle>0) rangle--; 
     if(zt1<zoom_iteration_cts){ zt1+=rate; } 	
    }	
    if(rangle<=0)
    {
       zinout = linear(0.49,0.8,zt_s,100);	
       ft_xoffset = (FT_DISPLAYWIDTH  - zinout*240)/2;
       ft_yoffset = (FT_DISPLAYHEIGHT - zinout*150)/2;
       FTImpl.Cmd_LoadIdentity();
       FTImpl.Cmd_Scale(zinout*65536,zinout*65536);
       FTImpl.Cmd_SetMatrix();			
       FTImpl.ColorRGB(197,0,15);
       FTImpl.Vertex2ii(ft_xoffset-96,ft_yoffset,3,0);
       FTImpl.RestoreContext();	
       ft_xoffset = 180;
       ft_yoffset = 110;
       FTImpl.ColorRGB(0,0,0);
       FTImpl.Vertex2ii(ft_xoffset+70,ft_yoffset+20,2,0);
       ft_xoffset = smoothstep(480,250,zt_s,100);
       ft_yoffset = 80;
       if(pausects<50 || pausects>=80)
	{
	  ft_xoffset = smoothstep(480,250,zt_s,100);
	  ft_yoffset = 80;
	}
	else if(pausects>=50 && pausects<80)
	{		
	  ft_xoffset = 240+random(10);
	  ft_yoffset = 70+random(10);; 
	}
	FTImpl.Vertex2ii(ft_xoffset,ft_yoffset,1,0);
	pausects++;
        if(zt_s<100)zt_s+=2;
    } 
    if(handle==0)
    {
      FTImpl.Cmd_LoadIdentity();
      FTImpl.Cmd_Scale(zinout*65536,zinout*65536);
      FTImpl.Cmd_SetMatrix();
      FTImpl.Vertex2ii(ft_xoffset,ft_yoffset,handle,0); 	
      FTImpl.RestoreContext();	
      if(ft_iteration_cts>=100)      // Wait for 100 itreations
      {
        adj = smoothstep(0,220,zt,200);		
        if(zt<200) zt+=4;	
        if(zt1<zoom_iteration_cts){ zt1+=rate; } 	
      }
      ft_xoffset = 206;  //(width-img_width)/2
      ft_yoffset = 102;  //(height-img_height)/2
      for(uint8_t z=0;z<48;z++)
      {
        h = pgm_read_word(hyp+z)+adj;
        int16_t temp_theta = h*pgm_read_float(t_can+z);
  	int16_t xoff = ft_xoffset+temp_theta;	
        temp_theta = h*pgm_read_float(t_san+z);
  	int16_t yoff = ft_yoffset+temp_theta;	
  	FTImpl.Vertex2f(xoff*16,yoff*16);
      }		
    }    
    end_frame();
    FTImpl.Finish();  
    ft_iteration_cts++;  
  }while(ft_iteration_cts<500);
  return 0;
}


uint8_t Logo_Example1()
{
  uint16_t zt=0,zt_s=0,zt1=0,zt_s1=0;
  uint16_t ft_iteration_cts = 0;
  int16_t ft_xoffset=0,ft_yoffset=0; 
  uint8_t bargraph=0;
  int32_t ftsize=0,wp=0,adsize,rp,val;
  int16_t bh = 0,xoff=280;
  float v1 = 133,v2 = 310,yoff=133,bw=20;
 
  Logo_Intial_setup(fuse_prp,2);
  FT_SDFile barfile,adfile;
  FtSd.OpenFile(barfile,"EFUSE.BAR");
  ftsize = barfile.Size;
  
  FtSd.OpenFile(adfile,"EFUSE.ULW");
  adsize = adfile.Size;
  
  FTImpl.Write32(REG_PLAYBACK_FREQ,44100);
  FTImpl.Write32(REG_PLAYBACK_START,AUDIO_ADDRESS);
  FTImpl.Write32(REG_PLAYBACK_FORMAT,FT_ULAW_SAMPLES);
  FTImpl.Write32(REG_PLAYBACK_LENGTH,8192L);
  FTImpl.Write32(REG_PLAYBACK_LOOP,1);
  FTImpl.Write(REG_VOL_PB,255);
  Load_file2gram(AUDIO_ADDRESS+0UL,2,adfile); 
  wp = 1024;   
  adsize-=1024;
  
  FTImpl.Write(REG_PLAYBACK_PLAY,1);  
  Load_file2gram(FT_RAM_G,1,barfile);
  ftsize-=512L;
  ft_iteration_cts = 0;
  ft_xoffset = 0;
  ft_yoffset = 0;
  zt = zt1 = 0;
  while(adsize > 0)
  { 
    if(FTImpl.IsPendown())
    {
        FTImpl.Write(REG_VOL_PB,0);
    FTImpl.Write(REG_PLAYBACK_PLAY,0);
      return 1;
    }  
      
     static PROGMEM prog_uint32_t std[] = {
     CMD_DLSTART,
     CLEAR_COLOR_RGB(143,10,10),
     CLEAR(1,1,1),
     CLEAR_COLOR_RGB(255,255,255),
   };MEMCMD(std);
    
   FTImpl.ScissorXY(ft_xoffset,ft_yoffset); 
   ft_xoffset = 630-(2*ft_xoffset); 
   ft_yoffset = 272-(2*ft_yoffset);
   FTImpl.ScissorSize(ft_xoffset,ft_yoffset);    
   FTImpl.Clear(1,1,1);
   FTImpl.ColorRGB(143,10,10);
   if(ft_iteration_cts>130)
    { 
      ft_yoffset = deceleration(0,60,zt,100);
      if(zt<100)zt+=5; 
    }else ft_yoffset = 0;    
    if(ft_yoffset>=60 && ft_iteration_cts>=220)
    {
      ft_xoffset = deceleration(0,260,zt1,100);
      bh = deceleration(0,158,zt1,100);
      if(zt1<100)zt1+=4; 
    }else ft_xoffset = 0;  
    
    if(ft_xoffset<260)
    {
       static PROGMEM prog_uint32_t std1[] = {
        SAVE_CONTEXT(),
        BEGIN(FT_BITMAPS),
        CMD_LOADIDENTITY,
        CMD_SCALE,262140,262140,
        CMD_SETMATRIX,
        VERTEX2II(0,0,0,0),
        RESTORE_CONTEXT(),
      };MEMCMD(std1);
    }
    
    int16_t temp_x = (int16_t)xoff-v2;
    int16_t temp_y = (int16_t)yoff-v1;
    FTImpl.Begin(FT_RECTS);
    FTImpl.Vertex2f(0*16,temp_y*16);   
    temp_y = (int16_t)yoff+v1+6;
    FTImpl.Vertex2f(FT_DISPLAYWIDTH*16,temp_y*16);         
    FTImpl.Vertex2f((int16_t)(xoff-v2)*16,bh*16);   
    temp_x = temp_x+bw;
    temp_y = 355 - bh;
    FTImpl.Vertex2f(temp_x*16,temp_y*16); 

    if(ft_xoffset>=260)
    {
      FTImpl.Begin(FT_BITMAPS);
      FTImpl.Vertex2ii(ft_xoffset+10,ft_yoffset+5,1,0);  
      bargraph = 0;
    }     
    if(bargraph)
    {
      static PROGMEM prog_uint32_t std2[] = {
        BITMAP_HANDLE(2),
        BITMAP_SOURCE(FT_RAM_G),
        BITMAP_LAYOUT(FT_BARGRAPH,256,1),				
        BEGIN(FT_BITMAPS),
      };MEMCMD(std2);
      
      FTImpl.BitmapSize(FT_BILINEAR, FT_BORDER,FT_BORDER,256,(uint16_t)yoff);	    
      int16_t vx = (int16_t)(temp_x+bw);
      int16_t vy = 0; 
            FTImpl.Vertex2ii(vx,vy,2,0);  
      FTImpl.SaveContext();
      vy = yoff/2;
      FTImpl.Cmd_LoadIdentity();
      FTImpl.Cmd_Translate((vx)*65536L, 65536L*(vy));
      FTImpl.Cmd_Scale(1*65536, 65536*-1);
      FTImpl.Cmd_Translate(-(vx)*65536L, 65536L*-(vy));
      FTImpl.Cmd_SetMatrix();       
      vy = yoff+5;
      FTImpl.Vertex2ii(vx,vy,2,0);                
      FTImpl.RestoreContext();		       
      if(vx<256)          // clip second half
      {
        vx = (int16_t)(256+temp_x+bw);
        vy = 0;
        FTImpl.Vertex2ii(vx,0,2,1);             
        vy = yoff/2;
        FTImpl.Cmd_LoadIdentity();
        FTImpl.Cmd_Translate((vx)*65536L, 65536L*(vy));
        FTImpl.Cmd_Scale(1*65536, 65536*-1);
        FTImpl.Cmd_Translate(-(vx)*65536L, 65536L*-(vy));
        FTImpl.Cmd_SetMatrix(); 
        vy = yoff+5;
        FTImpl.Vertex2ii(vx,vy,2,1);      
        FTImpl.RestoreContext();
       }  
       Load_file2gram(FT_RAM_G,1,barfile);
       ftsize-=512L;	
    }
   
    end_frame();
    FTImpl.Finish();  

   
    rp = FTImpl.Read16(REG_PLAYBACK_READPTR);
    val = 8191L & (rp-wp);				
    if (val > 1024) 
    {
        uint16_t n = min(1024,adsize);  
        Load_file2gram(AUDIO_ADDRESS+wp,2,adfile);  
        wp = (wp +1024) & 8191L;
        adsize-=n;
    }
   
    if(v1>1) v1 = v1-v1/10;     
    else if(v2>1)
    {                   
      v2 = v2-v2/10;   
      bw = bw-bw/100;  
      yoff+=.8;        // move the horizontal gap to bottom
      bargraph = 1;   
    }
    ft_iteration_cts++;
  }
  FTImpl.Write(REG_VOL_PB,0);
  FTImpl.Write(REG_PLAYBACK_PLAY,0);
  return 0;
}
static uint8_t keypressed=0;
uint8_t Read_Keys()
{
  static uint8_t Read_tag=0,temp_tag=0;	
  uint8_t ret_tag=0;
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

char *info[] = {  "FT801 Logos Application",
          	  "APP to demonstrate interactive Menu,",
                  "and logo animation,",	 
		  "using Bitmaps,&Track",	 
                };
                       
void Info()
{
	uint32_t status;
  uint16_t dloffset = 0,z;

  static PROGMEM prog_uint32_t std1[] = {
      CMD_DLSTART,
       CLEAR(1,1,1),
      COLOR_RGB(255,255,255),
  };MEMCMD(std1);
  FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,FT_DISPLAYHEIGHT/2,26,FT_OPT_CENTER,"Please tap on a dot");
  static PROGMEM prog_uint32_t std2[] = {
       CMD_CALIBRATE,0,
     DISPLAY(),
     CMD_SWAP,
  };MEMCMD(std2);
  FTImpl.Finish();
  
  FTImpl.Cmd_Logo();
  do{
  status=FTImpl.CheckLogo();
  }while(status!=FT_GE_FINISHED); 
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
    static PROGMEM prog_uint32_t std3[] = {
    BITMAP_TRANSFORM_A(256),
    BITMAP_TRANSFORM_B(0),
    BITMAP_TRANSFORM_C(0),
    BITMAP_TRANSFORM_D(0),
    BITMAP_TRANSFORM_E(256),
    BITMAP_TRANSFORM_F(0),
    SAVE_CONTEXT(),
    COLOR_RGB(219,180,150),
    COLOR_A(220),
    BEGIN(FT_EDGE_STRIP_A),
    };MEMCMD(std3);
    FTImpl.Vertex2f(0,FT_DISPLAYHEIGHT*16);
    FTImpl.Vertex2f(FT_DISPLAYWIDTH*16,FT_DISPLAYHEIGHT*16);
    FTImpl.ColorA(255);
    FTImpl.RestoreContext();	
    FTImpl.ColorRGB(0,0,0);
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,20,28,FT_OPT_CENTER,info[0]);
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,60,26,FT_OPT_CENTER,info[1]);
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,90,26,FT_OPT_CENTER,info[2]);  
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,120,26,FT_OPT_CENTER,info[3]);  
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,FT_DISPLAYHEIGHT-30,26,FT_OPT_CENTER,"Click to play");
    if(keypressed!='P')
    FTImpl.ColorRGB(255,255,255);
    else
    FTImpl.ColorRGB(100,100,100);
	
    static PROGMEM prog_uint32_t std4[] = {
    BEGIN(FT_POINTS),
    POINT_SIZE(20*16),
    TAG('P'),
    VERTEX2F(3840,3392),
    CLEAR(0,1,0),
    COLOR_RGB(180,35,35),
    
    STENCIL_OP(FT_INCR,FT_INCR),
    COLOR_MASK(0,0,0,0),//mask all the colors
    BEGIN(FT_EDGE_STRIP_L),

    VERTEX2II(((480/2)-14+10),(272-77+5),0,0),
    VERTEX2II(((480/2)-2+15),(272-67+8),0,0),
    VERTEX2II(((480/2)-14+10),(272-57+11),0,0),
    VERTEX2II(((480/2)-14+10),(272-77+5),0,0),

   COLOR_MASK(1,1,1,1),//enable all the colors
    STENCIL_FUNC(FT_EQUAL,1,255),
   BEGIN(FT_EDGE_STRIP_L),
    VERTEX2II(480,0,0,0),
    VERTEX2II(480,272,0,0),
	
    STENCIL_OP(FT_KEEP,FT_KEEP),
    STENCIL_FUNC(FT_ALWAYS,0,255),
    LINE_WIDTH(1*16),
	
    BEGIN(FT_LINE_STRIP),
    VERTEX2II(((480/2)-14+10),(272-77+5),0,0),
    VERTEX2II(((480/2)-2+15),(272-67+8),0,0),
    VERTEX2II(((480/2)-14+10),(272-57+11),0,0),
    VERTEX2II(((480/2)-14+10),(272-77+5),0,0),
    
    DISPLAY(),
    CMD_SWAP,
    };MEMCMD(std4);  
    FTImpl.Finish();
  }while(Read_Keys()!='P');
  
}

static int32_t trackval = 0;
uint8_t Selection_Menu()
{
  uint8_t Menu = 0, temp_Menu = 0, rotation_flag = 0, played = 0, key_detected;
  uint16_t rotation_cts = 0, scroller_vel;
  int16_t prevth = 0, currth = 0, key_deteted_cts;
  sTrackTag sTracktag;
  Menu_prp = 1;
  Logo_Intial_setup(menu_prp, 3);
  FTImpl.Cmd_Track(240, 136, 1, 1, 1);
  FTImpl.Cmd_Track(240, 136, 1, 1, 2);
  FTImpl.Cmd_Track(240, 136, 1, 1, 3);
  FTImpl.Cmd_Track(240, 136, 1, 1, 4);
  FTImpl.Cmd_Track(240, 136, 1, 1, 5);
  FTImpl.Cmd_Track(240, 136, 1, 1, 6);
  FTImpl.Finish();
  do
  {
    FTImpl.GetTrackTag(sTracktag);

    key_detected = sTracktag.tag;
    if (key_detected > 0)
    {
      rotation_cts = 0;
      key_deteted_cts++;
      if (key_detected < 6)
        temp_Menu = key_detected;
      else
        temp_Menu = 0;
      Menu = 0;
    }
    if (key_detected == 0)
    {
      if (key_deteted_cts > 0 && key_deteted_cts < 15)
      {
        if (!rotation_flag)
          Menu = temp_Menu;
        else
          rotation_flag = 0;
      }
      key_deteted_cts = 0;
      if (rotation_cts > 100)
      {
        trackval++;
        rotation_flag = 1;
      }
      else
        rotation_cts++;
    }
    if (key_deteted_cts > 15)
    {
      currth = sTracktag.track / 182;
      if (prevth != 0)
        trackval += (currth - prevth);
      prevth = currth;
      rotation_cts = 0;
      rotation_flag = 0;
    }
    else
    {
      prevth = 0;
    }

    static PROGMEM prog_uint32_t std[] =
        {
            CMD_DLSTART,
            CLEAR_COLOR_RGB(12, 61, 90),
            CLEAR(1, 1, 1),
            BEGIN(FT_BITMAPS),
            SAVE_CONTEXT(),
            BITMAP_TRANSFORM_A(128),
            BITMAP_TRANSFORM_E(128),
            VERTEX2II(0, 0, 2, 0),
            RESTORE_CONTEXT(),
            COLOR_RGB(0, 0, 0),
            TAG_MASK(1),
            TAG(6),
            VERTEX2II(0, 0, 1, 0),
            TAG_MASK(0),
            COLOR_RGB(255, 255, 255),
            POINT_SIZE(40 * 16),
        };
    MEMCMD(std);
    for (uint8_t x = 1; x < 6; x++)
    {
      int16_t theta = (72 * x);
      theta += (-trackval);
      theta %= 360;
      int16_t xoff = 200 + (int16_t)(170 * cos(theta * 0.0174));
      int16_t yoff = 80 - (int16_t)(80 * sin(theta * 0.0174));
      uint16_t zinout = 350 - yoff;
      xoff = xoff + ((100 - (100 * yoff * 0.0115))) / 2;
      FTImpl.BitmapTransformA(zinout);
      FTImpl.BitmapTransformE(zinout);
      if (x == key_detected)
        FTImpl.ColorRGB(150, 150, 150);
      else
        FTImpl.ColorRGB(255, 255, 255);
      FTImpl.Begin(FT_BITMAPS);
      FTImpl.Vertex2ii(xoff, yoff, 0, x - 1);
      FTImpl.Begin(FT_POINTS);
      FTImpl.ColorA(0);
      FTImpl.TagMask(1);
      FTImpl.Tag(x);                                       // 100% transparent bmp
      FTImpl.Vertex2f((xoff + 25) * 16, (yoff + 25) * 16); // 100% transparent bmp
      FTImpl.ColorA(255);
      FTImpl.TagMask(0);
    }
    end_frame();

    FTImpl.Finish();
  } while (Menu == 0);
  return Menu;
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

/* Display an on-screen message if the storage device is not found */
void CheckStorageDevicePrecence(){
	while(sd_present){
		sd_present = FtSd.Init();
		FTImpl.DLStart();     
		FTImpl.Clear(1,1,1);
		FTImpl.ColorRGB(255,255,255);
		FTImpl.Cmd_Text(FT_DISPLAYWIDTH>>1,FT_DISPLAYHEIGHT>>1,29,FT_OPT_CENTER,"STORAGE DEVICE NOT FOUND");
		FTImpl.DLEnd();
		FTImpl.Finish();
		delay(5000);
		}
}



//***********************************Logo*********************************//
void setup()
{

/*Initialize the SD object. Screen error message can only be displayed when the FT801 is ready*/
	sd_present = FtSd.Init();
	
	/* Initialize serial print related functionality */
	//Serial.begin(9600);
	
	/* Set the Display Enable pin*/   
	//Serial.println("--Start Application--");
	if(BootupConfigure())
	{
		//error case - do not do any thing
	}
  	else
	{
	Info();
  trackval = 0;
  while(1)
  {
    uint8_t exit = 0;
    uint8_t Menu = Selection_Menu(); 
    if(Menu!=0 && Menu<=5)
    {
      Play_Sound(0x52,255);
      do
      {
        Menu_prp = 0;
        switch(Menu)
        {
          case 1: 
          exit = Logo_Example2();    // target
          break;
          
          case 2: 
          exit = Logo_Example4();    //SYMAN  
          break;
          
          case 3: 
          exit = Logo_Example3();    //HBO
          break;
       
          case 4: 
          exit = Logo_Example1();    // FUSE
          break;
          
          case 5  :    
          exit  = Logo_Example5();  // MUSIC
          break;
        }  
      }while(exit==0);  
    }
  }
  FTImpl.Exit();
  }
  //Serial.println("--End Application--");

}

void loop(){
}


/* Nothing beyond this */






















