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
* @file                           FT_App_Clocks.ino
* @brief                          Sample application to demonstrate FT801 primitives, widgets and customized screen shots. 
* @version                        0.1.1
* @date                           Jun/13/2014

version 0.1.0 - Mar/21/2014 - Initial version
version 0.1.1 - Jun/13/2014 - Scrolling velocity modification for FT801 platform.
*
*/


#include "SPI.h"
#include "Wire.h"
#include "FT_NHD_43CTP_SHIELD.h"

#define RTC_PRESENT
              
FT801IMPL_SPI FTImpl(FT_CS_PIN,FT_PDN_PIN,FT_INT_PIN);
#ifdef RTC_PRESENT
FT_RTC Ft_RTC;
DateTime dateTime;
#endif

#define ON          1
#define OFF         0						 
#define Font        27					// Font Size
#define MAX_LINES   4					// Max Lines allows to Display

#define SPECIAL_FUN     251
#define BACK_SPACE	251				// Back space
#define CAPS_LOCK	252				// Caps Lock
#define NUMBER_LOCK	253				// Number Lock
#define BACK		254				// Exit 

#define LINE_STARTPOS	FT_DISPLAYWIDTH/50			// Start of Line
#define LINE_ENDPOS	FT_DISPLAYWIDTH			// max length of the line
		


/*deflated Icons*/
static uint8_t home_star_icon[] = {0x78,0x9C,0xE5,0x94,0xBF,0x4E,0xC2,0x40,0x1C,0xC7,0x7F,0x2D,0x04,0x8B,0x20,0x45,0x76,0x14,0x67,0xA3,0xF1,0x0D,0x64,0x75,0xD2,0xD5,0x09,0x27,0x17,0x13,0xE1,0x0D,0xE4,0x0D,0x78,0x04,0x98,0x5D,0x30,0x26,0x0E,0x4A,0xA2,0x3E,0x82,0x0E,0x8E,0x82,0xC1,0x38,0x62,0x51,0x0C,0x0A,0x42,0x7F,0xDE,0xB5,0x77,0xB4,0x77,0x17,0x28,0x21,0x26,0x46,0xFD,0x26,0xCD,0xE5,0xD3,0x7C,0xFB,0xBB,0xFB,0xFD,0xB9,0x02,0xCC,0xA4,0xE8,0x99,0x80,0x61,0xC4,0x8A,0x9F,0xCB,0x6F,0x31,0x3B,0xE3,0x61,0x7A,0x98,0x84,0x7C,0x37,0xF6,0xFC,0xC8,0xDD,0x45,0x00,0xDD,0xBA,0xC4,0x77,0xE6,0xEE,0x40,0xEC,0x0E,0xE6,0x91,0xF1,0xD2,0x00,0x42,0x34,0x5E,0xCE,0xE5,0x08,0x16,0xA0,0x84,0x68,0x67,0xB4,0x86,0xC3,0xD5,0x26,0x2C,0x20,0x51,0x17,0xA2,0xB8,0x03,0xB0,0xFE,0x49,0xDD,0x54,0x15,0xD8,0xEE,0x73,0x37,0x95,0x9D,0xD4,0x1A,0xB7,0xA5,0x26,0xC4,0x91,0xA9,0x0B,0x06,0xEE,0x72,0xB7,0xFB,0xC5,0x16,0x80,0xE9,0xF1,0x07,0x8D,0x3F,0x15,0x5F,0x1C,0x0B,0xFC,0x0A,0x90,0xF0,0xF3,0x09,0xA9,0x90,0xC4,0xC6,0x37,0xB0,0x93,0xBF,0xE1,0x71,0xDB,0xA9,0xD7,0x41,0xAD,0x46,0xEA,0x19,0xA9,0xD5,0xCE,0x93,0xB3,0x35,0x73,0x0A,0x69,0x59,0x91,0xC3,0x0F,0x22,0x1B,0x1D,0x91,0x13,0x3D,0x91,0x73,0x43,0xF1,0x6C,0x55,0xDA,0x3A,0x4F,0xBA,0x25,0xCE,0x4F,0x04,0xF1,0xC5,0xCF,0x71,0xDA,0x3C,0xD7,0xB9,0xB2,0x48,0xB4,0x89,0x38,0x20,0x4B,0x2A,0x95,0x0C,0xD5,0xEF,0x5B,0xAD,0x96,0x45,0x8A,0x41,0x96,0x7A,0x1F,0x60,0x0D,0x7D,0x22,0x75,0x82,0x2B,0x0F,0xFB,0xCE,0x51,0x3D,0x2E,0x3A,0x21,0xF3,0x1C,0xD9,0x38,0x86,0x2C,0xC6,0x05,0xB6,0x7B,0x9A,0x8F,0x0F,0x97,0x1B,0x72,0x6F,0x1C,0xEB,0xAE,0xFF,0xDA,0x97,0x0D,0xBA,0x43,0x32,0xCA,0x66,0x34,0x3D,0x54,0xCB,0x24,0x9B,0x43,0xF2,0x70,0x3E,0x42,0xBB,0xA0,0x95,0x11,0x37,0x46,0xE1,0x4F,0x49,0xC5,0x1B,0xFC,0x3C,0x3A,0x3E,0xD1,0x65,0x0E,0x6F,0x58,0xF8,0x9E,0x5B,0xDB,0x55,0xB6,0x41,0x34,0xCB,0xBE,0xDB,0x87,0x5F,0xA9,0xD1,0x85,0x6B,0xB3,0x17,0x9C,0x61,0x0C,0x9B,0xA2,0x5D,0x61,0x10,0xED,0x2A,0x9B,0xA2,0x5D,0x61,0x10,0xED,0x2A,0x9B,0xA2,0x5D,0x61,0x10,0xED,0x2A,0x9B,0xED,0xC9,0xFC,0xDF,0x14,0x54,0x8F,0x80,0x7A,0x06,0xF5,0x23,0xA0,0x9F,0x41,0xF3,0x10,0x30,0x4F,0x41,0xF3,0x18,0x30,0xCF,0xCA,0xFC,0xFF,0x35,0xC9,0x79,0xC9,0x89,0xFA,0x33,0xD7,0x1D,0xF6,0x5E,0x84,0x5C,0x56,0x6E,0xA7,0xDA,0x1E,0xF9,0xFA,0xAB,0xF5,0x97,0xFF,0x2F,0xED,0x89,0x7E,0x29,0x9E,0xB4,0x9F,0x74,0x1E,0x69,0xDA,0xA4,0x9F,0x81,0x94,0xEF,0x4F,0xF6,0xF9,0x0B,0xF4,0x65,0x51,0x08};

PROGMEM char * const info[] = {  "FT801 Clocks Application",

                          "APP to demonstrate interactive Clocks,",
                          "using Clocks, Track",
                          "& RTC control."
                       }; 

#define COUNTRIES 12

char *country[] = { "India","Singapore","New Zealand","Japan","Denmark","China","Australia","Belgium","Bahrain","Italy","Norway","Germany"};						  

typedef struct 
{
    uint8_t ghr;
    uint8_t gmin;
    uint8_t arith;	
}t_gmtprp;

t_gmtprp gmt_prp[12] = {{5,30,'+'}, {8,0,'+'}, {12,45,'+'},{9,0,'+'},{4,0,'+'},{8,0,'+'},{10,0,'+'},{1,0,'+'},{3,0,'+'},{1,0,'+'},{1,0,'+'},{1,0,'+'}};
                            
				

static struct {
  signed short dragprev;
  int vel;      // velocity
  long base;    // screen x coordinate, in 1/16ths pixel
  long limit;
} scroller;
struct
{
	uint8_t Hrs;
	uint16_t Mins;
	uint16_t Secs;
	uint16_t mSecs;
}ist,utc;
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
  FTImpl.WriteCmd(CMD_INFLATE);
  FTImpl.WriteCmd(250*1024L);
  FTImpl.WriteCmd(home_star_icon,sizeof(home_star_icon));
  /*Set the Bitmap properties for the ICONS*/ 
  FTImpl.DLStart();        // start
  FTImpl.Clear(1,1,1);
  FTImpl.ColorRGB(255, 255, 255);
  FTImpl.BitmapHandle(13);    // handle for background stars
  FTImpl.BitmapSource(250*1024L);      // Starting address in gram
  FTImpl.BitmapLayout(FT_L4, 16, 32);  // format 
  FTImpl.BitmapSize(FT_NEAREST, FT_REPEAT, FT_REPEAT, 512, 512  );
  FTImpl.BitmapHandle(14);    // handle for background stars
  FTImpl.BitmapSource(250*1024L);      // Starting address in gram
  FTImpl.BitmapLayout(FT_L4, 16, 32);  // format 
  FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, 32, 32  );
  FTImpl.DLEnd();
  FTImpl.Finish();
}



void Ft_Play_Sound(uint8_t sound,uint8_t vol,uint8_t midi)
{
  uint16_t val = (midi << 8) | sound; 
  FTImpl.Write(REG_SOUND,val);
  FTImpl.Write(REG_PLAY,1); 
}



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
  // Ftdi Logo animation  
  FTImpl.Cmd_Logo();
	while(FTImpl.CheckLogo() != FT_GE_FINISHED);
  //Copy the Displaylist from DL RAM to GRAM
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
    FTImpl.BitmapTransformA(256);
    FTImpl.BitmapTransformA(256);
    FTImpl.BitmapTransformB(0);
    FTImpl.BitmapTransformC(0);
    FTImpl.BitmapTransformD(0);
    FTImpl.BitmapTransformE(256);
    FTImpl.BitmapTransformF(0);  
    //Display the information with transparent Logo using Edge Strip  
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
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,20,28,FT_OPT_CENTERX|FT_OPT_CENTERY,(char*)pgm_read_word(&info[0]));
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,60,26,FT_OPT_CENTERX|FT_OPT_CENTERY,(char*)pgm_read_word(&info[1]));
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,90,26,FT_OPT_CENTERX|FT_OPT_CENTERY,(char*)pgm_read_word(&info[2]));  
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,120,26,FT_OPT_CENTERX|FT_OPT_CENTERY,(char*)pgm_read_word(&info[3]));  
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,FT_DISPLAYHEIGHT-30,26,FT_OPT_CENTERX|FT_OPT_CENTERY,"Click to play");
    //Check if the Play key and change the color
    if(sk!='P')
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
  }while(Read_Keys()!='P');
  Ft_Play_Sound(0x50,255,0xc0);
  /* wait until Play key is not pressed*/ 
}

/* Init the Scroller */
static void scroller_init(uint32_t limit)
{
  scroller.dragprev = -32768;
  scroller.vel = 0;      // velocity
  scroller.base = 0;     // screen x coordinate, in 1/16ths pixel
  scroller.limit = limit;
}

/* Run the Scroller in Horizontal linearlly*/

static void scroller_run()
{
  int32_t change;
	sTagXY sTagxy;
	FTImpl.GetTagXY(sTagxy);
	
  signed short sx = sTagxy.x;

  if ((sx != -32768) & (scroller.dragprev != -32768)) 
  {
    scroller.vel = (scroller.dragprev - sx) << 4;
  } 
  else
  {
    change = max(1, abs(scroller.vel) >> 5);
    if (scroller.vel < 0)
      scroller.vel += change;
    if (scroller.vel > 0)
      scroller.vel -= change;
  }
  scroller.dragprev = sx;
  scroller.base += scroller.vel;
  scroller.base = max(0, min(scroller.base, scroller.limit));
}


static uint8_t clk_adj = 0;
static uint16_t min_val;
uint8_t temp[7];
/* data converstion for rtc*/
#ifdef RTC_PRESENT
byte decToBcd(byte val)
{
  return ( (val/10*16) + (val%10) );
}

// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
  return ( (val/16*10) + (val%16) );
}

/* write the data into the RTC*/

void Rtc_puts()
{
  dateTime.hour = ist.Hrs;
  dateTime.min = ist.Mins;
  dateTime.sec = ist.Secs;

  Ft_RTC.SetDateTime(dateTime);
}

// Gets the date and time
void Rtc_gets()
{
  Ft_RTC.GetDateTime(dateTime);
  /*ist.Hrs = bcdToDec(dateTime.hour & 0x1f);
  ist.Mins = bcdToDec(dateTime.min & 0x7f); 
  ist.Secs = bcdToDec(dateTime.sec & 0x7f); */
  
  ist.Hrs =  dateTime.hour;
  ist.Mins = dateTime.min ; 
  ist.Secs = dateTime.sec ; 
}
#endif

/* API used to compute the time from the rotary value*/
static uint16_t adj_time=0;
static uint32_t t_adj=0;
  
  
static uint16_t Get_TagRotary_Value(uint16_t Tagval,uint16_t range,uint16_t max_rotation,uint16_t ct)		// -180 for 90 
{
  static int16_t thcurr=0,prevth=0,adj=0;
  static int32_t adj_val= 0;
  static uint16_t retval=0; 
  sTrackTag sTracktag;
  FTImpl.GetTrackTag(sTracktag);
  if(sTracktag.tag || sTracktag.track)
  {
    thcurr = sTracktag.track;	
    if(adj == Tagval)
    {
 	adj_val += (int16_t)(thcurr - prevth);
	if (adj_val < 0)
	adj_val += (max_rotation*65536L);
	if (adj_val >= (max_rotation*65536L))
	adj_val -= (max_rotation*65536L);
        retval = (adj_val/(65536L/range));
    }	
    prevth = thcurr;
    adj = sTracktag.tag;
    clk_adj = 0;
  }
  else
  {
    if(adj!=0)   clk_adj = 1;   
    else   retval = ct;   
// current time in 16bit presscion for the adjustment    
    adj_val=((ct/60)*65536L)+((ct%60)*(65536L/range)); 
    
    adj = 0;    
  }
  return retval;
}

/* api compute the standard time for the countries*/ 

void timer(uint8_t name)
{
  static uint8_t temp_time = 0;
  char hrs_t[2],min_t[2],*temp;
  static uint16_t temp_m=0,Hrs=0;
  static uint16_t temp_msecs=0,temp_secs=0;
  
  uint32_t t;
  int32_t Mins;
  
  Mins = gmt_prp[name].gmin; 
  Hrs =  gmt_prp[name].ghr; 
  Mins = Mins+(Hrs*60);
  
  if(gmt_prp[name].arith=='+')
  Mins = min_val+Mins;
  if(gmt_prp[name].arith=='-')
  {
    Mins = min_val-Mins;
    if(Mins<0)
    Mins = 720-Mins;
  }    
  
  utc.Hrs = (Mins/60)%12;      
  utc.Mins = (Mins)%60;
 /* sync RTC and MCU time*/
  #ifdef RTC_PRESENT
  t = millis();
  t = t%1000L;

  if(ist.Secs!=temp_secs)
  {
    temp_secs = ist.Secs;
    temp_msecs = 1000L-t;
  }
  utc.mSecs = (t+temp_msecs)%1000L;  
  #endif
//  utc.Secs = ist.Secs;
 /* if time adjustment is occured then writes into the RTC*/ 
  if(clk_adj)
  {
     clk_adj = 0;
     ist.Mins = min_val%60;
     ist.Hrs = (min_val/60)%12;
     #ifdef RTC_PRESENT
     Rtc_puts();
     #else
     t_adj = millis();
     t_adj = (t_adj/60000L)%60;
     adj_time = ((ist.Mins)+(ist.Hrs*60))-t_adj;
     #endif
  }
}


/* Clock Function*/
void Clocks(uint8_t clksize,uint8_t options)
{
  uint8_t 
	      clk_s = clksize/10,per_f,n_f,name,Tag,startup_time=0;

  uint16_t  dx = (clk_s*2)+(2*clksize),	
	       dy = (FT_DISPLAYHEIGHT/2)+clksize+10,
               temp_m,Hrs;

   sTrackTag sTracktag;
  uint8_t  col = FT_DISPLAYWIDTH/dx;
  int16_t Ox,Oy,sx,drag=0,prev=0,dragth=0,i,cts=0,th,pv;
  int32_t track,Mins;
  per_f = col;
  
  n_f = (COUNTRIES)/per_f;
  Oy = (FT_DISPLAYHEIGHT/2);
  scroller_init(16*(COUNTRIES*(dx)-FT_DISPLAYWIDTH));
  timer(0);
  do
  {
    Tag = FTImpl.Read(REG_TOUCH_TAG);
    #ifdef RTC_PRESENT
    Rtc_gets(); 
    temp_m =  (ist.Mins + (ist.Hrs*60))%720L; 
    #else
    uint32_t t = millis(); 
    utc.mSecs = t%1000L;
    ist.Secs = (t/1000L)%60;
    ist.Mins = (t/60000L)%60;
    ist.Hrs = (t/3600000L)%12;
    temp_m =  ((ist.Mins)+(ist.Hrs*60));
    temp_m+=adj_time; 
    temp_m%=720;
    #endif
    
    min_val = Get_TagRotary_Value(Tag,60,12,temp_m);
	FTImpl.GetTrackTag(sTracktag);
   if(sTracktag.tag==0)
    {
	scroller_run();
	drag = scroller.base>>4;
	cts = drag/dx; 
	dragth = drag%dx;
    }else
    {
	scroller.vel = 0;
    }
    FTImpl.DLStart(); 
    FTImpl.Clear(1,1,1);
    FTImpl.TagMask(1);
    FTImpl.Tag(0);
    FTImpl.Cmd_Gradient(0, 135, 0x000000, 0, 272, 0x605040);
    for(i=-1;i<(per_f+2);i++)
    {
        Ox = (clksize+clk_s)+(dx*i);
  	Ox-=dragth;    
        name = (COUNTRIES+i+cts)%COUNTRIES;
        timer(name);
   // Set the bg color of the clocks     
        if(Tag==name+1) FTImpl.Cmd_BGColor(0x4040a0); else 
        FTImpl.Cmd_BGColor(0x101040);
   // Draw the Clocks with tracker     
        FTImpl.Tag(name+1);
        FTImpl.Cmd_Clock(Ox,FT_DISPLAYHEIGHT/2,clksize,0,utc.Hrs,utc.Mins,ist.Secs,utc.mSecs);
        FTImpl.Cmd_Track(Ox,FT_DISPLAYHEIGHT/2,1,1,name+1);
    }   
    FTImpl.TagMask(0);
   // Display the Country name 
    for(i=-1;i<(per_f+2);i++)
    {
        Ox = (clksize+clk_s)+(dx*i);
  	Ox-=dragth;    
        name = (COUNTRIES+i+cts)%COUNTRIES;
        FTImpl.Cmd_Text(Ox,dy,29,FT_OPT_CENTERX,country[name]);
    }    						
    FTImpl.DLEnd();
    FTImpl.Finish();
	delay(30);
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
		FT_RTCStatus Status;
		
		home_setup();
		Info();		
		
		Status = Ft_RTC.Init();
		if(FT_RTC_OK != Status)
		{
			Serial.println("RTC is not present");
		}
		/* Configure with some date and time */
		dateTime.sec = 30;
		dateTime.hour = 11;
		dateTime.fampm = 0;	
		dateTime.hformat = 1;
		dateTime.min = 59;
		dateTime.day = 30;
		dateTime.mon = 1;	
		dateTime.year = 0;  
		dateTime.dow = (30%7) + 1;
		
		Ft_RTC.SetDateTime(dateTime);

		Clocks(80,0);  
	}
	Serial.println("--End Application--");
}

void loop()
{
}


/* Nothing beyond this */











