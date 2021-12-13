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
* @file                           FT_App_WashingMachine.ino
* @brief                          Sample application to demonstrate FT801 primitives and touch screen. 
* @version                        0.1.0
* @date                           2014/03/21
*
*/


#include "SPI.h"
#include "Wire.h"
#include "FT_NHD_43CTP_SHIELD.h"






#define F16(s)        ((int32_t)((s) * 65536))
#define SQ(v) (v*v)




/* SD card object*/
FT_SD FtSd(FT_SD_CSPIN);

/* sd_present holds error values during initialization.  0 means no error and all other errors are non zero value */
FT_SDStatus sd_present; 

/*SD file object for file accessing*/
FT_SDFile jpeg;

/* Global object for transport */
FT801IMPL_SPI FTImpl(FT_CS_PIN,FT_PDN_PIN,FT_INT_PIN);


char *info[] = { 
					  "FT801 Washing Machine Demo Application",
          			  "APP to demonstrate interactive Menu,",
					  "and animation,",	 
					  "using Bitmaps",	 
					 };









static uint8_t key_pressed = 0;

uint8_t Read_Keys()
{
	static uint8_t Read_tag=0,temp_tag=0,ret_tag=0;	
	ret_tag = NULL;		
	Read_tag = FTImpl.Read(REG_TOUCH_TAG);						
	if(Read_tag!=NULL)											// Allow if the Key is released
	{
		if(temp_tag!=Read_tag)
		{
			temp_tag = Read_tag;	
			key_pressed = Read_tag;										// Load the Read tag to temp variable	
		}  
	}
	else
	{
		key_pressed = 0;
		if(temp_tag!=0)
		{
			ret_tag = temp_tag;
			temp_tag = 0;
		}  
	}
	return ret_tag;
}



       
void Info()
{
		  uint16_t dloffset = 0,z=0;
		  FTImpl.DLStart(); 
		  FTImpl.Clear(1,1,1);
		  FTImpl.ColorRGB(255,255,255);
		  FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,FT_DISPLAYHEIGHT/2,29,FT_OPT_CENTERX|FT_OPT_CENTERY,"Please tap on a dot");
		  FTImpl.Cmd_Calibrate(0);
		  FTImpl.DLEnd();
		  FTImpl.Finish();
  
  FTImpl.Cmd_Logo();
  while(FTImpl.CheckLogo() != FT_GE_FINISHED);
  dloffset = FTImpl.Read16(REG_CMD_DL);
  dloffset -=4;
  FTImpl.WriteCmd(CMD_MEMCPY);
  FTImpl.WriteCmd(200*1024L);
  FTImpl.WriteCmd(FT_RAM_DL);
  FTImpl.WriteCmd(dloffset);
  do
  {
    FTImpl.DLStart();   
    FTImpl.Cmd_Append(200*1024L,dloffset);
    FTImpl.BitmapTransformA(256);
    FTImpl.BitmapTransformB(0);
    FTImpl.BitmapTransformC(0);
    FTImpl.BitmapTransformD(0);
    FTImpl.BitmapTransformE(256);
    FTImpl.BitmapTransformF(0);  
    FTImpl.SaveContext();	
    FTImpl.ColorRGB(219,180,150);
    FTImpl.ColorA(220);
    FTImpl.Begin(FT_EDGE_STRIP_A);
    FTImpl.Vertex2f(0,FT_DISPLAYHEIGHT*16);
    FTImpl.Vertex2f(FT_DISPLAYWIDTH*16,FT_DISPLAYHEIGHT*16);
    FTImpl.ColorA(255);
    FTImpl.RestoreContext();	
    FTImpl.ColorRGB(0,0,0);
   // INFORMATION 
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,20,28,FT_OPT_CENTERX|FT_OPT_CENTERY,info[0]);
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,60,26,FT_OPT_CENTERX|FT_OPT_CENTERY,info[1]);
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,90,26,FT_OPT_CENTERX|FT_OPT_CENTERY,info[2]);  
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,120,26,FT_OPT_CENTERX|FT_OPT_CENTERY,info[3]);  
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,FT_DISPLAYHEIGHT-30,26,FT_OPT_CENTERX|FT_OPT_CENTERY,"Click to play");
    if(key_pressed!='P')
    FTImpl.ColorRGB(255,255,255);
    else
    FTImpl.ColorRGB(100,100,100);


	
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

		  FTImpl.DLStart();   
		  FTImpl.ColorRGB(255,255,255);
		  FTImpl.Clear(1,1,1);   
		  FTImpl.ColorRGB(0,0,0);
		  FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2-80,FT_DISPLAYHEIGHT/2,29,FT_OPT_CENTERX|FT_OPT_CENTERY,"Data Loading");
		  FTImpl.Cmd_Spinner(FT_DISPLAYWIDTH/2+40,FT_DISPLAYHEIGHT/2,1,0);
		  FTImpl.DLEnd();
		  FTImpl.Finish();
		  FTImpl.PlaySound(255,0x50);
}

static struct {
  signed short dragprev;
  int vel;      // velocity
  long base;    // screen x coordinate, in 1/16ths pixel
  long limit;
} scroller;

static void scroller_init(uint32_t limit)
{
  scroller.dragprev = -32768;
  scroller.vel = 0;      // velocity
  scroller.base = 0;     // screen x coordinate, in 1/16ths pixel
  scroller.limit = limit;
}
static void scroller_run()
{
	sTagXY sTagxy;
	FTImpl.GetTagXY(sTagxy);
  signed short sy = sTagxy.y;
  
  if ((sy != -32768) & (scroller.dragprev != -32768)) {
    scroller.vel = (scroller.dragprev - sy) << 4;
  } else {
    int change = max(1, abs(scroller.vel) >> 5);
    if (scroller.vel < 0)
      scroller.vel += change;
    if (scroller.vel > 0)
      scroller.vel -= change;
  }
  scroller.dragprev = sy;
  
  scroller.base += scroller.vel;
  
  scroller.base = max(0, min(scroller.base, scroller.limit));
}

 typedef struct logoim
{
  prog_char name[14];
  uint16_t image_height;
  uint8_t image_format;
  uint8_t filter;
  uint16_t sizex;
  uint16_t sizey;
  uint16_t linestride;
  uint16_t gram_address;
}t_imageprp;

PROGMEM t_imageprp Main_Icons[10] = 
{
                                        
                                        {"shirt.bin",		40,    FT_ARGB4,  FT_NEAREST,    40,    40 ,    80,   0L      }, 
                                		{"Temp.bin",		40,    FT_ARGB4,  FT_NEAREST,    40,    40 ,    80,   3L      },  
                                		{"SpinW.bin",          40,    FT_ARGB4,  FT_NEAREST,    40,    40 ,    80,   7L      },  
                                		{"Butfly.bin",	        20,    FT_ARGB4,  FT_NEAREST,    20,    20 ,    40,   11L     },  
                                	//	{"DelSt.bin",		30,    FT_ARGB4,  BILINEAR,    30,    30 ,    60,  12L     },  
                                		{"Lock.bin",		30,    FT_ARGB4,  FT_NEAREST,    60,    60 ,    60,   12L     },  
                                		{"UnLock.bin",		30,    FT_ARGB4,  FT_NEAREST,    30,    60 ,    60,   14L     },  
                                		{"Set.bin",		30,    FT_ARGB4,  FT_NEAREST,    30,    30 ,    60,   16L     },  
                                		{"Start.bin",		30,    FT_ARGB4,  FT_NEAREST,    30,    30 ,    60,   18L     },  
                                		{"home.bin",		40,    FT_ARGB4,  FT_NEAREST,    40,    40 ,    80,   20L     },//4kb
                                		{"Bkgd.bin",            136,   FT_L8,     FT_NEAREST,    480,   272 ,   240,  24L     },

},

Wash_Window[7] = 
{
	                                        {"Wash.bin",		55,	FT_ARGB4,	FT_NEAREST,   50,	  50 ,	  100,	57L	}, // size 22KB
                                        	{"Rinse.bin",		54,	FT_ARGB4,	FT_NEAREST,   54,	  54 ,	  108,	79L	}, // size 24KB
                                        	{"Spin.bin",		54,	FT_ARGB4,	FT_NEAREST,   45,	  45 ,	  90,	103L	}, // size 30KB
                                        	{"WBar.bin",		52,	FT_RGB565,	FT_NEAREST,   400,	  400 ,	  800,	133L	}, //size 42KB
                                        	{"bub.bin",		30,	FT_ARGB4,	FT_NEAREST,   30,	  30 ,	  60,	175L	},//2kb
                                        	{"home.bin",		40,	FT_ARGB4,	FT_NEAREST,   40,	  40 ,	  80,	177L	},//4kb
                                        	{"Bkgd.bin",		136,    FT_L8,	FT_NEAREST,   480,	  272 ,	  240,	181L	},   //33kb
};

	uint8_t Stflag,OptionsSet,MainFlag,Tagval,Pf=0;
	uint16_t BrightNess,SoundLev=255,TimeSet,IterCount,Px;
	static uint16_t ButX=330;

void load_inflate_image(uint8_t address,char* name)
{
    uint8_t ibuff[512];    
    FT_SDFile imfile;
    
    FTImpl.WriteCmd(CMD_INFLATE);
    FTImpl.WriteCmd(address*1024L);
	if(FtSd.OpenFile(imfile,name)){
		//unable to open file
	}
	else{
		while (imfile.Offset < imfile.Size)
		{	
			uint16_t n = min(512, imfile.Size - imfile.Offset);
			n = (n + 3) & ~3;   // force 32-bit alignment
			imfile.ReadSector(ibuff);
			FTImpl.WriteCmd(ibuff, n);//alignment is already taken care by this api
		}
	}
}



void Logo_Intial_setup(PROGMEM struct logoim sptr[],uint8_t num )
{
   char buffer1[10]; 
   for(uint8_t z=0;z<num;z++)
   {
       uint16_t pc = pgm_read_word(&sptr[z].gram_address);
 
       prog_char *pc1 = sptr[z].name;
                      while (pgm_read_byte(pc1)) {
                      strcpy_P(buffer1, sptr[z].name);
                      pc1 += (strlen_P(pc1) + 1);
                      }
       load_inflate_image(pc,buffer1);
   }     
   FTImpl.DLStart();        // start
   FTImpl.Clear(1,1,1);

   for(uint8_t z=0;z<num;z++)
   {
	
           FTImpl.BitmapHandle(z);	
           
           uint16_t pc = pgm_read_word(&sptr[z].gram_address);     
         
             
           FTImpl.BitmapSource(pc*1024L);    
         
           uint8_t pc2 = pgm_read_byte(&sptr[z].image_format);
           uint16_t pc3 = pgm_read_word(&sptr[z].linestride);                      
           uint16_t pc4 = pgm_read_word(&sptr[z].image_height);

     
   
           FTImpl.BitmapLayout(pc2,pc3,pc4);      
          
           pc2 = pgm_read_byte(&sptr[z].filter);
           pc3 =pgm_read_word(&sptr[z].sizex);                      
           pc4 = pgm_read_word(&sptr[z].sizey);
         
     
           FTImpl.BitmapSize(pc2,FT_BORDER,FT_BORDER,pc3,pc4); 
           
      
   }
   
   FTImpl.DLEnd();
   FTImpl.Finish();    

}

static void rotate_around(int16_t x, int16_t y, int16_t a)
{
  FTImpl.Cmd_LoadIdentity();
  FTImpl.Cmd_Translate(65536L*(x),65536L*(y));
  FTImpl.Cmd_Rotate(a);
  FTImpl.Cmd_Translate(65536L*(-x),65536L*(-y));
  FTImpl.Cmd_SetMatrix();
}


PROGMEM prog_char Array_Cycle_Name[9][12] = {"\0","Eco Cold", "Normal", "Heavy","Perm",  "Wool", "Bedding", "ActiveWear"};
PROGMEM prog_char Array_Cycle_Options[3][12] = {"Soil Level", "Temperature", "Spin Speed"};
PROGMEM prog_char Array_Menu_Options[3][12] = { "Child Lock", "Settings", "Start"};



PROGMEM prog_char BrightLevel[6][10] = {"\0","Level1", "Level2", "Level3", "Level4", "Level5"};

PROGMEM prog_char SoundLevel[6][10] = {"\0","Mute", "Level1", "Level2", "Level3", "Level4"};

typedef struct Item_Prop
{
		prog_char SoilLev[8];
		prog_char Temp[8];
		prog_char SpinSpeed[8];	
}Item_Properties;

Item_Properties Item_Property[] PROGMEM= 
{
	{"Light", "Cold", "Low"},//Eco cold
	{"Normal", "Warm", "Medium"}, //Normal
	{"Heavy", "Hot", "High"}, // HEavy Duty
	{"Light", "Warm", "Medium"},//Perm Press
	{"Normal", "Hot", "Medium"},//Active Wear
	{"Heavy", "Warm", "High"},//Bedding
	{"Heavy", "Cold", "Low"},//Wool
};

typedef struct Bub
{
	int16_t xOffset;
	int16_t yOffset;
	char xDiff;
	char yDiff;
}Bubbles;

static uint8_t ProcFinish;
uint16_t WashWindow()
{
		uint16_t ItCt=0,BitmapTr=0,BubY=0,i=0,th=0,PwY=0,PwY1=0,PwY2=0;
		int16_t RotX=0,RotY=0;
		
		//SYSTEMTIME st;
		static uint8_t ProcessFlag;
		uint8_t Read_tag=0,Sdflag=0,Sd1=0,Sd2=0,SoundFlag =0;

	Bubbles W_Bubble[40];
				
	Logo_Intial_setup(Wash_Window,7);

	if(MainFlag == 1)
	{
		TimeSet = 36;
		Px=44;
		//MainFlag =0;
	}
	BubY = 110;

	for(i=0;i<40;i++)
	{
		W_Bubble[i].xOffset = random(FT_DISPLAYWIDTH);
		W_Bubble[i].yOffset = random(FT_DISPLAYHEIGHT);

		W_Bubble[i].xDiff = random(16) ;
		W_Bubble[i].yDiff = random(8) ;
	}

	
		
		 do
		{
			 
				FTImpl.DLStart();        // start
				FTImpl.ColorRGB(0,0,255);
				FTImpl.Clear(1,1,1);

		Read_tag = Read_Keys();

		/* Draw all the background bitmaps - balls with various resolutions */
		if(ProcessFlag == 2)
		{
			FTImpl.Begin(FT_BITMAPS);
			FTImpl.ColorRGB(255 ,255 ,255); // brown outside circle
                        if(Pf ==0)
			{
        			for(i = 0;i<25;i++)
        			{
          			        FTImpl.Vertex2ii(W_Bubble[i].xOffset,W_Bubble[i].yOffset,4,0);
        				W_Bubble[i].xOffset += W_Bubble[i].xDiff;
        				W_Bubble[i].yOffset += W_Bubble[i].yDiff;
        
        				if(W_Bubble[i].xOffset < -80)
        				{
        					W_Bubble[i].xOffset = FT_DISPLAYWIDTH + 80;
        				}
        				else if(W_Bubble[i].xOffset > FT_DISPLAYWIDTH + 80)
        				{
        					W_Bubble[i].xOffset = -80;
        				}
        				if(W_Bubble[i].yOffset < -80)
        				{
        					W_Bubble[i].yOffset = FT_DISPLAYHEIGHT + 80;
        				}
        				else if(W_Bubble[i].yOffset > FT_DISPLAYHEIGHT + 80)
        				{
        					W_Bubble[i].yOffset = -80;
        				}
      			      }
                        }
			W_Bubble[random(40)].xDiff = random(16) - 6;
			W_Bubble[random(40)].yDiff = random(8) - 4;
		}
		// Progress bar Image
		FTImpl.Begin(FT_BITMAPS);			
		FTImpl.ColorRGB(255,255,255);

		FTImpl.ColorA(180);
		FTImpl.BitmapTransformA(128);			
		FTImpl.BitmapTransformE(128);	
		FTImpl.Vertex2ii(0,0,6,0);// backgnd tag -13

		FTImpl.RestoreContext();

		FTImpl.Vertex2ii(40,130,3,0);

                // Pause Button

		FTImpl.TagMask(1);
		FTImpl.Tag('P');
 		if(ItCt % 100 >0 && ItCt % 100 <50 && Pf == 1 && ProcessFlag<5)
			FTImpl.Cmd_Text(205,190,31,0,"I I"); 				
		else if(Pf == 0 && ProcessFlag<5)
			FTImpl.Cmd_Text(205,190,31,0,"I I"); 				
		FTImpl.TagMask(0);

		if(Read_tag == 'P') 
		{
			if(Pf == 1)Pf = 0;
			else if(Pf ==0)Pf =1;
		}

		//Progress bar cover up
		if(Px <435)
                {
                        //Px = 435;
        		FTImpl.Begin(FT_RECTS);		
        		FTImpl.ColorRGB(129,178,218);
        		FTImpl.LineWidth(5*16);
        		FTImpl.Vertex2f(Px*16,134*16);
        		FTImpl.Vertex2f(435*16,178*16);
                }

		FTImpl.Begin(FT_POINTS);		
		FTImpl.ColorRGB(255,255,255);
		FTImpl.ColorA(100);
		FTImpl.PointSize(45*16);
		FTImpl.Vertex2f(225*16,85*16);

		// Wash image
		FTImpl.ColorA(255);
		FTImpl.ColorRGB(255,255,255);

                if(Pf == 0)
		{
        		if(TimeSet > 30)
        		{
        			ProcessFlag = 1;
        			FTImpl.Cmd_Text(40,190,23,0,"Pre Wash"); 
					ProcFinish =1;
        		}
        		else if(TimeSet > 20 && TimeSet <= 30)
        		{
        			FTImpl.Write(REG_VOL_PB,0);
        			if(Sdflag == 0)
        			{
        				 FTImpl.PlaySound(SoundLev,0x56);
        				 Sdflag = 1;
        			}
        			//FTImpl.Write(REG_PLAYBACK_PLAY,0);
        			ProcessFlag = 2;
        			FTImpl.Cmd_Text(40,190,23,0,"Washing"); 
					ProcFinish =1;
        		}
        		else if(TimeSet <= 20 && TimeSet >10) 
        		{
        			if(Sd1 == 0)
        			{
        				 FTImpl.PlaySound(SoundLev,0x56);
        				 Sd1 = 1;
        			}
        			ProcessFlag = 3;
        			FTImpl.Cmd_Text(40,190,23,0,"Rinsing"); 
					ProcFinish =1;
        		}
        		else if(TimeSet <= 10 && TimeSet >0) 
        		{
        			if(Sd2 == 0)
        			{
        				 FTImpl.PlaySound(SoundLev,0x56);
        				 Sd2 = 1;
        			}
        			ProcessFlag = 4;
        			FTImpl.Cmd_Text(40,190,23,0,"Spinning"); 
					ProcFinish =1;
        		}
        		else if(TimeSet == 0) 
        		{
        			ProcessFlag = 5;
        			FTImpl.ColorRGB(255,0,0);
        			if(ItCt %20 >0 && ItCt %20 < 10)
        			FTImpl.Cmd_Text(197,73,23,0,"DONE"); 
        			MainFlag = 0;
					ProcFinish =2;
					ButX = 330;
					Stflag = 0;
        		}
        		{
        			if(ItCt%25 == 0)
        				BitmapTr = 0;
        			else	if(ItCt%40 == 0)
        				BitmapTr = 1;
        			else	if(ItCt%60 == 0)
        				BitmapTr = 2;
        			else	if(ItCt%90 == 0)
        				BitmapTr = 3;
        		}
                }
		// Process Images
		FTImpl.Begin(FT_BITMAPS);			
		FTImpl.ColorRGB(255,255,255);
		// Pre Wash Image
		if(ProcessFlag == 1)// Images loading during pre wash stage
		{
			FTImpl.Vertex2ii(200,PwY,1,0);
			if(PwY == 61)
			{
				FTImpl.Vertex2ii(200,PwY1,0,0);
			}
			if(PwY1 == 61)
			{
				FTImpl.Vertex2ii(200,PwY2,2,0);
			}
		}
		else	if(ProcessFlag < 5)
		{
			if(ProcessFlag == 2 || ProcessFlag == 3)
			{
				RotX = 27;RotY = 27;
				FTImpl.Cmd_LoadIdentity();
                                if(Pf == 0)
				rotate_around(RotX,RotY,th);
				FTImpl.Vertex2ii(200,60,ProcessFlag-2,BitmapTr);						
				FTImpl.RestoreContext();	
			}

			else if(ProcessFlag == 4)
			{
				FTImpl.Vertex2ii(200,60,ProcessFlag-2,BitmapTr);					
			}
		}

		// Home Button
		FTImpl.TagMask(1);
		FTImpl.Tag('H');
		FTImpl.Vertex2ii(430,10,5,0);
		FTImpl.TagMask(0);

		// TIme remainig
		FTImpl.ColorRGB(255,255,255);		
		FTImpl.Cmd_Number( 40,  60, 31, FT_OPT_CENTERX|FT_OPT_CENTERY, 0		); 
		FTImpl.Cmd_Number( 60,  60, 31, FT_OPT_CENTERX|FT_OPT_CENTERY, 0		); 
		FTImpl.Cmd_Text( 75,  67, 21, FT_OPT_CENTERX|FT_OPT_CENTERY, "H"		); 
		FTImpl.Cmd_Text( 85,  57, 31, FT_OPT_CENTERX|FT_OPT_CENTERY, ":"		); 
		if(TimeSet < 10)
		{
			FTImpl.Cmd_Number(105,60,31,FT_OPT_CENTERX|FT_OPT_CENTERY,0); 
			FTImpl.Cmd_Number(128,60,31,FT_OPT_CENTERX|FT_OPT_CENTERY,TimeSet); 
		}
		else
			FTImpl.Cmd_Number( 115, 60, 31, FT_OPT_CENTERX|FT_OPT_CENTERY, TimeSet	); 
		FTImpl.Cmd_Text( 150, 67, 21, FT_OPT_CENTERX|FT_OPT_CENTERY, "Min"		); 				
			
		//GetLocalTime(&st);
		// Est Finish time
		FTImpl.ColorRGB(255,255,255);
		FTImpl.Cmd_Text(300,190,23,0,"Est. Finish Time"); 

		
                FTImpl.Cmd_Number(315,212,25,0,0); 
                FTImpl.Cmd_Number(335,212,25,0,0); 
		FTImpl.Cmd_Text(355,210,25,0,":"); 
                if(TimeSet < 10)
		{
			FTImpl.Cmd_Number( 370    , 212, 25, 0, 0	); 
			FTImpl.Cmd_Number( 390    , 212, 25, 0, TimeSet	); 
		}
		else
                    FTImpl.Cmd_Number( 370    , 212, 25, 0, TimeSet	); 
                

                if(TimeSet%10!=0)
                {
                  if(TimeSet % 2 == 0)
                  {
			if(SoundFlag == 0)
			{
				FTImpl.PlaySound(SoundLev,0x50);
				SoundFlag = 1;
			}
                  }
		  else
		        SoundFlag =0;
                }
		FTImpl.End();		
		FTImpl.DLEnd();
		FTImpl.Finish();     

						
		ItCt+=1;
		if(ItCt%50==0 && Pf == 0)	TimeSet--;
		if(TimeSet	>60)	TimeSet =0;
		// Process rectangle cover up co ordinates
		if(IterCount %50==0 && Pf == 0)
		  Px+=11;
		//Butterfly Line
		if(IterCount%15 == 0  && Pf==0 )ButX+=1;			

		if(ProcessFlag == 3)
		{
			if((ItCt%100 > 0) &&(ItCt%100 <50))
				th-=1000;
			if((ItCt%100 >= 50) &&(ItCt%100 <100))			
				th+= 1000;
		}
	
		if(PwY <61)PwY+=1;
		if(PwY1 <61 && PwY == 61)PwY1+=1;
		if(PwY2 <61 && PwY1 == 61 )PwY2+=1;
			IterCount+=1;

		}while(Read_tag !='H');
	OptionsSet = 1;
	return  Read_tag;			
}


static uint8_t BrightKey,VolKey;
uint8_t Settings()
{
	uint8_t Read_tag=0,i=0,SoundFlag=0;
	uint16_t Rx1 =0,Ry1=0,Rx2=0,Ry2=0;
        char buffer[10];
	if(BrightKey == 0)BrightKey = 5;
		if(VolKey == 0)VolKey = 10;
	do{
  
		Read_tag = Read_Keys();
		FTImpl.DLStart();   
			
		FTImpl.ColorRGB(0,0,255);
		FTImpl.Clear(1,1,1);

		FTImpl.Begin(FT_BITMAPS);
		FTImpl.ColorA(180);
		FTImpl.BitmapTransformA(128);			
		FTImpl.BitmapTransformE(128);	
		FTImpl.Vertex2ii(0,0,9,0);// backgnd tag -8

		FTImpl.RestoreContext();

		FTImpl.ColorRGB(0,200,255);

		FTImpl.TagMask(1);
		FTImpl.Tag('V');
		FTImpl.Cmd_Text(60,80,23,FT_OPT_CENTERX|FT_OPT_CENTERY,"Display");

		FTImpl.Tag('B');
		FTImpl.Cmd_Text(60,145,23,FT_OPT_CENTERX|FT_OPT_CENTERY,"Sound");
					
		FTImpl.TagMask(0);	

		FTImpl.Begin(FT_RECTS);
		
		FTImpl.LineWidth(4*16);
		FTImpl.ColorA(100);
		FTImpl.ColorRGB(255,255,255);
		FTImpl.Vertex2f(115*16, 25*16);
		FTImpl.Vertex2f(415*16, 240*16);

		//FTImpl.Begin(FT_RECTS);
		FTImpl.ColorA(255);
		FTImpl.LineWidth(4*16);
			
		FTImpl.TagMask(1);
		// Brightness
		Rx1 = 120;Ry1 = 70;Rx2 = 170;Ry2 = 100;
		for(i =1;i<6;i++)
		{
			if(i==1)FTImpl.ColorRGB(177,176,180);
					
			if(i==2)FTImpl.ColorRGB(122,154,171);

			if(i==3)FTImpl.ColorRGB(70,149,186);

			if(i==4)FTImpl.ColorRGB(28,140,202);

			if(i==5)FTImpl.ColorRGB(135,183,71);
				
			if(i == BrightKey)
			{
				FTImpl.Write(REG_PWM_DUTY,BrightNess);
				FTImpl.Tag(i);		
				FTImpl.Vertex2f((Rx1-5)*16, (Ry1-5)*16);
				FTImpl.Vertex2f((Rx2+5)*16, (Ry2+5)*16);
			}
			else
			{
				FTImpl.Tag(i);
				FTImpl.Vertex2f(Rx1*16, Ry1*16);
				FTImpl.Vertex2f(Rx2*16, Ry2*16);
			}
			Rx1+= 60;
			Rx2+= 60;
		}
		Rx1 = 143;Ry1 = 73;
		FTImpl.ColorRGB(0,0,0);
		
		for(i =1;i<6;i++)
		{
			FTImpl.Tag(i);
			strcpy_P(buffer, BrightLevel[i]);   
			FTImpl.Cmd_Text(Rx1,Ry1+10,18,FT_OPT_CENTERX|FT_OPT_CENTERY,buffer);
			Rx1+= 60;
			Rx2+= 60;
		}

			
		FTImpl.TagMask(0);

		FTImpl.End();

		// Volume
		FTImpl.Begin(FT_RECTS);
		FTImpl.ColorA(255);
		FTImpl.LineWidth(4*16);

		FTImpl.TagMask(1);
		Rx1 = 120;Ry1 = 130;Rx2 = 170;Ry2 = 160;
		for(i =1;i<6;i++)
		{
	                if(i==1)FTImpl.ColorRGB(177,176,180);

			if(i==2)FTImpl.ColorRGB(122,154,171);

			if(i==3)FTImpl.ColorRGB(70,149,186);

			if(i==4)FTImpl.ColorRGB(28,140,202);

			if(i==5)FTImpl.ColorRGB(135,183,71);

			if(i == VolKey-5)
			{
				if(SoundFlag == 0)
				{
					FTImpl.PlaySound(SoundLev,0x56);
					SoundFlag =1;
				}
				FTImpl.Tag(i+5);
				FTImpl.Vertex2f((Rx1-5)*16, (Ry1-5)*16);
				FTImpl.Vertex2f((Rx2+5)*16, (Ry2+5)*16);
			}
			else
			{
				FTImpl.Tag(i+5);
				FTImpl.Vertex2f(Rx1*16, Ry1*16);
				FTImpl.Vertex2f(Rx2*16, Ry2*16);
			}
			Rx1+= 60;
			Rx2+= 60;
		}
		Rx1 = 143;Ry1 = 133;
		FTImpl.ColorRGB(0,0,0);
		
		for(i =1;i<6;i++)
		{
                        FTImpl.Tag(i+5);
		        strcpy_P(buffer, SoundLevel[i]);   
			FTImpl.Cmd_Text(Rx1,Ry1+10,18,FT_OPT_CENTERX|FT_OPT_CENTERY,buffer);
			Rx1+= 60;
			Rx2+= 60;
		}

		FTImpl.TagMask(0);
		FTImpl.End();

		if((Read_tag >0) && (Read_tag <6) )
		{
				BrightKey =Read_tag;				
		}
		if((Read_tag >5) && (Read_tag <11) )
		{
				VolKey =Read_tag;
				SoundFlag = 0;
		}
		if( Read_tag >0 && Read_tag < 6 )	
			BrightNess = Read_tag*128/5;
		if( Read_tag >5 && Read_tag < 11 )
			SoundLev = (Read_tag-6)*63;

		// Home Button
		FTImpl.ColorRGB(255,255,255);
		FTImpl.Begin(FT_BITMAPS);
		FTImpl.TagMask(1);
		FTImpl.Tag('H');
		FTImpl.Vertex2ii(430,10,8,0);
		FTImpl.TagMask(0);

		FTImpl.End();
		
		FTImpl.DLEnd();
		FTImpl.Finish();      

	}while(Read_tag != 'H');
return Read_tag;
}

static  uint8_t MenuFlag;
uint8_t MainWindow()
{
    char buffer[20];
	uint16_t Ty=0,By=0,Ly=0,th=0,Addth=0;
	uint8_t PrevTag=0,Read_Tag=0,temp=0,dg_count=0,LockFlag=0,PlayFlag=0,Opt=0,i=0;
	int16_t sy = 0,	drag = 0,ret=0;
	static uint8_t j,k;
	scroller_init((FT_DISPLAYHEIGHT*2)*16);
        char ItemProp[20],ItemSp[10],ItemSp1[10],ItemSp2[10],ItemSp3[10];  
		sTagXY sTagxy;


	do
	{

		FTImpl.Write(REG_VOL_SOUND,0);
		FTImpl.Write(REG_VOL_PB,0);
			
		
		FTImpl.GetTagXY(sTagxy);
		sy = sTagxy.y;
		Read_Tag = Read_Keys();
			
		if(sy != -32768 )	// touch
		{
			dg_count++;     	
			temp = key_pressed;      
		}
		/*Move into the particular frame based on dragdt now 30pixels*/
		if(sy==-32768)
		{
			dg_count = 0;			 
			drag = scroller.base>>4; 	
		}
		/*if tag in but still pendown take a scroller basevalue*/
		else if(dg_count>15)
		{
			  Read_Tag = 0;
			  temp = key_pressed;
			  drag = scroller.base>>4;
		}
		if(drag>= 135) 
		{
			drag = 135;
		}
		if(temp == 101/* ||((temp > 0) &&(temp < 8)))*/ && drag <= 135)
		{		
			scroller_run();
		}
		FTImpl.DLStart();        // start
		FTImpl.ColorRGB(0,0,255);
		FTImpl.Clear(1,1,1);

	        // BackGround Image
		FTImpl.Begin(FT_BITMAPS);
		FTImpl.ColorA(180);
		FTImpl.BitmapTransformA(128);			
		FTImpl.BitmapTransformE(128);	
		FTImpl.Vertex2ii(0,0,9,0);// backgnd tag -8
		FTImpl.RestoreContext();

		// Draw rectanglw with zero alpha value to have scrolling option on left pane
		FTImpl.Begin(FT_RECTS);
		FTImpl.ColorA(0);
		FTImpl.TagMask(1);
		FTImpl.Tag(101);
		FTImpl.Vertex2f(0*16, 0*16);
		FTImpl.Vertex2f(120*16, 2720*16);
		FTImpl.TagMask(0);
		FTImpl.End();
		FTImpl.ColorA(255);
		Ty = 30;

		Ty = Ty-drag;
		// Options on the left pane
		for(i =1; i<8;i++)
		{
			if(i == j)
			{
				FTImpl.ColorRGB(255,255,255);
			}
			else
				FTImpl.ColorRGB(0,200,255);
    			FTImpl.TagMask(1);
			FTImpl.Tag(i);
                        strcpy_P(buffer, Array_Cycle_Name[i]);                           
			FTImpl.Cmd_Text(60,Ty,23,FT_OPT_CENTERX|FT_OPT_CENTERY, buffer);					
			FTImpl.TagMask(0);				
			Ty = Ty+ 50;
		}
		
		// Draw rectangle in the centre to show the options
		FTImpl.Begin(FT_RECTS);
		
		FTImpl.LineWidth(4*16);
		FTImpl.ColorA(100);
		FTImpl.ColorRGB(255,255,255);
		FTImpl.Vertex2f(140*16, 25*16);
		FTImpl.Vertex2f(320*16, 240*16);
		
		FTImpl.ColorA(255);
		FTImpl.ColorRGB(0,0,0);
		FTImpl.Cmd_Text(220,32,23,FT_OPT_CENTERX|FT_OPT_CENTERY, "Options"); 

		// Display icons in vertical order
		FTImpl.Begin(FT_BITMAPS);			
		By = 60;
		for(i=0;	i<3;	i++)
		{
			FTImpl.ColorRGB(255,255,255);
			if(i<2)
			FTImpl.Vertex2ii(150,By,i,0); 
			FTImpl.ColorRGB(0,0,0);
                        strcpy_P(buffer, Array_Cycle_Options[i]);   
			FTImpl.Cmd_Text(240,By+10,18,FT_OPT_CENTERX|FT_OPT_CENTERY, buffer); 
			By+=60;
		}

		// Lines draw after each icon
		FTImpl.Begin(FT_LINES);
		FTImpl.LineWidth(1*16);
		FTImpl.ColorA(100);					
		Ly = 55;
		for(i=0;	i<3;	i++)
		{
			FTImpl.Vertex2f(140*16, Ly*16);
			FTImpl.Vertex2f(320*16, Ly*16);
			Ly += 60;
		}
		FTImpl.ColorA(255);		
		By = 90;
		if(key_pressed > 0 && key_pressed < 9)
		{
			if(ProcFinish == 0 || ProcFinish ==2 || TimeSet ==0)
			{
				Tagval =key_pressed;
				OptionsSet =1;
				temp =0;
				j =key_pressed;
			}
		}
		if(Tagval !=0)
		{
			for(i=0;	i<3;	i++)
			{
				FTImpl.ColorRGB(0,0,255);
				if(i==0)
                                {
                                        strcpy_P(ItemProp, Item_Property[Tagval-1].SoilLev);
					FTImpl.Cmd_Text(240,By,29,FT_OPT_CENTERX|FT_OPT_CENTERY, ItemProp); 
                                }
				if(i==1)
                                {
                                        strcpy_P(ItemProp, Item_Property[Tagval-1].Temp);
				        FTImpl.Cmd_Text(240,By,29,FT_OPT_CENTERX|FT_OPT_CENTERY, ItemProp); 
                                }
				if(i==2)
                                {
                                        strcpy_P(ItemSp, Item_Property[Tagval-1].SpinSpeed);
                                       	FTImpl.Cmd_Text(240,By,29,FT_OPT_CENTERX|FT_OPT_CENTERY, ItemSp); 
                                }
				By+=60;
			}
		}


		// for soil icon
                strcpy_P(ItemProp, Item_Property[Tagval-1].SoilLev);
                strcpy(ItemSp1, "Light");
                strcpy(ItemSp2, "Normal");
                strcpy(ItemSp3, "Heavy");
                ret = strcmp(ItemProp, ItemSp1);
                if(ret ==0 )
                {
			FTImpl.ColorRGB(200,200,200);
                        k=1;
                }
		ret = strcmp(ItemProp, ItemSp2);
                if(ret ==0 )
                {
			FTImpl.ColorRGB(150,150,150);
                        k =2;
                }
			
		ret = strcmp(ItemProp, ItemSp3);
                if(ret ==0 )
                {
			FTImpl.ColorRGB(80,80,80);//orange
                        k = 3;
                }
		if((k != 1) && (k !=2) && (k!=3))
			FTImpl.ColorRGB(255,255,255);
		 FTImpl.Begin(FT_BITMAPS);
		 FTImpl.Vertex2ii(150,60,0,0); 


		// for temp icon - draw point and line inside icon
                strcpy_P(ItemProp, Item_Property[Tagval-1].Temp);
                strcpy(ItemSp1, "Cold");
                strcpy(ItemSp2, "Warm");
                strcpy(ItemSp3, "Hot");
                ret = strcmp(ItemProp, ItemSp1);
                if(ret ==0 )
                      FTImpl.ColorRGB(0,0,255);//blue
                      
                ret = strcmp(ItemProp, ItemSp2);
                if(ret ==0 )
                      FTImpl.ColorRGB(255,140,0);//blue
                      
                ret = strcmp(ItemProp, ItemSp3);
                if(ret ==0 )
                      FTImpl.ColorRGB(255,0,0);//red
		
		FTImpl.Begin(FT_POINTS);		
		FTImpl.PointSize(5*16);	
		FTImpl.Vertex2f(170*16, 150*16);
		FTImpl.Begin(FT_LINES);
		FTImpl.LineWidth(1*16);
		FTImpl.Vertex2f(170*16, 145*16);
		FTImpl.Vertex2f(170*16, 135*16);
                strcpy(ItemSp1, "Low");
                strcpy(ItemSp2, "Medium");
                strcpy(ItemSp3, "High");
                ret = strcmp(ItemSp, ItemSp1);
                if(ret ==0 )
                      Addth = 500;   
                          
                ret = strcmp(ItemSp, ItemSp2);
                if(ret ==0 )
                      Addth = 1000;   
                          
                ret = strcmp(ItemSp, ItemSp3);
                if(ret ==0 )
                      Addth = 1500;   
                      
                       
		FTImpl.ColorRGB(0,0,255);
		FTImpl.Begin(FT_BITMAPS);
		//Ft_App_WrCoCmd_Buffer(phost,CMD_LOADIDENTITY);
		FTImpl.Cmd_LoadIdentity();
		rotate_around(20,20,th);			
		FTImpl.Vertex2ii(150,180,2,0); 
		FTImpl.RestoreContext();	
			
                if(Stflag == 0)
				TimeSet = 0;
		if(TimeSet > 36)TimeSet = 0;
		FTImpl.ColorRGB(0,200,255);
		FTImpl.Cmd_Text(400,30,18,FT_OPT_CENTERX|FT_OPT_CENTERY,"Estimated Time"); 
		FTImpl.Cmd_Number(355,60,31,FT_OPT_CENTERX|FT_OPT_CENTERY,0); 
		FTImpl.Cmd_Text(372,67,21,FT_OPT_CENTERX|FT_OPT_CENTERY,"H"); 
		if(TimeSet < 10)
		{
			FTImpl.Cmd_Number(395,60,31,FT_OPT_CENTERX|FT_OPT_CENTERY,0); 
			FTImpl.Cmd_Number(415,60,31,FT_OPT_CENTERX|FT_OPT_CENTERY,TimeSet); 
		}
		else
			FTImpl.Cmd_Number(405,60,31,FT_OPT_CENTERX|FT_OPT_CENTERY,TimeSet); 
		FTImpl.Cmd_Text(442,67,21,FT_OPT_CENTERX|FT_OPT_CENTERY,"Min"); 


		FTImpl.Begin(FT_LINES);
		FTImpl.LineWidth(1*16);
		FTImpl.ColorRGB(92,148,212);					
		//Blue line below butterfly
		FTImpl.Vertex2f(340*16, 90*16);
		FTImpl.Vertex2f(460*16, 90*16);
		//Orange line below butterfly
		FTImpl.ColorRGB(255,140,0);	
		FTImpl.Vertex2f(340*16, 90*16);
		FTImpl.Vertex2f(ButX*16, 90*16);
		//Butterfly			
		FTImpl.Begin(FT_BITMAPS);
		FTImpl.ColorRGB(255,255,255);
		if(ButX > 460)ButX =460;
		FTImpl.Vertex2ii(ButX,75 ,3,0); 

		FTImpl.TagMask(1);

		//Lock & Unlock Buton
		if(Read_Tag == 'U')LockFlag = 0;
			
		//if(LockFlag =='U' || LockFlag == 0)
		{
			FTImpl.Tag('L');
			FTImpl.Vertex2ii(340,105 ,4,0); 
		}
			
		if(Read_Tag == 'L')LockFlag = 1;

		

		// Settings Button
		FTImpl.Tag('S');
		FTImpl.Vertex2ii(340,145 ,6,0); 

		// Start Button
		FTImpl.Tag('P');
		FTImpl.Vertex2ii(340,185 ,7,0); 

		FTImpl.TagMask(0);

		if(Read_Tag =='S')
                {
                     Settings();
                }
		if(BrightNess ==0)BrightNess =100;
		FTImpl.Write(REG_PWM_DUTY,BrightNess);// start

                if(TimeSet == 0) ProcFinish=2;
		if((Read_Tag == 'P'))
		{
			PlayFlag = 1;
			Opt=0;
                        if(ProcFinish == 2)
			{
				TimeSet = 36;
				ButX =330;
				Px = 44;
			}
		}
		if(PlayFlag ==1)
		{
			if(	OptionsSet == 1)
			{
				Stflag = 1;
				OptionsSet = 2;
			}
			else if(OptionsSet != 1 && Opt !=1)
			{
				if((IterCount % 100 > 0)&&( IterCount % 100 < 100))
				{
					FTImpl.ColorRGB(255,0,0);
					FTImpl.Begin(FT_RECTS);
					FTImpl.Vertex2f(110*16,115*16);
					FTImpl.Vertex2f(375*16,140*16);
					FTImpl.ColorRGB(0,0,0);
					FTImpl.Cmd_Text(120,125,27,FT_OPT_CENTERY,"Please Select the Washing Mode");
					if(IterCount % 100 == 99)
					{
						Opt=1;
						PlayFlag=0;
					}
				}
			}
			
		}
				
		if(temp >= 50)temp =0;
		FTImpl.RestoreContext();
		By = 115;
		for(i=0;i<3;i++)
		{
			FTImpl.ColorRGB(0,200,255);
                        strcpy_P(buffer, Array_Menu_Options[i]);                 
			FTImpl.Cmd_Text(380,By,18,0,buffer); 
			By+=40;
		}
		if(LockFlag ==1)
		{
			FTImpl.ColorRGB(219,180,150);
			FTImpl.ColorA(180);
			FTImpl.Begin(FT_EDGE_STRIP_A);
			FTImpl.Vertex2f(0,FT_DISPLAYHEIGHT*16);
			FTImpl.Vertex2f(FT_DISPLAYWIDTH*16,FT_DISPLAYHEIGHT*16);
			FTImpl.ColorA(255);
			FTImpl.RestoreContext();	

			FTImpl.Begin(FT_BITMAPS);
			FTImpl.BitmapTransformA(128) ;
			FTImpl.BitmapTransformE(128) ;
			FTImpl.Tag('U');
			FTImpl.Vertex2ii(220,105 ,4,0);
			FTImpl.RestoreContext();	
		}
			
		if(MainFlag == 0) Px = 44;
		FTImpl.End();			
		FTImpl.DLEnd();
		FTImpl.Finish();       
		th+=Addth;
		if(IterCount%15 == 0 && Stflag == 1  && Pf ==0)
			ButX+=1;			
		if(IterCount%50 == 0  && Pf ==0 && Stflag == 1)TimeSet--;
		if(IterCount %50 == 0 && Pf ==0	 )	Px+=11;
						
		if(temp >=1 )temp++;
		IterCount+=1;
	}while(OptionsSet !=2 );//(Read_Tag!= 'P');
	MainFlag ++;
	return Read_Tag;
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
		FTImpl.Cmd_Text(FT_DISPLAYWIDTH>>1,FT_DISPLAYHEIGHT>>1,26,FT_OPT_CENTER,"STORAGE DEVICE NOT FOUND");
		FTImpl.DLEnd();
		FTImpl.Finish();
		delay(5000);
		}
}


void setup()
{
char Val;
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
           while(1)
           {
             Logo_Intial_setup(Main_Icons,10);
       	     Val =  MainWindow();
             if(Val == 'P')
      	     {
	        FTImpl.PlaySound(SoundLev,0x56);
      	        WashWindow();
      	     }
           }
      FTImpl.Exit();  /*  Safely close all opened handles */
	  } 
	Serial.println("--End Application--");
}

void loop()
{
}


/* Nothing beyond this */














