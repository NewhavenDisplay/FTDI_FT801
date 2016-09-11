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
* @file                           FT_App_Metaball.ino
* @brief                          Sample application to demonstrat FT801 primitives, widgets and customized screen shots  
* @version                        0.1.0
* @date                           2014/03/21
*
*/


#include "SPI.h"
#include "Wire.h"
#include "FT_VM801P43_50.h"

#define F16(s)        ((int32_t)((s) * 65536))
#define SQ(v) (v*v)

 
// deflated icons
 PROGMEM prog_uchar home_star_icon[] = {0x78,0x9C,0xE5,0x94,0xBF,0x4E,0xC2,0x40,0x1C,0xC7,0x7F,0x2D,0x04,0x8B,0x20,0x45,0x76,0x14,0x67,0xA3,0xF1,0x0D,0x64,0x75,0xD2,0xD5,0x09,0x27,0x17,0x13,0xE1,0x0D,0xE4,0x0D,0x78,0x04,0x98,0x5D,0x30,0x26,0x0E,0x4A,0xA2,0x3E,0x82,0x0E,0x8E,0x82,0xC1,0x38,0x62,0x51,0x0C,0x0A,0x42,0x7F,0xDE,0xB5,0x77,0xB4,0x77,0x17,0x28,0x21,0x26,0x46,0xFD,0x26,0xCD,0xE5,0xD3,0x7C,0xFB,0xBB,0xFB,0xFD,0xB9,0x02,0xCC,0xA4,0xE8,0x99,0x80,0x61,0xC4,0x8A,0x9F,0xCB,0x6F,0x31,0x3B,0xE3,0x61,0x7A,0x98,0x84,0x7C,0x37,0xF6,0xFC,0xC8,0xDD,0x45,0x00,0xDD,0xBA,0xC4,0x77,0xE6,0xEE,0x40,0xEC,0x0E,0xE6,0x91,0xF1,0xD2,0x00,0x42,0x34,0x5E,0xCE,0xE5,0x08,0x16,0xA0,0x84,0x68,0x67,0xB4,0x86,0xC3,0xD5,0x26,0x2C,0x20,0x51,0x17,0xA2,0xB8,0x03,0xB0,0xFE,0x49,0xDD,0x54,0x15,0xD8,0xEE,0x73,0x37,0x95,0x9D,0xD4,0x1A,0xB7,0xA5,0x26,0xC4,0x91,0xA9,0x0B,0x06,0xEE,0x72,0xB7,0xFB,0xC5,0x16,0x80,0xE9,0xF1,0x07,0x8D,0x3F,0x15,0x5F,0x1C,0x0B,0xFC,0x0A,0x90,0xF0,0xF3,0x09,0xA9,0x90,0xC4,0xC6,0x37,0xB0,0x93,0xBF,0xE1,0x71,0xDB,0xA9,0xD7,0x41,0xAD,0x46,0xEA,0x19,0xA9,0xD5,0xCE,0x93,0xB3,0x35,0x73,0x0A,0x69,0x59,0x91,0xC3,0x0F,0x22,0x1B,0x1D,0x91,0x13,0x3D,0x91,0x73,0x43,0xF1,0x6C,0x55,0xDA,0x3A,0x4F,0xBA,0x25,0xCE,0x4F,0x04,0xF1,0xC5,0xCF,0x71,0xDA,0x3C,0xD7,0xB9,0xB2,0x48,0xB4,0x89,0x38,0x20,0x4B,0x2A,0x95,0x0C,0xD5,0xEF,0x5B,0xAD,0x96,0x45,0x8A,0x41,0x96,0x7A,0x1F,0x60,0x0D,0x7D,0x22,0x75,0x82,0x2B,0x0F,0xFB,0xCE,0x51,0x3D,0x2E,0x3A,0x21,0xF3,0x1C,0xD9,0x38,0x86,0x2C,0xC6,0x05,0xB6,0x7B,0x9A,0x8F,0x0F,0x97,0x1B,0x72,0x6F,0x1C,0xEB,0xAE,0xFF,0xDA,0x97,0x0D,0xBA,0x43,0x32,0xCA,0x66,0x34,0x3D,0x54,0xCB,0x24,0x9B,0x43,0xF2,0x70,0x3E,0x42,0xBB,0xA0,0x95,0x11,0x37,0x46,0xE1,0x4F,0x49,0xC5,0x1B,0xFC,0x3C,0x3A,0x3E,0xD1,0x65,0x0E,0x6F,0x58,0xF8,0x9E,0x5B,0xDB,0x55,0xB6,0x41,0x34,0xCB,0xBE,0xDB,0x87,0x5F,0xA9,0xD1,0x85,0x6B,0xB3,0x17,0x9C,0x61,0x0C,0x9B,0xA2,0x5D,0x61,0x10,0xED,0x2A,0x9B,0xA2,0x5D,0x61,0x10,0xED,0x2A,0x9B,0xA2,0x5D,0x61,0x10,0xED,0x2A,0x9B,0xED,0xC9,0xFC,0xDF,0x14,0x54,0x8F,0x80,0x7A,0x06,0xF5,0x23,0xA0,0x9F,0x41,0xF3,0x10,0x30,0x4F,0x41,0xF3,0x18,0x30,0xCF,0xCA,0xFC,0xFF,0x35,0xC9,0x79,0xC9,0x89,0xFA,0x33,0xD7,0x1D,0xF6,0x5E,0x84,0x5C,0x56,0x6E,0xA7,0xDA,0x1E,0xF9,0xFA,0xAB,0xF5,0x97,0xFF,0x2F,0xED,0x89,0x7E,0x29,0x9E,0xB4,0x9F,0x74,0x1E,0x69,0xDA,0xA4,0x9F,0x81,0x94,0xEF,0x4F,0xF6,0xF9,0x0B,0xF4,0x65,0x51,0x08};


PROGMEM char *info[] = {  "FT801 MetaBalls Application",
                  "APP to demonstrate interactive random MetaBalls,",
                  "using Points & FT_BITMAPS"
               };
			   
/* Global object for transport */
FT801IMPL_SPI FTImpl(FT_CS_PIN,FT_PDN_PIN,FT_INT_PIN);
			   
static int16_t v() 
{
  return random(64)-32;
}

			

void Metaball()
{

  uint8_t w = 31,h = 18,numBlobs = 80, *recip,fadein,f,temp[31];
				
  int32_t centerx = (16 * 16 * (w / 2)), centery = (16 * 16 * (h / 2)), touching, recipsz=(w*w + h*h)/4+1,tval,tval1,tval2,sx,sy,VEL,m,d,bx,by,dx,dy;
  struct 
  {
    int16_t x, y;
    int8_t dx, dy;
  } blobs[80];
  
// Compute the origin of the blobs  
  for (tval=0; tval<numBlobs; ++tval) 
  {
    blobs[tval].x = random(16 * FT_DISPLAYWIDTH);
    blobs[tval].y = random(16 * FT_DISPLAYHEIGHT);
    blobs[tval].dx = v();
    blobs[tval].dy = v();
  }

  recip = (uint8_t*)malloc(recipsz);
  for (tval = 0; tval < recipsz; tval++)
  {
    if (tval == 0 )
    {
      recip[tval] = 200;
    }else
    {
      recip[tval] = min(200, (FT_DISPLAYWIDTH * 10) / (4 * tval));
    }
  }
  fadein = 255;
  f = 0;

sTagXY tagXY;
  while(1)
  {
    {
 // check the touch     
     touching = FTImpl.IsPendown();
      if (touching) 
      {
        FTImpl.GetTagXY(tagXY);
	sx = tagXY.x;
	sy = tagXY.y;
		centerx = 16 * sx;
		centery = 16 * sy;
      } 
      else
      {
		centerx = FT_DISPLAYWIDTH * 16 / 2;
		centery = FT_DISPLAYHEIGHT * 16 / 2;
      }
    }
    // Change the moving speed of the balls
    VEL = touching ? 8 : 2;
    for (tval=0; tval<numBlobs; ++tval) 
    {
      if(blobs[tval].x < centerx)   
		blobs[tval].dx += VEL;    
	  else
		blobs[tval].dx -= VEL;
      if(blobs[tval].y < centery)
		blobs[tval].dy += VEL;   
	  else
		blobs[tval].dy -= VEL;
      blobs[tval].x += blobs[tval].dx << 3;
      blobs[tval].y += blobs[tval].dy << 3;
    }
    blobs[random(numBlobs)].dx = v();
    blobs[random(numBlobs)].dy = v();
    
   // Compute the fireballs and upload into GRAM 
    for (tval1 = 0; tval1 < h; tval1++) 
    {
      for (tval2 = 0; tval2 < w; tval2++) 
      {
	m = fadein;
	for (tval = 0; tval < 3; tval++)
	{
	  bx = blobs[tval].x >> 8;
	  by = blobs[tval].y >> 8;
	  dx = bx - tval2;
	  dy = by - tval1;
          d = SQ(dx) + SQ(dy);
	  m += recip[min(d >> 2, recipsz - 1)];
	}
  //      FTImpl.Write8((f << 12) + (tval1 << 6),min(m,255));
	temp[tval2] = min(m,255);
      }
      FTImpl.Write((f << 12) + (tval1 << 6),temp,w);        // slower
    }
    // Start the  new displaylist
    FTImpl.DLStart();       // start
    FTImpl.Clear(1,1,1);	
    FTImpl.BitmapSource(f << 12);
    FTImpl.BitmapLayout(FT_L8, 64, 64);
    FTImpl.BitmapSize(FT_BILINEAR, FT_BORDER, FT_BORDER, FT_DISPLAYWIDTH, FT_DISPLAYHEIGHT);	
    FTImpl.SaveContext();
    FTImpl.BitmapTransformA(0x100 / 16);
    FTImpl.BitmapTransformE(0x100 / 16);
    FTImpl.Begin(FT_BITMAPS);
// Draw the fireballs using aditive blending
    FTImpl.BlendFunc(FT_SRC_ALPHA, FT_ZERO);
    FTImpl.ColorRGB(255, 0, 0);
    FTImpl.Vertex2ii(0,0,0,0);   
    FTImpl.BlendFunc(FT_SRC_ALPHA, FT_ONE);

    FTImpl.ColorRGB(255, 255, 0);
    FTImpl.Vertex2ii(0,0,0,0);
    FTImpl.RestoreContext();
    FTImpl.ColorRGB(0, 0, 0);	
    FTImpl.Begin(FT_POINTS);
    for (tval = 3; tval < numBlobs; tval++)
    {
	FTImpl.PointSize(3 * tval);
	FTImpl.Vertex2f(blobs[tval].x, blobs[tval].y);
    }
    FTImpl.DLEnd();
	FTImpl.Finish();	
    fadein = max(fadein - 3, 1);
    f ^= 1;
  }
}

/********API to return the assigned TAG value when penup,for the primitives/widgets******/
static uint8_t sk=0;
uint8_t Read_Keys()
{
  static uint8_t Read_tag=0,temp_tag=0,ret_tag=0;	
  Read_tag = FTImpl.Read(REG_TOUCH_TAG);
  ret_tag = 0;
  if(Read_tag!=0)						// Allow if the Key is released
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


	   

void home_setup(){
   /*Icon  file is deflated use J1 Command to inflate the file and write into the GRAM*/
  FTImpl.WriteCmd(CMD_INFLATE);
  FTImpl.WriteCmd(250*1024L);
  FTImpl.WriteCmdfromflash(home_star_icon,sizeof(home_star_icon));
  /*Set the Bitmap properties for the ICONS*/ 	
  FTImpl.DLStart();        // start
  FTImpl.Clear(1,1,1);
  FTImpl.ColorRGB(255,255,255);
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


// API for Infoscreen
void Info()
{
uint32_t status;
  uint16_t dloffset = 0,z;
 // Ft_CmdBuffer_Index = 0;
  
// Touch Screen Calibration
  
  FTImpl.DLStart();
  FTImpl.Clear(1,1,1);
  FTImpl.ColorRGB(255,255,255);
  FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2, FT_DISPLAYHEIGHT/2,26,FT_OPT_CENTER, "Please tap on a dot");
  FTImpl.Cmd_Calibrate(0);
  FTImpl.DLEnd();
  FTImpl.Finish();
  // Ftdi Logo animation  
  FTImpl.Cmd_Logo();
  do{
  status=FTImpl.CheckLogo();
  }while(status!=FT_GE_FINISHED);
  //Copy the Displaylist from DL RAM to GRAM
  dloffset = FTImpl.Read16(REG_CMD_DL);
  dloffset -=4;
  FTImpl.WriteCmd(CMD_MEMCPY);
  FTImpl.WriteCmd(100000L);
  FTImpl.WriteCmd(FT_RAM_DL);
  FTImpl.WriteCmd(dloffset);
      //Enter into Info Screen
  do
  {
    FTImpl.DLStart();
    FTImpl.Cmd_Append(100000L,dloffset);
    //Reset the BITMAP properties used during Logo animation 
    FTImpl.BitmapTransformA((256));
    FTImpl.BitmapTransformB((0));
    FTImpl.BitmapTransformC((0));
    FTImpl.BitmapTransformD((0));
    FTImpl.BitmapTransformE((256));
    FTImpl.BitmapTransformF((0)); 
   //Display the information with transparent Logo using Edge Strip   
    FTImpl.SaveContext();	
    FTImpl.ColorRGB(219,180,150);
    FTImpl.ColorA((220));
    FTImpl.Begin(FT_EDGE_STRIP_A);
    FTImpl.Vertex2f(0,FT_DISPLAYHEIGHT*16);
    FTImpl.Vertex2f(FT_DISPLAYWIDTH*16,FT_DISPLAYHEIGHT*16);
    FTImpl.ColorA((255));
    FTImpl.RestoreContext();	
    FTImpl.ColorRGB(0,0,0);
   // INFORMATION 
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,20,28,FT_OPT_CENTER, (char*)pgm_read_word(&info[0]));
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,60,26,FT_OPT_CENTER, (char*)pgm_read_word(&info[1]));
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,90,26,FT_OPT_CENTER, (char*)pgm_read_word(&info[2]));  
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,FT_DISPLAYHEIGHT-30,26,FT_OPT_CENTER, "Click to play");
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
  /* wait until Play key is not pressed*/ 
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
		Metaball();   
		FTImpl.Exit();
	}
	Serial.println("--End Application--");
}

void loop(){
	
}