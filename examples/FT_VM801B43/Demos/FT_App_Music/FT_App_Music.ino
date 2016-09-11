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
* @file                           FT_App_Music.ino
* @brief                          Sample application to demonstrat FT801 primitives, widgets and customized screen shots. 
* @version                        0.1.0
* @date                           2014/03/21
*
*/


#include "SPI.h"
#include "FT_VM801B43.h"



FT801IMPL_SPI FTImpl(FT_CS_PIN,FT_PDN_PIN,FT_INT_PIN);

uint32_t coprostatus = 0;
PROGMEM prog_uchar home_star_icon[] = {0x78,0x9C,0xE5,0x94,0xBF,0x4E,0xC2,0x40,0x1C,0xC7,0x7F,0x2D,0x04,0x8B,0x20,0x45,0x76,0x14,0x67,0xA3,0xF1,0x0D,0x64,0x75,0xD2,0xD5,0x09,0x27,0x17,0x13,0xE1,0x0D,0xE4,0x0D,0x78,0x04,0x98,0x5D,0x30,0x26,0x0E,0x4A,0xA2,0x3E,0x82,0x0E,0x8E,0x82,0xC1,0x38,0x62,0x51,0x0C,0x0A,0x42,0x7F,0xDE,0xB5,0x77,0xB4,0x77,0x17,0x28,0x21,0x26,0x46,0xFD,0x26,0xCD,0xE5,0xD3,0x7C,0xFB,0xBB,0xFB,0xFD,0xB9,0x02,0xCC,0xA4,0xE8,0x99,0x80,0x61,0xC4,0x8A,0x9F,0xCB,0x6F,0x31,0x3B,0xE3,0x61,0x7A,0x98,0x84,0x7C,0x37,0xF6,0xFC,0xC8,0xDD,0x45,0x00,0xDD,0xBA,0xC4,0x77,0xE6,0xEE,0x40,0xEC,0x0E,0xE6,0x91,0xF1,0xD2,0x00,0x42,0x34,0x5E,0xCE,0xE5,0x08,0x16,0xA0,0x84,0x68,0x67,0xB4,0x86,0xC3,0xD5,0x26,0x2C,0x20,0x51,0x17,0xA2,0xB8,0x03,0xB0,0xFE,0x49,0xDD,0x54,0x15,0xD8,0xEE,0x73,0x37,0x95,0x9D,0xD4,0x1A,0xB7,0xA5,0x26,0xC4,0x91,0xA9,0x0B,0x06,0xEE,0x72,0xB7,0xFB,0xC5,0x16,0x80,0xE9,0xF1,0x07,0x8D,0x3F,0x15,0x5F,0x1C,0x0B,0xFC,0x0A,0x90,0xF0,0xF3,0x09,0xA9,0x90,0xC4,0xC6,0x37,0xB0,0x93,0xBF,0xE1,0x71,0xDB,0xA9,0xD7,0x41,0xAD,0x46,0xEA,0x19,0xA9,0xD5,0xCE,0x93,0xB3,0x35,0x73,0x0A,0x69,0x59,0x91,0xC3,0x0F,0x22,0x1B,0x1D,0x91,0x13,0x3D,0x91,0x73,0x43,0xF1,0x6C,0x55,0xDA,0x3A,0x4F,0xBA,0x25,0xCE,0x4F,0x04,0xF1,0xC5,0xCF,0x71,0xDA,0x3C,0xD7,0xB9,0xB2,0x48,0xB4,0x89,0x38,0x20,0x4B,0x2A,0x95,0x0C,0xD5,0xEF,0x5B,0xAD,0x96,0x45,0x8A,0x41,0x96,0x7A,0x1F,0x60,0x0D,0x7D,0x22,0x75,0x82,0x2B,0x0F,0xFB,0xCE,0x51,0x3D,0x2E,0x3A,0x21,0xF3,0x1C,0xD9,0x38,0x86,0x2C,0xC6,0x05,0xB6,0x7B,0x9A,0x8F,0x0F,0x97,0x1B,0x72,0x6F,0x1C,0xEB,0xAE,0xFF,0xDA,0x97,0x0D,0xBA,0x43,0x32,0xCA,0x66,0x34,0x3D,0x54,0xCB,0x24,0x9B,0x43,0xF2,0x70,0x3E,0x42,0xBB,0xA0,0x95,0x11,0x37,0x46,0xE1,0x4F,0x49,0xC5,0x1B,0xFC,0x3C,0x3A,0x3E,0xD1,0x65,0x0E,0x6F,0x58,0xF8,0x9E,0x5B,0xDB,0x55,0xB6,0x41,0x34,0xCB,0xBE,0xDB,0x87,0x5F,0xA9,0xD1,0x85,0x6B,0xB3,0x17,0x9C,0x61,0x0C,0x9B,0xA2,0x5D,0x61,0x10,0xED,0x2A,0x9B,0xA2,0x5D,0x61,0x10,0xED,0x2A,0x9B,0xA2,0x5D,0x61,0x10,0xED,0x2A,0x9B,0xED,0xC9,0xFC,0xDF,0x14,0x54,0x8F,0x80,0x7A,0x06,0xF5,0x23,0xA0,0x9F,0x41,0xF3,0x10,0x30,0x4F,0x41,0xF3,0x18,0x30,0xCF,0xCA,0xFC,0xFF,0x35,0xC9,0x79,0xC9,0x89,0xFA,0x33,0xD7,0x1D,0xF6,0x5E,0x84,0x5C,0x56,0x6E,0xA7,0xDA,0x1E,0xF9,0xFA,0xAB,0xF5,0x97,0xFF,0x2F,0xED,0x89,0x7E,0x29,0x9E,0xB4,0x9F,0x74,0x1E,0x69,0xDA,0xA4,0x9F,0x81,0x94,0xEF,0x4F,0xF6,0xF9,0x0B,0xF4,0x65,0x51,0x08};

char *info[] =  {  "FT801 Music Application",
                   "APP to demonstrate interactive Piano,",
                   "using Rectangles, Buttons,Slider ",
                   "&synthesized Sound"
                }; 

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


struct 
{
    uint8_t sound : 1;
}Flag;



void Music_Keys(uint8_t no,uint16_t xx,uint16_t y,uint8_t btspace,uint8_t btw,uint8_t bth,uint8_t tag_no,uint32_t fgc1,uint32_t fgc2,uint8_t rtag)
{
  uint16_t x,z;	
  for(z=0;z<no;z++)
  {
    x = z*(btw+btspace)+xx;								
    rtag==(z+tag_no) ? FTImpl.Cmd_FGColor(fgc1) : FTImpl.Cmd_FGColor(fgc2);
    FTImpl.Tag(z+tag_no);	
    FTImpl.Cmd_Button(x,y,btw,bth,16,FT_OPT_FLAT,"");		
  }
}


void Music(void)
{
  uint8_t  instr,Mkeys,Skeys,temp_keys,
           z,button_space,
	       button_w,button_h,
	       Volume,b1w,b1h,b1s,pk;

  uint32_t Read_Tag_val;
  uint16_t x,y,n,temp;
	sTrackTag sTracktag;

  const char *ins[] = {"Harp","Xylophone","Tuba","Glockenspiel","Organ","Trumpet","Piano","Chimes","Musicbox","Ding" };
// Intial stage
  instr = 0x40;
  Volume = 100;
  button_space = 2;						// space between buttons					
  button_w = (FT_DISPLAYWIDTH / 10)-button_space;			// Button width	
  button_h = (FT_DISPLAYHEIGHT - (FT_DISPLAYHEIGHT/3));		// Button Height		
  b1w = button_w/2;                                             // button width one
  b1h = button_h/2;
  b1s = b1w+button_space;
 // Set the tracker for the Volume adjuster 
  FTImpl.Cmd_DLStart();      
  FTImpl.Cmd_Track(200,20,100,10,'V');   
  FTImpl.Finish();
  FTImpl.Write(REG_VOL_SOUND,100);
  do
  {
    FTImpl.Cmd_DLStart();                                    // start		
    FTImpl.Clear(1,1,1);
    FTImpl.TagMask(1);
    FTImpl.Tag(0);
    FTImpl.Cmd_Gradient(0,0,0x0,480,0,0x505050);
// for music keys
    Music_Keys(10,0,100,2,button_w,button_h,1,0xA0A080,0xFFFFFF,Mkeys);    // noofbuttons,xaxis,yaxis,bs,bw,bh,starting tagno,fgcolor1,fgcolor2,current tag
    Music_Keys(2,button_w*0.782,98,b1s,b1w,b1h,11,0xA0A080,0,Mkeys);
    Music_Keys(3,(3*(button_w+2))+(button_w*0.782),98,b1s,b1w,b1h,13,0xA0A080,0,Mkeys);
    Music_Keys(2,(7*(button_w+2))+(button_w*0.782),98,b1s,b1w,b1h,16,0xA0A080,0,Mkeys);
// Set the Background colors		
    FTImpl.Cmd_BGColor(0x402000);
    FTImpl.Cmd_FGColor(0x703800);
    FTImpl.Tag('I');
    FTImpl.Cmd_Button(50,10,100,30,26,(keypressed=='I')?FT_OPT_FLAT:0,ins[instr-0x40]);      // view button
    
    FTImpl.Tag('V');	
    FTImpl.Cmd_Slider(200,20,100,10,0,Volume,255);
    
    FTImpl.Cmd_Text(155,20,26,FT_OPT_CENTERY,"Vol -");
    FTImpl.Cmd_Text(305,15,26,FT_OPT_CENTERY,"+");
    FTImpl.Cmd_Keys(0,50,FT_DISPLAYWIDTH,40,28,Mkeys|FT_OPT_CENTER,"123456789");
// Sound Selection screen
    if(Flag.sound)	
    {
      FTImpl.Clear(0,0,1);	
      FTImpl.SaveContext();
      FTImpl.ColorRGB(0,0,0);
      FTImpl.ColorA(175);
      FTImpl.Begin(FT_RECTS);
      FTImpl.Vertex2ii(0,0,0,0);
      FTImpl.Vertex2ii(FT_DISPLAYWIDTH,FT_DISPLAYHEIGHT,0,0);
      FTImpl.End();
      FTImpl.RestoreContext();
      x = (uint16_t)(FT_DISPLAYWIDTH*0.270);
      uint16_t dt = 130; 
	  #if (FT_DISPLAY_RESOLUTION == FT_DISPLAY_QVGA_320x240)
      dt = 50;
      #endif
	  
// Draw the sound selection menu bar        
      for(z=0;z<10;z++)
      {
        y = 10+(40*(z%5));						// space bw keys in col 
        x = dt+(120*(z/5));
		FTImpl.Tag(z+100);	//Tagval from 100
		FTImpl.Cmd_Button(x,y,100,30,26,(keypressed==z+100)?FT_OPT_FLAT:0,ins[z]);
      }
    }			
    FTImpl.TagMask(0);				// Disable the tag updation
    FTImpl.Display();
    FTImpl.Cmd_Swap();
    FTImpl.Finish();
		
//Sound change  
    Skeys = Read_Keys();
    Mkeys = FTImpl.Read(REG_TOUCH_TAG);
    if((sTracktag.tag)=='V'){Mkeys = 0;Skeys =0;}
    if(Skeys=='I'){Flag.sound  = 1;
    Mkeys = 0;}
    if(Skeys>=100 && Flag.sound==1)          //Tagval from 100
    {	
      Flag.sound  = 0;
      pk = (Skeys-100);		// assign the selection
      instr = pk|0x40;
    }	
 // Play the sound   
    if(!Flag.sound && Mkeys!='I')
    {
      if(Mkeys > '0' && temp_keys!=Mkeys)
      {
		  FTImpl.Write16(REG_SOUND, 0x50 + (Mkeys-'0'));
		  FTImpl.Write(REG_PLAY,1);
      }
      if(Mkeys !=0 &&  Mkeys < 18 && temp_keys!=Mkeys && Flag.sound==0)
      {	
	  FTImpl.Write16(REG_SOUND,((Mkeys |60) << 8) | instr);
	  FTImpl.Write(REG_PLAY,1);
      }
	  FTImpl.GetTrackTag(sTracktag);
      if((sTracktag.tag)=='V' && Flag.sound==0)
      {		
          Volume = (sTracktag.track>>8);
          FTImpl.Write(REG_VOL_SOUND,Volume);
      }
    }
    temp_keys=Mkeys;
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
		home_setup();
		Info();
		Music();   
		FTImpl.Exit();
	}
	Serial.println("--End Application--");
}

void loop()
{
}
/* Nothing beyond this */