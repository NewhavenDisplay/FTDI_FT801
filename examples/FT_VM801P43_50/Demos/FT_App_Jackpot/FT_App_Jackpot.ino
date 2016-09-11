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
* @file                           FT_App_Jackpot.ino
* @brief                          Sample application to demonstrate FT801 primitives and touch screen. 
* @version                        0.1.0
* @date                           2014/03/21
*
*/


#include "SPI.h"
#include "Wire.h"
#include "FT_VM801P43_50.h"


#define NORMAL_PRESSURE (1200)
#define MAX_ADDITIONAL_VEL (64)
#define BASE_VELOCITY (32)
#define COLUMN_GAP (8)
#define POINTS_PER_COLUMN (5)
#define BUTTON_GAP (5)
#define BUTTON_CURVATURE (5)
#define NUM_OF_ICONS (14)
#define STATUS_BAR_HEIGHT (50)
#define ICON_HEIGHT (bitmapInfo[0].Height)
#define ICON_WIDTH (bitmapInfo[0].Width)
#define ICON_STRIDE (bitmapInfo[0].Stride)
#define BOUNCE_AMOUNT (12)
#define LOADIMAGE (1)
#define INFLATE (2)
#define LOAD (3)
#define VISIBLE_ICONS (3)
#define COMBINATION_AMOUNT (5)
#define PAYOUT_TABLE_SIZE (15)
#define TOTAL_BET_LINES (12)
#define INITIAL_BALANCE (1000)
#define BET_TEXT ((char*)(strings[6]))
#define COLUMN_TEXT ((char*)(strings[7]))
#define LINE_TEXT ((char*)(strings[8]))
#define BALANCE_TEXT ((char*)(strings[9]))
#define RESET_TEXT ((char*)(strings[10]))
#define PAYOUT_TEXT ((char*)(strings[11]))
#define ANY_FRUIT_TEXT ((char*)(strings[12]))
#define SPIN_TEXT ((char*)(strings[13]))
#define EXIT_TEXT ((char*)(strings[14]))
#define SPIN_COLUMNS (5)


#define SOUND_VOL 125
#define SPINNING_SOUND (0x50)      //click sound effect
#define COIN_COLLISION_SOUND (0x43) //glockenspiel sound effect
#define BUTTON_PRESS_SOUND (0x51)  //switch sound effect
#define COIN_REWARD_SOUND (0x52)  //cowbell sound effect




typedef struct bitmap_header_t
{
	uint8_t Width;
	uint8_t Height;
	uint8_t Stride;
	int32_t Offset;
}bitmap_header;


typedef struct spinning_column_t
{
	uint8_t curIndex;
	uint8_t iconArray[20];
	int8_t velocity;
	uint8_t drawnIndex;
	int8_t bounceOffset;
	uint8_t bounceAmount;
}spinning_column;


typedef struct coins_t
{
	int16_t xPos;
	int16_t yPos;
	char xVel;
	int8_t yVel;
	uint8_t index;
	boolean fall;
}spin_coins;

typedef struct bet_lines_t
{
	int16_t x0;
	int16_t y0;
	int16_t x1;
	int16_t y1;
	int16_t x2;
	int16_t y2;
	int16_t x3;
	int16_t y3;
	uint8_t r;
	uint8_t g;
	uint8_t b;
}bet_lines;


typedef struct bet_lines_index_t
{
	int8_t line[5];
}bet_lines_index;


spin_coins spinCoins[8];


typedef struct ui_attributes_t
{
	uint8_t spinButtonWidth;
	uint8_t spinColumns;
	uint16_t spinColumnXOffset;
	uint16_t spinColumnYOffset;
	uint8_t visibleIconsHeight;
	uint16_t columnPixelHeight;
}ui_attributes;

typedef struct jackpot_attributes_t
{
	uint8_t spinColumns;
	boolean spinning;
	boolean released;
	boolean rewardedPoints;
	boolean showPayoutTable;
	boolean reset;
	boolean betChanged;
	boolean displayRewardLine;
	int16_t balance;
	uint8_t totalBetAmount; 
	uint8_t stoppedColumns;
	uint8_t spinColumnSelected;
	uint8_t payoutTableSize;
	uint8_t winningIndex[SPIN_COLUMNS];
	uint8_t touchTag;
	uint8_t pendown;
	int8_t payoutTableShift;
	uint8_t lineBet[12][5];
	uint8_t selectedBetLine;
	uint8_t selectedBetMultiplier;
	uint8_t winningLineIcons;
}jackpot_attributes;


uint8_t Read_Keys();
void loadImageToMemory(char* fileName, uint32_t destination, uint8_t type);
uint8_t Ft_Rom_Font_WH(uint8_t Char,uint8_t font);
uint16_t stringPixelWidth(const char* text,uint8_t font);
uint16_t unsignedNumberPixelWidth(int16_t digits, uint8_t font);
void drawPayoutTable();
void drawSpinColumns();
void scroll();
void updateIndex();
void touched(uint8_t touchVal);
uint8_t nextRandomInt8();
uint16_t getPoints();
void jackpot();
void startingAnimation();
void infoScreen();
uint8_t fontPixelHeight(uint8_t font);
void clearArray(uint8_t* index, uint16_t size, uint8_t defaultValue);
boolean ableToSpin();
void displayRewardText(uint16_t points);
boolean ableToContinue();
void coinsAnimation(uint8_t);
void lineBetButtons();
uint16_t getlineBetPoints();

/* SD card object*/
FT_SD FtSd(FT_SD_CSPIN);
/* sd_present holds error values during initialization.  0 means no error and all other errors are non zero value */
FT_SDStatus sd_present; 

/*SD file object for file accessing*/
FT_SDFile jpeg;

/* Global object for transport */
FT801IMPL_SPI FTImpl(FT_CS_PIN,FT_PDN_PIN,FT_INT_PIN);


uint8_t payoutTable[]={
	8,
	7,
	7,
	6,
	6,
	5,
	5,
	4,
	4,
	3,
	3,
	2,
	2,
	1,
	1
};


PROGMEM byte lineposition[] = {
	't','t','t','t','t',//0
	't','t','t','m','b',//1
	'm','m','m','m','m',//2
	'm','m','m','m','t',//3
	'b','b','b','b','b',//4
	'b','b','b','m','t',//5
	'm','t','t','t','t',//6
	't','m','b','m','t',//7
	'b','m','m','m','m',//8
	't','m','m','m','m',//9
	'm','m','m','m','b',//10
	'b','m','t','m','b' //11
};

PROGMEM bet_lines betLines[]={
	{ 44, 38, 63, 50,235, 50,404, 50,0x7e,0x1e,0x9c}, //1
	{ 46, 67, 63, 55,235, 55,404,170,0x15,0xb0,0x1a}, //2
	{ 47,100, 63,102,235,102,404,102,0x03,0x46,0xDf}, //3
	{ 46,129, 63,107,235,107,404, 60,0xff,0x81,0xc0}, //4
	{ 48,160, 63,170,235,170,404,170,0x65,0x37,0x00}, //5
	{ 49,189, 63,165,235,165,404, 60,0xe5,0x00,0x00}, //6
	{423, 36,404, 60,235, 60, 63,112,0x95,0xd0,0xfc}, //7
	{424, 67,398, 60,235,160, 63, 60,0x02,0x93,0x86}, //8
	{425, 95,404,112,164,112, 63,170,0xf9,0x73,0x06}, //9
	{424,126,404,117,159,117, 59, 58,0x96,0xf9,0x7b}, //10
	{424,157,404,170,299,122, 63,122,0xc2,0x00,0x78}, //11
	{424,186,400,180,235, 75, 83,170,0xff,0xff,0x14}, //12
};


spinning_column columns[]={
	{6, { 2,6, 1, 5,11, 7,10, 0, 4, 8,12,3, 9,13},70,6,30,30},
	{5, {11,8,12, 1, 7, 4, 0, 2, 5, 3,13,6,10, 9},42,8,30,30},
	{9, { 9,1, 2, 4,11,10,13, 6, 5, 7, 0,3,12, 8},56,1,30,30},
	{11,{10,5, 1,11, 2, 3, 7, 4, 8,12, 9,6, 0,13},42,5,30,30},
	{10,{ 5,2,11, 4, 8, 9, 7,12,10,13, 1,0, 3, 6},84,1,30,30},
};





bitmap_header bitmapInfo[] = 
{
	/*width,height,stride,memory offset */
	{64,55,	64*2,	0	}, // this one represents all other spinning icons
	{16,16, 16* 2,	0	},  //background icon
	{30,32,	30* 2,	0	},  //coin .bin file
	{30,32,	30* 2,	0	},
	{30,32,	30* 2,	0	},
	{30,32,	30* 2,	0	},
	{30,32,	30* 2,	0	},
	{30,32,	30* 2,	0	},
	{30,32,	30* 2,	0	},
	{64,165,64,  0   },  //overlay
	{72,190,72*2,0} //outer overlay

};

ui_attributes UI;
jackpot_attributes JPot;


static uint8_t currentLine=0;
boolean finished=false;

prog_char *info[] = {  "FT801 Jackpot Game",
                  "to demonstrate: FT801 primitives",
                  "& touch screen."
               };
			   
			 
prog_char *strings[] = {
"Loading Bitmaps...", 		
"Storage Device not Found",	
"JACKPOT by FTDI",			
"Please tap on a dot",		
"overlay.raw",				
"outer.raw",				
"Bet:", 					
"Column: ",					
"Line: ",					
"Balance: ",				
"reset",					
"Payout",					
"Fruits",					
"Spin",						
"exit"						
};


/* Check to see if the line is a winning line and ok to display it*/
boolean displayTheLine(uint8_t line){
	static uint32_t lastMilli=millis();
	uint32_t curMilli;
	boolean nextLine=false;
	uint8_t i,j;
	for(i=0;i<SPIN_COLUMNS;i++){ /* winning line check */
		if(JPot.lineBet[currentLine][i] < 254){
			nextLine=true;
			break;
		}
	}
	
	if(nextLine){ /* display the line check */
		curMilli = millis();
		if((curMilli-lastMilli)>1000){ /* more than 1 second has passed so advance to the next winning line */
			currentLine=(currentLine+1) >= TOTAL_BET_LINES ? 0 : (currentLine+1);
			lastMilli=curMilli;
			return false;
		}
		else{
		return true; /* Continue to display the current line */
		}
	}
	else{
		currentLine=(currentLine+1) >= TOTAL_BET_LINES ? 0 : (currentLine+1); /* current line is not a winning line so advance to to the next line */
		return false;
	}
}

/* Draw winning bet lines and their respective color */
void drawbetlines(){
	uint8_t i;

	FTImpl.LineWidth(3 * 16) ;
	FTImpl.ColorA(255);
	if(JPot.displayRewardLine){ /* Iterate through the winning lines after columns have stopped spinning.  Continue to display the winning lines till change has been made*/
		if(displayTheLine(currentLine)){ /* check to see if the line is ok to display */
				FTImpl.ColorRGB(pgm_read_byte_near(&betLines[currentLine].r),pgm_read_byte_near(&betLines[currentLine].g),pgm_read_byte_near(&betLines[currentLine].b));
				FTImpl.Begin(FT_LINE_STRIP);
				FTImpl.Vertex2ii(pgm_read_word_near(&betLines[currentLine].x0),pgm_read_word_near(&betLines[currentLine].y0),0,0);
				FTImpl.Vertex2ii(pgm_read_word_near(&betLines[currentLine].x1),pgm_read_word_near(&betLines[currentLine].y1),0,0);
				FTImpl.Vertex2ii(pgm_read_word_near(&betLines[currentLine].x2),pgm_read_word_near(&betLines[currentLine].y2),0,0);
				FTImpl.Vertex2ii(pgm_read_word_near(&betLines[currentLine].x3),pgm_read_word_near(&betLines[currentLine].y3),0,0);
				FTImpl.End();
			}
	}
	else{ /* display all selected bet lines */
	for(i=0;i<JPot.selectedBetLine;i++){
			FTImpl.ColorRGB(pgm_read_byte_near(&betLines[i].r),pgm_read_byte_near(&betLines[i].g),pgm_read_byte_near(&betLines[i].b));
			FTImpl.Begin(FT_LINE_STRIP);
			FTImpl.Vertex2ii(pgm_read_word_near(&betLines[i].x0),pgm_read_word_near(&betLines[i].y0),0,0);
			FTImpl.Vertex2ii(pgm_read_word_near(&betLines[i].x1),pgm_read_word_near(&betLines[i].y1),0,0);
			FTImpl.Vertex2ii(pgm_read_word_near(&betLines[i].x2),pgm_read_word_near(&betLines[i].y2),0,0);
			FTImpl.Vertex2ii(pgm_read_word_near(&betLines[i].x3),pgm_read_word_near(&betLines[i].y3),0,0);
			FTImpl.End();
	}
	}
	FTImpl.ColorRGB(255,255,255);
}





static uint8_t sk=0, temp_tag=0;  /* global variables for the Read_keys() function */


/* convert decimal to ascii char array  */
int32_t Ft_Dec2Ascii(char *pSrc,int32_t value)
{
	int16_t Length;
	char *pdst,charval;
	int32_t CurrVal = value,tmpval,i;
	char tmparray[16],idx = 0;

	Length = strlen(pSrc);
	pdst = pSrc + Length;

	if(0 == value)
	{
		*pdst++ = '0';
		*pdst++ = '\0';
		return 0;
	}

	if(CurrVal < 0)
	{
		*pdst++ = '-';
		CurrVal = - CurrVal;
	}
	/* insert the value */
	while(CurrVal > 0){
		tmpval = CurrVal;
		CurrVal /= 10;
		tmpval = tmpval - CurrVal*10;
		charval = '0' + tmpval;
		tmparray[idx++] = charval;
	}

	for(i=0;i<idx;i++)
	{
		*pdst++ = tmparray[idx - i - 1];
	}
	*pdst++ = '\0';

	return 0;
}

/* This function accept touch input only after it has been released. Most buttons are responded on release but spinning button and payout table tracking are being handled separately and differently.  */
uint8_t Read_Keys()
{
	static uint8_t ret_tag=0, touchVal,temp_tag1;	
	uint8_t i;
	touchVal = FTImpl.Read(REG_TOUCH_TAG); /* REG_TOUCH_TAG register holds the touched object tag value, default value is 0 if no tagged object is touched*/
	JPot.pendown = FTImpl.IsPendown()==1 ? 0 : 1;
	ret_tag = 0;

	if(JPot.spinning && !JPot.released && JPot.pendown){ /* The spin button is released, generate random velocity values for each column */
		JPot.released=true;
		for(i=0; i<SPIN_COLUMNS;i++){
			nextRandomInt8();
			columns[i].velocity =(BASE_VELOCITY +((nextRandomInt8()>>4) & 31L));	
		}
	}


	/* the respond-on-release sensing method does not apply to the payout table tracking and spin button*/
	if(touchVal==200 || touchVal==201){
		temp_tag = touchVal;
		return touchVal;
	}
	
	if(touchVal!=0) /* touched on a tagged object */
	{
		if(temp_tag!=touchVal) /* Note the new touched object tag */
		{
			temp_tag = touchVal;	
			sk = touchVal; /*Load the Read tag to temp variable*/
		}  
	}
	else /* No touch sensed */ 
	{
		if(temp_tag!=0)/* If the temp variable, holds the previous object tag, is not 0, it means the touch is just released, reset temp variable and return the value. If no touch is detected the REG_TOUCH_TAG variable has a value of 0*/
		{
			ret_tag = temp_tag;
			temp_tag=0;
		}  
		sk = 0;
	}
	return ret_tag;
}




void loadImageToMemory(char* fileName, uint32_t destination, uint8_t type){
	uint8_t imbuff[512];  
	//FT_SDFile jpeg;

	if(type==LOADIMAGE){  //LOADIMAGE method takes destination address and options before the actual bitmap data
		FTImpl.WriteCmd(CMD_LOADIMAGE);
		FTImpl.WriteCmd(destination);
		FTImpl.WriteCmd(FT_OPT_NODL);
	}
	else if(type==INFLATE){ //INFLATE method takes destination address before the actual bitmap
		FTImpl.WriteCmd(CMD_INFLATE);
		FTImpl.WriteCmd(destination);
	}
	else if(type==LOAD){ //Deflated bitmap data can go directly to the RAM_G
	}



	if(FtSd.OpenFile(jpeg,fileName)){
		//Serial.print("Error Loading: ");
		//Serial.println(fileName);
	}
	else{
	
		while (jpeg.Offset < jpeg.Size)
		{	
			uint32_t n = min(512, jpeg.Size - jpeg.Offset);
			n = (uint32_t)(n + 3) & (~3);   // force 32-bit alignment
			jpeg.ReadSector(imbuff);

			if(type==LOAD)  /* Deflated bitmap data can go directly to the RAM_G */
			{
				FTImpl.Write(destination,imbuff,n);//alignment is already taken care by this api
				destination += n*1L;
			}
			else /* All other bitmap data are required to go into the command buffer*/
			{
				FTImpl.WriteCmd(imbuff, n);//alignment is already taken care by this api
			}
		}
	}
	
}


//static uint32_t ramOffset=0;


void jackpotSetup(){

	char path[13];
	uint32_t ramOffset=0;
	char fileName[3];
	char bitmapExtention[] = "J.jpg";
	uint8_t i;


	UI.spinButtonWidth=80;
	UI.columnPixelHeight=NUM_OF_ICONS*ICON_HEIGHT;
	JPot.spinning=false;
	JPot.released=false;
	JPot.rewardedPoints=true;
	JPot.showPayoutTable=false;
	JPot.reset=false;
	JPot.balance=INITIAL_BALANCE;
	JPot.spinColumnSelected=0;
	JPot.pendown=0;
	JPot.payoutTableShift=0;
	JPot.selectedBetMultiplier=0;
	JPot.selectedBetLine=12;
	UI.visibleIconsHeight=VISIBLE_ICONS * ICON_HEIGHT;
	UI.spinColumnXOffset=(FT_DISPLAYWIDTH>>1) - ((SPIN_COLUMNS*ICON_WIDTH + SPIN_COLUMNS*COLUMN_GAP)>>1);  //center the spinning columns at the middle of the screen
	UI.spinColumnYOffset= (((FT_DISPLAYHEIGHT-STATUS_BAR_HEIGHT)>>1) - ((UI.visibleIconsHeight)>>1));   //center the spinning columns at the middle of the screen
	FTImpl.DLStart();        // start
	FTImpl.Clear(1,1,1);
	FTImpl.ColorRGB(255,255,255);
	FTImpl.Cmd_Text(FT_DISPLAYWIDTH>>1,FT_DISPLAYHEIGHT>>1,29,FT_OPT_CENTER,(char*)(strings[0]));
	FTImpl.Cmd_Spinner( (FT_DISPLAYWIDTH>>1),(FT_DISPLAYHEIGHT>>1)+50,0,0);
	FTImpl.DLEnd();
	FTImpl.Finish();



	FTImpl.DLStart();        // start
	FTImpl.Clear(1,1,1);

	//load the FT_BITMAPS into memory, all spinning icons are assumed to be the same in all dimensions.
	for(i=0; i<NUM_OF_ICONS;i++){
		path[0]=fileName[0]='\0';
		Ft_Dec2Ascii(fileName,i);
		strcat(path, fileName);
		strcat(path, bitmapExtention);
		FTImpl.BitmapHandle(i);
		FTImpl.BitmapSource(ramOffset);
		FTImpl.BitmapLayout(FT_RGB565,ICON_STRIDE,ICON_HEIGHT);
		FTImpl.BitmapSize(FT_NEAREST,FT_BORDER,FT_BORDER,ICON_WIDTH,ICON_HEIGHT);
		loadImageToMemory(path,ramOffset,LOADIMAGE);
		ramOffset+=(ICON_STRIDE*ICON_HEIGHT*1L);
	}
 
	//load status bar bitmap by itself because the width is being repeated 
	path[0]=fileName[0]='\0';
	Ft_Dec2Ascii(fileName,14);
	strcat(path, fileName);
	strcat(path, bitmapExtention);
	FTImpl.BitmapHandle(14);
	FTImpl.BitmapSource(ramOffset);
	FTImpl.BitmapLayout(FT_RGB565,2,STATUS_BAR_HEIGHT);
	FTImpl.BitmapSize(FT_NEAREST,FT_REPEAT,FT_BORDER,FT_DISPLAYWIDTH,STATUS_BAR_HEIGHT);
	loadImageToMemory(path,ramOffset,LOADIMAGE);
	ramOffset+=(2*STATUS_BAR_HEIGHT*1L);

	FTImpl.BitmapHandle(15);
	//load background bitmap into memory because the width and height are FT_REPEATed. All bitmap handles have been exhausted, this icon will be drawn by specifying its souce, layout, and size. 

	path[0]=fileName[0]='\0';
	Ft_Dec2Ascii(fileName,15);
	strcat(path, fileName);
	strcat(path, bitmapExtention);
	bitmapInfo[1].Offset=ramOffset;
	loadImageToMemory(path,ramOffset, LOADIMAGE);
	ramOffset+=(bitmapInfo[1].Stride*bitmapInfo[1].Height*1L);


	//load .bin coin bitmap files. 
	for(i=16;i<24;i++){
		path[0]=fileName[0]='\0';
		Ft_Dec2Ascii(fileName,i);
		strcat(path, fileName);
		strcat(path, "J.bin");
		bitmapInfo[i-14].Offset=ramOffset;
		loadImageToMemory(path,ramOffset,INFLATE);
		
		ramOffset+=(bitmapInfo[i-14].Stride*bitmapInfo[i-14].Height*1L);
	}
	
	while(FTImpl.Read16(REG_CMD_WRITE) != FTImpl.Read16(REG_CMD_READ));
	
path[0]='\0';
	strcat(path, (char*)(strings[5]));
	bitmapInfo[10].Offset=ramOffset;
	loadImageToMemory(path,ramOffset,LOAD);
	ramOffset+=(bitmapInfo[10].Stride*bitmapInfo[10].Height*1L);
	
	path[0]='\0';
	strcat(path, (char*)strings[4]);
	bitmapInfo[9].Offset=ramOffset;
	loadImageToMemory(path,ramOffset,LOAD);
	ramOffset+=(bitmapInfo[9].Stride*bitmapInfo[9].Height*1L);

	

	FTImpl.DLEnd();
	FTImpl.Finish();
}



void coinsAnimation(uint8_t coinAmount){
	char gravity=2;
	uint8_t i;

	FTImpl.BlendFunc(FT_SRC_ALPHA,FT_ONE_MINUS_SRC_ALPHA);
	FTImpl.ColorRGB(255,255,255);
	FTImpl.Begin(FT_BITMAPS);


	for(i=0; i<coinAmount; i++){ 
		if(spinCoins[i].index>=7){
			spinCoins[i].index=0;
		}
		if(((spinCoins[i].xPos+bitmapInfo[spinCoins[i].index+2].Width)<0) || (spinCoins[i].xPos>FT_DISPLAYWIDTH) || (spinCoins[i].yPos>FT_DISPLAYHEIGHT)){
			continue;
		}


		if((spinCoins[i].yPos+bitmapInfo[spinCoins[i].index+2].Height)> (FT_DISPLAYHEIGHT-STATUS_BAR_HEIGHT) && !spinCoins[i].fall){
			spinCoins[i].yPos=FT_DISPLAYHEIGHT-STATUS_BAR_HEIGHT-bitmapInfo[spinCoins[i].index+2].Height;
			spinCoins[i].yVel=-1 * ((spinCoins[i].yVel*3)/4);
			FTImpl.PlaySound(SOUND_VOL,COIN_COLLISION_SOUND); //play sound when the coin hits the status bar
			if(spinCoins[i].yVel==0){
				spinCoins[i].fall=true;
			}
		}

		FTImpl.BitmapSource(bitmapInfo[spinCoins[i].index+2].Offset);
		FTImpl.BitmapLayout(FT_ARGB4,bitmapInfo[spinCoins[i].index+2].Stride,bitmapInfo[spinCoins[i].index+2].Height);
		FTImpl.BitmapSize(FT_NEAREST,FT_BORDER,FT_BORDER,bitmapInfo[spinCoins[i].index+2].Width,bitmapInfo[spinCoins[i].index+2].Height);
		FTImpl.Vertex2f(spinCoins[i].xPos * 16 ,spinCoins[i].yPos * 16);
		spinCoins[i].index+=1;
		if(spinCoins[i].index>=7){
			spinCoins[i].index=0;
		}

		spinCoins[i].xPos+=spinCoins[i].xVel;
		spinCoins[i].yPos+=spinCoins[i].yVel;
		spinCoins[i].yVel+=gravity;

	}
}


uint8_t Ft_Rom_Font_WH(uint8_t Char,uint8_t font)
{
	uint32_t ptr,Wptr;
	uint8_t Width=0;
	ptr = FTImpl.Read32(0xffffc);					

	// read Width of the character
	Wptr = (ptr + (148 * (font- 16)))+Char;	// (table starts at font 16)
	Width = FTImpl.Read(Wptr);
	return Width;
}

uint8_t fontPixelHeight(uint8_t font){
	uint32_t ptr,hPtr;
	uint8_t height=0;
	ptr = FTImpl.Read32(0xffffc);					

	// the height is at the 140th byte
	hPtr = (ptr + (148 * (font- 16)))+140;	// (table starts at font 16)
	height = FTImpl.Read(hPtr);
	return height;
}

uint16_t stringPixelWidth(const char* text,uint8_t font){
	char tempChar;
	uint16_t length=0, index; 
	if(text==NULL){
		return 0;
	}

	if(text[0]=='\0'){
		return 0;
	}

	index=0;
	tempChar=text[index];
	while(tempChar!='\0'){
		length+=Ft_Rom_Font_WH(tempChar, font);
		tempChar=text[++index];
	}

	return length;
}

//Numbers of the same font have the same width
uint16_t unsignedNumberPixelWidth(int16_t digits, uint8_t font){
	return Ft_Rom_Font_WH(48,font)*digits;
}








void drawPayoutTable(){
	uint8_t  numFont=23, rateOfChange=2, payoutNumOffset=fontPixelHeight(numFont)>>1, anyFruitTextFont=25, tableSize=220, topIndex, i, currentMultiplier;
	static uint8_t anyFruitTextLength, payoutNumWidth, anyFruitTextHeight, firstTime=1, multiplier=5;
	static int16_t topPoint=0;
	int16_t startingX, startingY=0, payoutSetHeight=PAYOUT_TABLE_SIZE * ICON_HEIGHT;;

	scroll();  //updates payout table offset

	if(firstTime){
		anyFruitTextLength= (uint8_t)stringPixelWidth(ANY_FRUIT_TEXT, anyFruitTextFont);
		payoutNumWidth=(uint8_t)unsignedNumberPixelWidth(3,numFont);
		anyFruitTextHeight=fontPixelHeight(anyFruitTextFont);
		firstTime=0;
	}

	topPoint+=JPot.payoutTableShift;

	if(JPot.payoutTableShift>0){
		JPot.payoutTableShift-=rateOfChange;
		JPot.payoutTableShift = JPot.payoutTableShift < 0 ? 0 : JPot.payoutTableShift;
	}
	else if(JPot.payoutTableShift<0){
		JPot.payoutTableShift+=rateOfChange;
		JPot.payoutTableShift = JPot.payoutTableShift > 0 ? 0 : JPot.payoutTableShift;
	}

	if(topPoint<0){
		topPoint=(payoutSetHeight+topPoint);
		multiplier++;
		if(multiplier>5){
			multiplier=1;
		}
	}
	else if(topPoint>payoutSetHeight){
		topPoint=topPoint%payoutSetHeight;
		multiplier--;
		if(multiplier<1){
			multiplier=5;
		}

	}
	startingX=UI.spinColumnXOffset;
	topIndex = topPoint/ICON_HEIGHT;
	startingY=(topPoint%ICON_HEIGHT)*(-1);
	currentMultiplier=multiplier;

	FTImpl.ColorRGB(255,255,255);
	FTImpl.Begin(FT_RECTS);
	FTImpl.LineWidth(16);
	FTImpl.ColorA(0xff);
	FTImpl.ColorMask(1,1,1,1);
	FTImpl.Clear(1,1,1);
	FTImpl.ColorRGB(0,0,0);
	FTImpl.Vertex2ii(UI.spinColumnXOffset,0, 0,0);
	FTImpl.Vertex2ii(UI.spinColumnXOffset + ICON_WIDTH*3 + payoutNumWidth*2,tableSize, 0,0);
	FTImpl.ColorMask(1,1,1,1);
	FTImpl.BlendFunc(FT_DST_ALPHA,FT_ONE_MINUS_DST_ALPHA);
	FTImpl.ColorRGB(255,255,255);
	FTImpl.Begin(FT_BITMAPS);

	//a maximum of 5 icons can be drawn with a drawing table of 220 pixels in height.
	for(i=0;i<5;i++){
		if(topIndex>14){
			topIndex=0;
			currentMultiplier--;
			if(currentMultiplier<1){
				currentMultiplier=5;
			}
		}

		if(topIndex!=14){ 
			FTImpl.BitmapHandle(topIndex);
			FTImpl.Vertex2f(startingX*16,startingY*16);
			startingX+=(ICON_WIDTH + BUTTON_GAP);
		}
		else{ //fruit combination is at index 14
			FTImpl.BlendFunc(FT_SRC_ALPHA,FT_ZERO);
			FTImpl.Cmd_Text(startingX, startingY+((ICON_HEIGHT>>1)-(anyFruitTextHeight>>1)), anyFruitTextFont/*font*/, 0, ANY_FRUIT_TEXT);
			startingX += anyFruitTextLength + BUTTON_GAP;
			FTImpl.BlendFunc(FT_DST_ALPHA,FT_ONE_MINUS_DST_ALPHA);
		}

		//draw payout amount with the correct multiplier
		FTImpl.BlendFunc(FT_SRC_ALPHA,FT_ZERO);
		startingX+=BUTTON_GAP;
		FTImpl.Cmd_Text(startingX,startingY+payoutNumOffset,numFont,0,"X");
		startingX+=20;
		FTImpl.Cmd_Number( startingX, startingY+payoutNumOffset, numFont, 0, currentMultiplier);
		startingX+=ICON_WIDTH;
		if(currentMultiplier!=1 || topIndex==14)
			FTImpl.Cmd_Number( startingX, startingY+payoutNumOffset, numFont, 0, payoutTable[topIndex] * currentMultiplier);
		else{
			FTImpl.Cmd_Number( startingX, startingY+payoutNumOffset, numFont, 0, 0);
		}
		FTImpl.BlendFunc(FT_DST_ALPHA,FT_ONE_MINUS_DST_ALPHA);


		startingY+=ICON_HEIGHT;
		startingX=UI.spinColumnXOffset;
		topIndex++; 

	}


	FTImpl.Cmd_LoadIdentity();
	FTImpl.Cmd_SetMatrix();
	FTImpl.BlendFunc(FT_SRC_ALPHA,FT_ONE_MINUS_SRC_ALPHA);
	FTImpl.BitmapHandle(15);
}

void redrawColumnIcons(){
	int16_t topIndex, bottomIndex, i, j, startingX=UI.spinColumnXOffset, startingY=UI.spinColumnYOffset, highLightingIndex;

	FTImpl.Begin(FT_BITMAPS);
	FTImpl.ColorA(255);
	FTImpl.ColorRGB(255,255,255);
	FTImpl.LineWidth(16*2);
	for(i=0; i<SPIN_COLUMNS;i++){
		
		topIndex=columns[i].curIndex;
		bottomIndex=(topIndex+2)%NUM_OF_ICONS;

		//draw icons from top to the bottom index
		while(1){
			//the drawn icon in each column has a brighter color
			if((columns[i].iconArray[topIndex]==JPot.lineBet[currentLine][i]) && JPot.displayRewardLine){

				FTImpl.ColorRGB(255,255,255);
				FTImpl.Vertex2ii(startingX,(startingY),columns[i].iconArray[topIndex],0);
				//icons in the winning line will have corresponding color box on top of it
				FTImpl.ColorRGB(pgm_read_byte_near(&betLines[currentLine].r),pgm_read_byte_near(&betLines[currentLine].g),pgm_read_byte_near(&betLines[currentLine].b));
					
					FTImpl.Begin(FT_LINE_STRIP);
					FTImpl.Vertex2ii(startingX-1,startingY-1,0,0);
					FTImpl.Vertex2ii(startingX+ICON_WIDTH+1,startingY-1,0,0);
					FTImpl.Vertex2ii(startingX+ICON_WIDTH+1,startingY+ICON_HEIGHT+1,0,0);
					FTImpl.Vertex2ii(startingX-1,startingY+ICON_HEIGHT+1,0,0);
					FTImpl.Vertex2ii(startingX-1,startingY-1,0,0);

					FTImpl.ColorRGB(255,255,255);
					FTImpl.Begin(FT_BITMAPS);
			}

			if(topIndex==bottomIndex){
				break;
			}
			startingY+=ICON_HEIGHT;
			topIndex=(topIndex+1)%NUM_OF_ICONS;
		}

		startingX+=(ICON_WIDTH+COLUMN_GAP);
		startingY=UI.spinColumnYOffset;
	}
	FTImpl.BlendFunc(FT_SRC_ALPHA,FT_ONE_MINUS_SRC_ALPHA);
}


void drawSpinColumns(){
	int16_t topIndex, bottomIndex, i, j, startingX=UI.spinColumnXOffset, startingY=UI.spinColumnYOffset, highLightingIndex;
	static int8_t bounceOffset;

	updateIndex();
	FTImpl.Begin(FT_BITMAPS);
	bounceOffset=0;
	JPot.stoppedColumns=0;
	for(i=0; i<SPIN_COLUMNS;i++){
		FTImpl.ColorA(255);
		FTImpl.ColorRGB(255,255,255);
		topIndex=columns[i].curIndex;
		bottomIndex=(topIndex+2)%NUM_OF_ICONS;
		highLightingIndex=(columns[i].curIndex+1)%NUM_OF_ICONS;

		//bouncing effect happens after the column was stopped
		if(columns[i].velocity<=0){
			if(columns[i].bounceAmount>0){
				bounceOffset=columns[i].bounceOffset=columns[i].bounceOffset-5;

				
				
				if(bounceOffset<0){
					bounceOffset=columns[i].bounceOffset=columns[i].bounceAmount=columns[i].bounceAmount>>1;
					
				}
			}
			else{
				JPot.stoppedColumns++;
			}
		}
		else{
			FTImpl.PlaySound(SOUND_VOL,SPINNING_SOUND);  //spinning sound for each column
		}


		//draw icons from top to the bottom index
		while(topIndex!=bottomIndex){
			//the drawn icon in each column has a brighter color
			if((topIndex==highLightingIndex) && (columns[i].velocity<=0)){
				//columns[i].drawnIndex=columns[i].iconArray[highLightingIndex];
				columns[i].drawnIndex=highLightingIndex;
				FTImpl.ColorRGB(255,255,255);
				FTImpl.Vertex2ii(startingX,(startingY+bounceOffset),columns[i].iconArray[topIndex],0);
				//icons in the winning combination will have a red box on top of it.
				if(JPot.winningIndex[i]!=255 && JPot.rewardedPoints){
					//highlight box
					FTImpl.ColorRGB(255,0,0);
					FTImpl.LineWidth(16*2);
					FTImpl.Begin(FT_LINES);
					//top line
					FTImpl.Vertex2ii(startingX+2,startingY+bounceOffset,0,0);
					FTImpl.Vertex2ii(startingX+ICON_WIDTH,startingY+bounceOffset,0,0);
					//left line
					FTImpl.Vertex2ii(startingX+3,startingY+bounceOffset,0,0);
					FTImpl.Vertex2ii(startingX+3,startingY+ICON_HEIGHT+bounceOffset,0,0);
					//right line
					FTImpl.Vertex2ii(startingX+ICON_WIDTH-1,startingY+bounceOffset,0,0);
					FTImpl.Vertex2ii(startingX+ICON_WIDTH-1,startingY+ICON_HEIGHT+bounceOffset,0,0);
					//bottom line
					FTImpl.Vertex2ii(startingX-1,startingY+ICON_HEIGHT+bounceOffset-1,0,0);
					FTImpl.Vertex2ii(startingX+ICON_WIDTH-1,startingY+ICON_HEIGHT+bounceOffset-1,0,0);
					FTImpl.ColorRGB(255,255,255);
					FTImpl.Begin(FT_BITMAPS);
				}
			}
			else{
				FTImpl.Vertex2ii(startingX,(startingY+bounceOffset),columns[i].iconArray[topIndex],0);
			}
			startingY+=ICON_HEIGHT;
			topIndex=(topIndex+1)%NUM_OF_ICONS;
		}

		FTImpl.Vertex2ii(startingX,(startingY+bounceOffset),columns[i].iconArray[topIndex],0);


		FTImpl.BitmapHandle(15);
		FTImpl.ColorRGB(0,0,0);
		FTImpl.ColorA(255);
		
		FTImpl.BitmapSource(bitmapInfo[9].Offset);
		FTImpl.BitmapLayout(FT_L8,bitmapInfo[9].Stride,bitmapInfo[9].Height);
		FTImpl.BitmapSize(FT_NEAREST,FT_BORDER,FT_BORDER,bitmapInfo[9].Width,bitmapInfo[9].Height);
		FTImpl.Vertex2f((startingX)*16,(UI.spinColumnYOffset)*16);

		FTImpl.ColorRGB(255,255,255);
		FTImpl.BitmapSource(bitmapInfo[10].Offset);
		FTImpl.BitmapLayout(FT_ARGB4,bitmapInfo[10].Stride,bitmapInfo[10].Height);
		FTImpl.BitmapSize(FT_NEAREST,FT_BORDER,FT_BORDER,bitmapInfo[10].Width,bitmapInfo[10].Height);
		FTImpl.Vertex2f((startingX-3)*16,(UI.spinColumnYOffset-12)*16);

		//selected columns indication: dots
		if((i+1)<=JPot.spinColumnSelected){
			FTImpl.PointSize(6 * 16) ;
			FTImpl.Begin(FT_POINTS) ;
			FTImpl.Vertex2ii(startingX+(ICON_WIDTH>>1), UI.spinColumnYOffset-6,0,0 );
			FTImpl.Vertex2ii(startingX+(ICON_WIDTH>>1), startingY+ICON_HEIGHT+6,0,0 );
			FTImpl.Begin(FT_BITMAPS) ;
		}

		startingX+=(ICON_WIDTH+COLUMN_GAP);
		startingY=UI.spinColumnYOffset;
		bounceOffset=0;
	}
	FTImpl.BlendFunc(FT_SRC_ALPHA,FT_ONE_MINUS_SRC_ALPHA);
}


//payout table scrolling
void scroll(){
	sTagXY sTagxy;
	static uint16_t curScreenY, preScreenY1, preScreenY2;
	static boolean continueTouch=false;
	FTImpl.GetTagXY(sTagxy);
	/* Note the position of the first touch */
	if(!continueTouch && JPot.touchTag==200){
		continueTouch=true;

		curScreenY = sTagxy.y;
		preScreenY2=preScreenY1 = curScreenY;
	}

	/* scroll while the screen is still be touched */
	if(continueTouch && !JPot.pendown){
		preScreenY2=preScreenY1;

		curScreenY = sTagxy.y;
		if((curScreenY != 32768) && (preScreenY1 != 32768)){
			if(curScreenY != preScreenY1){
				JPot.payoutTableShift=(preScreenY1-curScreenY);
			}
		}
	}

	/* scrolling speed on release*/
	if(continueTouch && JPot.pendown){
		if(preScreenY2!= 32768 && curScreenY!=32768){
			JPot.payoutTableShift=(preScreenY2-curScreenY)<<1;
		}
		continueTouch=false;
	}
	preScreenY1=curScreenY;
}

//decrease the spinning column velocity afer the spin button has been released
void updateIndex(){

	static uint8_t i;
	uint16_t pressure;

	if(JPot.spinning){
		//button has been released
		if(JPot.released){
			for(i=0; i<SPIN_COLUMNS;i++){
				if(columns[i].velocity>0){
					columns[i].velocity = (columns[i].velocity-1) <= 0 ? 0 : (columns[i].velocity-1);
					columns[i].curIndex=(columns[i].curIndex+i+1)%NUM_OF_ICONS;
				}
			}
		}
		//button is being pressed
		else if(JPot.touchTag==201){
		#if defined FT800_CHIPID
			pressure=FTImpl.Read16(REG_TOUCH_RZ);
		#else 
			pressure=NORMAL_PRESSURE;
		#endif
			for(i=0; i<SPIN_COLUMNS;i++){
				columns[i].velocity = ((NORMAL_PRESSURE-pressure)*MAX_ADDITIONAL_VEL)/NORMAL_PRESSURE;
				//if the the touch was barely sensed, randomly generate a new velocity for each column
				if(columns[i].velocity<10){
					columns[i].velocity =(BASE_VELOCITY +(nextRandomInt8() & 31));
				}
				columns[i].curIndex=(columns[i].curIndex+1+(nextRandomInt8() & 3))%NUM_OF_ICONS;
			}
		}
		else{
		#if defined FT800_CHIPID
			pressure=FTImpl.Read16(REG_TOUCH_RZ);
		#else
			pressure=NORMAL_PRESSURE;
		#endif
			for(i=0; i<SPIN_COLUMNS;i++){
				columns[i].velocity = ((NORMAL_PRESSURE-pressure)*MAX_ADDITIONAL_VEL)/NORMAL_PRESSURE;
				//if the the touch was barely sensed
				if(columns[i].velocity<10){
					columns[i].velocity =(BASE_VELOCITY +(nextRandomInt8() & 31));
				}
				columns[i].curIndex=(columns[i].curIndex+1+(nextRandomInt8() & 3))%NUM_OF_ICONS;
			}
			JPot.released=true;
		}
	}
}


boolean ableToSpin(){
	if(JPot.balance>=JPot.totalBetAmount && !JPot.spinning){
		return true;
	}
	else{
		return false;
	}
}

boolean ableToContinue(){
	if(JPot.balance>=POINTS_PER_COLUMN){
		return true;
	}
	else{
		return false;
	}
}

void unsignedNumToAsciiArray(char *buf, uint16_t num){

	uint8_t temp, i, index=0,last;
	do{
		buf[index]=(num%10)+48;
		index++;
		num/=10;
	}while(num!=0);
	buf[index]='\0';

	last=index-1;
	for (i = 0; i < index/2; i++) {
		temp= buf[i];
		buf[i]=buf[last];
		buf[last] = temp;
		last--;
	}

}



void touched(uint8_t touchVal){
	static uint8_t i,j;
	uint8_t tempVal=0, betMax=10, lineBetMax=10;
	JPot.touchTag=touchVal;

	if(touchVal!=0 && touchVal!=200 && touchVal != 201){
		FTImpl.PlaySound(SOUND_VOL,BUTTON_PRESS_SOUND);
	}

	//bet line selection
	if(touchVal>0 && touchVal<=12){
		if(!JPot.spinning){
		JPot.selectedBetLine=touchVal;
		JPot.totalBetAmount = POINTS_PER_COLUMN * JPot.spinColumnSelected + JPot.selectedBetLine * POINTS_PER_COLUMN;
		JPot.displayRewardLine=false;
		}
		return;
	}

	//multiplier selection
	if(touchVal>=100 && touchVal<100+SPIN_COLUMNS){
		JPot.selectedBetMultiplier=touchVal-100;
		return;
	}


	//status bar buttons
	switch(touchVal){
	case 201: //spin button
		if(!JPot.pendown && ableToSpin() && !JPot.showPayoutTable){
			JPot.spinning=true;
			JPot.released=false;
			JPot.rewardedPoints=false;
			JPot.balance-=JPot.totalBetAmount;
			if(ableToContinue()){
				JPot.reset=false;
			}
			for(i=0; i<SPIN_COLUMNS;i++){
				columns[i].velocity = BASE_VELOCITY;
				columns[i].bounceAmount=columns[i].bounceOffset=BOUNCE_AMOUNT;
			}
		}
		break;
	case 202:  //column betting increase button
		if(!JPot.spinning && !JPot.showPayoutTable){
			if(JPot.spinColumnSelected+1<=SPIN_COLUMNS && JPot.balance>=POINTS_PER_COLUMN){
				JPot.spinColumnSelected++;
				JPot.totalBetAmount = POINTS_PER_COLUMN * JPot.spinColumnSelected + JPot.selectedBetLine * POINTS_PER_COLUMN;
				if(ableToContinue()){
					JPot.reset=false;
				}
			}
		}
		break;
	case 203:  //column betting decrease button
		if(!JPot.spinning  && !JPot.showPayoutTable){
			if(JPot.spinColumnSelected-1>=1){
				JPot.spinColumnSelected--;
				JPot.totalBetAmount = POINTS_PER_COLUMN * JPot.spinColumnSelected + JPot.selectedBetLine * POINTS_PER_COLUMN;
				if(ableToContinue()){
					JPot.reset=false;
				}
			}

		}
		break;
	case 204:  //line betting increase button
		if(!JPot.spinning  && !JPot.showPayoutTable){
			JPot.selectedBetLine = (JPot.selectedBetLine+1) > TOTAL_BET_LINES ? TOTAL_BET_LINES : (JPot.selectedBetLine+1);
			JPot.totalBetAmount = POINTS_PER_COLUMN * JPot.spinColumnSelected + JPot.selectedBetLine * POINTS_PER_COLUMN;
			if(ableToContinue()){
					JPot.reset=false;
				}
			JPot.betChanged=true;
		}

		break;
	case 205:  //line betting decrease
		if(!JPot.spinning  && !JPot.showPayoutTable){
			JPot.selectedBetLine = (JPot.selectedBetLine-1) < 0 ? 0 : (JPot.selectedBetLine-1);
			JPot.totalBetAmount = POINTS_PER_COLUMN * JPot.spinColumnSelected + JPot.selectedBetLine * POINTS_PER_COLUMN;
			if(ableToContinue()){
				JPot.reset=false;
			}
			JPot.betChanged=true;
		}
		break;
	case 206:  //reset game button
		if(!JPot.spinning  && !JPot.showPayoutTable){
		if(JPot.reset){
			JPot.balance=INITIAL_BALANCE;
			JPot.selectedBetLine=1;
			if(ableToContinue()){
				JPot.reset=false;
			}
		}
		else{
			JPot.selectedBetLine=1;
			JPot.spinColumnSelected=1;
			JPot.totalBetAmount=POINTS_PER_COLUMN;
			JPot.reset=true;
		}
		}
		break;
	case 207: //payout table button
		if(ableToContinue()){
			JPot.reset=false;
		}
		if(JPot.showPayoutTable){
			JPot.showPayoutTable=false;
		}
		else{
			JPot.showPayoutTable=true;
		}
		JPot.betChanged=true;
		break;

	}

}

//xor shift random number generator
uint8_t nextRandomInt8() {
	static uint8_t x = 111;
	x ^= (x << 7);
	x ^= (x >> 5);
	return x ^= (x << 3);
}


void clearArray(uint8_t* index, uint16_t size, uint8_t defaultValue){
	memset(index,defaultValue,size);
}



uint16_t getPoints(){
	uint8_t i,j, lookingFor=5, fruitCount=0, fruitTableSize=7,tempMin, tempVal=0;
	uint8_t fruitTable[] = {3,5,6,9,10,11,12};
	uint8_t tempIndex[NUM_OF_ICONS];
	uint16_t points=0;
	boolean finished=false;

	JPot.rewardedPoints=true;
	clearArray(JPot.winningIndex,SPIN_COLUMNS,255);
	clearArray(tempIndex, NUM_OF_ICONS, 0);
	//count the winning indices
	for(i=0; i<JPot.spinColumnSelected; i++){
		//tempIndex[columns[i].drawnIndex]+=1;
		tempIndex[columns[i].iconArray[columns[i].drawnIndex]]+=1;
	}

	for(i=0; i<NUM_OF_ICONS;){
		//check for combination with the highest payout value first.
		if(tempIndex[i]>=lookingFor){
			for(j=0; j<JPot.spinColumnSelected; j++){
				//if(columns[j].drawnIndex==i){
				if(columns[j].iconArray[columns[j].drawnIndex]==i){
					JPot.winningIndex[j]=tempIndex[i];
				}
			}
			if(lookingFor>2){
				return payoutTable[i] * lookingFor;
			}
			else if(lookingFor==2){
				points+=payoutTable[i] * 2;
			}
		}
		i++;
		if(i==NUM_OF_ICONS){
			lookingFor--;
			i=0;
			if(lookingFor<2){
				if(points>0){
					return points; 
				}
				break;
			}
		}
	}


	//check for any fruits
	for(i=0; i<fruitTableSize; i++){
		for(j=0; j<JPot.spinColumnSelected; j++){
			if(fruitTable[i]==columns[j].iconArray[columns[j].drawnIndex]){
				JPot.winningIndex[j]=columns[j].iconArray[columns[j].drawnIndex];
				fruitCount++;
				tempVal++;
			}
		}
	}

	return fruitCount*payoutTable[14];
}






void startingAnimation(){
	uint8_t i;
	JPot.spinning=true;
	JPot.released=true;


	for(i=0;i<SPIN_COLUMNS;i++){
		columns[i].bounceAmount=columns[i].bounceOffset=BOUNCE_AMOUNT;

	}

	while(1){
		FTImpl.DLStart(); 
		FTImpl.Clear(1,1,1);
		FTImpl.ColorRGB(255,255,255);
		updateIndex();
		drawSpinColumns();
		FTImpl.ColorRGB(255,0,0);
		FTImpl.Cmd_Text(FT_DISPLAYWIDTH>>1, UI.spinColumnYOffset+UI.visibleIconsHeight+30, 31, FT_OPT_CENTER, (char*)(strings[2]));
		FTImpl.DLEnd();
	   FTImpl.Finish();
		if(JPot.stoppedColumns == SPIN_COLUMNS){
			break;
		}
	}

	JPot.spinning=false;
	JPot.released=false;
	JPot.spinColumnSelected=SPIN_COLUMNS;

	for(i=0;i<8;i++){
		spinCoins[i].fall=false;
		spinCoins[i].xPos=FT_DISPLAYWIDTH>>1;
		spinCoins[i].yPos=FT_DISPLAYHEIGHT/3;
		spinCoins[i].yVel=-2+ (nextRandomInt8() & 3)*-1;
		nextRandomInt8();
		if((nextRandomInt8()>>7) & 1){
			spinCoins[i].xVel=-4+ (nextRandomInt8() & 3)*-1;
		}
		else{
			spinCoins[i].xVel=4 + (nextRandomInt8() & 3);
		}
		spinCoins[i].index=(nextRandomInt8() & 7);
	}

	for(i=0;i<8;i++){
		FTImpl.PlaySound(SOUND_VOL,COIN_REWARD_SOUND);
	}
}



/* Display FTDI logo animation and application information*/
void infoScreen()
{
	uint16_t dloffset;
	FTImpl.DLStart(); 
	FTImpl.Clear(1,1,1);
	FTImpl.ColorRGB(255,255,255);
	FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,FT_DISPLAYHEIGHT/2,29,FT_OPT_CENTER,(char*)(strings[3]));
	FTImpl.Cmd_Calibrate(0);
	FTImpl.DLEnd();
	FTImpl.Finish();
	FTImpl.Cmd_Logo();
	while(FTImpl.CheckLogo() != FT_GE_FINISHED);
	dloffset = FTImpl.Read16(REG_CMD_DL);
	dloffset -=4;
	FTImpl.Cmd_Memcpy(100000L,FT_RAM_DL,dloffset);  /* Copy the current display list commands to a new location*/
	do
	{
		FTImpl.DLStart();   
		FTImpl.Cmd_Append(100000L,dloffset); /* Append display list commands in RAM_G to the display list buffer*/
			
		/* Reset default values from previous operations*/
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
		FTImpl.Cmd_Text(FT_DISPLAYWIDTH>>1,20,28,FT_OPT_CENTER,(const char*)(info[0]));
		FTImpl.Cmd_Text(FT_DISPLAYWIDTH>>1,60,26,FT_OPT_CENTER,(const char*)(info[1]));
		FTImpl.Cmd_Text(FT_DISPLAYWIDTH>>1,90,26,FT_OPT_CENTER,(const char*)(info[2]));  
		FTImpl.Cmd_Text(FT_DISPLAYWIDTH>>1,FT_DISPLAYHEIGHT-30,27,FT_OPT_CENTER,"Click to play");
		if(sk!='P'){
			FTImpl.ColorRGB(255,255,255);
		}
		else{
			FTImpl.ColorRGB(100,100,100);
			FTImpl.PlaySound(SOUND_VOL,BUTTON_PRESS_SOUND);
		}
		FTImpl.Begin(FT_POINTS);   
		FTImpl.PointSize(20*16);
		FTImpl.Tag('P');
		FTImpl.Vertex2f((FT_DISPLAYWIDTH/2)*16,(FT_DISPLAYHEIGHT-60)*16);
		FTImpl.ColorRGB(180,35,35);
		FTImpl.Begin(FT_LINE_STRIP);
		FTImpl.LineWidth(16*2);
		FTImpl.Vertex2ii((FT_DISPLAYWIDTH/2)-4,(FT_DISPLAYHEIGHT-70),0,0);
		FTImpl.Vertex2ii((FT_DISPLAYWIDTH/2)+10,(FT_DISPLAYHEIGHT-60),0,0);
		FTImpl.Vertex2ii((FT_DISPLAYWIDTH/2)-4,(FT_DISPLAYHEIGHT-50),0,0);
		FTImpl.Vertex2ii((FT_DISPLAYWIDTH/2)-4,(FT_DISPLAYHEIGHT-70),0,0);
		FTImpl.DLEnd();
		FTImpl.Finish();
	}while(Read_Keys()!='P');
}

//display fading out reward amount
void displayRewardText(uint16_t points){
	uint8_t limit=10;
	static uint8_t counter=0;
	static uint16_t lastPoints=0;
	static boolean pointSet=false;

	if(points!=lastPoints && !pointSet){
		counter=255;
		lastPoints=points;
		pointSet=true;
	}
	else if(points!=lastPoints && pointSet){
		counter=255;
		lastPoints=points;
		pointSet=true;
	}

	if(counter>limit){
		if(!JPot.spinning && points>0){
			FTImpl.ColorRGB(255,0,0);
			FTImpl.ColorA(counter);
			FTImpl.Cmd_Text(FT_DISPLAYWIDTH>>1,ICON_HEIGHT*2 + (ICON_HEIGHT>>1) , 31, FT_OPT_RIGHTX, "+ ");
			FTImpl.Cmd_Number( FT_DISPLAYWIDTH>>1,ICON_HEIGHT*2 + (ICON_HEIGHT>>1) , 31, 0, points);
			FTImpl.ColorA(255);
		}
		else{
			lastPoints=0;
		}
		counter-=5;
		if(counter<=limit){
			lastPoints=points;
			pointSet=false;
		}
	}
}

//selected a different bet line button will display the corresponding indexed bitmap/text for a brief moment
void displaySelectedIcon(uint8_t index){
	uint8_t limit=50;
	static uint8_t counter=0, lastIndex=0;
	static boolean counterSet=false;

	if(index!=lastIndex && !counterSet){
		counter=255;
		lastIndex=index;
		counterSet=true;
	}
	else if(index!=lastIndex && counterSet){
		counter=255;
		lastIndex=index;
		counterSet=true;
	}

	if(counter>limit){
		if(!JPot.spinning){
			FTImpl.Begin(FT_BITMAPS);
			FTImpl.ColorRGB(255,255,255);
			FTImpl.ColorA(counter);
			if(index!=14){
				FTImpl.Vertex2ii(FT_DISPLAYWIDTH>>1,FT_DISPLAYHEIGHT>>1,index,0);
			}
			else{
				FTImpl.Cmd_Text(FT_DISPLAYWIDTH>>1,FT_DISPLAYHEIGHT>>1,25, 0, ANY_FRUIT_TEXT);
			}
			FTImpl.ColorA(255);
		}
		counter-=10;
		if(counter<=limit){
			lastIndex=index;
			counterSet=false;
		}
	}
}



void lineBetButtons(){


	uint8_t i, buttonWidth=40, buttonHeight=18, font=22, tempIndex=0;
	int16_t lineButtonStartingX = UI.spinColumnXOffset-50;
	int16_t lineButtonStartingY = UI.spinColumnYOffset;


	char name[4];
	char points[5];
	char value[10];

	uint32_t normalButtonColor =0x8B8682;
	FTImpl.TagMask(1);
	FTImpl.ColorRGB(255,255,255);
	FTImpl.Cmd_FGColor(0xB8860B);
	//line button
	for(i=0; i<TOTAL_BET_LINES;i++){
		name[0]='\0';
		Ft_Dec2Ascii(name,i+1);
		FTImpl.Tag(i+1);
		if(temp_tag == (i+1)){
			FTImpl.Cmd_Button(lineButtonStartingX, lineButtonStartingY, buttonWidth ,buttonHeight , font,FT_OPT_FLAT, name);
		}
		else if(JPot.selectedBetLine == (i+1)){  //current line is selected
			FTImpl.Cmd_FGColor(0x0000CD);
			FTImpl.Cmd_Button(lineButtonStartingX, lineButtonStartingY, buttonWidth ,buttonHeight , font,FT_OPT_FLAT, name);
			FTImpl.Cmd_FGColor(0xB8860B);
		}
		else{
			FTImpl.Cmd_Button(lineButtonStartingX, lineButtonStartingY, buttonWidth ,buttonHeight , font, 0, name);
		}


		lineButtonStartingY+=(buttonHeight+12);
		if(i==5){
			lineButtonStartingY=UI.spinColumnYOffset;
			lineButtonStartingX=UI.spinColumnXOffset + (SPIN_COLUMNS*ICON_WIDTH + SPIN_COLUMNS * COLUMN_GAP);
		}
	}

}



void statusBar(){
	static boolean firstTime=1;
	static uint8_t betTextPixelLength,columnTextPixelLength, lineTextPixelLength,balanceTextPixelLength,resetTextPixelLength,payoutTextPixelLength;
	static uint8_t count=0;
	static uint8_t font=18, betLineFont=27;


	if(firstTime){
		balanceTextPixelLength = (uint8_t)stringPixelWidth(BALANCE_TEXT, betLineFont);
		resetTextPixelLength = (uint8_t)stringPixelWidth(RESET_TEXT, 22);
		payoutTextPixelLength = (uint8_t)stringPixelWidth(PAYOUT_TEXT, 22);
		betTextPixelLength = (uint8_t)stringPixelWidth(BET_TEXT, betLineFont);
		lineTextPixelLength = (uint8_t)stringPixelWidth(LINE_TEXT, betLineFont);
		columnTextPixelLength = (uint8_t)stringPixelWidth(COLUMN_TEXT, betLineFont);
		firstTime=0;
	}

	FTImpl.Tag(0);

	FTImpl.ColorRGB(255,255,255);
	FTImpl.Begin(FT_BITMAPS);

	FTImpl.BlendFunc(FT_SRC_ALPHA,FT_ZERO);
	FTImpl.Vertex2ii(0,FT_DISPLAYHEIGHT-STATUS_BAR_HEIGHT,14,0);
	FTImpl.BlendFunc(FT_SRC_ALPHA,FT_ONE_MINUS_SRC_ALPHA);

	//betting column amount
	FTImpl.ColorRGB(0,0,0);
	FTImpl.Cmd_Text((FT_DISPLAYWIDTH>>1)+BUTTON_GAP, FT_DISPLAYHEIGHT-((STATUS_BAR_HEIGHT*3)>>2) - BUTTON_GAP*2, betLineFont, 0, COLUMN_TEXT);
	FTImpl.Cmd_Number( (FT_DISPLAYWIDTH>>1)+BUTTON_GAP + columnTextPixelLength , FT_DISPLAYHEIGHT-((STATUS_BAR_HEIGHT*3)>>2) - BUTTON_GAP*2, betLineFont, 0, JPot.spinColumnSelected);

	//total bet amount
	FTImpl.Cmd_Text(resetTextPixelLength + BUTTON_GAP +10, FT_DISPLAYHEIGHT-((STATUS_BAR_HEIGHT*3)>>2) - BUTTON_GAP*2, betLineFont, 0, BET_TEXT);
	FTImpl.Cmd_Number( resetTextPixelLength + BUTTON_GAP*2 + betTextPixelLength +8, FT_DISPLAYHEIGHT-((STATUS_BAR_HEIGHT*3)>>2) - BUTTON_GAP*2, betLineFont, 0, JPot.totalBetAmount);


	//selected bet line and its bet amount
	FTImpl.Cmd_Text((FT_DISPLAYWIDTH>>1)-BUTTON_GAP-(UI.spinButtonWidth>>1)*2 -2, FT_DISPLAYHEIGHT-((STATUS_BAR_HEIGHT*3)>>2) - BUTTON_GAP*2, betLineFont, 0, LINE_TEXT);
	FTImpl.Cmd_Number( (FT_DISPLAYWIDTH>>1)-BUTTON_GAP-(UI.spinButtonWidth>>1)*2 -2 + lineTextPixelLength, FT_DISPLAYHEIGHT-((STATUS_BAR_HEIGHT*3)>>2) - BUTTON_GAP*2, betLineFont, 0, JPot.selectedBetLine);

	//balance text and value
	FTImpl.LineWidth(BUTTON_CURVATURE*16);

	FTImpl.Cmd_Text(BUTTON_CURVATURE*2, FT_DISPLAYHEIGHT-(STATUS_BAR_HEIGHT>>1), betLineFont, 0, BALANCE_TEXT);
	FTImpl.Cmd_Number( BUTTON_CURVATURE*10 + betTextPixelLength + 35, FT_DISPLAYHEIGHT-(STATUS_BAR_HEIGHT>>1), betLineFont, FT_OPT_RIGHTX, JPot.balance);

	FTImpl.TagMask(1);

	//payout table tracking box
	FTImpl.ColorMask(0,0,0,0);
	FTImpl.LineWidth(16);
	FTImpl.Begin(FT_RECTS);	
	FTImpl.Tag(200);
	FTImpl.Vertex2ii(UI.spinColumnXOffset,0, 0,0);
	FTImpl.Vertex2ii(FT_DISPLAYWIDTH, FT_DISPLAYHEIGHT-STATUS_BAR_HEIGHT-BUTTON_GAP*2,0,0);
	FTImpl.ColorMask(1,1,1,1);



	//spin button
	FTImpl.ColorRGB(255,0,0);
	FTImpl.Cmd_FGColor(0xB8860B);
	FTImpl.Tag(201);
	if(temp_tag==201 && !JPot.pendown)
		FTImpl.Cmd_Button(FT_DISPLAYWIDTH-UI.spinButtonWidth-BUTTON_GAP, FT_DISPLAYHEIGHT-STATUS_BAR_HEIGHT + BUTTON_GAP, UI.spinButtonWidth ,STATUS_BAR_HEIGHT-BUTTON_GAP*2 , 30, FT_OPT_FLAT, SPIN_TEXT);
	else
		FTImpl.Cmd_Button(FT_DISPLAYWIDTH-UI.spinButtonWidth-BUTTON_GAP, FT_DISPLAYHEIGHT-STATUS_BAR_HEIGHT + BUTTON_GAP, UI.spinButtonWidth ,STATUS_BAR_HEIGHT-BUTTON_GAP*2 , 30, 0, SPIN_TEXT);




	FTImpl.ColorRGB(255,0,0);


	//column selection button increase
	FTImpl.Tag(202);
	if(temp_tag==202)
		FTImpl.Cmd_Button((FT_DISPLAYWIDTH>>1)+BUTTON_GAP, FT_DISPLAYHEIGHT-(STATUS_BAR_HEIGHT>>1), UI.spinButtonWidth>>1 ,STATUS_BAR_HEIGHT>>1, 30, FT_OPT_FLAT, "+");
	else
		FTImpl.Cmd_Button((FT_DISPLAYWIDTH>>1)+BUTTON_GAP, FT_DISPLAYHEIGHT-(STATUS_BAR_HEIGHT>>1), UI.spinButtonWidth>>1 ,STATUS_BAR_HEIGHT>>1, 30,0, "+");

	//column selection button decrease
	FTImpl.Tag(203);
	if(temp_tag==203)
		FTImpl.Cmd_Button((FT_DISPLAYWIDTH>>1)+BUTTON_GAP+2 + (UI.spinButtonWidth>>1), FT_DISPLAYHEIGHT-(STATUS_BAR_HEIGHT>>1), UI.spinButtonWidth>>1 ,STATUS_BAR_HEIGHT>>1 , 30,FT_OPT_FLAT, "-");
	else
		FTImpl.Cmd_Button((FT_DISPLAYWIDTH>>1)+BUTTON_GAP+2 + (UI.spinButtonWidth>>1), FT_DISPLAYHEIGHT-(STATUS_BAR_HEIGHT>>1), UI.spinButtonWidth>>1 ,STATUS_BAR_HEIGHT>>1 , 30, 0, "-");

	//coin selection button decrease
	FTImpl.Tag(205);
	if(temp_tag==205)
		FTImpl.Cmd_Button((FT_DISPLAYWIDTH>>1)-BUTTON_GAP-(UI.spinButtonWidth>>1), FT_DISPLAYHEIGHT-(STATUS_BAR_HEIGHT>>1), UI.spinButtonWidth>>1 ,STATUS_BAR_HEIGHT>>1, 30, FT_OPT_FLAT, "-");
	else
		FTImpl.Cmd_Button((FT_DISPLAYWIDTH>>1)-BUTTON_GAP-(UI.spinButtonWidth>>1), FT_DISPLAYHEIGHT-(STATUS_BAR_HEIGHT>>1), UI.spinButtonWidth>>1 ,STATUS_BAR_HEIGHT>>1, 30, 0, "-");

	//coin selection button increase
	FTImpl.Tag(204);
	if(temp_tag==204)
		FTImpl.Cmd_Button((FT_DISPLAYWIDTH>>1)-BUTTON_GAP-(UI.spinButtonWidth>>1)*2 -2, FT_DISPLAYHEIGHT-(STATUS_BAR_HEIGHT>>1), UI.spinButtonWidth>>1 ,STATUS_BAR_HEIGHT>>1, 30, FT_OPT_FLAT, "+");
	else
		FTImpl.Cmd_Button((FT_DISPLAYWIDTH>>1)-BUTTON_GAP-(UI.spinButtonWidth>>1)*2 -2, FT_DISPLAYHEIGHT-(STATUS_BAR_HEIGHT>>1), UI.spinButtonWidth>>1 ,STATUS_BAR_HEIGHT>>1, 30, 0, "+");

	//reset or new game button
	FTImpl.Tag(206);
	if(JPot.reset && !JPot.spinning){
		if(temp_tag==206){
			FTImpl.Cmd_Button(count, FT_DISPLAYHEIGHT-STATUS_BAR_HEIGHT+BUTTON_GAP, resetTextPixelLength ,(STATUS_BAR_HEIGHT>>1)-BUTTON_GAP*2, 21, FT_OPT_FLAT, "N.G.");
		}
		else{
			FTImpl.Cmd_Button(count, FT_DISPLAYHEIGHT-STATUS_BAR_HEIGHT+BUTTON_GAP, resetTextPixelLength ,(STATUS_BAR_HEIGHT>>1)-BUTTON_GAP*2, 21, 0, "N.G.");
		}
	}
	else{
		if(temp_tag==206){
			FTImpl.Cmd_Button(count, FT_DISPLAYHEIGHT-STATUS_BAR_HEIGHT+BUTTON_GAP, resetTextPixelLength ,(STATUS_BAR_HEIGHT>>1)-BUTTON_GAP*2, 22, FT_OPT_FLAT, RESET_TEXT);
		}
		else{
			FTImpl.Cmd_Button(count, FT_DISPLAYHEIGHT-STATUS_BAR_HEIGHT+BUTTON_GAP, resetTextPixelLength ,(STATUS_BAR_HEIGHT>>1)-BUTTON_GAP*2, 22, 0, RESET_TEXT);
		}
	}

	//payout table button
	FTImpl.Tag(207);
	if(JPot.showPayoutTable){
		if(temp_tag==207)
			FTImpl.Cmd_Button(0, FT_DISPLAYHEIGHT-STATUS_BAR_HEIGHT-17, payoutTextPixelLength ,(STATUS_BAR_HEIGHT>>1)-BUTTON_GAP*2, 22, FT_OPT_FLAT, EXIT_TEXT);
		else
			FTImpl.Cmd_Button(0, FT_DISPLAYHEIGHT-STATUS_BAR_HEIGHT-17, payoutTextPixelLength ,(STATUS_BAR_HEIGHT>>1)-BUTTON_GAP*2, 22, 0, EXIT_TEXT);
	}
	else{
		if(temp_tag==207)
			FTImpl.Cmd_Button(0, FT_DISPLAYHEIGHT-STATUS_BAR_HEIGHT-17, payoutTextPixelLength ,(STATUS_BAR_HEIGHT>>1)-BUTTON_GAP*2, 22,FT_OPT_FLAT, PAYOUT_TEXT);
		else
			FTImpl.Cmd_Button(0, FT_DISPLAYHEIGHT-STATUS_BAR_HEIGHT-17, payoutTextPixelLength ,(STATUS_BAR_HEIGHT>>1)-BUTTON_GAP*2, 22,0, PAYOUT_TEXT);
	}

	FTImpl.TagMask(0);
}

uint16_t fillLineIndexandPoints(){
	uint8_t i,j,k,l,m,n,o,fruitCount=0,fruitTableSize=7,lookingFor=SPIN_COLUMNS,tempPoints, lastLookingFor;
	uint8_t fruitTable[] = {3,5,6,9,10,11,12}, wonIndex[2];
	boolean finished=false,fruitFound=false,setVal;
	uint8_t tempIndex[NUM_OF_ICONS];
	uint16_t totalPoints=0;


		for(i=0;i<TOTAL_BET_LINES;i++){

		if(i>=JPot.selectedBetLine){
			for(n=0;n<SPIN_COLUMNS;n++){
				JPot.lineBet[i][n]=254;
			}
			continue;
		}

		finished=false;
		clearArray(tempIndex, NUM_OF_ICONS, 0);
		lookingFor=SPIN_COLUMNS;
		fruitCount=0;
		tempPoints=0;
		lastLookingFor=254;
		wonIndex[0]=254;
		wonIndex[1]=254;
		setVal=false;
		for(j=0;j<SPIN_COLUMNS;j++){
			//if(linePosition[i].line[j] == 't'){ 
			if(pgm_read_byte(lineposition + i*SPIN_COLUMNS + j) == 't'){

				JPot.lineBet[i][j] = (columns[j].drawnIndex-1) < 0 ? columns[j].iconArray[NUM_OF_ICONS-1] : columns[j].iconArray[columns[j].drawnIndex-1];
			}
			//else if(linePosition[i].line[j] == 'm'){
			else if(pgm_read_byte(lineposition + i*SPIN_COLUMNS + j) == 'm'){

				JPot.lineBet[i][j] = columns[j].iconArray[columns[j].drawnIndex];
			}
			else{
				JPot.lineBet[i][j] = (columns[j].drawnIndex+1) >= NUM_OF_ICONS ? columns[j].iconArray[0] : columns[j].iconArray[columns[j].drawnIndex+1];
			}
			tempIndex[JPot.lineBet[i][j]]++;
		}




		for(k=0; k<NUM_OF_ICONS;){
			//check for combination with the highest payout value first.
			if(tempIndex[k]>=lookingFor){
					if(setVal==false){
					for(o=0;o<NUM_OF_ICONS;o++){
						if(tempIndex[o]>=lookingFor){
							if(wonIndex[0]==254){  //for 5 columns
								wonIndex[0]=o;
							}
							else{
								wonIndex[1]=o;
							}
						}
					}
					setVal=true;
				}
				for(l=0; l<SPIN_COLUMNS; l++){
					if(JPot.lineBet[i][l] != wonIndex[0] && JPot.lineBet[i][l] != wonIndex[1]){
							JPot.lineBet[i][l]=254;
					}
				}
				if(lookingFor>2){
					totalPoints = payoutTable[k] * lookingFor;
					finished=true;
				}
				else if(lookingFor==2){
					tempPoints+=payoutTable[k] * 2;
				}
			}
			if(finished){
				break;
			}
			k++;
			if(k==NUM_OF_ICONS){
				lookingFor--;
				k=0;
				if(lookingFor<2){
					break;
				}
			}
		}

		if(!finished && tempPoints>0){
			totalPoints+=tempPoints;
			finished=true;
		}

		//check for any fruits
		if(!finished){
			for(m=0;m<SPIN_COLUMNS;m++){
				fruitFound=false;
				for(n=0;n<fruitTableSize;n++){
					if(JPot.lineBet[i][m] == fruitTable[n]){
						fruitFound=true;
						fruitCount++;
					}
				}
				if(!fruitFound){
					JPot.lineBet[i][m]=254;
				}
			}
			totalPoints += fruitCount*payoutTable[14];
		}
	}
	return totalPoints;
}


void jackpot(){
	uint16_t earnedPoints=0;
	uint8_t i, spinCoinAmount=8;
	JPot.totalBetAmount = POINTS_PER_COLUMN * JPot.spinColumnSelected + JPot.selectedBetLine * POINTS_PER_COLUMN;
	while(1){


		FTImpl.DLStart();
		FTImpl.ColorRGB(0,0,0);
		FTImpl.Clear(1,1,1);
		touched(Read_Keys());

		FTImpl.ColorRGB(255,255,255);
		FTImpl.Begin(FT_BITMAPS);

		//coin FT_REPEAT background

		FTImpl.BitmapHandle(15);
		FTImpl.BitmapSource(bitmapInfo[1].Offset);
		FTImpl.BitmapLayout(FT_RGB565,bitmapInfo[1].Width,bitmapInfo[1].Height);
		FTImpl.BitmapSize(FT_NEAREST,FT_REPEAT,FT_REPEAT,FT_DISPLAYWIDTH,FT_DISPLAYHEIGHT-STATUS_BAR_HEIGHT);
		FTImpl.Vertex2f(0,0);

		
		if(JPot.showPayoutTable){
			drawPayoutTable();
		}
		else{
			drawSpinColumns();
			if(!JPot.spinning)
				drawbetlines();

			if(JPot.rewardedPoints){
				redrawColumnIcons();
			}
		}


		//reward points when all columns have stopped
		if(JPot.stoppedColumns==SPIN_COLUMNS){
			JPot.spinning=false;
			if(!JPot.rewardedPoints){
				earnedPoints=fillLineIndexandPoints();
				JPot.displayRewardLine=true;
				currentLine=0;
				earnedPoints+=getPoints();
				JPot.betChanged=false;
				if(earnedPoints>0){
					FTImpl.PlaySound(SOUND_VOL, 0x50);  //winning sound
					JPot.balance+=earnedPoints;
					spinCoinAmount=(earnedPoints>>4);
					if(spinCoinAmount>8){
						spinCoinAmount=8;
					}
					for(i=0;i<spinCoinAmount;i++){
						spinCoins[i].fall=false;
						spinCoins[i].xPos=FT_DISPLAYWIDTH>>1; 
						spinCoins[i].yPos=FT_DISPLAYHEIGHT/3;
						spinCoins[i].yVel=-2+ (nextRandomInt8() & 3)*-1;
						nextRandomInt8();
						if(nextRandomInt8() & 1){
							spinCoins[i].xVel=-4 + (nextRandomInt8() & 3)*-1;
						}
						else{
							spinCoins[i].xVel=4 + (nextRandomInt8() & 3);
						}
						spinCoins[i].index=(nextRandomInt8() & 7);
					}

					for(i=0;i<spinCoinAmount;i++){
						FTImpl.PlaySound(SOUND_VOL, COIN_REWARD_SOUND);
					}

				}

				//check the balance to see if the game can be continued. 
				if(!ableToContinue() && JPot.rewardedPoints){
					JPot.reset=true;
				}
				else{
					JPot.reset=false;
				}
			}
		}

		statusBar();
		if(!JPot.showPayoutTable){
			lineBetButtons();
		}

		coinsAnimation(spinCoinAmount);


		displayRewardText(earnedPoints);
		FTImpl.DLEnd();
		FTImpl.Finish();

		delay(50);

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
		//Serial.print("Error in chip id read ");
		//Serial.println(chipid,HEX);
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
		FTImpl.Cmd_Text(FT_DISPLAYWIDTH>>1,FT_DISPLAYHEIGHT>>1,29,FT_OPT_CENTER,(char*)pgm_read_word(&strings[1]));
		FTImpl.DLEnd();
		FTImpl.Finish();
		delay(5000);
		}
}


void setup()
{
/* Initialize serial print related functionality */
	//Serial.begin(9600);

	/*Initialize the SD object. Screen error message can only be displayed when the FT801 is ready*/
	sd_present = FtSd.Init();
	
	
	
	/* Set the Display Enable pin*/   
	//Serial.println("--Start Application--");
	if(BootupConfigure())
	{
		//error case - do not do any thing
	}
  	else
	{
		CheckStorageDevicePrecence(); 
		infoScreen();
		jackpotSetup();
		startingAnimation();
		jackpot();
		FTImpl.Exit();  /*  Safely close all opened handles */
	}
	//Serial.println("--End Application--");
	
	
	

}

void loop()
{

}



/* Nothing beyond this */














