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
* @file                           FT_App_Refrigerator.ino
* @brief                          Sample application to demonstrate FT801 primitives, widgets and customized screen shots. 
* @version                        0.1.0
* @date                           2014/03/21
*
*/



#include "SPI.h"
#include "Wire.h"
#include "FT_NHD_43CTP_SHIELD.h"





int32_t Ft_App_LoadRawFromFile(FT_SDFile &r,int32_t DstAddr);
void Play_Sound(uint8_t sound,uint8_t vol,uint8_t midi);
void FT_App_Refrigerator();
void animation();


/* SD card object*/
FT_SD FtSd(FT_SD_CSPIN);
/* sd_present holds error values during initialization.  0 means no error and all other errors are non zero value */
FT_SDStatus sd_present; 

/*SD file object for file accessing*/
FT_SDFile g_reader;

/* Global object for transport */
FT801IMPL_SPI FTImpl(FT_CS_PIN,FT_PDN_PIN,FT_INT_PIN);



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

typedef struct S_RefrigeratorAppSnowLinear
{
	int16_t xOffset;
	int16_t yOffset;
	int8_t dx;
	int8_t dy;
}S_RefrigeratorAppSnowLinear_t;



/* cos funtion */
int16_t qcos(uint16_t a)
{
  return (qsin(a + 16384));
}


static void polarxy(int32_t r, float th, int16_t *x, int16_t *y,uint16_t ox,uint16_t oy)
{
  th = (th * 32768L / 180);
  *x = (16 * ox + (((long)r * qsin(th)) >> 11) + 16);
  *y = (16 * oy - (((long)r * qcos(th)) >> 11));
}


/* API to load raw data from file into perticular locatio of FT801 */
int32_t Ft_App_LoadRawFromFile(FT_SDFile &r,int32_t DstAddr)
{
	int32_t FileLen = 0,FT801_addr = FT_RAM_G;
        uint8_t bytes_size,sector_size;
	//uint8_t *pbuff = NULL;
        uint8_t pbuff[512];
       
 	FT801_addr = DstAddr;
        FileLen = r.Size;
      
 	while(FileLen > 0)
	{
                /* download the data into the command buffer by 512bytes one shot */
		uint16_t blocklen = FileLen>512?512:FileLen;
                r.ReadSector(pbuff);
		FileLen -= blocklen;
		/* copy data continuously into FT801 memory */
		FTImpl.Write(FT801_addr,pbuff,blocklen);
		FT801_addr += blocklen;
	}
	return 0;
}


/********API to return the assigned TAG value when penup,for the primitives/widgets******/

uint8_t keypressed=0;
uint8_t temp_tag_keys=0;
uint8_t tracker_tag = 1;
uint8_t Read_Keys()
{
  
  uint8_t  ret_tag_keys=0,Read_tag_keys=0;
  Read_tag_keys = FTImpl.Read(REG_TOUCH_TAG);
  ret_tag_keys = 0;
  if(Read_tag_keys!=0)                  
  {
    if(temp_tag_keys!=Read_tag_keys && Read_tag_keys!=255)
    {
      temp_tag_keys = Read_tag_keys;     
      keypressed = Read_tag_keys;
      if(!tracker_tag)
      Play_Sound(0x51,100,108);
    }  
  }
  else
  {
    if(temp_tag_keys!=0 && !FTImpl.IsPendown())
    {
        ret_tag_keys = temp_tag_keys;
                temp_tag_keys = 0;
    }  
    keypressed = 0;
  }
  return ret_tag_keys;
}

void SAMAPP_CoPro_Widget_Calibrate()
{
	uint8_t *pbuff;
	uint32_t NumBytesGen = 0,TransMatrix[6];
	uint16_t CurrWriteOffset = 0;

	/*************************************************************************/
	/* Below code demonstrates the usage of calibrate function. Calibrate    */
	/* function will wait untill user presses all the three dots. Only way to*/
	/* come out of this api is to reset the coprocessor bit.                 */
	/*************************************************************************/
	{
	
	FTImpl.DLStart();
	FTImpl.ClearColorRGB(64,64,64);
	FTImpl.Clear(1,1,1);
	FTImpl.ColorRGB(0xff,0xff,0xff);
	/* Draw number at 0,0 location */
	//FTImpl.ColorA(30);
	FTImpl.Cmd_Text((FT_DISPLAYWIDTH/2), (FT_DISPLAYHEIGHT/2), 27, FT_OPT_CENTER, "Please Tap on the dot");
	FTImpl.Cmd_Calibrate(0);

	FTImpl.DLEnd();
	FTImpl.Finish();
	}
}

void Play_Sound(uint8_t sound,uint8_t vol,uint8_t midi)
{
  uint16_t val = (midi << 8) | sound;
  FTImpl.Write(REG_SOUND,val);
  FTImpl.Write(REG_PLAY,1);
}

typedef struct Bitfield
{
	uint8_t unlock;
	uint8_t freeze;
	uint8_t brightness;
	uint8_t fridge;
	uint8_t cubed;
	uint8_t settings;
	uint8_t change_background;
	uint8_t font_size;
	uint8_t sketch;
	uint8_t food_tracker;	
}Bitfield;




PROGMEM prog_char font[4] = {26, 27, 28, 29};
int16_t freezer_temp[] = {-2,-4,-6,-8,-10,-12,-14,34,36,38,40,42,44,46};
PROGMEM prog_char bgd_raw_files[9][7] = {"31.raw","32.raw","33.raw","34.raw","35.raw","36.raw"};//,"37.raw","38.raw","39.raw"};
Bitfield bitfield;

void change_background()
{
  char filename[10];
  int32_t xoffset = 10;
  uint8_t Read_tag = 0;
  
  if(FtSd.OpenFile(g_reader,"BG.raw")==0){
  Ft_App_LoadRawFromFile(g_reader,158*1024L);
  }
  else{
	//Serial.println("Unable to open BG.raw");
  }

  FTImpl.DLStart(); 
  FTImpl.BitmapHandle(5);	
  FTImpl.BitmapSource(158*1024L);	
  FTImpl.BitmapLayout(FT_RGB565,100*2,50);	
  FTImpl.BitmapSize(FT_BILINEAR, FT_BORDER, FT_BORDER,  100, 50);	
  FTImpl.End();
  FTImpl.DLEnd();
  FTImpl.Finish();
  do
  {
    Read_tag = Read_Keys();
    if(Read_tag >=30 && Read_tag < 40)
    {
      strcpy_P(filename,&bgd_raw_files[Read_tag - 30][0]);
      if(FtSd.OpenFile(g_reader,filename)==0){
      Ft_App_LoadRawFromFile(g_reader,FT_RAM_G);
	  }
		else{
		//Serial.print("Unable to open ");
		//Serial.println(filename);
		}		
		
    }
    		
    FTImpl.DLStart(); 
    FTImpl.Clear(1,1,1);
    FTImpl.Begin(FT_BITMAPS);
    FTImpl.BitmapTransformA(128);
    FTImpl.BitmapTransformE(128);
    FTImpl.Tag(0);
    FTImpl.BitmapHandle(5);	
    FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, 480, 272);
    FTImpl.Vertex2ii(0, 0, 1, 0);
    FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, 100, 50);
    FTImpl.BitmapTransformA(256);
    FTImpl.BitmapTransformE(256);
    FTImpl.Tag(8);
    FTImpl.Vertex2ii(450, 10, 3, 3);
    FTImpl.ColorRGB(255, 255, 255);
    
	  
    xoffset = 10; 
    int yoff = 30;
    for(int x = 0;x < 6; x++)
    {
      if(x == 4 )
      {
        yoff += 80; xoffset = 10;
       }
    FTImpl.Tag(30 + x);
    FTImpl.Vertex2ii(xoffset , yoff, 5, x);
    xoffset += 110;
    }
                
    FTImpl.DLEnd();
    FTImpl.Finish();				
    }while(Read_tag!=8);
	bitfield.change_background = 0;
}


void Sketch(int16_t val_font)
{

  uint8_t tag =0;
  int32_t xoffset1 = 10;
  
  FTImpl.DLStart();
  FTImpl.Cmd_Sketch(0,50,FT_DISPLAYWIDTH-60,FT_DISPLAYHEIGHT-70,110*1024L,FT_L8); 
  FTImpl.Cmd_Memzero(110*1024L,140*1024L);  
  FTImpl.BitmapSource(110*1024L);
  FTImpl.BitmapLayout(FT_L8,(FT_DISPLAYWIDTH-60),FT_DISPLAYHEIGHT-50);
  FTImpl.BitmapSize(FT_NEAREST,FT_BORDER,FT_BORDER,(FT_DISPLAYWIDTH-60),(FT_DISPLAYHEIGHT-50));
  FTImpl.Cmd_Swap();
  FTImpl.Finish();				
  
  while(1)
  {
    tag = Read_Keys();
    if(tag==2)
    {
      FTImpl.DLStart();  
      FTImpl.Cmd_Memzero(110*1024L,140*1024L); // Clear the gram frm 1024 		
      FTImpl.Finish();	
    }
    if(tag == 8)
    {
      bitfield.sketch = 0;
      FTImpl.WriteCmd(  CMD_STOP);//to stop the sketch command
      return;
    }
    
    FTImpl.DLStart();                  // Start the display list
    FTImpl.Clear(1,1,1);	
    
     	
    FTImpl.Begin(FT_BITMAPS);
    FTImpl.SaveContext();
    FTImpl.BitmapTransformA(128);
    FTImpl.BitmapTransformE(128);	
    FTImpl.Vertex2ii(0, 0, 1, 0);
    FTImpl.RestoreContext();
    FTImpl.Cmd_Text(200,10,pgm_read_byte(&font[val_font]),FT_OPT_CENTERX,"Add a description on the Notes");
    FTImpl.TagMask(1);
    FTImpl.Cmd_FGColor(0x060252);
    FTImpl.Tag(2);          // assign the tag value 
    FTImpl.Cmd_Button((FT_DISPLAYWIDTH-55),(FT_DISPLAYHEIGHT-45),45,25,pgm_read_byte(&font[val_font]),0,"CLR");
   
    FTImpl.Tag(8);
    FTImpl.Vertex2ii(450, 10, 3, 3);
    FTImpl.TagMask(0);
    FTImpl.LineWidth(1*16);
    FTImpl.Begin(FT_RECTS);
    FTImpl.Vertex2f(0,50*16);
    FTImpl.Vertex2f((int16_t)(FT_DISPLAYWIDTH-60)*16,(int16_t)(FT_DISPLAYHEIGHT-20)*16);
    
    FTImpl.ColorRGB(8, 8, 62);
    FTImpl.Begin(FT_BITMAPS);
    FTImpl.Vertex2ii(0,50,0,0);
    animation();
  
    FTImpl.End();
    FTImpl.DLEnd();
    FTImpl.Finish();	
  }
}

PROGMEM prog_char raw_files[10][10] = {"app1.raw","car0.raw","ki3.raw","tom1.raw","bery1.raw","chic1.raw","snap1.raw","prk1.raw","bf1.raw","sas1.raw"};
uint8_t item_selection = 40;

void food_tracker(int16_t val_font)
{
  int32_t z,Read_tag,j,y,Baddr,m,n;
  int32_t r_x1_rect, r_x2_rect, r_y1_rect, r_y2_rect;
  char filename[10] = "\0";
  FTImpl.DLStart(); 
  FTImpl.ColorRGB(255, 255, 255);
  FTImpl.Clear(1, 1, 1); // clear screen
  FTImpl.BitmapHandle(7);//handle 7 is used for fridge items
  FTImpl.BitmapSource(78*1024L);
  FTImpl.BitmapLayout(FT_ARGB4, 80*2, 80);
  FTImpl.BitmapSize(FT_BILINEAR, FT_BORDER, FT_BORDER, 80, 80);

  FTImpl.DLEnd();
  FTImpl.Finish();
  Baddr = 78*1024L;
  for(y = 0; y < 10; y++)
  {
    strcpy_P(filename,&raw_files[y][0]);
    if(FtSd.OpenFile(g_reader,filename)==0){
    Ft_App_LoadRawFromFile(g_reader,Baddr);
	}
	else{
		//Serial.print("Unable to open ");
		//Serial.println(filename);
	}
    Baddr += 80*80*2;
  }
  while(1)
  {
    Read_tag = Read_Keys();
    if(keypressed!=0 && keypressed>=40)
	item_selection=keypressed;
    if(Read_tag == 8)
    {
        bitfield.food_tracker = 0;
	return;
    }
    
    FTImpl.DLStart();  
    FTImpl.Begin(FT_BITMAPS);
    FTImpl.SaveContext();
    FTImpl.BitmapTransformA(128);
    FTImpl.BitmapTransformE(128);
    FTImpl.ColorRGB(255,255,255);
    FTImpl.Vertex2ii(0, 0, 1, 0);
    FTImpl.RestoreContext();
    FTImpl.Cmd_Text(240,0,pgm_read_byte(&font[val_font]),FT_OPT_CENTERX,"Food Manager");
    FTImpl.Cmd_Text(90,30,pgm_read_byte(&font[val_font]),FT_OPT_CENTERX,"Left Door");
    FTImpl.Cmd_Text(310,30,pgm_read_byte(&font[val_font]),FT_OPT_CENTERX,"Right Door");
    animation();
    FTImpl.Begin(FT_RECTS);
    FTImpl.LineWidth(5*16);
    FTImpl.ColorA(80);
  
    FTImpl.Vertex2f(20*16,60*16);
    FTImpl.Vertex2f(180*16,220*16);
		
    FTImpl.Vertex2f(200*16,60*16);
    FTImpl.Vertex2f(450*16,220*16);

    r_x1_rect = 30; 
    r_y1_rect = 70;
    for(n = 0; n < 4 ; n++)
    {
      if(n==2)
      {
       r_y1_rect += 80;
       r_x1_rect = 30;
      }
      if(item_selection==40+n)
        FTImpl.ColorA(255);			
      else
	FTImpl.ColorA(80);	
	FTImpl.Begin(FT_RECTS);
	FTImpl.Tag(40+n);
	FTImpl.Vertex2f(16*r_x1_rect,16*(r_y1_rect));
	FTImpl.Vertex2f(16*(r_x1_rect+60),16*(r_y1_rect+60));
	FTImpl.Begin(FT_BITMAPS);
	FTImpl.ColorA(255);
	FTImpl.Tag(40+n);
	FTImpl.Vertex2ii(r_x1_rect-10, r_y1_rect-10, 7, n);
	r_x1_rect += 80;
      }
      
      r_x1_rect = 210;
      r_y1_rect = 70;
      for(n = 0; n < 6 ; n++)
      {
	if(n==3)
        {
         r_y1_rect += 80;
         r_x1_rect = 210;
        }
	if(item_selection==44+n)
          FTImpl.ColorA(255);		
	else
	  FTImpl.ColorA(80);		
	  FTImpl.Begin(FT_RECTS);
	  FTImpl.Tag(44+n);
	  FTImpl.Vertex2f(16*r_x1_rect,16*(r_y1_rect));
	  FTImpl.Vertex2f(16*(r_x1_rect+60),16*(r_y1_rect+60));
	  FTImpl.Begin(FT_BITMAPS);
	  FTImpl.ColorA(255);	
	  FTImpl.Tag(44+n);
	  FTImpl.Vertex2ii(r_x1_rect-10, r_y1_rect-10, 7, 4+n);
	  r_x1_rect += 80;
        }

	  FTImpl.Tag(8);
	  FTImpl.Vertex2ii(450, 0, 3, 3);

      FTImpl.DLEnd();
	  FTImpl.Finish();
      }
}


void cubed_setting(uint16_t *cubed,int16_t val_font)
{
   int32_t Read_tag,Baddr;
   static int16_t cubed_funct= 0,img1_flag = 1,img2_flag = 0;
   Baddr = 94*1024L;
   cubed_funct = *cubed;

    FTImpl.DLStart(); 
    if(FtSd.OpenFile(g_reader,"cube1.raw")==0){
    Ft_App_LoadRawFromFile(g_reader,Baddr);
	}
	else{
		//Serial.println("Unable to open cube1.raw");
	}
    FTImpl.BitmapHandle(8);
    FTImpl.BitmapSource(Baddr);
    FTImpl.BitmapLayout(FT_ARGB4, 200*2, 200);
    FTImpl.BitmapSize(FT_BILINEAR, FT_BORDER, FT_BORDER, 200, 200);
    Baddr += 80*1024L;
    if(FtSd.OpenFile(g_reader,"crush1.raw")==0){
    Ft_App_LoadRawFromFile(g_reader,Baddr);
	}
	else{
		//Serial.println("Unable to open crush1.raw");
	}
    FTImpl.BitmapHandle(9);
    FTImpl.BitmapSource(Baddr);
    FTImpl.BitmapLayout(FT_ARGB4, 200*2, 200);
    FTImpl.BitmapSize(FT_BILINEAR, FT_BORDER, FT_BORDER, 200, 200);
    FTImpl.DLEnd();
    FTImpl.Finish();
    do
    {
      Read_tag = Read_Keys();
      FTImpl.DLStart();
      FTImpl.SaveContext();
      FTImpl.Begin(FT_BITMAPS);
      FTImpl.BitmapTransformA(128);
      FTImpl.BitmapTransformE(128);
      FTImpl.ColorRGB(255,255,255);
      FTImpl.Vertex2ii(0, 0, 1, 0);
      FTImpl.RestoreContext();

      FTImpl.Cmd_Text(120,30,pgm_read_byte(&font[val_font]),FT_OPT_CENTER,"Cubed");
      FTImpl.Cmd_Text(350,30,pgm_read_byte(&font[val_font]),FT_OPT_CENTER,"Crushed");
		
      animation();
      FTImpl.ColorA(50);  
      FTImpl.Begin(FT_RECTS);
      
      if(keypressed == 25 || img1_flag == 1)
      {
	FTImpl.Vertex2f(10*16,50*16);
	FTImpl.Vertex2f(210*16,250*16);
	cubed_funct = 0;
	img1_flag = 1;
	img2_flag = 0;
      }
      if(keypressed == 26 || img2_flag == 1)
      {
        FTImpl.Vertex2f(250*16,50*16);
	FTImpl.Vertex2f(450*16,250*16);
	cubed_funct = 1;
	img1_flag = 0;
	img2_flag = 1;
    }  
    FTImpl.RestoreContext();
    FTImpl.Begin(FT_BITMAPS);
    FTImpl.Tag(25);
    FTImpl.ColorA(100);
    FTImpl.Vertex2ii(10, 50, 8, 0);
    FTImpl.Tag(26);
    FTImpl.Vertex2ii(250, 50, 9, 0);
    FTImpl.ColorA(255);
    FTImpl.Tag(8);
    FTImpl.Vertex2ii(450, 5, 3, 3);
    *cubed = cubed_funct;

    FTImpl.DLEnd();
    FTImpl.Finish();
    }while(Read_tag!=8);	
}


PROGMEM prog_uint16_t NumSnowRange = 4, NumSnowEach = 10,RandomVal = 16;
int16_t xoffset,yoffset, j;
S_RefrigeratorAppSnowLinear_t S_RefrigeratorSnowArray[4*10],*pRefrigeratorSnow = NULL;


void animation()
{
  /* Draw background snow bitmaps */
  pRefrigeratorSnow = S_RefrigeratorSnowArray;
  FTImpl.Begin(FT_BITMAPS);
  
  FTImpl.ColorA(64);
  FTImpl.ColorRGB(255,255,255);
  FTImpl.BitmapHandle(6);
  for(j=0;j<(NumSnowRange*NumSnowEach);j++)
  {			
    if( ( (pRefrigeratorSnow->xOffset > ((FT_DISPLAYWIDTH + 60)*16)) || (pRefrigeratorSnow->yOffset > ((FT_DISPLAYHEIGHT + 60) *16)) ) ||
				( (pRefrigeratorSnow->xOffset < (-60*16)) || (pRefrigeratorSnow->yOffset < (-60*16)) ) )
    {
	pRefrigeratorSnow->xOffset = random(FT_DISPLAYWIDTH*16);
	pRefrigeratorSnow->yOffset = FT_DISPLAYHEIGHT*16 + random(80*16);
	pRefrigeratorSnow->dx = random(RandomVal*8) - RandomVal*4;
	pRefrigeratorSnow->dy = -1*random(RandomVal*8);
    }
    
    FTImpl.Vertex2f(pRefrigeratorSnow->xOffset, pRefrigeratorSnow->yOffset);
    pRefrigeratorSnow->xOffset += pRefrigeratorSnow->dx;
    pRefrigeratorSnow->yOffset += pRefrigeratorSnow->dy;
    pRefrigeratorSnow++;
    }	

}

int16_t origin_xy[][4] = {{ 65,0,0,-90},
                             { 0,0,90,-65},
			     { 0,0,65,-90},
			     { 0,65,90,0},
			     { -65,90,0,0},
                             { -90,0,0,65},
                             { 0,0,-65,90},
			      };


void construct_structure(int32_t xoffset, int16_t flag)
{
  int32_t z;
  int16_t x0,y0,x1,y1,x2,x3,y2,y3,next_flag,thcurrval=0;
  uint16_t angle = 0, angle1 = 0, angle2 = 0;
  
  FTImpl.ColorA(0);
  FTImpl.Begin(FT_POINTS);
  FTImpl.PointSize(60*16);
  FTImpl.StencilOp(FT_INCR,FT_INCR);
  FTImpl.Vertex2f(xoffset*16,150*16);
		
  FTImpl.Begin(FT_LINES);
  FTImpl.LineWidth(2 * 16);
  for(z = 45; z < 361; z+=45)
  {
    polarxy(100,z,&x0,&y0,xoffset,150);
    polarxy(60,z,&x1,&y1,xoffset,150);
    FTImpl.Vertex2f(x0,y0);
    FTImpl.Vertex2f(x1,y1);
  }
  
  FTImpl.StencilOp(FT_KEEP,FT_KEEP);
  FTImpl.Begin(FT_POINTS) ;
  FTImpl.ColorRGB(102,180,232);
  FTImpl.PointSize(100*16);
  FTImpl.ColorA(255);
  FTImpl.StencilFunc(FT_EQUAL,0,255);//stencil function to increment all the values
  FTImpl.Vertex2f(xoffset*16,150*16);
  FTImpl.LineWidth(65*16);
  FTImpl.StencilOp(FT_INCR,FT_INCR);
  
  FTImpl.Begin(FT_LINES);
  if(flag >= 0 && flag < 7 )
  {
    next_flag = flag+1;
    FTImpl.ColorA(0);
    angle2 = flag*45;
    polarxy(150,angle2,&x0,&y0,xoffset,150);
    polarxy(150,angle2+180,&x1,&y1,xoffset,150);
    FTImpl.Vertex2f(x0 + origin_xy[flag][0] *16,y0 + origin_xy[flag][2]*16 );
    FTImpl.Vertex2f(x1 + origin_xy[flag][0] *16,y1 + origin_xy[flag][2] *16);
			
    polarxy(150,angle2+45,&x0,&y0,xoffset,150);
    polarxy(150,angle2+45+180,&x1,&y1,xoffset,150);
    FTImpl.Vertex2f(x0 + origin_xy[flag][1] *16,y0 + origin_xy[flag][3]*16 );
    FTImpl.Vertex2f(x1 + origin_xy[flag][1] *16,y1 + origin_xy[flag][3] *16);
    
    FTImpl.StencilOp(FT_KEEP,FT_KEEP);
    FTImpl.Begin(FT_POINTS);
    FTImpl.ColorRGB(19,43,59);
    FTImpl.PointSize(100*16);
    FTImpl.ColorA(255);
    FTImpl.StencilFunc(FT_EQUAL,0,255);
    FTImpl.Vertex2f(xoffset*16,150*16);
   }
    FTImpl.StencilOp(FT_KEEP,FT_KEEP);
    FTImpl.Begin(FT_POINTS);
    FTImpl.ColorRGB(19,43,59);
    FTImpl.PointSize(100*16);
    FTImpl.ColorA(255);
    FTImpl.StencilFunc(FT_EQUAL,0,255);
    FTImpl.Vertex2f(xoffset*16,150*16);			
    FTImpl.RestoreContext();
    FTImpl.ColorA(255);
}





void change_temperature(uint32_t *freeze_temp ,uint32_t *fridge_temp, int16_t val_font)
{
  uint32_t tracker = 0;
  int32_t Read_tag,trackval = 0, y, z = 45 ,i;
  uint16_t track_val = 0;
  int16_t x0,y0,x2,r,temp,thcurrval=0,thcurr = 0;
  static uint16_t angle = 0, angle1 = 0, angle2 = 0,flag = 0,flag_1=0;
  uint8_t freezer_tempr = 0, fridge_tempr = 0;

  thcurr = *freeze_temp;
  thcurrval  = *fridge_temp;
	sTrackTag sTracktag;
  FTImpl.Cmd_Track(120,150,1,1,20);
  FTImpl.Cmd_Track(350,150,1,1,30);

  while(1)
  {
    Read_tag = Read_Keys();
	FTImpl.GetTrackTag(sTracktag);
	track_val = sTracktag.track;
    if(keypressed == 8)
    {
      bitfield.freeze = 0;
      bitfield.fridge = 0;
      return;
    }

    if(FTImpl.IsPendown())
    {
      temp = track_val/182;
      if(temp < 316)
      {
        if(keypressed==30)
	  angle2 = track_val/182;
	else if(keypressed==20)
	  angle1 = track_val/182;

	flag =   angle1/45;
	flag_1 =   angle2/45;
	}
      }

      FTImpl.DLStart();  	
      FTImpl.Clear(1, 1, 1); // clear screen
      FTImpl.Begin(FT_BITMAPS);
      FTImpl.SaveContext();
      FTImpl.BitmapTransformA(128);
      FTImpl.BitmapTransformE(128);
      FTImpl.ColorRGB(255,255,255);
      FTImpl.Vertex2ii(0, 0, 1, 0);
      FTImpl.RestoreContext();
      FTImpl.Cmd_Text(120,30,pgm_read_byte(&font[val_font]),FT_OPT_CENTER,"Freezer Temperature");
      FTImpl.Cmd_Text(350,30,pgm_read_byte(&font[val_font]),FT_OPT_CENTER,"Fridge Temperature");
    
      construct_structure(120,flag);
      FTImpl.RestoreContext();
      FTImpl.Clear(0,1,0);
      construct_structure(350,flag_1);

      for(r = 0;r<2;r++)
      {
	if(r == 0)
          x2 = 120;
	else
          x2 = 350;
	  angle = 135+23; 
          for(z=0;z<7;z++)
	  {
	    angle += 45;
	    angle %= 360;
            polarxy(80,angle,&x0,&y0,x2,150);
	    FTImpl.Cmd_Number(x0/16,y0/16,28,FT_OPT_SIGNED|FT_OPT_CENTER,freezer_temp[(r*7)+z]);      
          }
	}

	FTImpl.ColorA(255);
	FTImpl.ColorRGB(1,1,1);
	thcurr = flag*-2-2;
	*freeze_temp = thcurr;
        polarxy(0,0,&x0,&y0,120,150);
	FTImpl.Cmd_Number(x0/16,y0/16,30,FT_OPT_SIGNED|FT_OPT_CENTER,thcurr);
	FTImpl.Cmd_Text(150,145,17,FT_OPT_CENTER,"x");
	FTImpl.Cmd_Text(160,150,30,FT_OPT_CENTER,"F");
	thcurrval = flag_1*2 + 34;
	*fridge_temp = thcurrval;
        polarxy(0,0,&x0,&y0,350,150);
	FTImpl.Cmd_Number(x0/16,y0/16,30,FT_OPT_CENTER,thcurrval);
	FTImpl.Cmd_Text(370,145,17,FT_OPT_CENTER,"x");
	FTImpl.Cmd_Text(380,150,30,FT_OPT_CENTER,"F");
				
	FTImpl.Vertex2ii(140 , 205, 3,0);
	FTImpl.Vertex2ii(370 , 205, 3,1);
		
        animation();
	FTImpl.PointSize(100*16);
	FTImpl.ColorA(0);
	FTImpl.Begin(FT_POINTS);
	FTImpl.TagMask(1);
	FTImpl.Tag(20);
	FTImpl.Vertex2f(120*16,150*16);
	FTImpl.Tag(30);
	FTImpl.Vertex2f(350*16,150*16);


        FTImpl.PointSize(60*16);
	FTImpl.ColorA(0);
        FTImpl.Begin(FT_POINTS);
	FTImpl.TagMask(1);
	FTImpl.Tag(0);
	FTImpl.Vertex2f(120*16,150*16);
	FTImpl.Tag(0);
	FTImpl.Vertex2f(350*16,150*16);
	FTImpl.TagMask(0);

	FTImpl.RestoreContext();
	FTImpl.ColorRGB(255,255,255);
	FTImpl.Begin(FT_BITMAPS);
	FTImpl.Tag(8);
	FTImpl.Vertex2ii(450, 5, 3, 3);
	
	
	FTImpl.DLEnd();
	FTImpl.Finish();

	}

  }
  
uint16_t sleep_cts=0;
uint32_t val;

void change_brightness(uint32_t val,int16_t val_font)
{
  int32_t Read_tag,x;
  static uint32_t tracker = 0,track_val = 420;
  uint16_t pwmi_val = 0;
  sTrackTag sTracktag;
  val = 128;
  FTImpl.Cmd_Track(20,200,420,40,12);

  while(1)
  {
    Read_tag = Read_Keys();
	FTImpl.GetTrackTag(sTracktag);
	if((sTracktag.tag)==12)
    {
	  pwmi_val = sTracktag.track;
      if(pwmi_val<65535)
	  track_val=pwmi_val/154;
      }
      if(keypressed == 8)
      {
	bitfield.brightness = 0;
	return;
      }
		
      FTImpl.DLStart();
      FTImpl.Begin(FT_BITMAPS);
      FTImpl.SaveContext();
      FTImpl.BitmapTransformA(128);
      FTImpl.BitmapTransformE(128);
      FTImpl.Vertex2ii(0, 0, 1, 0);
      FTImpl.RestoreContext();
      animation();
      FTImpl.ColorA(255);
      FTImpl.Cmd_Text(210,10,pgm_read_byte(&font[val_font]),FT_OPT_CENTERX,"Track the bar to adjust the brightness ");
      FTImpl.Tag(8);
      FTImpl.Vertex2ii(450, 5, 3, 3);
                
      FTImpl.Tag(12);
      FTImpl.ScissorXY(20, 200); // Scissor rectangle bottom left at (20, 200)
      FTImpl.ScissorSize(420, 40); // Scissor rectangle is 420 x 40 pixels
      FTImpl.Cmd_Gradient( 20,0,0x0000ff,440,0,0xff0000);
      if(track_val){
	val = 10+(track_val/3);
	FTImpl.Write(REG_PWM_DUTY,val);
      }
    FTImpl.ColorRGB(255,255,255);
    FTImpl.Begin(FT_RECTS);
    FTImpl.Vertex2f((20 + track_val)*16,190*16);
    FTImpl.Vertex2f(440*16,250*16);	
    FTImpl.DLEnd();
    FTImpl.Finish();
    }
}
	

void scrensaver()
{
  while(1)
  {
    if(FTImpl.IsPendown())
    {
      sleep_cts = 0;
      return;
    }
    else
    {
	FTImpl.Cmd_ScreenSaver();//screen saver command will continuously update the macro0 with vertex2f command	
	FTImpl.DLStart();
	FTImpl.Begin(FT_BITMAPS);
	FTImpl.BitmapTransformA(128);
	FTImpl.BitmapTransformE(128);
	FTImpl.Vertex2ii(0, 0, 1, 0);

    FTImpl.Cmd_LoadIdentity();
	FTImpl.Cmd_Scale(2*65536,2*65536);//scale the bitmap 3 times on both sides
	FTImpl.Cmd_SetMatrix();
	FTImpl.Begin(FT_BITMAPS);
	FTImpl.BitmapHandle(4);
	FTImpl.Macro(0);
	FTImpl.DLEnd();
	FTImpl.Finish();
      }
   }
	
}


void FT_App_Refrigerator()
{
  const char ice[2][8] = {"Cubed","Crushed"};
  int32_t y, j,Baddr,i = 0,pwm = 0;
  uint32_t val = 0, tracker = 0,val_font = 0, trackval = 0,thcurr = 0,thcurrval=0;
  uint16_t cubed = 0;
  int16_t rval=0,pwm_val=0,m,x,freeze_temp = 0,fridge_temp = 0;
  uint8_t k,Read_tag = 0,ice_cell;
  int32_t time =0;
  time = millis();
  thcurr = -2;
  thcurrval = 34;
  val = 128;
  pwm_val = 10;
  rval = 0;
	sTrackTag sTracktag;
  
  FTImpl.DLStart();  
  
  if(FtSd.OpenFile(g_reader,"gn.raw")==0){
  Ft_App_LoadRawFromFile(g_reader,FT_RAM_G);
  }
  else{
	//Serial.println("Unable to open gn.raw");
  }
  FTImpl.BitmapHandle(1);
  FTImpl.BitmapSource(FT_RAM_G);
  FTImpl.BitmapLayout(FT_RGB565, 240*2, 136);
  FTImpl.BitmapSize(FT_BILINEAR, FT_BORDER, FT_BORDER, 480, 272);
	
  Baddr = FT_RAM_G + 240*2*136L;
  if(FtSd.OpenFile(g_reader,"5icons_5.raw")==0){
  Ft_App_LoadRawFromFile(g_reader,Baddr);
  }
  else{
	//Serial.println("Unable to open 5icons_5.raw");
  }
  FTImpl.BitmapHandle(2);
  FTImpl.BitmapSource(Baddr);
  FTImpl.BitmapLayout(FT_L4, 18, 37);
  FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, 35, 35);

  Baddr += 221L*(36/2);
  if(FtSd.OpenFile(g_reader,"f1icons.raw")==0){
  Ft_App_LoadRawFromFile(g_reader,Baddr);
  }
  else{
	//Serial.println("Unable to open f1icons.raw");
  }
  FTImpl.BitmapHandle(3);
  FTImpl.BitmapSource(Baddr);
  FTImpl.BitmapLayout(FT_L4, 15, 32);
  FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, 30, 32);
  
  Baddr += 192*(32/2);
  if(FtSd.OpenFile(g_reader,"logo1.raw")==0){
  Ft_App_LoadRawFromFile(g_reader,Baddr);
  }
  else{
	//Serial.println("Unable to open logo1.raw");
  }
  FTImpl.BitmapHandle(4);
  FTImpl.BitmapSource(Baddr);
  FTImpl.BitmapLayout(FT_ARGB4, 65*2, 25);
  FTImpl.BitmapSize(FT_BILINEAR, FT_BORDER, FT_BORDER, 512, 512);
  Baddr += 65*25*2L;

  if(FtSd.OpenFile(g_reader,"snow_1.raw")==0){
  Ft_App_LoadRawFromFile(g_reader,Baddr);
  }
  else{
	//Serial.println("Unable to open snow_1.raw");
  }
  FTImpl.BitmapHandle(6);
  FTImpl.BitmapSource(Baddr);
  FTImpl.BitmapLayout(FT_L4, 25, 50);
  FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, 50, 50);
  	
  FTImpl.Cmd_Track(170,180,120,8,17);
  //FTImpl.Begin(FT_BITMAPS);
  FTImpl.ColorRGB(255,255,255);
  FTImpl.Vertex2ii(0,0,1,0);
             
  FTImpl.DLEnd();
  FTImpl.Finish();
  FTImpl.Write(REG_PWM_DUTY,128);//Ft_App_fadein();

  /* compute the random values at the starting*/
  pRefrigeratorSnow = S_RefrigeratorSnowArray;
  for(j=0;j<(NumSnowRange*NumSnowEach);j++)
  {
      pRefrigeratorSnow->xOffset = random(FT_DISPLAYHEIGHT*16);//random(FT_DISPLAYWIDTH*16);
      pRefrigeratorSnow->yOffset = random(FT_DISPLAYWIDTH*16);//random(FT_DISPLAYHEIGHT*16);
      pRefrigeratorSnow->dx =  random(RandomVal*8) - RandomVal*8;//-1*random(RandomVal*8);//always -ve
      pRefrigeratorSnow->dy = -1*random(RandomVal*8);//random(RandomVal*8) - RandomVal*8;
      pRefrigeratorSnow++;
  } 

    while(1)
    {  
      Read_tag = Read_Keys();
	  FTImpl.GetTrackTag(sTracktag);
      if(tracker_tag)
         Read_tag = 0;
      if(!FTImpl.IsPendown())
      {
          tracker_tag = 0;     
          if(bitfield.unlock)
           { 
              sleep_cts++;
               if(sleep_cts>100)
                   scrensaver();
          }
        }

	pwm_val = (val - rval)>> 4;
        rval += pwm_val;  
        rval = (rval <= 10)?10:rval;
	if(Read_tag == 1)
	{
	    bitfield.unlock = (bitfield.unlock == 0)?1 : 0;
            bitfield.settings = 0;
            bitfield.font_size = 0;
	    if(bitfield.unlock == 0)
	        FTImpl.Write(REG_PWM_DUTY,rval);
        }	
		
      if(keypressed == 17)
      {
        tracker_tag = 1;    
	trackval = sTracktag.track;
	val_font = (trackval/16384);
      }

      FTImpl.DLStart();  
      FTImpl.SaveContext();
      FTImpl.Begin(FT_BITMAPS);
      FTImpl.BitmapTransformA(128);
      FTImpl.BitmapTransformE(128);
      FTImpl.Tag(0);
      FTImpl.Vertex2ii(0, 0, 1, 0);
      FTImpl.RestoreContext();
      FTImpl.Vertex2ii(390, 0, 4, 0);
      animation();
      FTImpl.RestoreContext();
    
      uint32_t disp =0,minutes,hr ;
      uint32_t temp = millis()-time;
      hr = (temp/3600000L)%12;
      minutes = (temp/60000L)%60;
      FTImpl.Cmd_Text(5 ,50,pgm_read_byte(&font[val_font]),0,"February");
      FTImpl.Cmd_Number(65 + (val_font *15), 50,pgm_read_byte(&font[val_font]),0,25);
      FTImpl.Cmd_Text(85 + (val_font *18),50,pgm_read_byte(&font[val_font]),0,",");
      FTImpl.Cmd_Number(90+ (val_font * 20) , 50,pgm_read_byte(&font[val_font]),0,2014);  
      FTImpl.Cmd_Number(10,75,pgm_read_byte(&font[val_font]),2,hr);
      FTImpl.Cmd_Text(35,75,pgm_read_byte(&font[val_font]),0,":"); 
      FTImpl.Cmd_Number(40,75,pgm_read_byte(&font[val_font]),2,minutes);
               
      FTImpl.Cmd_Text((300 - (val_font * 45)) ,110,pgm_read_byte(&font[val_font]),FT_OPT_CENTERX|FT_OPT_CENTERY,"Freeze");
      FTImpl.Cmd_Number((270 - (val_font * 45)),135,pgm_read_byte(&font[val_font]),FT_OPT_CENTERX|FT_OPT_CENTERY|FT_OPT_SIGNED,thcurr);
      FTImpl.Cmd_Text((282  - (val_font * 43) ),135,17,FT_OPT_CENTERX|FT_OPT_CENTERY,"x");
      FTImpl.Cmd_Text((292 - (val_font * 43)),135,pgm_read_byte(&font[val_font]),FT_OPT_CENTERX|FT_OPT_CENTERY,"F");


      FTImpl.Cmd_Text((370 - (val_font * 37) ),110,pgm_read_byte(&font[val_font]),FT_OPT_CENTERX|FT_OPT_CENTERY,"Fridge");
      FTImpl.Cmd_Number((350 - (val_font * 37)),135,pgm_read_byte(&font[val_font]),FT_OPT_CENTERX|FT_OPT_CENTERY,thcurrval);
      FTImpl.Cmd_Text((362 - (val_font * 36)),135,17,FT_OPT_CENTERX|FT_OPT_CENTERY,"x");
      FTImpl.Cmd_Text((372 - (val_font * 36)),135,pgm_read_byte(&font[val_font]),FT_OPT_CENTERX|FT_OPT_CENTERY,"F");
                
      FTImpl.Cmd_Text((440 - (val_font * 25)) ,110,pgm_read_byte(&font[val_font]),FT_OPT_CENTERX|FT_OPT_CENTERY,ice[cubed]);
      ice_cell  = (cubed == 0)?4 : 5;
      FTImpl.Vertex2ii((420 - (val_font * 30)), 120, 3, ice_cell); 

      FTImpl.Tag(1);   
      if(bitfield.unlock)
      {
	FTImpl.Vertex2ii(15, 200, 2, 0);
	FTImpl.Write(REG_PWM_DUTY,25);
	FTImpl.TagMask(0);
      }
      else
	{
	  FTImpl.Vertex2ii(15, 200, 2, 1);
	 }
                
         int xoff = 80;
         for(int z=2;z<5;z++)
         {
           FTImpl.Tag(z);
           FTImpl.Vertex2ii(xoff, 200, 2, z); 
           xoff+=75;                
         }
                
	FTImpl.Tag(11);
	FTImpl.Vertex2ii(290, 200, 2, 5);
        
        xoff = 250;
        int adj = 50;
        for(int z=5;z<8;z++)
        {
	  FTImpl.Tag(z);
	  FTImpl.Vertex2ii(xoff - (val_font * adj) , 95, 3,z-5);
          xoff+=70;
          adj-=10;
        }

	if(bitfield.settings)
	{
	  FTImpl.Begin(FT_RECTS);
          FTImpl.ColorRGB(34, 50, 224);
	  FTImpl.Vertex2f(170*16,30*16);
	  FTImpl.Vertex2f(390*16,60*16);			
	  FTImpl.ColorRGB(162, 229, 242);
          FTImpl.Tag(9);
	  FTImpl.Vertex2f(170*16,60*16);
	  FTImpl.Vertex2f(390*16,130*16);
	  FTImpl.ColorRGB(34, 50, 224);
	  FTImpl.Tag(10);
	  FTImpl.Vertex2f(170*16,110*16);
	  FTImpl.Vertex2f(390*16,160*16);
          FTImpl.TagMask(0);
			

	  FTImpl.ColorRGB(0, 0, 0);
	  FTImpl.Cmd_Text(278,85,pgm_read_byte(&font[val_font]),FT_OPT_CENTERX|FT_OPT_CENTERY,"Change Background");
	  FTImpl.Cmd_Text(265,130,pgm_read_byte(&font[val_font]),FT_OPT_CENTERX|FT_OPT_CENTERY,"Change Font Size");
	  FTImpl.ColorRGB(255, 255, 255);
	  FTImpl.Cmd_Text(240,45,pgm_read_byte(&font[val_font]),FT_OPT_CENTERX|FT_OPT_CENTERY,"SETTINGS");	
	  FTImpl.TagMask(1);
	  FTImpl.Tag(8);
	  FTImpl.Vertex2ii(340, 30, 3, 3);
	}


	if(bitfield.font_size)
	{
	  FTImpl.Tag(17);
	  FTImpl.Cmd_Slider(170, 180, 120, 8, 0,trackval, 65535);
	  FTImpl.Cmd_Text(200,170,pgm_read_byte(&font[val_font]),FT_OPT_CENTERX|FT_OPT_CENTERY,"FONT SIZE");
	  FTImpl.Cmd_Number(170,200,pgm_read_byte(&font[val_font]),FT_OPT_CENTERX|FT_OPT_CENTERY,val_font);
	}

	FTImpl.DLEnd();
	FTImpl.Finish();

        if(Read_tag == 2)
          bitfield.settings = 1;
                
	if((Read_tag == 8) && (bitfield.settings == 1))
        {
          bitfield.settings = 0;
          bitfield.font_size = 0;
         }  

        if(Read_tag == 10)
	  bitfield.font_size ^= 1;
	if(Read_tag == 3)
          bitfield.brightness ^= 1;
        if(Read_tag == 7)
        {
            bitfield.settings = 0;
            bitfield.font_size = 0;
          cubed_setting(&cubed,val_font);
        }
        if(bitfield.brightness)
        {
            bitfield.settings = 0;
            bitfield.font_size = 0;
          change_brightness(val,val_font);
        }
	if(Read_tag == 9)
        {
           bitfield.settings = 0;
            bitfield.font_size = 0;
	  change_background();		
        }
        if(Read_tag == 4)
        {
            bitfield.settings = 0;
            bitfield.font_size = 0;
	   Sketch(val_font);
        }
        if(Read_tag == 11)
        {
            bitfield.settings = 0;
            bitfield.font_size = 0;
          food_tracker(val_font);
        }
        if(Read_tag == 5 || Read_tag == 6)
        {
            bitfield.settings = 0;
            bitfield.font_size = 0;
	  change_temperature(&thcurr,&thcurrval,val_font);
        }
			
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

/* Display an on-screen message if the storage device is not found */
void CheckStorageDevicePrecence(){
	while(sd_present){
		FTImpl.DLStart();     
		FTImpl.Clear(1,1,1);
		FTImpl.ColorRGB(255,255,255);
		FTImpl.Cmd_Text(FT_DISPLAYWIDTH>>1,FT_DISPLAYHEIGHT>>1,29,FT_OPT_CENTER,"STORAGE DEVICE NOT FOUND");
		FTImpl.DLEnd();
		FTImpl.Finish();
		delay(5000);
		}
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
		CheckStorageDevicePrecence(); 
		SAMAPP_CoPro_Widget_Calibrate();
		FT_App_Refrigerator();
		FTImpl.Exit();  /*  Safely close all opened handles */
	}
	Serial.println("--End Application--");
}

void loop()
{
}



/* Nothing beyond this */














