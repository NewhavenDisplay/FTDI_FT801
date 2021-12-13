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
* @file                           MainWindow.ino
* @brief                          Sketch to multi-touch MainWindow on FT801 platform.
								  Tested platform version: Arduino 1.0.4 and later
* @version                        0.1.0
* @date                           Jun/17/2014
*
*/



/* Arduino standard includes */
#include "SPI.h"
#include "Wire.h"

/* Platform specific includes */
#include "FT_NHD_43CTP_SHIELD.h"

#define APPBUFFERSIZE					(65536L)
#define APPBUFFERSIZEMINUSONE		(APPBUFFERSIZE - 1)

#define ImH (66)
#define ImW (66)

#define NO_OF_TOUCH (5)
 typedef struct logoim
{
  char name[20];
  uint16_t image_height;
  uint8_t image_format;
  uint8_t filter;
  uint16_t sizex;
  uint16_t sizey;
  uint16_t linestride;
  uint16_t gram_address;
}t_imageprp;

typedef struct Squares 
{
  uint16_t x, y;
}Squares;


t_imageprp Main_Icons[1] = 
{
{"tile3.bin",		ImH,    FT_RGB565,  FT_NEAREST,    ImW,    ImH ,    ImW*2,   0L },
};
int32_t RowNo[5],ColNo[5];
int32_t Volume;

  void load_inflate_image(uint32_t address, const char *filename);
  void Logo_Intial_setup(struct logoim sptr[],uint8_t num);
  void Load_file2gram(uint32_t add,uint8_t sectors);
  void CheckTouch_tile(Squares *Sq, int32_t TouchXY,uint8_t TouchNo);
  void MainWindow();

/* Global object for FT801 Implementation */
FT801IMPL_SPI FTImpl(FT_CS_PIN,FT_PDN_PIN,FT_INT_PIN);

/* SD card object*/
FT_SD FtSd(FT_SD_CSPIN);
/* sd_present holds error values during initialization.  0 means no error and all other errors are non zero value */
FT_SDStatus sd_present; 

/*SD file object for file accessing*/
FT_SDFile jpeg;

/* Api to bootup FT801, verify FT801 hardware and configure display/audio pins */
/* Returns 0 in case of success and 1 in case of failure */
int16_t BootupConfigure()
{
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
	delay(220); //touch controller delay
	return 0;
}

//load the bitmap appears on touch
  void load_inflate_image(uint32_t address, const char *filename)
{
	uint8_t imbuff[512]; 

	FTImpl.WriteCmd(CMD_INFLATE);
	FTImpl.WriteCmd(address*1024L);	
	
	if(FtSd.OpenFile(jpeg, filename)){
		Serial.print("Unable to open: ");
		Serial.println(filename);
	}
	else{
		while(jpeg.Offset < jpeg.Size)
		{
			uint32_t n = min(512, jpeg.Size - jpeg.Offset);
			n = (uint32_t)(n + 3) & (~3);   // force 32-bit alignment
			jpeg.ReadSector(imbuff);
			FTImpl.WriteCmd(imbuff, n);
		}
	}									
}

void Logo_Intial_setup(struct logoim sptr[],uint8_t num)
{
	uint8_t z;
   for(z=0;z<num;z++)
   {
     load_inflate_image(sptr[z].gram_address,sptr[z].name);
   }     
   
	FTImpl.DLStart();
   for(z=0;z<num;z++)
   {
		FTImpl.BitmapHandle(z);			 
		FTImpl.BitmapSource(sptr[z].gram_address*1024L);			 
		FTImpl.BitmapLayout(sptr[z].image_format,sptr[z].linestride,sptr[z].image_height);		
		FTImpl.BitmapSize(sptr[z].filter,FT_BORDER, FT_BORDER,480,272);	 
   }
	FTImpl.DLEnd();
	FTImpl.Finish();    
}

//load sound file in chunks
void Load_file2gram(uint32_t add,uint8_t sectors)
{
  uint8_t pbuff[512];
  uint8_t z = 0;
  for(z=0;z<sectors;z++)
  {
	jpeg.ReadSector(pbuff);
	FTImpl.Write(add,pbuff,512L);
	add+=512;
  }	
}


//check and draw a bitmap on the current touched tile.
void CheckTouch_tile(Squares *Sq, int32_t TouchXY,uint8_t TouchNo)
{
	uint8_t i=0;

	if((TouchXY >> 16) != -32768)
	{
		Sq->x = TouchXY>>16;	
		Sq->y =  (TouchXY & 0xffff);	
		Volume  = (TouchNo+1)*51;
		for(i =0;i<7;i++)
		{
			/* find row number*/
			if((Sq->y > i*(ImH+2)) && (Sq->y < (i+1)*(ImH+2)))
				RowNo[TouchNo] = i;
			if(((Sq->x) > (i*(ImW+2)))  && ((Sq->x)< ((i+1)*(ImW+2))))
				ColNo[TouchNo] =i;
		}
	}
	else
	{
		RowNo[TouchNo] = -1000;
		ColNo[TouchNo] =-1000;
	}
	FTImpl.ColorRGB(255,255,255);
	FTImpl.Begin(FT_BITMAPS);
	FTImpl.BitmapHandle(0);
	FTImpl.Vertex2f(((ImW+2)*ColNo[TouchNo])*16,((ImH+2)*RowNo[TouchNo])*16);
	
}

void MainWindow()
{
		uint8_t i=0,TouchNo=0;
		uint32_t val[6];
	

		int32_t ftsize=0,AddrOffset;

		
		int32_t rp=0,audioval,wp=0;

		Squares SqCall,*Sq;
		Sq = &SqCall;

		Logo_Intial_setup(Main_Icons,1);
		FTImpl.SetCTouchMode(FT_CTOUCH_MODE_EXTENDED);	

		AddrOffset = 102400L;

		
		if(FtSd.OpenFile(jpeg,"sound.wav")){
			Serial.println("Unable to open sound file.");
		}
		
		ftsize = jpeg.Size;

		
		Load_file2gram(AddrOffset+0UL,64*2); 

		wp = 1024;   
		ftsize-=1024;
		

		FTImpl.Write32(REG_PLAYBACK_FREQ,44100);
		FTImpl.Write32(REG_PLAYBACK_START,AddrOffset);
		FTImpl.Write32(REG_PLAYBACK_FORMAT,FT_ULAW_SAMPLES);
		FTImpl.Write32(REG_PLAYBACK_LENGTH,APPBUFFERSIZE);
		FTImpl.Write32(REG_PLAYBACK_LOOP,1);
		FTImpl.Write(REG_VOL_PB, Volume);
		FTImpl.Write(REG_PLAYBACK_PLAY,1);
		

		while(ftsize>0)
		{
			val[0] = FTImpl.Read32(REG_CTOUCH_TOUCH0_XY);		 
			val[1] = FTImpl.Read32(REG_CTOUCH_TOUCH1_XY);	
			val[2] = FTImpl.Read32(REG_CTOUCH_TOUCH2_XY);		
			val[3] = FTImpl.Read32(REG_CTOUCH_TOUCH3_XY);		
			val[4] = ((int32_t)(FTImpl.Read16(REG_CTOUCH_TOUCH4_X))<<16) | (FTImpl.Read16(REG_CTOUCH_TOUCH4_Y)& 0xffff);			
			
			FTImpl.StartTransferCmd();
			FTImpl.DLStart();
			//FTImpl.ColorRGB(0,0,0);		 
			FTImpl.ColorRGB(255,255,255);


					
			for(i=0; i<NO_OF_TOUCH; i++)
			{
					 CheckTouch_tile(&SqCall,(int32_t)val[i],i);
			}
			if((val[0] ==  2147516416) && (val[1] == 2147516416) && (val[2] == 2147516416) && (val[3] == 2147516416) && (val[4] == 2147516416) )
				Volume =0;
			FTImpl.DLEnd();
			FTImpl.Flush();	 
			FTImpl.EndTransferCmd();
			
			rp = FTImpl.Read16(REG_PLAYBACK_READPTR);
			audioval = APPBUFFERSIZEMINUSONE & (rp-wp);				
			if (audioval > 1024) 
			{
				  uint16_t n = min(1024,ftsize);  
				  Load_file2gram(AddrOffset+wp,2);  
				  wp = (wp +1024) & APPBUFFERSIZEMINUSONE;
				  ftsize-=n;
				  FTImpl.Write(REG_VOL_PB, Volume);
			}
			
			if(ftsize==0){
				AddrOffset = 102400L;
				FtSd.OpenFile(jpeg,"sound.wav");
				ftsize = jpeg.Size;
				Load_file2gram(AddrOffset+0UL,64*2); 
				rp=0;
				wp = 1024;   
				ftsize-=1024;
			}
			
		}
		FTImpl.Write(REG_VOL_PB,0);
		FTImpl.Write(REG_PLAYBACK_PLAY,0);
}

/* Display an on-screen message if the storage device is not found */
void CheckStorageDevicePrecence(){
	while(sd_present){
		FTImpl.DLStart();     
		FTImpl.Clear(1,1,1);
		FTImpl.ColorRGB(255,255,255);
		FTImpl.Cmd_Text(FT_DISPLAYWIDTH>>1,FT_DISPLAYHEIGHT>>1,29,FT_OPT_CENTER,"NO STORAGE DEVICE FOUND");
		FTImpl.DLEnd();
		FTImpl.Finish();
		delay(5000);
		}
}


void setup()
{
	/* Initialize serial print related functionality */
	Serial.begin(9600);
	
	/*Initialize the SD object. Screen error message can only be displayed when the FT800 is ready*/
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
		FTImpl.DLStart();
		FTImpl.Cmd_Calibrate(0);
		FTImpl.DLEnd();
		FTImpl.Finish();
		MainWindow();
	}
	Serial.println("--End Application--");
}

/* Nothing in loop api */
void loop()
{
}
