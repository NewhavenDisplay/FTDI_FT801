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
* @file                           FT_App_chineseFont.ino
* @brief                          Sample application to demonstrate FT801 custom fonts. 
* @version                        0.1.0
* @date                           2014/03/21
*
*/

#include "SPI.h"
#include "Wire.h"
#include "FT_VM801P43_50.h"




#define MAX_FT_LINES   8					// Max FT_LINES allows to Display

#if (FT_DISPLAY_RESOLUTION == FT_DISPLAY_WQVGA_480x272)
#define MAX_COLS    16
#elif (FT_DISPLAY_RESOLUTION == FT_DISPLAY_QVGA_320x240)
#define MAX_COLS    10
#endif

#define TRUE true
#define FALSE false
#define TOGGLE_BUTTON 250            //Toggle for Simplified and Traditional Chinese
#define BACK_SPACE	251				// Back space button
#define SPACE_BUTTON	252				// space button
#define ENTER_BUTTON    253             //Enter return button
#define INPUT_DONE		254				// Exit 

#define CURSOR_CELL_CODE   270




struct 
{
	uint8_t Display_candiate;
	uint8_t Key_Detect;
}Flag;


struct Notepad_buffer
{
	uint16_t  notepad[MAX_FT_LINES][MAX_COLS];
}Buffer;


static boolean showCursor = TRUE;
static uint16_t   sc_flag = 65535;//flag for toggling simplified chinease and traditional chinese

static uint16_t currentLine = 0,currentCol =0;


/* SD card object*/
FT_SD FtSd(FT_SD_CSPIN);

/* sd_present holds error values during initialization.  0 means no error and all other errors are non zero value */
FT_SDStatus sd_present; 

/* Global object for transport */
FT801IMPL_SPI FTImpl(FT_CS_PIN,FT_PDN_PIN,FT_INT_PIN);



char *info[] = {  "FT801 Font Application",
				   "APP to demo simplified and traditional chinese font", 
				   "using Button",
				   "& other primitives."
                }; 

PROGMEM prog_uchar home_star_icon[] = {0x78,0x9C,0xE5,0x94,0xBF,0x4E,0xC2,0x40,0x1C,0xC7,0x7F,0x2D,0x04,0x8B,0x20,0x45,0x76,0x14,0x67,0xA3,0xF1,0x0D,0x64,0x75,0xD2,0xD5,0x09,0x27,0x17,0x13,0xE1,0x0D,0xE4,0x0D,0x78,0x04,0x98,0x5D,0x30,0x26,0x0E,0x4A,0xA2,0x3E,0x82,0x0E,0x8E,0x82,0xC1,0x38,0x62,0x51,0x0C,0x0A,0x42,0x7F,0xDE,0xB5,0x77,0xB4,0x77,0x17,0x28,0x21,0x26,0x46,0xFD,0x26,0xCD,0xE5,0xD3,0x7C,0xFB,0xBB,0xFB,0xFD,0xB9,0x02,0xCC,0xA4,0xE8,0x99,0x80,0x61,0xC4,0x8A,0x9F,0xCB,0x6F,0x31,0x3B,0xE3,0x61,0x7A,0x98,0x84,0x7C,0x37,0xF6,0xFC,0xC8,0xDD,0x45,0x00,0xDD,0xBA,0xC4,0x77,0xE6,0xEE,0x40,0xEC,0x0E,0xE6,0x91,0xF1,0xD2,0x00,0x42,0x34,0x5E,0xCE,0xE5,0x08,0x16,0xA0,0x84,0x68,0x67,0xB4,0x86,0xC3,0xD5,0x26,0x2C,0x20,0x51,0x17,0xA2,0xB8,0x03,0xB0,0xFE,0x49,0xDD,0x54,0x15,0xD8,0xEE,0x73,0x37,0x95,0x9D,0xD4,0x1A,0xB7,0xA5,0x26,0xC4,0x91,0xA9,0x0B,0x06,0xEE,0x72,0xB7,0xFB,0xC5,0x16,0x80,0xE9,0xF1,0x07,0x8D,0x3F,0x15,0x5F,0x1C,0x0B,0xFC,0x0A,0x90,0xF0,0xF3,0x09,0xA9,0x90,0xC4,0xC6,0x37,0xB0,0x93,0xBF,0xE1,0x71,0xDB,0xA9,0xD7,0x41,0xAD,0x46,0xEA,0x19,0xA9,0xD5,0xCE,0x93,0xB3,0x35,0x73,0x0A,0x69,0x59,0x91,0xC3,0x0F,0x22,0x1B,0x1D,0x91,0x13,0x3D,0x91,0x73,0x43,0xF1,0x6C,0x55,0xDA,0x3A,0x4F,0xBA,0x25,0xCE,0x4F,0x04,0xF1,0xC5,0xCF,0x71,0xDA,0x3C,0xD7,0xB9,0xB2,0x48,0xB4,0x89,0x38,0x20,0x4B,0x2A,0x95,0x0C,0xD5,0xEF,0x5B,0xAD,0x96,0x45,0x8A,0x41,0x96,0x7A,0x1F,0x60,0x0D,0x7D,0x22,0x75,0x82,0x2B,0x0F,0xFB,0xCE,0x51,0x3D,0x2E,0x3A,0x21,0xF3,0x1C,0xD9,0x38,0x86,0x2C,0xC6,0x05,0xB6,0x7B,0x9A,0x8F,0x0F,0x97,0x1B,0x72,0x6F,0x1C,0xEB,0xAE,0xFF,0xDA,0x97,0x0D,0xBA,0x43,0x32,0xCA,0x66,0x34,0x3D,0x54,0xCB,0x24,0x9B,0x43,0xF2,0x70,0x3E,0x42,0xBB,0xA0,0x95,0x11,0x37,0x46,0xE1,0x4F,0x49,0xC5,0x1B,0xFC,0x3C,0x3A,0x3E,0xD1,0x65,0x0E,0x6F,0x58,0xF8,0x9E,0x5B,0xDB,0x55,0xB6,0x41,0x34,0xCB,0xBE,0xDB,0x87,0x5F,0xA9,0xD1,0x85,0x6B,0xB3,0x17,0x9C,0x61,0x0C,0x9B,0xA2,0x5D,0x61,0x10,0xED,0x2A,0x9B,0xA2,0x5D,0x61,0x10,0xED,0x2A,0x9B,0xA2,0x5D,0x61,0x10,0xED,0x2A,0x9B,0xED,0xC9,0xFC,0xDF,0x14,0x54,0x8F,0x80,0x7A,0x06,0xF5,0x23,0xA0,0x9F,0x41,0xF3,0x10,0x30,0x4F,0x41,0xF3,0x18,0x30,0xCF,0xCA,0xFC,0xFF,0x35,0xC9,0x79,0xC9,0x89,0xFA,0x33,0xD7,0x1D,0xF6,0x5E,0x84,0x5C,0x56,0x6E,0xA7,0xDA,0x1E,0xF9,0xFA,0xAB,0xF5,0x97,0xFF,0x2F,0xED,0x89,0x7E,0x29,0x9E,0xB4,0x9F,0x74,0x1E,0x69,0xDA,0xA4,0x9F,0x81,0x94,0xEF,0x4F,0xF6,0xF9,0x0B,0xF4,0x65,0x51,0x08};


static uint8_t sk=0;
uint8_t Read_Keys()
{
  static uint8_t Read_tag=0,temp_tag=0,ret_tag=0;	
  Read_tag = FTImpl.Read(REG_TOUCH_TAG);
  ret_tag = 0;
  if(Read_tag!=0)								// Allow if the Key is released
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

int32_t get_free_memory()
{
	extern int32_t __heap_start, *__brkval;
	int32_t v;
	return (int32_t) &v - (__brkval == 0 ? (int32_t) &__heap_start : (int32_t) __brkval);
}

void home_setup()
{


  FTImpl.WriteCmd(CMD_INFLATE);
  FTImpl.WriteCmd(250*1024L);
  FTImpl.WriteCmdfromflash(home_star_icon,sizeof(home_star_icon));
  
  FTImpl.DLStart();
  FTImpl.ClearColorRGB(0,0,0);
  FTImpl.Clear(1,1,1);
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

void Info()
{
  uint32_t status;
  uint16_t dloffset = 0,z;
  //Ft_CmdBuffer_Index = 0;  
 
  FTImpl.DLStart();
  FTImpl.Clear(1,1,1);
  FTImpl.ColorRGB(255,255,255);
  FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,FT_DISPLAYHEIGHT/2,26,FT_OPT_CENTER,"Please tap on a dot");
  FTImpl.Cmd_Calibrate(0);
  FTImpl.DLEnd();
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
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,20,28,FT_OPT_CENTER,info[0]);
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,60,26,FT_OPT_CENTER,info[1]);
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,90,26,FT_OPT_CENTER,info[2]);  
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,120,26,FT_OPT_CENTER,info[3]);  
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,FT_DISPLAYHEIGHT-30,26,FT_OPT_CENTER,"Click to play");
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
}


void readfile2flash(const char *filename, uint32_t FT801_addr)
{
FT_SDStatus Status;
  FT_SDFile fontfile;
  byte file = 0; 
  Status=FtSd.OpenFile(fontfile,filename);
  if(Status){
	//Serial.print("File open error ");
    //Serial.println(Status,DEC);  
  }
  else{
	uint8_t imbuff[512];  
	uint16_t blocklen;
	while (fontfile.Offset < fontfile.Size)
	{
		uint16_t n = min(512, fontfile.Size - fontfile.Offset);
		n = (n + 3) & ~3;   // force 32-bit alignment
		fontfile.ReadSector(imbuff);
		//FTImpl.WriteCmd(FT801_addr,imbuff,n);  //need replacement function
		FTImpl.Write(FT801_addr,imbuff,n);
		FT801_addr += n;
	}
  }
}


void download_bmpfile(uint16_t sc_flag)
{

#define INDEX_BASE      1

#define A_F_FILE_ADDR   FT_RAM_G
#define A_F_HANDLE      1
#define A_F_SOURCE     (A_F_FILE_ADDR+148-34*14*INDEX_BASE)

#define G_L_FILE_ADDR   (A_F_FILE_ADDR + 28708)
#define G_L_HANDLE      2
#define G_L_SOURCE     (G_L_FILE_ADDR+148-34*14*INDEX_BASE)


#define M_R_FILE_ADDR   (G_L_FILE_ADDR + 28708)
#define M_R_HANDLE      3
#define M_R_SOURCE     (M_R_FILE_ADDR+148-34*14*INDEX_BASE)

#define S_X_FILE_ADDR   (M_R_FILE_ADDR + 28708)
#define S_X_HANDLE      4
#define S_X_SOURCE     (S_X_FILE_ADDR+148-34*14*INDEX_BASE)

#define Y_Z_FILE_ADDR   (S_X_FILE_ADDR + 28708)
#define Y_Z_HANDLE      5
#define Y_Z_SOURCE     (Y_Z_FILE_ADDR+148-34*14*INDEX_BASE)

#define SC_NOTE_FILE_ADDR   (Y_Z_FILE_ADDR + 13476)
#define SC_NOTE_HANDLE      6
#define SC_NOTE_SOURCE     (SC_NOTE_FILE_ADDR+148-34*15*INDEX_BASE)


	if (sc_flag)
	{
		readfile2flash("A-F_SC.raw", A_F_FILE_ADDR);
		readfile2flash("G-L_SC.raw", G_L_FILE_ADDR);
		readfile2flash("M-R_SC.raw", M_R_FILE_ADDR);
		readfile2flash("S-X_SC.raw", S_X_FILE_ADDR);
		readfile2flash("Y-Z_SC.raw", Y_Z_FILE_ADDR);
	}else
	{
		readfile2flash("A-F_TC.raw", A_F_FILE_ADDR);
		readfile2flash("G-L_TC.raw", G_L_FILE_ADDR);
		readfile2flash("M-R_TC.raw", M_R_FILE_ADDR);
		readfile2flash("S-X_TC.raw", S_X_FILE_ADDR);
		readfile2flash("Y-Z_TC.raw", Y_Z_FILE_ADDR);
	}

	readfile2flash("SC_Note.raw", SC_NOTE_FILE_ADDR);
}

void setup_bmp()
{
	FTImpl.TagMask(0);

	//A_F bitmap configuration
	FTImpl.BitmapHandle(A_F_HANDLE);
	FTImpl.BitmapSource(A_F_SOURCE);	
	FTImpl.BitmapLayout(FT_L4,14,34);
	FTImpl.BitmapSize(FT_NEAREST,FT_BORDER,FT_BORDER,28,34);		
	FTImpl.Cmd_SetFont(A_F_HANDLE, A_F_FILE_ADDR);	

	//G_L bitmap configuration
	FTImpl.BitmapHandle(G_L_HANDLE);
	FTImpl.BitmapSource(G_L_SOURCE);
	FTImpl.BitmapLayout(FT_L4,14,34);
	FTImpl.BitmapSize(FT_NEAREST,FT_BORDER,FT_BORDER,28,34);		
	FTImpl.Cmd_SetFont(G_L_HANDLE, G_L_FILE_ADDR);	

	//M_R bitmap configuration
	FTImpl.BitmapHandle(M_R_HANDLE);
	FTImpl.BitmapSource(M_R_SOURCE);
	FTImpl.BitmapLayout(FT_L4,14,34);
	FTImpl.BitmapSize(FT_NEAREST,FT_BORDER,FT_BORDER,28,34);		
	FTImpl.Cmd_SetFont(M_R_HANDLE, M_R_FILE_ADDR);	


	//S_X bitmap configuration
	FTImpl.BitmapHandle(S_X_HANDLE);
	FTImpl.BitmapSource(S_X_SOURCE);
	FTImpl.BitmapLayout(FT_L4,14,34);
	FTImpl.BitmapSize(FT_NEAREST,FT_BORDER,FT_BORDER,28,34);		
	FTImpl.Cmd_SetFont(S_X_HANDLE, S_X_FILE_ADDR);	

	//Y_Z bitmap configuration
	FTImpl.BitmapHandle(Y_Z_HANDLE);
	FTImpl.BitmapSource(Y_Z_SOURCE);
	FTImpl.BitmapLayout(FT_L4,14,34);
	FTImpl.BitmapSize(FT_NEAREST,FT_BORDER,FT_BORDER,28,34);		
	FTImpl.Cmd_SetFont(Y_Z_HANDLE, Y_Z_FILE_ADDR);	


	//SC Note bitmap configuration
	FTImpl.BitmapHandle(SC_NOTE_HANDLE);
	FTImpl.BitmapSource(SC_NOTE_SOURCE);
	FTImpl.BitmapLayout(FT_L4,15,34);
	FTImpl.BitmapSize(FT_NEAREST,FT_BORDER,FT_BORDER,30,34);		
	FTImpl.Cmd_SetFont(SC_NOTE_HANDLE, SC_NOTE_FILE_ADDR);	
}




static boolean istouch()
{
	if(FTImpl.IsPendown())
		return true;
	else
		return false;
}



static uint8_t Read_Keypad()
{
  static uint8_t temp_tag=0;	
  uint8_t Read_tag = 0;

  Read_tag = FTImpl.Read(REG_TOUCH_TAG);
  Flag.Key_Detect = 0;

  if(Read_tag)								// Allow if the Key is released
  {
	Flag.Key_Detect = 1; 

    if(temp_tag != Read_tag)
    {
	  //First touch
      temp_tag = Read_tag;		
     FTImpl.PlaySound(100,0x51);      	  
    }
  }
  else
  {
	//Key is released
    if((temp_tag != 0) && !istouch())
    {	
      Read_tag = temp_tag;
	  Flag.Key_Detect = 2; 
    }
	temp_tag = 0;
  }

  return Read_tag;
}


static void Display_Candidates(uint8_t handle, uint8_t index,uint8_t index_pressed)
{
#define MAX_CHARS_PERLINE   10
	char str2disp[MAX_CHARS_PERLINE+1];
	int32_t i;
	str2disp[MAX_CHARS_PERLINE] = '\x00';
	for (i=0;i<MAX_CHARS_PERLINE;i++)
		str2disp[i] = index + i + 1;
	
	FTImpl.Cmd_Keys(0,(FT_DISPLAYHEIGHT*0.30),FT_DISPLAYWIDTH,(FT_DISPLAYHEIGHT*0.15),handle,0 | index_pressed,str2disp);
}

void display_notepad(uint16_t startLine)
{
	uint16_t i,j;
	int16_t baseX=0,baseY=0;
	uint16_t currentCode;

	FTImpl.TagMask(0);
	FTImpl.Begin(FT_BITMAPS);

	for (i=startLine;i<MAX_FT_LINES;i++)
	{		
		baseX = 0;
		for (j=0;j<MAX_COLS;j++)
		{													
			currentCode = Buffer.notepad[i][j];
			if (currentCode ==0)
			{
				break;
			}					

			if (currentCode >= 269)
			{
				
				if ((!showCursor && currentCode == CURSOR_CELL_CODE))
				{
				}else
				{
					FTImpl.Vertex2ii(baseX,baseY,6,currentCode - 268);
				}

			    if (currentCode == CURSOR_CELL_CODE)
				{
					return;
				}

			}
			else
			{
				if (currentCode%60)
					FTImpl.Vertex2ii(baseX,baseY,currentCode/60+1,currentCode%60);
				else
					FTImpl.Vertex2ii(baseX,baseY,currentCode/60,60);
			}
			baseX += 30;//30 pixel is the width of character
		}
		baseY += 34; //34 pixel is the height of character
	}
}


uint8_t find_lastChar(uint8_t line)
{
	int i=0;
	for (i=0;i<MAX_COLS;i++)
		if (((Buffer.notepad[line][i] == 0)) || (Buffer.notepad[line][i] == CURSOR_CELL_CODE))
			return i;
}


void update_notepad_buff(uint8_t Read_sfk,uint8_t   Candidate_Handle)
{

	switch (Read_sfk){
		case TOGGLE_BUTTON:
			//Toggle key for simplified Chinese and Traditional chinese
			break;
		case ENTER_BUTTON:
			//Enter Key
			Buffer.notepad[currentLine][currentCol] = 0;
			
			currentLine ++;
			currentCol = 0;

			if (currentLine == MAX_FT_LINES)
			{
				currentLine = MAX_FT_LINES - 1;
				currentCol = find_lastChar(currentLine);
			}
			Buffer.notepad[currentLine][currentCol] = CURSOR_CELL_CODE;
			break;
		case SPACE_BUTTON:
			//Space Key
			Buffer.notepad[currentLine][currentCol] = 269;
			currentCol ++;
			if (currentCol > 14)
			{
				currentLine ++;
				currentCol = 0;
			}
			if (currentLine == MAX_FT_LINES)
			{
				currentLine = MAX_FT_LINES - 1;
				currentCol = find_lastChar(currentLine);
			}
			
			Buffer.notepad[currentLine][currentCol] = CURSOR_CELL_CODE;
		    break;
		case BACK_SPACE:
			//Backspace(delete) key
			Buffer.notepad[currentLine][currentCol] = 0;

			if (currentCol>0)
				currentCol --;
			else
			{
				if (currentLine>0)
				{
					currentLine --;
					currentCol = find_lastChar(currentLine);
					
				}
			}
			Buffer.notepad[currentLine][currentCol] = CURSOR_CELL_CODE;

			break;
		case INPUT_DONE:
			//Done button
			break;
		default:
			//Other key
			Buffer.notepad[currentLine][currentCol] = (Candidate_Handle -1) * 60 + Read_sfk;
			currentCol ++;
			if (currentCol >= MAX_COLS)
			{
				currentLine ++;
				currentCol = 0;
			}

			if (currentLine == MAX_FT_LINES)
			{
				currentLine = MAX_FT_LINES - 1;
				currentCol = find_lastChar(currentLine);
			}
			Buffer.notepad[currentLine][currentCol] = CURSOR_CELL_CODE;
			break;
	}
}

//display all the characters in a new frame with scrolling way
void display_note()
{
	int16_t i,j;
	int16_t baseX=0,baseY=0;

	int16_t startY=0;
	uint16_t currentCode;
    
	for (;;)
	{
		FTImpl.DLStart();
		FTImpl.ClearColorRGB(100,100,100);        
		FTImpl.Clear(1,1,1);	
		FTImpl.ColorRGB(255,255,255);

		baseY = startY;
		{
			FTImpl.TagMask(0);
			FTImpl.Begin(FT_BITMAPS);

			for (i=0;i<MAX_FT_LINES;i++)
			{		
				baseX = 0;
				for (j=0;j<MAX_COLS;j++)
				{													
					currentCode = Buffer.notepad[i][j];
					if (currentCode ==0)
					{
						break;
					}					

					if (currentCode >= 269)
					{
				
						if ((!showCursor && currentCode == CURSOR_CELL_CODE))
						{
						}else
						{
							FTImpl.BitmapHandle(6);
							FTImpl.Cell(currentCode - 268);
							FTImpl.Vertex2f(baseX*16,baseY*16);
						}

						if (currentCode == CURSOR_CELL_CODE)
						{
							//return;
							goto ___exit;
						}
					}
					else
					{
						if (currentCode%60)
						{
							FTImpl.BitmapHandle(currentCode/60+1);
							FTImpl.Cell(currentCode%60);
							FTImpl.Vertex2f(baseX*16,baseY*16);
						}
						else
						{
							FTImpl.BitmapHandle(currentCode/60);
							FTImpl.Cell(60);
							FTImpl.Vertex2f(baseX*16,baseY*16);
						}
					}
					baseX += 30;
				}
				baseY += 34;
			}
		}

___exit:
		FTImpl.DLEnd();
		FTImpl.Finish();
		
		if (istouch())
			break;


		if (startY-- < (-34L*(i+1)))
			startY = FT_DISPLAYHEIGHT;	
	}
}

void clear_notepad_buff()
{
  uint16_t tval;
  for(tval=0;tval<MAX_FT_LINES;tval++)
	memset(&Buffer.notepad[tval],'\0',sizeof(Buffer.notepad[tval]));

  Buffer.notepad[0][0] = CURSOR_CELL_CODE;
}

void Notepad(void)
{

  /*local variables*/
  const uint8_t keypad_font = 27;

  uint16_t Disp_pos = 0,But_opt =0;
  uint8_t Read_sfk=0;	
  uint8_t   Candidate_Handle = 0;
  uint8_t   Candidate_Index = 0;
  
  clear_notepad_buff();
 
  {
    download_bmpfile(sc_flag);
	FTImpl.DLStart();
	FTImpl.ClearColorRGB(100,100,100);        
	FTImpl.Clear(1,1,1);	
	FTImpl.ColorRGB(255,255,255);

	setup_bmp();

	FTImpl.DLEnd();
	FTImpl.Finish();
  }

  Flag.Display_candiate = 0;
  Read_sfk = 0;
  do
  {	
        showCursor = TRUE;
	// Display List start 
	FTImpl.DLStart();
	FTImpl.ClearColorRGB(100,100,100);        
	FTImpl.Clear(1,1,1);	
	FTImpl.ColorRGB(255,255,255);

	FTImpl.TagMask(1);            // enable tagbuffer updation
	FTImpl.Cmd_FGColor(0x101010);

	if (((Read_sfk>='A') && (Read_sfk<='Z') && (1==Flag.Key_Detect)))
	{
		if ((Read_sfk>='A') && (Read_sfk<='F'))
		{
			//Handle_A_F
			Candidate_Handle = A_F_HANDLE;Candidate_Index = (Read_sfk - 'A') * 10;
		}
		if ((Read_sfk>='G') && (Read_sfk<='L'))
		{
			//Handle_G_L
			Candidate_Handle = G_L_HANDLE;Candidate_Index = (Read_sfk - 'G') * 10;
		}
		if ((Read_sfk>='M') && (Read_sfk<='R'))
		{
			//Handle_M_R
			Candidate_Handle = M_R_HANDLE;Candidate_Index = (Read_sfk - 'M') * 10;
		}
		if ((Read_sfk>='S') && (Read_sfk<='X'))
		{
			//Handle_S_X
			Candidate_Handle = S_X_HANDLE;Candidate_Index = (Read_sfk - 'S') * 10;
		}
		if ((Read_sfk>='Y') && (Read_sfk<='Z'))
		{
			//Handle_Y_Z
			Candidate_Handle = Y_Z_HANDLE;Candidate_Index = (Read_sfk - 'Y') * 10;
		}
		Flag.Display_candiate = 1;
	}

	if (1==Flag.Display_candiate)
	{
		
		if (((Read_sfk >=1) && (Read_sfk <=60))  || 
			(Read_sfk>=TOGGLE_BUTTON))
		{
			if (1==Flag.Key_Detect)
				Display_Candidates(Candidate_Handle,Candidate_Index,Read_sfk);

			if (2==Flag.Key_Detect)
			{
				//Key is released and now update the notepad buffer
				update_notepad_buff(Read_sfk,Candidate_Handle);
				Display_Candidates(Candidate_Handle,Candidate_Index,0);
			}
		}else
		{
			Display_Candidates(Candidate_Handle,Candidate_Index,0);
		}
	}

	display_notepad(currentLine>1 ? currentLine - 1:0);

	FTImpl.TagMask(1);            // enable tagbuffer updation

	//Toggle for traditional Chinese and simplified chinese
	FTImpl.Tag(TOGGLE_BUTTON);	
	But_opt = (Read_sfk== TOGGLE_BUTTON)? FT_OPT_FLAT:0;
	if (TOGGLE_BUTTON == Read_sfk  && 2==Flag.Key_Detect)
	{
		if (sc_flag == 0)
		{
			sc_flag = 65535;	
		}
		else
		{
			sc_flag = 0;		
		}
		download_bmpfile(sc_flag);
	}
	FTImpl.Cmd_Toggle(FT_DISPLAYWIDTH*0.040,(FT_DISPLAYHEIGHT*0.89),(FT_DISPLAYWIDTH*0.07),Y_Z_HANDLE,But_opt,sc_flag,"\x19\xff\x1A");

	// BackSpace
	FTImpl.Tag(BACK_SPACE);												
	But_opt = (Read_sfk== BACK_SPACE)? FT_OPT_FLAT:0;
	FTImpl.Cmd_Button(FT_DISPLAYWIDTH*0.875,(FT_DISPLAYHEIGHT*0.70),(FT_DISPLAYWIDTH*0.125),(FT_DISPLAYHEIGHT*0.112),29,But_opt,"<--");
	
	
	// Space Button
	FTImpl.Tag(SPACE_BUTTON);											
	But_opt = (Read_sfk== SPACE_BUTTON)? FT_OPT_FLAT:0;
	FTImpl.Cmd_Button((FT_DISPLAYWIDTH*0.170),(FT_DISPLAYHEIGHT*0.83),(FT_DISPLAYWIDTH*0.65),(FT_DISPLAYHEIGHT*0.12),Y_Z_HANDLE,But_opt,"\x15\x16");


	//construct the keypad
	FTImpl.Cmd_Keys((FT_DISPLAYWIDTH*0.042),(FT_DISPLAYHEIGHT*0.442),FT_DISPLAYWIDTH,(FT_DISPLAYHEIGHT*0.112),keypad_font,Read_sfk,"QWERTYUIOP");
	FTImpl.Cmd_Keys((FT_DISPLAYWIDTH*0.042),(FT_DISPLAYHEIGHT*0.57),(FT_DISPLAYWIDTH*0.96),(FT_DISPLAYHEIGHT*0.112),keypad_font,Read_sfk,"ASDFGHJKL");
	FTImpl.Cmd_Keys((FT_DISPLAYWIDTH*0.132),(FT_DISPLAYHEIGHT*0.70),(FT_DISPLAYWIDTH*0.73),(FT_DISPLAYHEIGHT*0.112),keypad_font,Read_sfk,"ZXCVBNM");	

	//Enter Button
	FTImpl.Tag(ENTER_BUTTON);	
	But_opt = (Read_sfk== ENTER_BUTTON)? FT_OPT_FLAT:0;
	FTImpl.Cmd_Button(FT_DISPLAYWIDTH*0.001,(FT_DISPLAYHEIGHT*0.70),(FT_DISPLAYWIDTH*0.124),(FT_DISPLAYHEIGHT*0.112),Y_Z_HANDLE,But_opt,"\x1B\x1C");

	//Done Button
	FTImpl.Tag(INPUT_DONE);														
	But_opt = (Read_sfk== INPUT_DONE)?  FT_OPT_FLAT:0;          
	FTImpl.Cmd_Button((FT_DISPLAYWIDTH*0.855),(FT_DISPLAYHEIGHT*0.83),(FT_DISPLAYWIDTH*0.146),(FT_DISPLAYHEIGHT*0.12),Y_Z_HANDLE,But_opt,"\x17\x18");	

	FTImpl.DLEnd();
	FTImpl.Finish();	

	//Input is done , now display all the input characters
	if (INPUT_DONE == Read_sfk && 2==Flag.Key_Detect)
	{
		showCursor = false;
		display_note();

		currentLine = currentCol = 0;
		Flag.Display_candiate = 0;
		clear_notepad_buff();
	}

	Read_sfk = Read_Keypad();                // read the keys   
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

/* Display an on-screen message if the storage device is not found */
void CheckStorageDevicePrecence(){
	Serial.println("STORAGE DEVICE NOT FOUND.");
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












