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
* @file                           FT_App_MeterDemo.ino
* @brief                          Sample application to demonstrate FT801 primitives, widgets and customized screen shots. 
* @version                        0.1.1
* @date                           Jun/13/2014

version 0.1.0 - Mar/21/2014 - Initial version
version 0.1.1 - Jun/13/2014 - Scrolling velocity modification on FT801 platform. 
*
*/


#include "SPI.h"
#include "Wire.h"
#include "FT_VM801P43_50.h"


/* Macro for absolute and relative dial. */
//#define Absolute_Dial
#define Relative_Dial

#define STARTUP_ADDRESS	100*1024L
#define DIAL		(1)
#define F16(s)           ((int32_t)((s) * 65536))




int32_t BaseTrackVal = 0,BaseTrackValInit = 0,BaseTrackValSign = 0,MemLoc=0;
int16_t MoveDeg =0;



#define MAX_IMAGES  2	
static int32_t fsize = 0;



#define CALIBRATION_FONTSIZE 28



/* SD card object*/
FT_SD FtSd(FT_SD_CSPIN);
/* sd_present holds error values during initialization.  0 means no error and all other errors are non zero value */
FT_SDStatus sd_present; 

/*SD file object for file accessing*/
FT_SDFile jpeg;

/* Global object for transport */
FT801IMPL_SPI FTImpl(FT_CS_PIN,FT_PDN_PIN,FT_INT_PIN);


PROGMEM char *info[] = {  "FT801 Meter Dial Application",
                          "APP to demonstrate interactive Meter dial,",
                          "using Points, Track",
                          "& Stencil"
                       }; 


/* Optimized implementation of sin and cos table - precision is 16 bit */
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
//#endif




#ifdef Relative_Dial
static struct {
  int32_t dragprev;
  int32_t vel;      // velocity
  int32_t base;    // screen x coordinate, in 1/16ths pixel
//  int32_t limit;
}scroller ;

static void scroller_init(uint32_t limit)
{
  scroller.dragprev = 0;
  scroller.vel = 0;      // velocity
  scroller.base = 0;     // screen x coordinate, in 1/16ths pixel
}

static void scroller_run()
{
	  int32_t change;
	  uint32_t sx;
	 sTrackTag sTracktag;

	  FTImpl.Cmd_Track((240)+40,136, DIAL, DIAL, 1);
   
	  FTImpl.GetTrackTag(sTracktag);
		//sx = sTracktag.track;

	  if ((sTracktag.track != 0) && (scroller.dragprev !=0)) 
	  {
		scroller.vel = (scroller.dragprev - sTracktag.track) << 4;
	  } 
	  else
	  {
		change = max(1, abs(scroller.vel) >> 5);
		if (scroller.vel < 0)
		  scroller.vel += change;
		if (scroller.vel > 0)
		  scroller.vel -= change;
	  }
	  scroller.dragprev = sTracktag.track;
	  scroller.base += scroller.vel;
	  scroller.base = max(0, (scroller.base));
 
}

#endif

static void polarxy(uint32_t r, float th, int16_t *x, int16_t *y,uint16_t ox,uint16_t oy)
{
	th = (th+180) * 65536 / 360;
	*x = (16 * ox + (((long)r * SAMAPP_qsin(th)) >> 11));
	*y = (16 * oy - (((long)r * SAMAPP_qcos(th)) >> 11));
}






                             
static int jpeg_index;
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

/* Apis used to upload the image to GRAM from SD card*/
void Load_Jpeg(uint8_t bmphandle, FT_SDFile file)
{
 
    uint8_t imbuff[512];
    uint16_t blocklen; 
    int32_t  fsize = file.Size,dstaddr = FT_RAM_G,dstaddr1=FT_RAM_G+45000;   
 
    while (fsize>0)
    {  
      uint16_t n = min(512, fsize);
      n = (n + 3) & ~3;   // force 32-bit alignment
      file.ReadSector(imbuff); 
      if(bmphandle==0)
      FTImpl.Write(dstaddr,imbuff,n);
      else
      FTImpl.Write(dstaddr1,imbuff,n);
      fsize-=512;
      dstaddr += n;
      dstaddr1+=n;

    }    
}


void Loadimage2ram(uint8_t bmphandle)
{
  static byte image_index = 0,max_files=2;
  FT_SDFile file;
  FT_SDStatus Status;
  
  
    if(bmphandle==0){
    #ifdef Absolute_Dial
    Status=FtSd.OpenFile(file,"AbsDial.raw"); 
    #endif
    #ifdef Relative_Dial
    Status=FtSd.OpenFile(file,"RelDial.raw"); 
    #endif
	}
    else{
    Status=FtSd.OpenFile(file,"strips.raw"); 
	}
    if(Status)
    {
       Serial.println("file n f");; 
    }else
    {  
      Load_Jpeg(bmphandle,file);
      Serial.println(bmphandle,DEC);      
    }  
 
  
  image_index++;
  if(image_index > (max_files-1)) image_index = 0;
  return;
}

#ifdef Absolute_Dial
void Abs_Meter_Dial()
{
	uint8_t i=0,Volume=0,Read_tag,prevtag=0;
	int16_t ScreenWidthMid,ScreenHeightMid,th,theta,X1,Y1,X2,Y2,Touch,NoTouch,Sx1=0,Sy1=0;
	int16_t OuterCircleRadius,PrevVal=0,CircleX,CircleY,ButY,ButWid,NoY,Tx1,Tx2,Tx3,Ty1,Ty2,Ty3 ;

	int32_t PrevTh,CurrTh,adjusting=0,BaseTrackValSign = 0,PrevBaseVal=0,Delta =0;

	uint32_t RotaryTag=0,G1,G2;

	sTagXY sTagxy;
	sTrackTag sTracktag;



	ScreenWidthMid = FT_DISPLAYWIDTH/2;
	ScreenHeightMid = FT_DISPLAYHEIGHT/2;
	
	NoTouch = -32768;
	
	
	Tx1= Tx2=Tx3 =Ty1=Ty2=Ty3=0;

	#if (FT_DISPLAY_RESOLUTION == FT_DISPLAY_WQVGA_480x272)
	OuterCircleRadius = ScreenWidthMid - 144;
	CircleX = ScreenWidthMid;
	CircleY = ScreenHeightMid;
	ButY = FT_DISPLAYHEIGHT -262;
	ButWid = 30;
	NoY = (FT_DISPLAYHEIGHT-262 + 30/2);
	Tx1 = ScreenWidthMid-86;
	Tx2 = ScreenWidthMid-70 + 146/2;
	Ty2 = ScreenHeightMid-67 + 146/2;
	Tx3 = ScreenWidthMid+80;
	#elif (FT_DISPLAY_RESOLUTION == FT_DISPLAY_QVGA_320x240)
	ButY = 1;
	ButWid = 23;
	NoY = 12;
	OuterCircleRadius = ScreenWidthMid - 64;
	CircleX = ScreenWidthMid-70 + 146/2;
	CircleY = ScreenHeightMid-67 + 146/2;
	Tx1 = ScreenWidthMid-63;
	Tx2 = ScreenWidthMid-70 + 146/2;
	Ty2 = ScreenHeightMid-67 + 146/2;
	Tx3 = ScreenWidthMid+72;
	#endif


        FTImpl.WriteCmd( CMD_MEMSET);
	FTImpl.WriteCmd( 0L);//starting address of memset
	FTImpl.WriteCmd( 55L);//value of memset
	FTImpl.WriteCmd(256*1024);

        FTImpl.DLStart();
        FTImpl.ColorRGB(123,0,0);	
	FTImpl.Clear(1,1,1);

        for(uint8_t r = 0;r<2;r++)
        {      
             Loadimage2ram(r);	    
        }
    
       for(uint8_t r = 0;r<2;r++)
        {
           FTImpl.BitmapHandle(r);
           if(r==0)
           {    
             FTImpl.BitmapSource(0);
             FTImpl.BitmapLayout(FT_RGB565, 146*2, 146);        
          }
          else
          {    
             FTImpl.BitmapSource(FT_RAM_G+45000);
             FTImpl.BitmapLayout(FT_L4,146/2,146);
          }
          FTImpl.BitmapSize(FT_BILINEAR,FT_BORDER,FT_BORDER,146,146);  
	}
 
	/* set the vol to max */
	FTImpl.Write( REG_VOL_SOUND,0);
	/* Play carousel*/
	FTImpl.Write16( REG_SOUND, 8);
	

	
	FTImpl.Begin(FT_BITMAPS);	

        FTImpl.ColorRGB(255,255,255);       
        FTImpl.Vertex2ii(0,0,0, 0);	
        
        FTImpl.Vertex2ii(150,0,1, 0);	
	FTImpl.DLEnd();

	FTImpl.Finish();
 

       
	/*asiign track to the black dot*/
	FTImpl.Cmd_Track(CircleX,CircleY, DIAL, DIAL, 1);
	G1 = 0x00000;
	G2 = 0x4D4949;
        FTImpl.Write( REG_PLAY,1);
	do{
                
		FTImpl.GetTagXY(sTagxy);
		Touch = sTagxy.x;
		Read_tag = sTagxy.tag;

		FTImpl.GetTrackTag(sTracktag);
		FTImpl.DLStart();
		FTImpl.Clear(1,1,1);
		
		FTImpl.Cmd_Gradient(0, 0, G1,0, FT_DISPLAYHEIGHT, G2);

		
		/* Assign Stencil Value to get the triangle shape */
		FTImpl.ColorMask(0,0,0,1) ;//only alpha is enabled,so that no color is used for strip
		FTImpl.LineWidth(1*16);
		FTImpl.StencilOp(FT_INCR,FT_DECR) ;
		FTImpl.StencilFunc(FT_EQUAL,0,255) ;
		FTImpl.Begin(FT_EDGE_STRIP_B);		
		FTImpl.ColorRGB(136 ,136 ,136); 


		FTImpl.Vertex2ii(Tx1,FT_DISPLAYHEIGHT,0,0);
		FTImpl.Vertex2ii(Tx2,Ty2,0,0);
		FTImpl.Vertex2ii(Tx3,FT_DISPLAYHEIGHT,0,0);
		
		FTImpl.StencilOp(FT_KEEP,FT_INCR) ;
		FTImpl.StencilFunc(FT_NOTEQUAL,255,255) ;
		FTImpl.ColorMask(1,1,1,1) ;//enable all colors

	
		/* Outer Black circle*/
		/* Based on stencil update Points color,leaving the triangular area*/		
		FTImpl.Begin(FT_POINTS);
		FTImpl.PointSize((OuterCircleRadius) * 16) ;
		FTImpl.ColorRGB(0 ,0 ,0); 
		FTImpl.Vertex2ii(CircleX,CircleY,0,0);
				
		FTImpl.PointSize(((OuterCircleRadius-3) * 16)) ;		
		FTImpl.ColorRGB(15,77,116); 
		FTImpl.Vertex2ii(CircleX,CircleY,0,0);
				
		FTImpl.PointSize(((OuterCircleRadius-5) * 16)) ;		
		FTImpl.ColorRGB(54,115,162); 
		FTImpl.Vertex2ii(CircleX,CircleY,0,0);
		
		FTImpl.PointSize(((OuterCircleRadius-7) * 16)) ;		
		FTImpl.ColorRGB(76,195,225); 
		FTImpl.Vertex2ii(CircleX,CircleY,0,0);
		
		FTImpl.PointSize(((OuterCircleRadius-9) * 16)) ;		
		FTImpl.ColorRGB(89,211,232); 
		FTImpl.Vertex2ii(CircleX,CircleY,0,0);
		
		FTImpl.PointSize(((OuterCircleRadius-11) * 16)) ;		
		FTImpl.ColorRGB(123,248,250); 
		FTImpl.Vertex2ii(CircleX,CircleY,0,0);
		
		FTImpl.PointSize((OuterCircleRadius-17) * 16) ;		
		FTImpl.ColorRGB(89,211,232); 
		FTImpl.Vertex2ii(CircleX,CircleY,0,0);

		FTImpl.PointSize((OuterCircleRadius-18) * 16) ;
		FTImpl.ColorRGB(76,195,225); 
		FTImpl.Vertex2ii(CircleX,CircleY,0,0);
		
		FTImpl.PointSize((OuterCircleRadius-19) * 16) ;
		FTImpl.ColorRGB(54,115,162); 
		FTImpl.Vertex2ii(CircleX,CircleY,0,0);
		
		FTImpl.PointSize((OuterCircleRadius-20) * 16) ;	
		FTImpl.ColorRGB(15,77,116);  
		FTImpl.Vertex2ii(CircleX,CircleY,0,0);
	
		/*Managing the track*/	
		if((Touch != NoTouch))
		{
			if(sTracktag.tag == 1)
			{
				CurrTh = sTracktag.track;
				 /* BaseTrackVal is the rotary track value of the particular white circle that is moving */
				if((adjusting!=0) )
				{					
					BaseTrackValSign += (int16_t)(CurrTh - PrevTh);
				} 
				PrevTh = CurrTh;
				adjusting = sTracktag.tag;	
			
			}
			
		}

		/* alpha bend for strips */
		FTImpl.ColorA(255);
		FTImpl.ColorRGB(0xff,0xff,0xff);
		FTImpl.ColorMask(0,0,0,1);
		FTImpl.Clear(1,0,0);
		FTImpl.Begin(FT_POINTS);
		FTImpl.PointSize((OuterCircleRadius-2) * 16) ;		
		FTImpl.Vertex2ii(CircleX,CircleY,0,0);
		FTImpl.ColorMask(1,1,1,1);

		FTImpl.BlendFunc(FT_DST_ALPHA,FT_ONE_MINUS_DST_ALPHA);

		/* Parameters for Strips */
		FTImpl.LineWidth(2*16);	
		FTImpl.ColorRGB(30 ,30 ,30); 
		Delta = BaseTrackValInit - BaseTrackValSign;

		

		if((prevtag == 0) && (Read_tag == 1)   &&(Touch != -32768) )	
		{			
			PrevBaseVal = BaseTrackValInit;
		}
		if((prevtag == 1) && (Read_tag == 1)   &&(Touch!= -32768) )	
		{
			PrevBaseVal = PrevBaseVal - Delta;			
			MoveDeg = (uint16_t)(PrevBaseVal / 182);				
		}
		
		if(PrevBaseVal > 60294) PrevBaseVal = 60294;
		if(PrevBaseVal <0)PrevBaseVal = 0;
		if(MoveDeg > 330) MoveDeg = 330;
		if(MoveDeg < 30) MoveDeg =31;
		
		if(MoveDeg <= 90)
		{
			/* First Left edge strip */
			FTImpl.Begin(FT_EDGE_STRIP_L);				
			polarxy((OuterCircleRadius-2),MoveDeg,&X1,&Y1,CircleX,CircleY);			
			FTImpl.Vertex2f(CircleX*16,CircleY*16);		
			FTImpl.Vertex2f(X1,Y1);
		}
	
		if((MoveDeg <= 90) ||
				(MoveDeg > 90) && 
					(MoveDeg <= 180))
		{
			/* Second Above edge strip */
			FTImpl.Begin(FT_EDGE_STRIP_A);		
			if(MoveDeg<=90)
			{
				polarxy((OuterCircleRadius-2),90,&X1,&Y1,CircleX,CircleY);			
			}
			else if (MoveDeg>90)
			{
				polarxy((OuterCircleRadius-2),MoveDeg,&X1,&Y1,CircleX,CircleY);			
			}
			FTImpl.Vertex2f(CircleX*16,CircleY*16);		
			FTImpl.Vertex2f(X1,Y1);
		
		}
		if((MoveDeg <= 90) || 
			((MoveDeg > 90) && (MoveDeg <= 180))||
					((MoveDeg > 180) && (MoveDeg <= 270)))
		{
			/* Third Above edge strip */
			FTImpl.Begin(FT_EDGE_STRIP_R);		
			if(MoveDeg<=180)
			{
				polarxy((OuterCircleRadius-2),180,&X1,&Y1,CircleX,CircleY);			
			}
			else if (MoveDeg>180)
			{
				polarxy((OuterCircleRadius-2),MoveDeg,&X1,&Y1,CircleX,CircleY);			
			}	
			FTImpl.Vertex2f(X1,Y1);
			FTImpl.Vertex2f(CircleX*16,CircleY*16);					
		}
		
		if((MoveDeg <= 90) ||
				((MoveDeg > 90)&& (MoveDeg <= 180))||
					((MoveDeg > 180)&& (MoveDeg <= 270))||
						((MoveDeg > 270)&& (MoveDeg <= 330)))
		{
			/* Fourth Above edge strip */
			FTImpl.Begin(FT_EDGE_STRIP_B);		
			if(MoveDeg<=270)
			{
				polarxy((OuterCircleRadius-1),270,&X1,&Y1,CircleX,CircleY);		
			}
			else if (MoveDeg>270)
			{
				polarxy((OuterCircleRadius-1),MoveDeg,&X1,&Y1,CircleX,CircleY);								
			}
			FTImpl.Vertex2f((CircleX)*16,CircleY*16);		
			FTImpl.Vertex2f(X1,Y1);
		}		
		
			
		FTImpl.BlendFunc(FT_SRC_ALPHA,FT_ONE_MINUS_SRC_ALPHA);

		FTImpl.StencilOp(FT_KEEP,FT_KEEP);
		FTImpl.StencilFunc(FT_ALWAYS,0,255);

		/*alpha blend for rgb bitmap*/
		FTImpl.ColorA(0xff);
		FTImpl.ColorRGB(0xff,0xff,0xff);
		FTImpl.ColorMask(0,0,0,1);
		FTImpl.Clear(1,0,0);
		FTImpl.Begin(FT_POINTS);		
		FTImpl.PointSize((146/2) * 16) ;	
		FTImpl.Vertex2ii(CircleX,CircleY,0,0);
		FTImpl.ColorMask(1,1,1,1);

		FTImpl.BlendFunc(FT_DST_ALPHA,FT_ONE_MINUS_DST_ALPHA);

		/* Meter Dial bitmap placement */
		FTImpl.Begin(FT_BITMAPS);
		FTImpl.Vertex2ii(CircleX - 146/2,CircleY - 146/2,0,0);
		FTImpl.BlendFunc(FT_SRC_ALPHA,FT_ONE_MINUS_SRC_ALPHA);
		FTImpl.End();

		FTImpl.Begin(FT_BITMAPS);
		FTImpl.ColorA(20);
		FTImpl.Tag(1);
		
		/* Alpha Meter Dial bitmap placement & rotation */
		FTImpl.SaveContext();
		FTImpl.Cmd_LoadIdentity();
		FTImpl.Cmd_Translate(F16(73),F16(73));
		FTImpl.Cmd_Rotate(MoveDeg*65536/360);
		FTImpl.Cmd_Translate(F16(-(73)),F16(-(73)));		  
		FTImpl.Cmd_SetMatrix();	
		FTImpl.Vertex2ii(CircleX - 146/2,CircleY - 146/2,1,0);		
		FTImpl.RestoreContext();

		/* restore default alpha value */
		FTImpl.ColorA(255);		

		FTImpl.Cmd_FGColor( 0x59D3E8);
		FTImpl.Cmd_GradColor( 0x7BF8FA);

		FTImpl.Cmd_Button( (ScreenWidthMid - 40), ButY, 80, ButWid, 28, 0, "");// placement of button

		FTImpl.Begin(FT_BITMAPS);
	
		FTImpl.ColorRGB(0,0,0);//color of number(text) color
	
	
		
		if((MoveDeg >= 30)&& (MoveDeg <= 330))
		{			
			Volume = PrevBaseVal>>8;						
			FTImpl.Begin(FT_LINES);
			FTImpl.LineWidth(1*16);
			FTImpl.ColorRGB(30 ,30 ,30); 
			polarxy((OuterCircleRadius-3),MoveDeg,&X1,&Y1,CircleX,CircleY);	
			polarxy(74,MoveDeg,&X2,&Y2,CircleX,CircleY);	
			FTImpl.Vertex2f(X1,Y1);
			FTImpl.Vertex2f(X2,Y2);						
		}

		FTImpl.Cmd_Number((ScreenWidthMid),NoY,28,FT_OPT_CENTER,Volume);
		FTImpl.Write( REG_VOL_SOUND,Volume);
		FTImpl.DLEnd();

		FTImpl.Finish();
		prevtag = Read_tag;		
		BaseTrackValInit = BaseTrackValSign;
		delay(30)
		}while(1);		


}
#endif

#ifdef Relative_Dial
void Relative_Meter_Dial()
{
	uint8_t i=0,j=0,k=0,Read_tag,prevtag=0,currtag;
	int16_t ScreenWidthMid,ScreenHeightMid,NextTh=0,Touch,drag=0;
	int16_t OuterCircleRadius,PrevValDeg=0,CircleX,CircleY,ButY,ButWid,NoY,Delta,G1,G2,Sx1,Sy1,X2=0,Y2=0;

	int32_t PrevTh=0,CurrTh,adjusting=0,BaseTrackValSign = 0,Vx1=0,Vy1=0,Vx2=0,Vy2=0,Volume=0,PrevBaseTrackVal=0;

	uint32_t RotaryTag=0;	

	int32_t change;
	int32_t sx,storesx,storepresx;	
	sTagXY sTagxy;
	sTrackTag sTracktag;
	

	ScreenWidthMid = FT_DISPLAYWIDTH/2;
	ScreenHeightMid = FT_DISPLAYHEIGHT/2;
	
	PrevValDeg =0;
	Sx1 = Sy1 = 180;
	

#if (FT_DISPLAY_RESOLUTION == FT_DISPLAY_WQVGA_480x272)
	Sx1 = Sy1 = 240;
	OuterCircleRadius = ScreenWidthMid - 144;
	CircleX = ScreenWidthMid;
	CircleY = ScreenHeightMid;
	ButY = FT_DISPLAYHEIGHT -262;
	ButWid = 30;
	NoY = (FT_DISPLAYHEIGHT-262 + 30/2);
#elif (FT_DISPLAY_RESOLUTION == FT_DISPLAY_QVGA_320x240)
	Sx1 = Sy1 = 130;
	ButY = 10;
	ButWid = 23;
	NoY = 12;
	OuterCircleRadius = ScreenWidthMid - 64;
	CircleX = ScreenWidthMid-70 + 146/2;
	CircleY = ScreenHeightMid-67 + 146/2;
#endif

	

	FTImpl.WriteCmd( CMD_MEMSET);
	FTImpl.WriteCmd( 0L);//starting address of memset
	FTImpl.WriteCmd( 55L);//value of memset
	FTImpl.WriteCmd(256*1024);

	

	/* set the vol to max */
	FTImpl.Write( REG_VOL_SOUND,0);
	/* Play carousel*/
	FTImpl.Write16( REG_SOUND, 8);
	
	/* Construction of starting screen shot, assign all the bitmap handles here */
	FTImpl.DLStart();
	FTImpl.ColorRGB(255,255,255);
	
	FTImpl.Clear(1,1,1);

        for(uint8_t r = 0;r<2;r++)
        {      
             Loadimage2ram(r);	    
        }
    
        for(uint8_t r = 0;r<2;r++)
        {
           FTImpl.BitmapHandle(r);
           if(r==0)
           {    
             FTImpl.BitmapSource(0);
             FTImpl.BitmapLayout(FT_RGB565, 146*2, 146);        
          }
          else
          {    
             FTImpl.BitmapSource(FT_RAM_G+45000);
             FTImpl.BitmapLayout(FT_L4,146/2,146);
          }
          FTImpl.BitmapSize(FT_BILINEAR,FT_BORDER,FT_BORDER,146,146);  
	}


	FTImpl.Begin(FT_BITMAPS);	

        FTImpl.ColorRGB(255,255,255);       
        
	FTImpl.DLEnd();
	FTImpl.Finish();


	/*asiign track to the bitmap*/
	FTImpl.Cmd_Track((CircleX)+40,CircleY, DIAL, DIAL, 1);
	FTImpl.Write( REG_PLAY,1);


	do{
		FTImpl.Tag(255);
		FTImpl.GetTagXY(sTagxy);
		Touch = sTagxy.x;
		Read_tag = sTagxy.tag;
		
		FTImpl.GetTrackTag(sTracktag);
		FTImpl.DLStart();
		FTImpl.Clear(1,1,1);
		FTImpl.ColorRGB(0,0,0);
		
		FTImpl.ScissorXY(0,0) ;
		
#if (FT_DISPLAY_RESOLUTION == FT_DISPLAY_WQVGA_480x272)
		FTImpl.Cmd_Gradient(0, FT_DISPLAYHEIGHT, 0x1E1E1E,ScreenWidthMid-60/*180*/, ScreenHeightMid-21/*115*/, 0x999999);
		
		FTImpl.ColorRGB(0,0,0);
		FTImpl.Begin(FT_LINES);
		
		FTImpl.Vertex2f((FT_DISPLAYWIDTH-2)*16,(FT_DISPLAYHEIGHT)*16 );
		FTImpl.Vertex2f(ScreenWidthMid*16,0 *16);

#elif (FT_DISPLAY_RESOLUTION ==  FT_DISPLAY_QVGA_320x240)

                FTImpl.Cmd_Gradient(0, 0, 0x1E1E1E,FT_DISPLAYWIDTH, FT_DISPLAYHEIGHT, 0x999999);
                
#endif
	

   
		sx = 0;	 
		currtag = sTracktag.tag;
		if(1 == currtag)
		{
		  sx = sTracktag.track;
		}
	   

		if((prevtag == 1) && (1 == currtag))
		{
			scroller.vel = (-scroller.dragprev + sx);
	
			if((scroller.vel > 32000))
			{
				scroller.vel-= 65536;
			}

			else if (scroller.vel < -32768)
			{
				scroller.vel+= 65536;
			}
			scroller.vel <<= 4;
		
			storepresx = scroller.dragprev;
			storesx = sx;			
	  } 
	  else
	  {
		change = max(1, abs(scroller.vel) >> 5);
		if (scroller.vel < 0)
		  scroller.vel += change;

		if (scroller.vel > 0)
		  scroller.vel -= change;
		
	  }
	  prevtag = currtag;
	  scroller.dragprev = sx;
	  scroller.base += scroller.vel;
	 
	
	  drag = scroller.base>>4;

		
		/*Managing the track*/	
		{
			if((1 == currtag))
			{
				CurrTh = sx;				

				if((adjusting!=0) )
				{
					
					PrevBaseTrackVal = (CurrTh - PrevTh);
					
					if((PrevBaseTrackVal > 32000))
					{
						PrevBaseTrackVal-= 65536;
					}
					else if (PrevBaseTrackVal < -32768)
					{
						PrevBaseTrackVal+= 65536;
					}
					BaseTrackValSign += PrevBaseTrackVal;
					BaseTrackVal += PrevBaseTrackVal;
					
				}
				
				adjusting = sTracktag.tag;
				PrevTh = CurrTh;
				NextTh = (uint16_t)(BaseTrackValSign / 182);			
				PrevValDeg = NextTh;
				
			}

			else if((0 != scroller.vel))
			{

				BaseTrackValSign += (scroller.vel>>4); //original copy

				NextTh = (uint16_t)(BaseTrackValSign / 182);			
				PrevValDeg = NextTh;
				adjusting = 0;
			}
			else
			{
				adjusting = 0;
			}
			
		}

		/*alpha blend for rgb bitmap*/
		
		FTImpl.ColorA(0xff);
		FTImpl.ColorRGB(0xff,0xff,0xff);
		FTImpl.ColorMask(0,0,0,1);
		FTImpl.Clear(1,0,0);
		FTImpl.Begin(FT_POINTS);		
		FTImpl.PointSize((146/2) * 16) ;	
		FTImpl.Vertex2ii(CircleX +40,CircleY,0,0);
		FTImpl.ColorMask(1,1,1,1);

		FTImpl.BlendFunc(FT_DST_ALPHA,FT_ONE_MINUS_DST_ALPHA);

		/* Meter Dial bitmap placement */
		FTImpl.Begin(FT_BITMAPS);
		

		FTImpl.Vertex2ii((CircleX - 146/2)+40,CircleY - 146/2,0,0);

		FTImpl.BlendFunc(FT_SRC_ALPHA,FT_ONE_MINUS_SRC_ALPHA);
			

		Delta = BaseTrackValSign - BaseTrackValInit;

	
		FTImpl.ColorA(20);
				
		/* Alpha Meter Dial bitmap placement & rotation */
		FTImpl.SaveContext();
		FTImpl.TagMask(1);
		
		FTImpl.Cmd_LoadIdentity();
		FTImpl.Cmd_Translate(F16(73),F16(73));
		FTImpl.Cmd_Rotate(PrevValDeg*65536/360);
		FTImpl.Cmd_Translate(F16(-(73)),F16(-(73)));		  
		FTImpl.Cmd_SetMatrix();	
		FTImpl.Tag(1);
		FTImpl.Vertex2ii((CircleX - 146/2)+40,CircleY - 146/2,1,0);			
		FTImpl.TagMask(0);

		FTImpl.RestoreContext();
	
		/* Draw points around the bitmap*/
		FTImpl.ColorA(255);	
		FTImpl.Begin(FT_POINTS);		
		FTImpl.PointSize(5*16);
		FTImpl.ColorRGB(227 ,249 ,20); 
		polarxy(55,PrevValDeg,&X2,&Y2,CircleX+40,CircleY);

		FTImpl.Vertex2f(X2,Y2);		

		
		for(k=0;k<=11;k++)
		{
			Vy1 = 200;			
			if(k==0)/* First button Vertices*/
			{
				Vx1 = 10;Vy1 = 200;
			}
			else if((k== 1)||(k==2)||(k==3))/* Second,third,fourth button Vertices*/
			{
				Vx1 = 10;Vy1 = Vy1-(k)*60;
			}
			else if(k==4)/* fifth button Vertices*/
			{
				Vx1 = 40;Vy1 = 200;
			}
			else if((k== 5)|(k==6)|(k==7))/* sixth,seventh,eighth button Vertices*/
			{
				Vx1 =40;Vy1 = Vy1-(k-4)*60 ;
			}
			else if(k==8)/* ninth button Vertices*/
			{
				Vx1 = 70;Vy1 = 200;
			}
			else if((k== 9)|(k==10)|(k==11))/* tenth,eleven,twelth button Vertices*/
			{
				Vx1 =70;Vy1 = Vy1-(k-8)*60 ;
			}
			

			if(BaseTrackVal < 11*65535)
			{
				/* Rotary tag when within range of particular index, highlight only the indexed button in yellow*/
				if(((BaseTrackVal >= k*65535)&&(BaseTrackVal < (k+1)*65535)))//|| ((BaseTrackValSign<0)&&(k==0)))
				{
					FTImpl.Cmd_FGColor(0xE3F914);//yellow
					FTImpl.Cmd_Button(Vx1,Vy1,20,30,16,0,"");	
				}
				else/* Color the non indexed buttons in grey color*/
				{
					FTImpl.Cmd_FGColor(0x787878);//grey
					FTImpl.Cmd_Button(Vx1,Vy1,20,30,16,0,"");
				}
			}
			
			/*Limit track value when its greater than 12th button(11th index) and also highlight twelth button*/
			else if((BaseTrackVal >= 11*65535))
			{
				BaseTrackVal = 11*65535;
				
				if(k<11)
				{
					FTImpl.Cmd_FGColor(0x787878);//grey
				}
				else	
				{
					FTImpl.Cmd_FGColor(0xE3F914);//yellow					
				}
				FTImpl.Cmd_Button(Vx1,Vy1,20,30,16,0,"");
			}

			/*Limit track value when its negative and also highlight first button*/
			if((BaseTrackVal<0)&&(k==0))
			{
				if((Delta>0))
				BaseTrackVal =0;
				FTImpl.Cmd_FGColor(0xE3F914);//yellow	
				FTImpl.Cmd_Button(Vx1,Vy1,20,30,16,0,"");
			}
	
		}
		
		/* play volume*/		

		Volume = BaseTrackValSign>>8;
		if(Volume >255)	Volume = 255; /* Maintain max volume after it has reached maximum*/
		if(Volume <0)Volume = 0;/* Maintain min volume*/
		FTImpl.Write( REG_VOL_SOUND,Volume);
				
		
		FTImpl.DLEnd();
		FTImpl.Finish();
		BaseTrackValInit = BaseTrackValSign;
		prevtag = Read_tag;
		delay(30);
	}while(1);
			


}
#endif





/* Api to bootup ft800, verify FT800 hardware and configure display/audio pins */
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
	    Info();

	 #if defined(Absolute_Dial)
      	      Abs_Meter_Dial();
      	  #elif defined(Relative_Dial)
      	      Relative_Meter_Dial();
      	  #endif
		FTImpl.Exit();  /*  Safely close all opened handles */
	}
	Serial.println("--End Application--");

}

void loop()
{
}



/* Nothing beyond this */




