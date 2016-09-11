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
* @file                           FT_App_Keyboard.ino
* @brief                          Sample application to demonstrate FT801 primitives, widgets and customized screen shots 
* @version                        0.1.0
* @date                           2014/03/21
*
*/


#include "SPI.h"
#include "Wire.h"
#include "FT_NHD_43CTP_SHIELD.h"

              


#define ON          1
#define OFF         0						 
#define Font        27					// Font Size
#define MAX_FT_LINES   4					// Max FT_LINES allows to Display

#define SPECIAL_FUN     251
#define BACK_SPACE	251				// Back space
#define CAPS_LOCK	252				// Caps Lock
#define NUMBER_LOCK	253				// Number Lock
#define BACK		254				// Exit 

#define LINE_STARTPOS	FT_DISPLAYWIDTH/50			// Start of Line
#define LINE_ENDPOS	FT_DISPLAYWIDTH			// max length of the line

/* Global object for transport */
FT801IMPL_SPI FTImpl(FT_CS_PIN,FT_PDN_PIN,FT_INT_PIN);


/* deflated icons*/
static uint8_t home_star_icon[] = {0x78,0x9C,0xE5,0x94,0xBF,0x4E,0xC2,0x40,0x1C,0xC7,0x7F,0x2D,0x04,0x8B,0x20,0x45,0x76,0x14,0x67,0xA3,0xF1,0x0D,0x64,0x75,0xD2,0xD5,0x09,0x27,0x17,0x13,0xE1,0x0D,0xE4,0x0D,0x78,0x04,0x98,0x5D,0x30,0x26,0x0E,0x4A,0xA2,0x3E,0x82,0x0E,0x8E,0x82,0xC1,0x38,0x62,0x51,0x0C,0x0A,0x42,0x7F,0xDE,0xB5,0x77,0xB4,0x77,0x17,0x28,0x21,0x26,0x46,0xFD,0x26,0xCD,0xE5,0xD3,0x7C,0xFB,0xBB,0xFB,0xFD,0xB9,0x02,0xCC,0xA4,0xE8,0x99,0x80,0x61,0xC4,0x8A,0x9F,0xCB,0x6F,0x31,0x3B,0xE3,0x61,0x7A,0x98,0x84,0x7C,0x37,0xF6,0xFC,0xC8,0xDD,0x45,0x00,0xDD,0xBA,0xC4,0x77,0xE6,0xEE,0x40,0xEC,0x0E,0xE6,0x91,0xF1,0xD2,0x00,0x42,0x34,0x5E,0xCE,0xE5,0x08,0x16,0xA0,0x84,0x68,0x67,0xB4,0x86,0xC3,0xD5,0x26,0x2C,0x20,0x51,0x17,0xA2,0xB8,0x03,0xB0,0xFE,0x49,0xDD,0x54,0x15,0xD8,0xEE,0x73,0x37,0x95,0x9D,0xD4,0x1A,0xB7,0xA5,0x26,0xC4,0x91,0xA9,0x0B,0x06,0xEE,0x72,0xB7,0xFB,0xC5,0x16,0x80,0xE9,0xF1,0x07,0x8D,0x3F,0x15,0x5F,0x1C,0x0B,0xFC,0x0A,0x90,0xF0,0xF3,0x09,0xA9,0x90,0xC4,0xC6,0x37,0xB0,0x93,0xBF,0xE1,0x71,0xDB,0xA9,0xD7,0x41,0xAD,0x46,0xEA,0x19,0xA9,0xD5,0xCE,0x93,0xB3,0x35,0x73,0x0A,0x69,0x59,0x91,0xC3,0x0F,0x22,0x1B,0x1D,0x91,0x13,0x3D,0x91,0x73,0x43,0xF1,0x6C,0x55,0xDA,0x3A,0x4F,0xBA,0x25,0xCE,0x4F,0x04,0xF1,0xC5,0xCF,0x71,0xDA,0x3C,0xD7,0xB9,0xB2,0x48,0xB4,0x89,0x38,0x20,0x4B,0x2A,0x95,0x0C,0xD5,0xEF,0x5B,0xAD,0x96,0x45,0x8A,0x41,0x96,0x7A,0x1F,0x60,0x0D,0x7D,0x22,0x75,0x82,0x2B,0x0F,0xFB,0xCE,0x51,0x3D,0x2E,0x3A,0x21,0xF3,0x1C,0xD9,0x38,0x86,0x2C,0xC6,0x05,0xB6,0x7B,0x9A,0x8F,0x0F,0x97,0x1B,0x72,0x6F,0x1C,0xEB,0xAE,0xFF,0xDA,0x97,0x0D,0xBA,0x43,0x32,0xCA,0x66,0x34,0x3D,0x54,0xCB,0x24,0x9B,0x43,0xF2,0x70,0x3E,0x42,0xBB,0xA0,0x95,0x11,0x37,0x46,0xE1,0x4F,0x49,0xC5,0x1B,0xFC,0x3C,0x3A,0x3E,0xD1,0x65,0x0E,0x6F,0x58,0xF8,0x9E,0x5B,0xDB,0x55,0xB6,0x41,0x34,0xCB,0xBE,0xDB,0x87,0x5F,0xA9,0xD1,0x85,0x6B,0xB3,0x17,0x9C,0x61,0x0C,0x9B,0xA2,0x5D,0x61,0x10,0xED,0x2A,0x9B,0xA2,0x5D,0x61,0x10,0xED,0x2A,0x9B,0xA2,0x5D,0x61,0x10,0xED,0x2A,0x9B,0xED,0xC9,0xFC,0xDF,0x14,0x54,0x8F,0x80,0x7A,0x06,0xF5,0x23,0xA0,0x9F,0x41,0xF3,0x10,0x30,0x4F,0x41,0xF3,0x18,0x30,0xCF,0xCA,0xFC,0xFF,0x35,0xC9,0x79,0xC9,0x89,0xFA,0x33,0xD7,0x1D,0xF6,0x5E,0x84,0x5C,0x56,0x6E,0xA7,0xDA,0x1E,0xF9,0xFA,0xAB,0xF5,0x97,0xFF,0x2F,0xED,0x89,0x7E,0x29,0x9E,0xB4,0x9F,0x74,0x1E,0x69,0xDA,0xA4,0x9F,0x81,0x94,0xEF,0x4F,0xF6,0xF9,0x0B,0xF4,0x65,0x51,0x08};


const char *info[] = { "FT801 KeyBoard Application",

                         "APP to demonstrate interactive Key Board,",
                         "using String, Keys",
                         "& Buttons."

                       }; 



struct 
{
	uint8_t Key_Detect :1;
	uint8_t Caps :1;
	uint8_t Numeric : 1;
	uint8_t Exit : 1;	
}Flag;


// Buffer for the notepads
struct Notepad_buffer
{
	char *temp;
	char  notepad[MAX_FT_LINES][80];
}Buffer;


static uint8_t sk=0;
/********API to return the assigned TAG value when penup,for the primitives/widgets******/
uint8_t Read_keys()
{
  static uint8_t Read_tag=0,temp_tag=0,ret_tag=0;	
  Read_tag = FTImpl.Read(REG_TOUCH_TAG);
  ret_tag = 0;
  if(Read_tag!=0 && temp_tag!=Read_tag)
  {
     temp_tag = Read_tag;											// Load the Read tag to temp variable	
     sk = Read_tag;			
  } 
  if(Read_tag==0)
  {
     ret_tag =  temp_tag;
     temp_tag = 0;
     sk = 0;
  }
  return ret_tag;
}


uint8_t Read_Keypad()
{
  static uint8_t Read_tag=0,temp_tag=0,ret_tag=0,touch_detect=1;	
  Read_tag = FTImpl.Read(REG_TOUCH_TAG);
  ret_tag = 0;
  if(FTImpl.IsPendown()==0)  touch_detect = 0;
  if(Read_tag!=0)								// Allow if the Key is released
  {
    if(temp_tag!=Read_tag && touch_detect==0)
    {
      temp_tag = Read_tag;											// Load the Read tag to temp variable	
      FTImpl.PlaySound(100,0x51);
      touch_detect = 1;
    }
  }
  else
  {
    if(temp_tag!=0)
    {	
      Flag.Key_Detect = 1;
      Read_tag = temp_tag;
    }
    temp_tag = 0;
  }
  return Read_tag;
}

// API used to calculate the width of the character
uint8_t Ft_Gpu_Rom_Font_WH(uint8_t Char,uint8_t font)
{
	uint32_t ptr,Wptr;
	uint8_t Width=0;
	ptr = FTImpl.Read32(0xffffc);						
// Read Width of the character
	Wptr = (ptr + (148 * (font- 16)))+Char;	// (table starts at font 16)
	Width = FTImpl.Read(Wptr);
	return Width;
}

// Notepad buffer
void Notepad(void)
{
  /*local variables*/
  uint8_t Line=0;
  uint16_t Disp_pos = 0,But_opt;
  uint8_t Read_sfk=0,	tval;
  uint16_t noofchars=0,line2disp =0,nextline = 0;	
  uint8_t   font = 27;

//Clear Linebuffer
  for(tval=0;tval<MAX_FT_LINES;tval++)
  memset(&Buffer.notepad[tval],'\0',sizeof(Buffer.notepad[tval]));

/*intial setup*/
  Line = 0;					// Starting line
  Disp_pos=LINE_STARTPOS;	                        // starting pos                                                      
  Flag.Numeric = OFF;                             // Disable the numbers and spcial charaters
  memset((Buffer.notepad[Line]+0),'_',1);	  	// For Cursor	
  Disp_pos += Ft_Gpu_Rom_Font_WH(Buffer.notepad[Line][0],Font);	// Update the Disp_Pos
  noofchars+=1;                                                   // for cursor
/*enter*/
  Flag.Exit = 0;
  do
  {
    Read_sfk = Read_Keypad();                // read the keys   
     
    if(Flag.Key_Detect)
    {                    // check if key is pressed
    Flag.Key_Detect = 0;                     // clear it  
    if(Read_sfk >= SPECIAL_FUN)
    {              // check any special function keys are pressed 
      switch(Read_sfk)
      {
        case BACK_SPACE:
          if(noofchars>1)  // check in the line there is any characters are present,cursor not include
          {
	    noofchars-=1;                      // clear the character inthe buffer
	    Disp_pos -= Ft_Gpu_Rom_Font_WH(*(Buffer.notepad[Line]+noofchars-1),Font); // Update the Disp_Pos										
          }else 
          {
            if(Line >= (MAX_FT_LINES-1)) Line--; else Line=0;              // check the FT_LINES
            noofchars = strlen(Buffer.notepad[Line]);		    // Read the len of the line 
            for(tval=0;tval<noofchars;tval++)			     // Compute the length of the Line
            Disp_pos += Ft_Gpu_Rom_Font_WH(Buffer.notepad[Line][tval],Font);  // Update the Disp_Pos
          }
          Buffer.temp = (Buffer.notepad[Line]+noofchars);     // load into temporary buffer         									
          Buffer.temp[-1] = '_';				  // update the string  							
          Buffer.temp[0] = '\0';	
        break;

        case CAPS_LOCK:
          Flag.Caps ^= 1;        // toggle the caps lock on when the key detect
        break;
      
        case NUMBER_LOCK:
          Flag.Numeric ^= 1;    // toggle the number lock on when the key detect
        break;

        case BACK:
          for(tval=0;tval<MAX_FT_LINES;tval++)
          memset(&Buffer.notepad[tval],'\0',sizeof(Buffer.notepad[tval]));    
          Line = 0;					// Starting line
          Disp_pos=LINE_STARTPOS;	                        // starting pos                                                      
   //       Flag.Numeric = OFF;                             // Disable the numbers and spcial charaters
          memset((Buffer.notepad[Line]+0),'_',1);	  	// For Cursor	
          Disp_pos += Ft_Gpu_Rom_Font_WH(Buffer.notepad[Line][0],Font);	// Update the Disp_Pos
          noofchars+=1;                                                
        break;
      }
    }
    else 
    {
  // String and Line Termination    
      Disp_pos += Ft_Gpu_Rom_Font_WH(Read_sfk,Font);              // update dispos
      Buffer.temp = Buffer.notepad[Line]+strlen(Buffer.notepad[Line]);  // load into temporary buffer 
      Buffer.temp[-1] = Read_sfk;		 // update the string  									
      Buffer.temp[0] = '_';													
      Buffer.temp[1] = '\0';	
      noofchars = strlen(Buffer.notepad[Line]);    // get the string len
      if(Disp_pos > LINE_ENDPOS)                  // check if there is place to put a character in a specific line
      {
	Buffer.temp = Buffer.notepad[Line]+(strlen(Buffer.notepad[Line])-1);					
	Buffer.temp[0] = '\0';
	noofchars-=1;
	Disp_pos = LINE_STARTPOS;
	Line++;	if(Line >= MAX_FT_LINES)	Line = 0;													
	memset((Buffer.notepad[Line]),'\0',sizeof(Buffer.notepad[Line]));	// Clear the line buffer
	for(;noofchars>=1;noofchars--,tval++)
	{
	  if(Buffer.notepad[Line-1][noofchars] == ' ' ||Buffer.notepad[Line-1][noofchars] =='.')// In case of space(New String) or end of statement(.)
	  {
            memset(Buffer.notepad[Line-1]+noofchars,'\0',1);
	    noofchars+=1;							// Include the space			
	    memcpy(&Buffer.notepad[Line],(Buffer.notepad[Line-1]+noofchars),tval);
	    break;
	  }
	}
	noofchars = strlen(Buffer.notepad[Line]);
	Buffer.temp = Buffer.notepad[Line]+noofchars;	
	Buffer.temp[0] = '_';	
	Buffer.temp[1] = '\0';						
	for(tval=0;tval<noofchars;tval++)									
	Disp_pos += Ft_Gpu_Rom_Font_WH(Buffer.notepad[Line][tval],Font);	// Update the Disp_Pos
      }
    }
  }
		
// Start the new Display list
  FTImpl.DLStart();
  FTImpl.ClearColorRGB(100,100,100);        
  FTImpl.Clear(1,1,1);	
  FTImpl.ColorRGB(255,255,255);
  FTImpl.TagMask(1);            // enable tag buffer updation
  FTImpl.Cmd_FGColor(0x703800);  
  FTImpl.Cmd_BGColor(0x703800);
  But_opt = (Read_sfk== BACK)?  FT_OPT_FLAT:0;          // button color change if the button during press
  FTImpl.Tag(BACK);														// Back		 Return to Home
  FTImpl.Cmd_Button((FT_DISPLAYWIDTH*0.855),(FT_DISPLAYHEIGHT*0.83),(FT_DISPLAYWIDTH*0.146),(FT_DISPLAYHEIGHT*0.112),font,But_opt,"Clear");	
  But_opt = (Read_sfk==BACK_SPACE)? FT_OPT_FLAT:0;
  FTImpl.Tag(BACK_SPACE);													// BackSpace
  FTImpl.Cmd_Button((FT_DISPLAYWIDTH*0.875),(FT_DISPLAYHEIGHT*0.70),(FT_DISPLAYWIDTH*0.125),(FT_DISPLAYHEIGHT*0.112),font,But_opt,"<-");	
  But_opt = (Read_sfk==' ')? FT_OPT_FLAT:0;
  FTImpl.Tag(' ');															// Space
  FTImpl.Cmd_Button((FT_DISPLAYWIDTH*0.115),(FT_DISPLAYHEIGHT*0.83),(FT_DISPLAYWIDTH*0.73),(FT_DISPLAYHEIGHT*0.112),font,But_opt,"Space");

  if(Flag.Numeric==OFF)
  {
    FTImpl.Cmd_Keys(0,(FT_DISPLAYHEIGHT*0.442),FT_DISPLAYWIDTH,(FT_DISPLAYHEIGHT*0.112),font,Read_sfk,(Flag.Caps == ON ?"QWERTYUIOP":"qwertyuiop"));
    FTImpl.Cmd_Keys((FT_DISPLAYWIDTH*0.042),(FT_DISPLAYHEIGHT*0.57),(FT_DISPLAYWIDTH*0.96),(FT_DISPLAYHEIGHT*0.112),font,Read_sfk,(Flag.Caps == ON ?"ASDFGHJKL":"asdfghjkl"));
    FTImpl.Cmd_Keys((FT_DISPLAYWIDTH*0.125),(FT_DISPLAYHEIGHT*0.70),(FT_DISPLAYWIDTH*0.73),(FT_DISPLAYHEIGHT*0.112),font,Read_sfk,(Flag.Caps == ON ?"ZXCVBNM":"zxcvbnm"));	

    But_opt = (Read_sfk== CAPS_LOCK)? FT_OPT_FLAT:0;
    FTImpl.Tag(CAPS_LOCK);																	// Capslock
    FTImpl.Cmd_Button(0,(FT_DISPLAYHEIGHT*0.70),(FT_DISPLAYWIDTH*0.10),(FT_DISPLAYHEIGHT*0.112),font,But_opt,"a^");
    But_opt = (Read_sfk== NUMBER_LOCK)? FT_OPT_FLAT:0;
    FTImpl.Tag(NUMBER_LOCK);																// Numberlock		
    FTImpl.Cmd_Button(0,(FT_DISPLAYHEIGHT*0.83),(FT_DISPLAYWIDTH*0.10),(FT_DISPLAYHEIGHT*0.112),font,But_opt,"12*");
  }
  if(Flag.Numeric==ON)
  {		
    FTImpl.Cmd_Keys((FT_DISPLAYWIDTH*0),(FT_DISPLAYHEIGHT*0.442),FT_DISPLAYWIDTH,(FT_DISPLAYHEIGHT*0.112),font,Read_sfk,"1234567890");
    FTImpl.Cmd_Keys((FT_DISPLAYWIDTH*0.042),(FT_DISPLAYHEIGHT*0.57),(FT_DISPLAYWIDTH*0.96),(FT_DISPLAYHEIGHT*0.112),font,Read_sfk,"-@#$%^&*(");
    FTImpl.Cmd_Keys((FT_DISPLAYWIDTH*0.125),(FT_DISPLAYHEIGHT*0.70),(FT_DISPLAYWIDTH*0.73),(FT_DISPLAYHEIGHT*0.112),font,Read_sfk,")_+[]{}");				
    But_opt = (Read_sfk== NUMBER_LOCK)? FT_OPT_FLAT:0;
    FTImpl.Tag(253);													// Numberlock
    FTImpl.Cmd_Button(0,(FT_DISPLAYHEIGHT*0.83),(FT_DISPLAYWIDTH*0.10),(FT_DISPLAYHEIGHT*0.112),font,But_opt,"AB*");
  }
  FTImpl.TagMask(0);													// Disable the tag buffer updates
  FTImpl.ScissorXY(0,0);
  FTImpl.ScissorSize(FT_DISPLAYWIDTH,(uint16_t)(FT_DISPLAYHEIGHT*0.405));	
  FTImpl.ClearColorRGB(255,255,255);
  FTImpl.Clear(1,1,1);
  FTImpl.ColorRGB(0,0,0);												// Text Color
  line2disp = 0;
  while(line2disp <= Line)
  {
    nextline = 3 + (line2disp * (FT_DISPLAYHEIGHT*.073));
    FTImpl.Cmd_Text(line2disp,nextline,font,0,(const char*)&Buffer.notepad[line2disp]);
    line2disp++;
  }
  FTImpl.DLEnd();
  FTImpl.Finish();
 }while(1);		

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
  FTImpl.DLStart();       // start
  FTImpl.Clear(1,1,1);
  FTImpl.BitmapHandle(13);    // handle for background stars
  FTImpl.BitmapSource(250*1024L);      // Starting address in gram
  FTImpl.BitmapLayout(FT_L4, 16, 32);  // format 
  FTImpl.BitmapSize(FT_NEAREST, FT_REPEAT, FT_REPEAT, 512, 512  );
  FTImpl.BitmapHandle(14);    // handle for background stars
  FTImpl.BitmapSource(250*1024L);      // Starting address in gram
  FTImpl.BitmapLayout(FT_L4, 16, 32);  // format 
  FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, 32, 32);

  FTImpl.DLEnd();
  FTImpl.Finish();
}
/* Information API*/
void Info()
{
  uint32_t status;
  uint16_t dloffset = 0,z;

  /* Touch Screen Calibration */
  FTImpl.DLStart();
  FTImpl.Clear(1,1,1);
  FTImpl.ColorRGB(255,255,255);
  FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,FT_DISPLAYHEIGHT/2,26,FT_OPT_CENTERX|FT_OPT_CENTERY,"Please tap on a dot");
  FTImpl.Cmd_Calibrate(0);
  FTImpl.DLEnd();
  FTImpl.Finish();
  // Ftdi Logo animation  
  FTImpl.Cmd_Logo();
  do{
  status=FTImpl.CheckLogo();
  }while(status!=FT_GE_FINISHED);
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
    FTImpl.Begin((FT_EDGE_STRIP_A));
    FTImpl.Vertex2f(0,FT_DISPLAYHEIGHT*16);
    FTImpl.Vertex2f(FT_DISPLAYWIDTH*16,FT_DISPLAYHEIGHT*16);
    FTImpl.ColorA((255));
    FTImpl.RestoreContext();	
    FTImpl.ColorRGB(0,0,0);
   // INFORMATION 
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,20,28,FT_OPT_CENTERX|FT_OPT_CENTERY,info[0]);
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,60,26,FT_OPT_CENTERX|FT_OPT_CENTERY,info[1]);
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,90,26,FT_OPT_CENTERX|FT_OPT_CENTERY,info[2]);  
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,120,26,FT_OPT_CENTERX|FT_OPT_CENTERY,info[3]);  
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,FT_DISPLAYHEIGHT-30,26,FT_OPT_CENTERX|FT_OPT_CENTERY,"Click to play");
    //Check if the Play key and change the color
    if(sk!='P')
    FTImpl.ColorRGB(255,255,255);
    else
    FTImpl.ColorRGB(100,100,100);
    FTImpl.Begin((FT_POINTS));   
    FTImpl.PointSize(20*16);
    FTImpl.Tag('P');
    FTImpl.Vertex2f((FT_DISPLAYWIDTH/2)*16,(FT_DISPLAYHEIGHT-60)*16);
    FTImpl.ColorRGB(180,35,35);
    FTImpl.Begin((FT_BITMAPS));
    FTImpl.Vertex2ii((FT_DISPLAYWIDTH/2)-14,(FT_DISPLAYHEIGHT-75),14,4);
    FTImpl.DLEnd();
	FTImpl.Finish();
  }while(Read_keys()!='P');
  /* wait until Play key is not pressed*/ 
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
		Notepad();
		FTImpl.Exit();
	}
	Serial.println("--End Application--");
}

void loop()
{
	
}


/* Nothing beyond this */

















