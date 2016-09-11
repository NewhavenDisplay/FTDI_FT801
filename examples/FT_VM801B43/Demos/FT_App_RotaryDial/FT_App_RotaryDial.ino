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
* @file                           FT_App_RotaryDial.ino
* @brief                          Sample application to demonstrate FT801 primitives, widgets and customized screen shots. 
* @version                        0.1.0
* @date                           2014/03/21
*
*/




#include "SPI.h"
#include "FT_VM801B43.h"
#include "FT_App_RotaryDial.h"


#define DIAL		1


int32_t BaseTrackVal = 0,BaseTrackValInit = 0,BaseTrackValSign = 0;









/* Resolution dependent definitions */
#if (FT_DISPLAY_RESOLUTION == FT_DISPLAY_WQVGA_480x272)
#define FT_DISP_WIDTH (480)
#define FT_DISP_HEIGHT (272)
#elif (FT_DISPLAY_RESOLUTION == FT_DISPLAY_QVGA_320x240)
#define FT_DISP_WIDTH (320)
#define FT_DISP_HEIGHT (240)
#endif

FT801IMPL_SPI FTImpl(FT_CS_PIN,FT_PDN_PIN,FT_INT_PIN);








PROGMEM char *info[] = {  "FT801 Rotary Dial Application",
                          "APP to demonstrate interactive rotary dial,",
                          "using Points, Track",
                          "& Stencil"
                       }; 
					   
#define CALIBRATION_FONTSIZE 28
#define STARTUP_ADDRESS	100*1024L
#define ICON_ADDRESS	250*1024L
                       





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

/* cos funtion */
int16_t SAMAPP_qcos(uint16_t a)
{
  return (SAMAPP_qsin(a + 16384));
}




static void polarxy(uint32_t r, uint16_t th, uint16_t *x, uint16_t *y,uint16_t ox,uint16_t oy)
{
	
		*x = (16 * ox + (((long)r * SAMAPP_qsin(th)) >> 11) + 16);
		*y = (16 * oy - (((long)r * SAMAPP_qcos(th)) >> 11));
}
uint16_t da(int32_t i)
{

	return (i+90) * 65536 / 360;
}




uint32_t Load_RawDataFromfile(unsigned char ImageArrayname[], /*Image Array*/
								//prog_uchar* ImageArrayname,
								 uint32_t ptr,/* Array Size*/
								 uint32_t RamAddr,
								 int16_t Handle,
								 int16_t Format,
								 int16_t Stride,
								 int16_t Width,
								 int16_t Height,
								 uint8_t i
								 )
{

	int32_t fsize=0;
	fsize = SAMAPP_Uncompressed_FileSize[i];

    RamAddr = (uint32_t)(RamAddr + 7) & (~7);
	FTImpl.WriteCmd(  CMD_INFLATE);
	FTImpl.WriteCmd(  RamAddr);

	FTImpl.WriteCmdfromflash((prog_uchar*)ImageArrayname,ptr);


	FTImpl.BitmapHandle(Handle);
	FTImpl.BitmapSource(RamAddr);
	FTImpl.BitmapLayout(Format,Stride,Height);		
	FTImpl.BitmapSize(FT_NEAREST,FT_BORDER,FT_BORDER,Width,Height);		
	//FTImpl.Vertex2ii(0,0,Handle,0);				
		

	fsize = (fsize + 7) & (~7);//align the buffer to 8
	return (RamAddr + fsize);
}

static uint8_t sk=0;

uint8_t Read_Keys()
{
  static uint8_t Read_tag=0,temp_tag=0,ret_tag=0;	
  Read_tag = FTImpl.Read(REG_TOUCH_TAG);
  ret_tag = NULL;
  if(Read_tag!=NULL)		
  {
    if(temp_tag!=Read_tag)
    {
      temp_tag = Read_tag;	
      sk = Read_tag;										
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

void Info()
{
  uint16_t dloffset = 0,z;
// Touch Screen Calibration

  FTImpl.DLStart(); 
  FTImpl.Clear(1,1,1);
  FTImpl.ColorRGB(255,255,255);
  FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,FT_DISPLAYHEIGHT/2,CALIBRATION_FONTSIZE,FT_OPT_CENTERX|FT_OPT_CENTERY,"Please tap on a dot");
  FTImpl.Cmd_Calibrate(0);
  FTImpl.DLEnd();
  FTImpl.Finish();
// Ftdi Logo animation  
  FTImpl.Cmd_Logo();
 while(FTImpl.CheckLogo() != FT_GE_FINISHED); 
//Copy the Displaylist from DL RAM to GRAM
  dloffset = FTImpl.Read16(REG_CMD_DL);
  dloffset -=4; 
  FTImpl.WriteCmd(CMD_MEMCPY);
  FTImpl.WriteCmd(STARTUP_ADDRESS);
  FTImpl.WriteCmd(FT_RAM_DL);
  FTImpl.WriteCmd(dloffset);
//Enter into Info Screen
  do
  {
    FTImpl.DLStart();   
    FTImpl.Cmd_Append(STARTUP_ADDRESS,dloffset);
//Reset the BITMAP properties used during Logo animation
    FTImpl.BitmapTransformA(256);
    FTImpl.BitmapTransformB(0);
    FTImpl.BitmapTransformC(0);
    FTImpl.BitmapTransformD(0);
    FTImpl.BitmapTransformE(256);
    FTImpl.BitmapTransformF(0);  
//Display the information with transparent Logo using Edge Strip  
    FTImpl.ColorRGB(219,180,150);
    FTImpl.ColorA(220);
    FTImpl.Begin(FT_EDGE_STRIP_A);
    FTImpl.Vertex2f(0,FT_DISPLAYHEIGHT*16);
    FTImpl.Vertex2f(FT_DISPLAYWIDTH*16,FT_DISPLAYHEIGHT*16);
    FTImpl.ColorA(255);
    FTImpl.ColorRGB(0,0,0);

    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,20,28,FT_OPT_CENTERX|FT_OPT_CENTERY,info[0]);
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,60,26,FT_OPT_CENTERX|FT_OPT_CENTERY,info[1]);
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,90,26,FT_OPT_CENTERX|FT_OPT_CENTERY,info[2]);  
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,120,26,FT_OPT_CENTERX|FT_OPT_CENTERY,info[3]);  
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,FT_DISPLAYHEIGHT-30,26,FT_OPT_CENTERX|FT_OPT_CENTERY,"Click to play");
//Check if the Play key and change the color
    if(sk!='P')
    FTImpl.ColorRGB(255,255,255);
    else
    {
      FTImpl.ColorRGB(100,100,100);
    	
    }

    FTImpl.Begin(FT_POINTS);   
    FTImpl.PointSize(20*16);
    FTImpl.Tag('P');
    FTImpl.Vertex2f((FT_DISPLAYWIDTH/2)*16,(FT_DISPLAYHEIGHT-60)*16);


    FTImpl.Clear(0,1,0);

    FTImpl.ColorRGB(255, 0, 0) ;
    FTImpl.StencilOp(FT_INCR,FT_INCR) ;
    FTImpl.ColorMask(0,0,0,0) ;//mask all the colors
    FTImpl.Begin(FT_EDGE_STRIP_L);

    FTImpl.Vertex2ii(((FT_DISPLAYWIDTH/2)-14+10),(FT_DISPLAYHEIGHT-77+5),0,0);
    FTImpl.Vertex2ii(((FT_DISPLAYWIDTH/2)-2+15),(FT_DISPLAYHEIGHT-67+8),0,0);
    FTImpl.Vertex2ii(((FT_DISPLAYWIDTH/2)-14+10),(FT_DISPLAYHEIGHT-57+11),0,0);
    FTImpl.Vertex2ii(((FT_DISPLAYWIDTH/2)-14+10),(FT_DISPLAYHEIGHT-77+5),0,0);

    FTImpl.ColorMask(1,1,1,1) ;//enable all the colors
    FTImpl.StencilFunc(FT_EQUAL,1,255);
    FTImpl.Begin(FT_EDGE_STRIP_L);
    FTImpl.Vertex2ii(FT_DISPLAYWIDTH,0,0,0);
    FTImpl.Vertex2ii(FT_DISPLAYWIDTH,FT_DISPLAYHEIGHT,0,0);
	
    FTImpl.StencilOp(FT_KEEP,FT_KEEP);
    FTImpl.StencilFunc(FT_ALWAYS,0,255);
    FTImpl.LineWidth(1*16);
	
    FTImpl.Begin(FT_LINE_STRIP);
    FTImpl.Vertex2ii(((FT_DISPLAYWIDTH/2)-14+10),(FT_DISPLAYHEIGHT-77+5),0,0);
    FTImpl.Vertex2ii(((FT_DISPLAYWIDTH/2)-2+15),(FT_DISPLAYHEIGHT-67+8),0,0);
    FTImpl.Vertex2ii(((FT_DISPLAYWIDTH/2)-14+10),(FT_DISPLAYHEIGHT-57+11),0,0);
    FTImpl.Vertex2ii(((FT_DISPLAYWIDTH/2)-14+10),(FT_DISPLAYHEIGHT-77+5),0,0);
	
    FTImpl.DLEnd();
    FTImpl.Finish();
  }while(Read_Keys()!='P');
  /* To cover up some distortion while demo is being loaded */
  FTImpl.DLStart();
  FTImpl.ColorRGB(255,255,255);
  FTImpl.Clear(1,1,1);
  FTImpl.DLEnd();
  FTImpl.Finish();
/* wait until Play key is not pressed*/ 
}


/* Definitions used for optimizations */
#define ROTARY_APP_WHITECIRCELE_FIFO_SIZE (4*6+4*7*10+4*1)
#define ROTARY_APP_START_FIFO_SIZE_INWORDS (59)
#define ROTARY_APP_START_FIFO_SIZE (4*ROTARY_APP_START_FIFO_SIZE_INWORDS)
#define ROTARY_APP_START_FIFO_RANDOMBITMAP_SIZE (4*4)
#define ROTARY_APP_END_FIFO_SIZE_INWORDS (48)
#define ROTARY_APP_END_FIFO_SIZE (4*ROTARY_APP_END_FIFO_SIZE_INWORDS)
#define ROTARY_APP_BLOCK1_SIZE_INWORDS (17)
#define ROTARY_APP_BLOCK1_SIZE (4*ROTARY_APP_BLOCK1_SIZE_INWORDS)

/* Array optimization for code space */
#if 1        
       PROGMEM prog_uint32_t ROTARY_APP_DL_BLOCK0[ROTARY_APP_START_FIFO_SIZE_INWORDS] =
		{	
          SCISSOR_SIZE(512,512),
	  SCISSOR_XY(0,0),
 	  /* Draw all the background bitmaps - balls with various resolutions */
	  BEGIN(FT_BITMAPS),
	  COLOR_RGB(255 ,255 ,255), // brown outside circle
          VERTEX2II(0,0,0,0),VERTEX2II(0,0,0,0),VERTEX2II(0,0,0,0),VERTEX2II(0,0,0,0),VERTEX2II(0,0,0,0),
          VERTEX2II(0,0,0,0),VERTEX2II(0,0,0,0),VERTEX2II(0,0,0,0),VERTEX2II(0,0,0,0),VERTEX2II(0,0,0,0),
          VERTEX2II(0,0,0,0),VERTEX2II(0,0,0,0),VERTEX2II(0,0,0,0),VERTEX2II(0,0,0,0),VERTEX2II(0,0,0,0),
          VERTEX2II(0,0,0,0),VERTEX2II(0,0,0,0),VERTEX2II(0,0,0,0),VERTEX2II(0,0,0,0),VERTEX2II(0,0,0,0),
          VERTEX2II(0,0,0,0),VERTEX2II(0,0,0,0),VERTEX2II(0,0,0,0),VERTEX2II(0,0,0,0),VERTEX2II(0,0,0,0),
          VERTEX2II(0,0,0,0),VERTEX2II(0,0,0,0),VERTEX2II(0,0,0,0),VERTEX2II(0,0,0,0),VERTEX2II(0,0,0,0),
          VERTEX2II(0,0,0,0),VERTEX2II(0,0,0,0),VERTEX2II(0,0,0,0),VERTEX2II(0,0,0,0),VERTEX2II(0,0,0,0),
          VERTEX2II(0,0,0,0),VERTEX2II(0,0,0,0),VERTEX2II(0,0,0,0),VERTEX2II(0,0,0,0),VERTEX2II(0,0,0,0),
#if (FT_DISPLAY_RESOLUTION == FT_DISPLAY_WQVGA_480x272)
  	  POINT_SIZE(((FT_DISP_WIDTH/4) - 19) * 16),		
#endif
#if (FT_DISPLAY_RESOLUTION == FT_DISPLAY_QVGA_320x240)
  	  POINT_SIZE(((FT_DISP_WIDTH/4) + 5) * 16),		
#endif
	  BEGIN(FT_POINTS),
	  COLOR_RGB(235 ,117 ,0), // brown outside circle
	  VERTEX2F((FT_DISP_WIDTH/2)*16 - 24,(FT_DISP_HEIGHT/2)*16 - 24),
	  COLOR_RGB(0 ,0 ,0), // brown outside circle
	  VERTEX2F((FT_DISP_WIDTH/2)*16 + 24,(FT_DISP_HEIGHT/2)*16 + 24),
	  COLOR_RGB(128 ,64 ,0), // brown outside circle
	  VERTEX2F((FT_DISP_WIDTH/2)*16,(FT_DISP_HEIGHT/2)*16),
#if (FT_DISPLAY_RESOLUTION == FT_DISPLAY_WQVGA_480x272)
	  POINT_SIZE(((FT_DISP_WIDTH/4) - 19 - 50) * 16) ,
#endif
#if (FT_DISPLAY_RESOLUTION == FT_DISPLAY_QVGA_320x240)
	  POINT_SIZE(40*16) ,
#endif
	  COLOR_RGB(87 ,64 ,8), // inner circle - light brown color
	  VERTEX2F((FT_DISP_WIDTH/2)*16 - 24,(FT_DISP_HEIGHT/2)*16 - 24),
	  COLOR_RGB(0 ,0 ,0), // inner circle - light brown color
	  VERTEX2F((FT_DISP_WIDTH/2)*16 + 24,(FT_DISP_HEIGHT/2)*16+24),
	  COLOR_RGB(204 ,151 ,16), // inner circle - light brown color
	  VERTEX2F((FT_DISP_WIDTH/2)*16,(FT_DISP_HEIGHT/2)*16),
        };
        
        PROGMEM prog_uint32_t ROTARY_APP_DL_BLOCK1 [ROTARY_APP_BLOCK1_SIZE_INWORDS] =   
        {     
		 CMD_DLSTART,                
		 CLEAR_COLOR_RGB(237 ,186 ,17) , // background color
		CLEAR(1,1,1),
                /* Draw two gradients, one at the top and another at the bottom. Height of former is 
                   80% of display height and later 20% of display height */
		SCISSOR_SIZE(512,218),
		SCISSOR_XY(0,0),
		//FTImpl.Cmd_Gradient(0, 0, 0xEDBA11,0, FT_DISP_HEIGHT, 0x82660A);
                CMD_GRADIENT,
                0,
                0xEDBA11,
                (((uint32_t)FT_DISP_HEIGHT<<16)),
                0x82660A,

		SCISSOR_SIZE(512,512),
		SCISSOR_XY(0,218),
		//FTImpl.Cmd_Gradient(0, 0, 0x2B2205,0, (FT_DISP_HEIGHT -(FT_DISP_HEIGHT*0.20))/*218*/, 0x82660A);                
                CMD_GRADIENT,
                0,
                0x2B2205,
              (((uint32_t)(FT_DISP_WIDTH*0.8)<<16)),
              0x82660A,                
                };
                
                
                 static PROGMEM prog_uint32_t ROTARY_APP_DL_BLOCK2 [ROTARY_APP_END_FIFO_SIZE_INWORDS] =   
                {     
		  //CMD_DLSTART,  
                  STENCIL_FUNC(FT_GEQUAL,1,255),
		  COLOR_RGB(0 ,0 ,0),
		  BEGIN(FT_BITMAPS),
#if (FT_DISPLAY_RESOLUTION == FT_DISPLAY_WQVGA_480x272)
	          VERTEX2II(283,182,28,'0'),VERTEX2II(248,198,28,'9'),VERTEX2II(209,195,28,'8'),VERTEX2II(176,172,28,'7'),VERTEX2II(160,137,28,'6'),
                  VERTEX2II(163,98,28,'5'),VERTEX2II(186,65,28,'4'),VERTEX2II(221,49,28,'3'),VERTEX2II(260,52,28,'2'),VERTEX2II(293,75,28,'1'),
 #endif
#if (FT_DISPLAY_RESOLUTION == FT_DISPLAY_QVGA_320x240)
                   VERTEX2II(191,151,28,'0'),VERTEX2II(164,164,28,'9'),VERTEX2II(135,161,28,'8'),VERTEX2II(111,144,28,'7'),VERTEX2II(98,117,28,'6'),
                  VERTEX2II(101,88,28,'5'),VERTEX2II(118,64,28,'4'),VERTEX2II(145,51,28,'3'),VERTEX2II(174,54,28,'2'),VERTEX2II(198,71,28,'1'),
#endif         
		  STENCIL_OP(FT_KEEP,FT_KEEP),
                  STENCIL_FUNC(FT_ALWAYS,0,255),
                  COLOR_A(128),
		
#if (FT_DISPLAY_RESOLUTION == FT_DISPLAY_WQVGA_480x272)
	          VERTEX2II(283,182,28,'0'),VERTEX2II(248,198,28,'9'),VERTEX2II(209,195,28,'8'),VERTEX2II(176,172,28,'7'),VERTEX2II(160,137,28,'6'),
                  VERTEX2II(163,98,28,'5'),VERTEX2II(186,65,28,'4'),VERTEX2II(221,49,28,'3'),VERTEX2II(260,52,28,'2'),VERTEX2II(293,75,28,'1'),
 #endif
#if (FT_DISPLAY_RESOLUTION == FT_DISPLAY_QVGA_320x240)
                   VERTEX2II(191,151,28,'0'),VERTEX2II(164,164,28,'9'),VERTEX2II(135,161,28,'8'),VERTEX2II(111,144,28,'7'),VERTEX2II(98,117,28,'6'),
                  VERTEX2II(101,88,28,'5'),VERTEX2II(118,64,28,'4'),VERTEX2II(145,51,28,'3'),VERTEX2II(174,54,28,'2'),VERTEX2II(198,71,28,'1'),
#endif      

		  COLOR_RGB(255,255,255),
		  COLOR_A(255),
		  BEGIN(FT_BITMAPS),		

		  /* dial image*/
		  COLOR_RGB(255,255,255),
		  TAG_MASK(1),
		  TAG(100),

		  VERTEX2II(((FT_DISP_WIDTH/2)-131),((FT_DISP_HEIGHT)-70),2,0),
		  TAG_MASK(0),	
                  /* fingerstop jpg */		
		  COLOR_RGB(255,255,255),
#if (FT_DISPLAY_RESOLUTION == FT_DISPLAY_WQVGA_480x272)
		  VERTEX2II(((FT_DISP_WIDTH/2) + 56)/*296*/,((FT_DISP_HEIGHT/2) + 17)/*153*/,0,0), //exact location of picture	
 #endif
#if (FT_DISPLAY_RESOLUTION == FT_DISPLAY_QVGA_320x240)
                   VERTEX2II(((FT_DISP_WIDTH/2) + 42)/*296*/,((FT_DISP_HEIGHT/2) + 10)/*153*/,0,0), //exact location of picture	
#endif
		  /*reject button jpg*/
		  COLOR_RGB(255,255,255),
		  TAG_MASK(1),
		  TAG(200),
		  VERTEX2II(((FT_DISP_WIDTH/2)+80),((FT_DISP_HEIGHT)-70),1,0),
		  TAG_MASK(0),
		 
                 CMD_FGCOLOR,
                 0xCC9710,

		
               CMD_BUTTON,
               ((uint32_t)((uint32_t)5<<16)| (FT_DISP_WIDTH/6)),               
#if (FT_DISPLAY_RESOLUTION == FT_DISPLAY_WQVGA_480x272)
               (((uint32_t)((uint32_t)((FT_DISP_HEIGHT/2) - 111)<<16)|(FT_DISP_WIDTH - 160))),   
 #endif
#if (FT_DISPLAY_RESOLUTION == FT_DISPLAY_QVGA_320x240)
               ((uint32_t)((uint32_t)((FT_DISP_HEIGHT/2) - 95)<<16)|(FT_DISP_WIDTH - 110)),
#endif
               (((uint32_t)0<<16)|28),
               0,

		
                };
        
#endif


void Rotary_Dial_phone()
{
	uint8_t ReadTag = 0,PrevTag = 0,StoreTag,flag =0, PrintTag=0,Tagcheck=0,FontWid = 0, FontHt = 0,Penup=0;
	char StringArray[31];	
	uint32_t temp =0,tagval,PrevTh = 0,adjusting = 0, CurrTh=0;	
	uint16_t OuterCircleRadius=0, InnerCircleRadius,CentreWhiteCircle,PrevTouch=0,VertexX, VertexY;
	uint16_t ScreenWidthMid,ScreenHeightMid,FontPLaceX[10],FontPLaceY[10],SmallNumCircleRad, i,j,k=0,l=0,m=0,StartTheta,EndTheta,IncTheta;
	uint16_t NoX,NoY,vX,MinTheta,MaxTheta,thresholdflag=0,ButHt, ButWid,FingerstopX,FingerstopY,th,theta;	
	uint32_t RamFifoStartAddr,RamFifoendsize,RamAddrOffest;
        int16_t NoTouch,Touch, NextTh=0;
	SAMAPP_RotaryDial_Bmps_t BmpBalls[40];
	sTagXY sTagxy;
	sTrackTag sTracktag;
	
	ScreenWidthMid = FT_DISPLAYWIDTH/2;
	ScreenHeightMid = FT_DISPLAYHEIGHT/2; 
      	
        uint32_t time = 0;

#if (FT_DISPLAY_RESOLUTION == FT_DISPLAY_WQVGA_480x272)
	OuterCircleRadius = ScreenWidthMid/2 - 19;	
	InnerCircleRadius = OuterCircleRadius - 50;
	SmallNumCircleRad = InnerCircleRadius/2 - 8;
	FingerstopX = ScreenWidthMid + 56;
	FingerstopY = ScreenHeightMid + 17;
	NoX = FT_DISPLAYWIDTH/6;//80; //(ScreenWidthMid)-(ScreenWidthMid/2);
	NoY = 5;//FT_DISPLAYHEIGHT - (FT_DISPLAYHEIGHT - 7);//7;
	ButHt = (FT_DISPLAYHEIGHT/2) - 111;
	ButWid = FT_DISPLAYWIDTH - 160;
#endif

#if (FT_DISPLAY_RESOLUTION == FT_DISPLAY_QVGA_320x240)
	OuterCircleRadius = ScreenWidthMid/2 + 5;	
	InnerCircleRadius = 30;
	SmallNumCircleRad = 13;
	FingerstopX = ScreenWidthMid + 42;
	FingerstopY = ScreenHeightMid +10;
	NoX = FT_DISPLAYWIDTH/6;//80; //(ScreenWidthMid)-(ScreenWidthMid/2);
	NoY = 6;//FT_DISPLAYHEIGHT - (FT_DISPLAYHEIGHT - 7);//7;
	ButHt = (FT_DISPLAYHEIGHT/2) - 95;
	ButWid = FT_DISPLAYWIDTH - 110;
#endif

	CentreWhiteCircle = InnerCircleRadius + ((OuterCircleRadius - InnerCircleRadius)/2);	
	VertexX = 0;
	VertexY = 0;	
	StartTheta = 50;
	EndTheta = 320;
	IncTheta = 30;
	NoTouch = -32768;
	MinTheta = 60;
	MaxTheta = 90;
	vX = ScreenWidthMid;
	StringArray[0] = '\0';
	tagval = 10;


	FTImpl.WriteCmd( CMD_MEMSET);
	FTImpl.WriteCmd( 0L);//starting address of memset
	FTImpl.WriteCmd( 55L);//value of memset
	FTImpl.WriteCmd(256*1024);
        i =0;
//        for(theta = StartTheta; theta<= EndTheta; theta+=IncTheta)
//	{		
//	    th = theta;	
//	    th = da(th);
//            temp = 0;//FontSize - 16;	
//	    if(theta  == StartTheta +30) i = 9;		
//	    j = i+'0';
//            /* To save space in arduino, the width and height of the fonts has been hardcoded */ 	   
//            FontWid = 12;/* for characters 0 to 9*/
//	    FontHt = 25;/*for all characters*/
//	    polarxy(CentreWhiteCircle,th,&VertexX,&VertexY,ScreenWidthMid,ScreenHeightMid);
//	    FontPLaceX[l] = (VertexX/16) - (FontWid/2);            
//	    FontPLaceY[l] = (VertexY/16) - (FontHt/2);            
//	    i--;
//	    l++;
//	}


	/* set the vol to max */
	FTImpl.Write( REG_VOL_SOUND,0xFF);

	/* Calculation of balls offsets and rate */
	for(i=0;i<40;i++)
	{
		BmpBalls[i].xOffset = random(FT_DISPLAYWIDTH);
		BmpBalls[i].yOffset = random(FT_DISPLAYHEIGHT);

		BmpBalls[i].xDiff = random(8) - 4;
		BmpBalls[i].yDiff = random(4) - 2;
	}

	/* Construction of starting screen shot, assign all the bitmap handles here */
	FTImpl.DLStart();
	FTImpl.ClearColorRGB(255,255,255);
	FTImpl.Clear(1,1,1);
	
	/* track circles outside the while loop */
	for(theta=StartTheta;theta<=EndTheta ;theta+=IncTheta)			
	{	
		th = theta;		
		if(theta == StartTheta + IncTheta) tagval = 9;
		th = da(th);				
		FTImpl.Cmd_Track(ScreenWidthMid, ScreenHeightMid, DIAL, DIAL, tagval);		
		tagval--;
	}
	
	/* Download the raw data into intended locations */
	{
		RamAddrOffest = FT_RAM_G;
		FTImpl.Begin(FT_BITMAPS);

        RamAddrOffest = Load_RawDataFromfile(FingerStop,SAMAPP_FingerStop_SIZE,RamAddrOffest,0,FT_ARGB4,56*2,56,38,0);//finger stopper
		RamAddrOffest = Load_RawDataFromfile(Call,SAMAPP_CallIcon,RamAddrOffest,1,FT_ARGB4,56*2,56,56,1);//reject button
		RamAddrOffest = Load_RawDataFromfile(CallCancel,SAMAPP_CallCancelIcon,RamAddrOffest,2,FT_ARGB4,56*2,56,56,2);//call button
		RamAddrOffest = Load_RawDataFromfile(YellowBall20x20,SAMAPP_YellowBall_20x20,RamAddrOffest,3,FT_ARGB4,20*2,20,20,3);//ball 20x20
		RamAddrOffest = Load_RawDataFromfile(YellowBall30x30,SAMAPP_YellowBall_30x30,RamAddrOffest,4,FT_ARGB4,30*2,30,30,4);//ball 30x30
		RamAddrOffest = Load_RawDataFromfile(YellowBall40x40,SAMAPP_YellowBall_40x40,RamAddrOffest,5,FT_ARGB4,40*2,40,40,5);//ball 40x40
		RamAddrOffest = Load_RawDataFromfile(YellowBall60x60,SAMAPP_YellowBall_60x60,RamAddrOffest,6,FT_ARGB4,60*2,60,60,6);//ball 60x60

		FTImpl.End();
	}






	FTImpl.DLEnd();
	FTImpl.Finish();

        /* Performance optimization - Construct static array of graphics instructions once and download into GRAM and run time use cmd_append() */
        {
          RamFifoStartAddr = RamAddrOffest;
          FTImpl.Writefromflash(RamFifoStartAddr,(prog_uchar *)ROTARY_APP_DL_BLOCK0, ROTARY_APP_START_FIFO_SIZE);
          RamAddrOffest += ROTARY_APP_START_FIFO_SIZE;  
        }
        
	do{
                //Serial.println(millis()-time,DEC);   
                //time = millis();   

                /* Generate random bitmap offsets into the RGAM address directly and followed by Coprocessor append command */

                //Ft_Gpu_Hal_StartTransfer(phost,FT_GPU_WRITE,RamFifoStartAddr + ROTARY_APP_START_FIFO_RANDOMBITMAP_SIZE);
				FTImpl.StartTransfer(RamFifoStartAddr + ROTARY_APP_START_FIFO_RANDOMBITMAP_SIZE);

		for(i = 0;i<40;i++)
		{
			FTImpl.Transfer32(VERTEX2II(BmpBalls[i].xOffset,BmpBalls[i].yOffset,3+(i%4),0));
			BmpBalls[i].xOffset += BmpBalls[i].xDiff;
			BmpBalls[i].yOffset += BmpBalls[i].yDiff;
			if(BmpBalls[i].xOffset < -80)
			{
				BmpBalls[i].xOffset = FT_DISPLAYWIDTH + 80;
			}
			else if(BmpBalls[i].xOffset > FT_DISPLAYWIDTH + 80)
			{
				BmpBalls[i].xOffset = -80;
			}

			if(BmpBalls[i].yOffset < -80)
			{
				BmpBalls[i].yOffset = FT_DISPLAYHEIGHT + 80;
			}
			else if(BmpBalls[i].yOffset > FT_DISPLAYHEIGHT + 80)
			{
				BmpBalls[i].yOffset = -80;
			}
		}
                //Ft_Gpu_Hal_EndTransfer(phost);
				FTImpl.EndTransfer();

		BmpBalls[random(40)].xDiff = random(8) - 4;
		BmpBalls[random(40)].yDiff = random(4) - 2;

                /* use coprocessor append command to download the DL commands into DL memory from graphics ram */

                /* Array optimization */





                FTImpl.WriteCmdfromflash((prog_uchar*)ROTARY_APP_DL_BLOCK1,ROTARY_APP_BLOCK1_SIZE);
                FTImpl.Cmd_Append(RamFifoStartAddr,ROTARY_APP_START_FIFO_SIZE);
              

		i = 0;//font inside the white circles - start at 0
		tagval = 10;// tag for the white circles - decrementing tagvalue inside loop

		FTImpl.GetTagXY(sTagxy);
		Touch = sTagxy.x;
		Tagcheck = sTagxy.tag;
		FTImpl.GetTrackTag(sTracktag);
		ReadTag = sTracktag.tag;
                /* Anticlockwise movement */
		if((PrevTouch&0x8000) && (0 == PrevTag))
		{
			BaseTrackValInit = sTracktag.track & 0xffff;
		}
		else
		{
			BaseTrackValInit = 0;
		}
                /* verify for the track value */
                if((Touch != NoTouch))
		{
			if((ReadTag >= 1) && (ReadTag <= 10))
			{
				CurrTh = (int16_t)(sTracktag.track & 0xffff);
                                /* BaseTrackVal is the rotary track value of the particular white circle that is moving */
				if(adjusting!=0)
				{
					flag = 0;
					BaseTrackVal += (int16_t)(CurrTh - PrevTh);
					BaseTrackValSign += (int16_t)(CurrTh - PrevTh);
					/* handling of loopback */
					BaseTrackVal &= 0xffff;	
                                        /* NextTh value is the track value in degrees */					
					NextTh = (uint16_t)(BaseTrackValSign / 182);	
                                        NextTh = abs(NextTh);

				
				} 
				PrevTh = CurrTh;
				adjusting = sTracktag.tag;
				PrintTag = ReadTag;
			
			}
		}
                else if(Touch == NoTouch)
                {
			BaseTrackVal=0;		
			flag = 1;
                        Penup = 1;
			if(adjusting != 0)
			{        
                                /* If white circles have moved beyond their initial value, Nextth value is incremented */
				if(NextTh > 0 )
				{
                                        /* After the white circle has moved to the desired location, and when penup has been done,
                                        the white crcles moved back to its original positon of 0 degrees. */
                                        /* NextTh moves back in degrees of twenty in steps. Also sound has to be played when moving back. */
					NextTh-=20;	
					FTImpl.Write( REG_VOL_SOUND,0xff);
					FTImpl.Write16( REG_SOUND,0x51);
					FTImpl.Write( REG_PLAY,1);
					MinTheta = 60;
					MaxTheta = 90;
					
				}
				else
				{ 
    					/* When the white circles have moved back to its original position, the tag value of the particular white circle 
                                               (PrintTag) is stored in the StringArray. StringArray stores upto 17 values. The thresholdflag is also checked 
                                               before entering this loop */
					NextTh = 0;
					if((PrintTag >= 1) && (PrintTag <= 10) && (thresholdflag ==1))
					{
						if (PrintTag == 10)
                                                {
                                                  PrintTag = 0;
                                                }
                                                
						StringArray[k] = PrintTag + '0';

						if (k >= 17)
						{
							k = 0;
							StringArray[k] = PrintTag + '0';
							
						}
                                                k++;
						thresholdflag=0;
					}
                            
					StringArray[k] = '\0';					
					adjusting = 0;
					
				}					
			}
                        /* When movement has been finished, the Printtag is cleared*/
			else if(adjusting == 0)
			{
				PrintTag = '\0';			
				
			}
			BaseTrackValSign = 0;
		}

                /* Sound play wrt user touch */
                /* When white circles are touched for the first time after penup - play sound */
                if((PrevTag == 0)&& ((ReadTag !=0)&&(ReadTag !=255)) && ( Penup = 1))
                {
		      FTImpl.Write( REG_VOL_SOUND,0xFF);
		      FTImpl.Write16( REG_SOUND,0x51);
		      FTImpl.Write( REG_PLAY,1);
                      Penup = 0;
                }
  
		/* dial button (tag 100) & penup has been done - play dtmf sound */
		if((Tagcheck == 100)  && (flag == 1))
		{
			if(k>0)
			{				
				for(i=0;i<k;i++)
				{
					/* Play the dtmf sound */
					FTImpl.Write16( REG_SOUND,StringArray[i]);
					FTImpl.Write( REG_PLAY,1);
                                        delay(80);
					FTImpl.Write16( REG_SOUND,0);
					FTImpl.Write( REG_PLAY,1);
					delay(80);
				}
			}

		}

		/* reject button (tag -200)& penup has been done. clear the string */
		if((Tagcheck == 200) && (flag ==1) )
		{
		        StringArray[0] = '\0';
			k=0;
		}                  

                //Ft_Gpu_Hal_CheckCmdBuffer(phost,ROTARY_APP_WHITECIRCELE_FIFO_SIZE);
                //Ft_Gpu_Hal_StartTransfer(phost,FT_GPU_WRITE,FT_RAM_CMD + phost->ft_cmd_fifo_wp);
				FTImpl.ChkGetFreeSpace(ROTARY_APP_WHITECIRCELE_FIFO_SIZE);
				FTImpl.StartTransfer(FT_RAM_CMD + FTImpl.CmdFifoWp);
		FTImpl.Transfer32(BEGIN(FT_POINTS)); 
		FTImpl.Transfer32(STENCIL_OP(FT_INCR,FT_INCR));//pass or fail increment	 
		FTImpl.Transfer32(POINT_SIZE(SmallNumCircleRad * 16)); 
		FTImpl.Transfer32(COLOR_A(255));
		FTImpl.Transfer32(COLOR_RGB(255,255,255));	

		/* Placement of number circles which will move around the rotary */		
                FTImpl.Transfer32(TAG_MASK(1));
		for(theta=StartTheta;theta<=EndTheta ;theta+=IncTheta)			
		{	
			
			th = theta;		
			if(theta == (StartTheta + IncTheta)) 
                        {
                          tagval = 9;	
                        }
			
			/* limit threshold level near finger stop*/
			/* for value 0 - tag is 10*/
			if((NextTh > EndTheta) && (ReadTag == 10))
			{
				NextTh = EndTheta;
				thresholdflag =1;
			}
			
			/* for value 1 to 9 - threshold*/
			for(m = 1,l=9; m<=9, l>=1;m++,l--)
			{
				if((NextTh > EndTheta - (m*IncTheta)) && (ReadTag == l))
				{
				    NextTh = EndTheta - (m*IncTheta);
		                    thresholdflag =1;
				}
			}
			/* To stop anti clockwise movement of the dial */			
			if((BaseTrackValInit + BaseTrackValSign) < BaseTrackValInit)
			{
				NextTh = 0;
			}
			th = th + NextTh;		
			
			th = da(th);	

			// calculate x and  y for placing circles according to degree
			polarxy(CentreWhiteCircle,th,&VertexX ,&VertexY ,ScreenWidthMid,ScreenHeightMid);				
		
			FTImpl.Transfer32(TAG(tagval));
			FTImpl.Transfer32(COLOR_RGB(200 ,99 ,0)); // for 3d effect adding circles of darker color
			FTImpl.Transfer32(VERTEX2F(VertexX-24,VertexY-24));
			FTImpl.Transfer32(COLOR_RGB(0 ,0 ,0));  
			FTImpl.Transfer32(VERTEX2F(VertexX+24,VertexY+24));// for 3d effect adding circles of darker color
			FTImpl.Transfer32(COLOR_RGB(255 ,255 ,255));	
			FTImpl.Transfer32(VERTEX2F(VertexX,VertexY));// the base number circle
			//Serial.print(VertexX);
			//Serial.print(" ");
			//Serial.println(VertexY);
			tagval--;	
		}
                FTImpl.Transfer32(TAG_MASK(0));		
				FTImpl.EndTransfer();
                FTImpl.CmdFifoWp = (FTImpl.CmdFifoWp + ROTARY_APP_WHITECIRCELE_FIFO_SIZE) & 4095;
				FTImpl.CmdFifoWp = (FTImpl.CmdFifoWp + 3) & 0xffc;
				FTImpl.Write16(REG_CMD_WRITE,FTImpl.CmdFifoWp);
                /* When NextTh value moves between 30 degrees, play sound */
            	if((NextTh > MaxTheta) && (Touch != NoTouch))
		{
			FTImpl.Write( REG_VOL_SOUND,0xFF);
			FTImpl.Write16( REG_SOUND,0x51);
			FTImpl.Write( REG_PLAY,1);
			MinTheta = MinTheta + IncTheta;
			MaxTheta = MaxTheta + IncTheta;		
		}
		else if(((NextTh < MinTheta) && (NextTh >(MinTheta-30))) && (NextTh!=0)&&(Touch != NoTouch))
		{
			FTImpl.Write( REG_VOL_SOUND,0xFF);
			FTImpl.Write16( REG_SOUND,0x51);
			FTImpl.Write( REG_PLAY,1);
			MinTheta = MinTheta - IncTheta;
			MaxTheta = MaxTheta - IncTheta;		
		}
        

		/* Placement of NUmbers inside the white circle - using stencil effect*/
                RamFifoendsize = 0;
                
                FTImpl.WriteCmdfromflash((prog_uchar*)ROTARY_APP_DL_BLOCK2,ROTARY_APP_END_FIFO_SIZE);
                

		/* display numbers in the centre of the point while dialing*/
		if((PrintTag >0 ) && (PrintTag <= 10) && (Touch != NoTouch))
		{
      		FTImpl.StartTransfer(FT_RAM_CMD + FTImpl.CmdFifoWp);
                        /* The tag value of zero font is 10. So Set PrintTag is set to zero to print it correctly */
			if (PrintTag == 10)PrintTag = 0;
			FTImpl.Transfer32(COLOR_RGB(0,0,0));
            FTImpl.Transfer32(VERTEX2II((ScreenWidthMid - 16)/*225*/,(ScreenHeightMid - 27)/*110*/,31,PrintTag + '0'));
			FTImpl.Transfer32(COLOR_RGB(0,0,0));
            FTImpl.Transfer32(VERTEX2II((ScreenWidthMid - 14)/*225*/,(ScreenHeightMid - 25)/*110*/,31,PrintTag + '0'));
			FTImpl.Transfer32(COLOR_RGB(255,255,255));
            FTImpl.Transfer32(VERTEX2II((ScreenWidthMid - 15)/*225*/,(ScreenHeightMid - 26)/*110*/,31,PrintTag + '0'));     
						
						/* Restore the PrintTag to ten after printing has been done */
			if (PrintTag == 0)PrintTag = 10;
                        RamFifoendsize = 6*4;
				FTImpl.EndTransfer();  
				FTImpl.CmdFifoWp = (FTImpl.CmdFifoWp + RamFifoendsize) & 4095;
				FTImpl.CmdFifoWp = (FTImpl.CmdFifoWp + 3) & 0xffc;
				FTImpl.Write16(REG_CMD_WRITE,FTImpl.CmdFifoWp);
		}

                FTImpl.ColorRGB(0,0,0);

		FTImpl.Cmd_Text((NoX + ButWid/2),(NoY + ButHt/2),28,FT_OPT_CENTER,StringArray);






		FTImpl.DLEnd();
		FTImpl.Finish();

                /* update all the prev with current */
		PrevTag = ReadTag;
		PrevTouch = Touch;
		}while(1);
			

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
		Info();
		Rotary_Dial_phone();
		FTImpl.Exit();  /*  Safely close all opened handles */
	}
	Serial.println("--End Application--");
}

void loop()
{
 
}



/* Nothing beyond this */


