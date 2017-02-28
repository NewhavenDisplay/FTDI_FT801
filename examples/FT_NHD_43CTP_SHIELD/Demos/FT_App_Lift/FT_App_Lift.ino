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
* @file                           FT_App_Lift.ino
* @brief                          Sample application to demonstrate FT801 primitives, widgets and customized screen shots. 
* @version                        0.1.0
* @date                           2014/03/21
*
*/

#include "SPI.h"
#include "Wire.h"
#include "FT_NHD_43CTP_SHIELD.h"
#include "FT_App_Lift.h"




#define STARTUP_ADDRESS	100*1024L

/* Application specific Macro for portrait and landscape screen orientation.*/
#define ORIENTATION_PORTRAIT
//#define ORIENTATION_LANDSCAPE



/* SD card object*/
FT_SD FtSd(FT_SD_CSPIN);

/* sd_present holds error values during initialization.  0 means no error and all other errors are non zero value */
FT_SDStatus sd_present; 

/*SD card file object*/
FT_SDFile g_reader;

/* Global object for transport */
FT801IMPL_SPI FTImpl(FT_CS_PIN,FT_PDN_PIN,FT_INT_PIN);




PROGMEM const FT_Fonts_t g_Gpu_Fonts[] = {
/* VC1 Hardware Fonts index 28*/
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,6,8,13,12,15,13,5,7,7,9,12,5,9,6,9,12,12,12,12,12,12,12,12,12,12,5,5,11,12,11,10,19,13,13,13,14,12,12,14,15,6,12,14,12,18,15,14,13,15,13,13,13,14,14,18,13,13,13,6,9,6,9,10,7,12,12,11,12,11,8,12,12,5,5,11,5,18,12,12,12,12,7,11,7,12,11,16,11,11,11,7,5,7,14,5,
2,
9,
18,
25,
950172},
};











/* API to load raw data from file into perticular locatio of ft801 */

int32_t Ft_App_LoadRawFromFile(FT_SDFile &r,int32_t DstAddr)
{
	int32_t FileLen = 0,Ft801_addr = FT_RAM_G;
        uint8_t bytes_size,sector_size;
        uint8_t pbuff[512];
       
 	Ft801_addr = DstAddr;
        FileLen = r.Size;
      
 	while(FileLen > 0)
	{
                /* download the data into the command buffer by 512bytes one shot */
		uint16_t blocklen = FileLen>512?512:FileLen;
                r.ReadSector(pbuff);
		FileLen -= blocklen;
		/* copy data continuously into FT801 memory */
		FTImpl.Write(Ft801_addr,pbuff,blocklen);
		Ft801_addr += blocklen;
	}
	return 0;
}

/* APIs useful for bitmap postprocessing */
/* Identity transform matrix */
int16_t Ft_App_TrsMtrxLoadIdentity(Ft_App_GraphicsMatrix_t *pMatrix)
{
	/* Load the identity matrix into the input pointer */
	pMatrix->a = 1*FT_APP_MATIRX_PRECITION;
	pMatrix->b = 0*FT_APP_MATIRX_PRECITION;
	pMatrix->c = 0*FT_APP_MATIRX_PRECITION;
	pMatrix->d = 0*FT_APP_MATIRX_PRECITION;
	pMatrix->e = 1*FT_APP_MATIRX_PRECITION;
	pMatrix->f = 0*FT_APP_MATIRX_PRECITION;
	return 0;
}
int16_t Ft_App_UpdateTrsMtrx(Ft_App_GraphicsMatrix_t *pMatrix,Ft_App_PostProcess_Transform_t *ptrnsmtrx)
{
	/* perform the translation from float to FT801 specific transform units */
	ptrnsmtrx->Transforma = pMatrix->a*FT_APP_MATRIX_GPU_PRECITION;
	ptrnsmtrx->Transformd = pMatrix->d*FT_APP_MATRIX_GPU_PRECITION;
	ptrnsmtrx->Transformb = pMatrix->b*FT_APP_MATRIX_GPU_PRECITION;
	ptrnsmtrx->Transforme = pMatrix->e*FT_APP_MATRIX_GPU_PRECITION;
	ptrnsmtrx->Transformc = pMatrix->c*FT_APP_MATRIX_GPU_PRECITION;
	ptrnsmtrx->Transformf = pMatrix->f*FT_APP_MATRIX_GPU_PRECITION;

	return 0;
}

/* give out the translated values. */
int16_t Ft_App_TrsMtrxTranslate(Ft_App_GraphicsMatrix_t *pMatrix,float x,float y,float *pxres,float *pyres)
{
	float a,b,c,d,e,f;
        
	/* matrix multiplication */
	a = pMatrix->a;	b = pMatrix->b;	c = pMatrix->c;	d = pMatrix->d;	e = pMatrix->e;	f = pMatrix->f;

	*pxres = x*a + y*b + c;
	*pyres = x*d + y*e + f;
	pMatrix->c = *pxres;
	pMatrix->f = *pyres;

	return 0;
}
/* API for rotation of angle degree - using vc++ math library - if not then use array of sine tables to compule both sine and cosine */
/* +ve degree for anti clock wise and -ve for clock wise */
/* note not checking the overflow cases */
int16_t Ft_App_TrsMtrxRotate(Ft_App_GraphicsMatrix_t *pMatrix,int32_t th)
{
	float pi = 3.1415926535;
	float angle = -th*pi/180;
	Ft_App_GraphicsMatrix_t tempmtx,tempinput;


	tempinput.a = pMatrix->a;
	tempinput.b = pMatrix->b;
	tempinput.c = pMatrix->c;
	tempinput.d = pMatrix->d;
	tempinput.e = pMatrix->e;
	tempinput.f = pMatrix->f;

	Ft_App_TrsMtrxLoadIdentity(&tempmtx);
	tempmtx.a = cos(angle)*FT_APP_MATIRX_PRECITION;
	tempmtx.b = sin(angle)*FT_APP_MATIRX_PRECITION;
	tempmtx.d = -sin(angle)*FT_APP_MATIRX_PRECITION;
	tempmtx.e = cos(angle)*FT_APP_MATIRX_PRECITION;

	/* perform matrix multiplecation and store in the input */
	pMatrix->a = tempinput.a*tempmtx.a + tempinput.b*tempmtx.d;
	pMatrix->b = tempinput.a*tempmtx.b + tempinput.b*tempmtx.e;
	pMatrix->c = tempinput.a*tempmtx.c + tempinput.b*tempmtx.f + tempinput.c*1;
	pMatrix->d = tempinput.d*tempmtx.a + tempinput.e*tempmtx.d;
	pMatrix->e = tempinput.d*tempmtx.b + tempinput.e*tempmtx.e;
	pMatrix->f = tempinput.d*tempmtx.c + tempinput.e*tempmtx.f + tempinput.f*1;
	
	return 0;
}
/* Scaling done for x and y factors - from 1 to 255 */
/* Input units are in terms of 65536 */
int16_t Ft_App_TrsMtrxScale(Ft_App_GraphicsMatrix_t *pMatrix,float xfactor,float yfactor)
{
	pMatrix->a /= xfactor;
	pMatrix->d /= xfactor;

	pMatrix->b /= yfactor;
	pMatrix->e /= yfactor;

	return 0;
}
/* flip the image - 1 for right flip, 2 for bottom flip */
int16_t Ft_App_TrsMtrxFlip(Ft_App_GraphicsMatrix_t *pMatrix,int32_t Option)
{
	/* need to verify both */
	if(FT_APP_GRAPHICS_FLIP_RIGHT == (Option & FT_APP_GRAPHICS_FLIP_RIGHT))
	{
		pMatrix->a = -pMatrix->a;
		pMatrix->d = -pMatrix->d;
	}
	if(FT_APP_GRAPHICS_FLIP_BOTTOM == (Option & FT_APP_GRAPHICS_FLIP_BOTTOM))
	{
		pMatrix->b = -pMatrix->b;
		pMatrix->e = -pMatrix->e;
	}

	return 0;
}
/* Arrawy used for custom fonts */
S_LiftAppFont_t G_LiftAppFontArrayNumbers[1] = 
{
	//font structure
	{
	/* Max Width */
	80,
	/* Max Height */
	156,
	/* Max Stride */
	80,
	/* format */
	FT_L8,
	/* Each character width */
	80,80,80,80,80,80,80,80,80,80,80,80,
	}
};
S_LiftAppFont_t G_LiftAppFontArrayArrow[1] = 
{	//arrow structure
	{
	/* Max Width */
	80,
	/* Max Height */
	85,
	/* Max Stride */
	80,
	/* format */
	FT_L8,
	/* Each character width */
	80,
	}
};

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

/* API to compute the bitmap offset wrt input characters and scale 
   ResizeVal is in terms of 8 bits precision - ex 1 will be equal to 256
   xOffset and  yOffset are in terms of 4 bits precision - 1/16th pixel format and are the center of the display character
   Even the FT801 display list is generated in this API
*/
int32_t FT_LiftAppComputeBitmap(S_LiftAppFont_t *pLAFont, int32_t FloorNum,uint8_t TotalNumChar,int32_t ResizeVal,int16_t xOffset,int16_t yOffset)
{
	/* compute the total number of digits in the input */
	uint8_t TotalChar = 1;
	int32_t TotHzSz,TotVtSz,FlNum,i,xoff,yoff;
	char FontValArray[8];

	TotalChar = 1;
	FlNum = FloorNum;
	TotHzSz = pLAFont->MaxWidth;
	FontValArray[0] = '\0';//requirement from dec2ascii api

	Ft_Dec2Ascii(FontValArray,FloorNum);
	TotalChar = strlen(FontValArray);
	if(0 != TotalNumChar)
	{
		TotalChar = TotalNumChar;
	}
	TotHzSz = (pLAFont->MaxWidth * 16 * ResizeVal)/LIFTAPPCHARRESIZEPRECISION;
	TotVtSz = (pLAFont->MaxHeight * 16 * ResizeVal)/LIFTAPPCHARRESIZEPRECISION;
	
	//change the x and y offset to center of the characters
	xoff = xOffset*16 - TotHzSz*TotalChar/2;
	yoff = yOffset*16 - TotVtSz/2;
	/* since resize value is same for both the sides the resize is same for all characters */
	for(i=0;i<TotalChar;i++)
	{
		/* Calculation of cell number from  */
		if('-' == FontValArray[i])
		{
			FTImpl.Cell(0);
		}
		else
		{

			FTImpl.Cell(FontValArray[i]-'0' + 1);
		}
		FTImpl.Vertex2f(xoff,yoff);
		//increment the character after every ittiration
		xoff += TotHzSz;
	}
	return 0;
}

int32_t FT_LiftAppComputeBitmapRowRotate(S_LiftAppFont_t *pLAFont, int32_t FloorNum,uint8_t TotalNumChar,int32_t ResizeVal,int16_t xOffset,int16_t yOffset)
{
	/* compute the total number of digits in the input */
	uint8_t TotalChar = 1;
	int32_t TotHzSz,TotVtSz,FlNum,i,xoff,yoff;
	char FontValArray[8];

	TotalChar = 1;
	FlNum = FloorNum;
	TotHzSz = pLAFont->MaxWidth;
	FontValArray[0] = '\0';//requirement from dec2ascii api

	Ft_Dec2Ascii(FontValArray,FloorNum);
	TotalChar = strlen(FontValArray);
	if(0 != TotalNumChar)
	{
		TotalChar = TotalNumChar;
	}
	TotHzSz = (pLAFont->MaxWidth * 16 * ResizeVal)/LIFTAPPCHARRESIZEPRECISION;
	TotVtSz = (pLAFont->MaxHeight * 16 * ResizeVal)/LIFTAPPCHARRESIZEPRECISION;
	
	//change the x and y offset to center of the characters
	xoff = xOffset*16 - TotVtSz/2;
	yoff = yOffset*16 - (TotHzSz*TotalChar/2) + (TotHzSz*(TotalChar-1));
	/* since resize value is same for both the sides the resize is same for all characters */
	for(i=0;i<TotalChar;i++)
	{
		/* Calculation of cell number from  */
		if('-' == FontValArray[i])
		{
			FTImpl.Cell(0);
		}
		else
		{

			FTImpl.Cell(FontValArray[i]-'0' + 1);
		}
		FTImpl.Vertex2f(xoff,yoff);
		//increment the character after every ittiration
		yoff -= TotHzSz;
	}
	return 0;
}


/* API for floor number logic */
int32_t FT_LiftApp_FTransition(S_LiftAppCtxt_t *pLACtxt, int32_t dstaddr/*,uint8_t opt_landscape*/)
{

	char StringArray[8];
	int32_t dst_addr_st_file;
	StringArray[0] = '\0';
	
	/* check for the current floor number logic */
	if(pLACtxt->CurrFloorNum == pLACtxt->NextFloorNum)
	{

		/* first time only entry */
		if(LIFTAPP_DIR_NONE != pLACtxt->ArrowDir)
		{
			pLACtxt->CurrFloorNumStagnantRate = pLACtxt->SLATransPrms.FloorNumStagnantRate;
			pLACtxt->CurrFloorNumResizeRate = pLACtxt->SLATransPrms.ResizeRate;
		}
		/* Make the direction to 0 */
		pLACtxt->ArrowDir = LIFTAPP_DIR_NONE;//arrow is disabled in none direction
		{
                  //for landscape orienation
                  if(pLACtxt->opt_orientation == 1)
                  {
                	if((LIFTAPPAUDIOSTATE_NONE == pLACtxt->AudioState) && (0 == pLACtxt->AudioPlayFlag))
                	{
                          char *pstring;
                          
                          pLACtxt->AudioState = LIFTAPPAUDIOSTATE_INIT;
                          pLACtxt->AudioFileIdx = 0;
                          pLACtxt->AudioCurrAddr = dstaddr;
                          pLACtxt->AudioCurrFileSz = 0;
                          pstring = pLACtxt->AudioFileNames;
                          *pstring = '\0';
                          strcpy(pstring,"bl.wav");
                          pstring += strlen(pstring);
                          *pstring++ = '\0';
                          StringArray[0] = '\0';
                          Ft_Dec2Ascii(StringArray,(int32_t)pLACtxt->CurrFloorNum);
                          strcat(StringArray, ".wav");
                          strcpy(pstring,StringArray);
                          pstring += strlen(pstring);
                          *pstring++ = '\0';
                          *pstring = '\0';
                          pLACtxt->AudioPlayFlag = 1;
                          //Ft_App_AudioPlay(pLACtxt,dstaddr,2*1024);/* load the floor number audio */
                        }
                	  /* load the ring tone */
                 	Ft_App_AudioPlay(pLACtxt,dstaddr,2*1024L);/* load the floor number audio */
                        
                  }
          
                     //for portrait orientation
                    else if(pLACtxt->opt_orientation == 0)  
                    {
                       
                	if((LIFTAPPAUDIOSTATE_NONE == pLACtxt->AudioState) && (0 == pLACtxt->AudioPlayFlag))
                	{
                          char *pstring;
                          
                          pLACtxt->AudioState = LIFTAPPAUDIOSTATE_INIT;
                          pLACtxt->AudioFileIdx = 0;
                          pLACtxt->AudioCurrAddr = dstaddr;
                          pLACtxt->AudioCurrFileSz = 0;
                          pstring = pLACtxt->AudioFileNames;
                          *pstring = '\0';
                          strcpy(pstring,"bf.wav");
                          pstring += strlen(pstring);
                          *pstring++ = '\0';
                          StringArray[0] = '\0';
                          Ft_Dec2Ascii(StringArray,(int32_t)pLACtxt->CurrFloorNum);
                          strcat(StringArray, ".wav");
                          strcpy(pstring,StringArray);
                          pstring += strlen(pstring);
                          *pstring++ = '\0';
                          *pstring = '\0';
                          //Ft_App_AudioPlay(pLACtxt,dstaddr,2*1024);/* load the floor number audio */
                          pLACtxt->AudioPlayFlag = 1;
                        }
                	  /* load the ring tone */
                          Ft_App_AudioPlay(pLACtxt,dstaddr,2*1024L);/* load the floor number audio */
                         
                    }

		}


		/* resizing of floor number */
		if(pLACtxt->CurrFloorNumResizeRate > 0)
		{
			pLACtxt->CurrFloorNumResizeRate--;
			return 0;
		}
		
		pLACtxt->CurrFloorNumResizeRate = 0;

		/* rate implmenetation */
		if(pLACtxt->CurrFloorNumStagnantRate > 0)
		{
			
			pLACtxt->CurrFloorNumStagnantRate--;//rate based logic
			return 0;//do not perform any thing

		}

		pLACtxt->CurrFloorNumStagnantRate = 0;
                 
		/* delay the below code till rate is completed - modify the rate if application over rights */
		//limitation - make sure the max is +ve
		while((pLACtxt->CurrFloorNum == pLACtxt->NextFloorNum) || (0 == pLACtxt->NextFloorNum))//to make sure the same floor number is not assigned
		{
			pLACtxt->NextFloorNum = pLACtxt->SLATransPrms.MinFloorNum + random(pLACtxt->SLATransPrms.MaxFloorNum - pLACtxt->SLATransPrms.MinFloorNum);
		}

		pLACtxt->ArrowDir = LIFTAPP_DIR_DOWN;
		/* generate a new random number and change the direction as well */
                	//if(LIFTAPPAUDIOSTATE_NONE == pLACtxt->AudioState)	
                {
                  char *pstring;
                  
                  pLACtxt->AudioState = LIFTAPPAUDIOSTATE_INIT;
                  pLACtxt->AudioPlayFlag = 0;
                  pstring = pLACtxt->AudioFileNames;
                  *pstring = '\0';                
        	  /* load the ring tone */
		if(pLACtxt->NextFloorNum > pLACtxt->CurrFloorNum)
		{
			pLACtxt->ArrowDir = LIFTAPP_DIR_UP;
			//Ft_App_LoadRawAndPlay("gu.wav",(int32_t)dstaddr,0,0);
                        strcpy(pstring,"gu.wav");
		}
		else
		{
			//Ft_App_LoadRawAndPlay("gd.wav",(int32_t)dstaddr,0,0);
                        strcpy(pstring,"gd.wav");
		}
                          pLACtxt->AudioFileIdx = 0;
                          pLACtxt->AudioCurrAddr = dstaddr;
                          pLACtxt->AudioCurrFileSz = 0;

                   pstring += strlen(pstring);
                  *pstring++ = '\0';
                  *pstring++ = '\0';
                  *pstring = '\0';
                }
		Ft_App_AudioPlay(pLACtxt,dstaddr,2*1024L);/* load the floor number audio */

		/* set the starting of the arrow resize */
		pLACtxt->CurrArrowResizeRate = pLACtxt->SLATransPrms.ResizeRate;
		//set the curr floor number change 
		pLACtxt->CurrFloorNumChangeRate = pLACtxt->SLATransPrms.FloorNumChangeRate;
		return 0;
	}

	/* moving up or moving down logic */
	/* rate based on count */
        Ft_App_AudioPlay(pLACtxt,dstaddr,2*1024L);/* load the floor number audio */
        
	pLACtxt->CurrArrowResizeRate--;
	if(pLACtxt->CurrArrowResizeRate <= 0)
	{
		pLACtxt->CurrArrowResizeRate = pLACtxt->SLATransPrms.ResizeRate;
	}


	//changing floor numbers
	pLACtxt->CurrFloorNumChangeRate--;
	if(pLACtxt->CurrFloorNumChangeRate <= 0)
	{
		pLACtxt->CurrFloorNumChangeRate = pLACtxt->SLATransPrms.FloorNumChangeRate;
		//change the floor number wrt direction
		if(LIFTAPP_DIR_DOWN == pLACtxt->ArrowDir)
		{
			pLACtxt->CurrFloorNum -=1;
                        if(0 == pLACtxt->CurrFloorNum)
			{
				pLACtxt->CurrFloorNum -=1;
			}
		}
		else
		{
			pLACtxt->CurrFloorNum +=1;
			if(0 == pLACtxt->CurrFloorNum)
			{
				pLACtxt->CurrFloorNum +=1;
			}
		}
	}
	return 0;
}

//API for single bitmap computation - follow the above api for more details
int32_t FT_LiftAppComputeBitmap_Single(S_LiftAppFont_t *pLAFont, int32_t BitmapIdx,int32_t ResizeVal,int16_t xOffset,int16_t yOffset)
{
	/* compute the total number of digits in the input */
	int32_t TotHzSz,TotVtSz,FlNum,i,xoff,yoff;

	TotHzSz = (pLAFont->MaxWidth * 16 * ResizeVal)/LIFTAPPCHARRESIZEPRECISION;
	TotVtSz = (pLAFont->MaxHeight * 16 * ResizeVal)/LIFTAPPCHARRESIZEPRECISION;
	
	//change the x and y offset to center of the characters
	xoff = xOffset*16 - TotHzSz/2;
	yoff = yOffset*16 - TotVtSz/2;

	FTImpl.Cell(BitmapIdx);
	FTImpl.Vertex2f(xoff,yoff);

	return 0;
}

/* API to demonstrate calibrate widget/functionality */
void SAMAPP_CoPro_Widget_Calibrate()
{
	uint8_t *pbuff;
	uint32_t NumBytesGen = 0,TransMatrix[6];
	uint16_t CurrWriteOffset = 0;

	/*************************************************************************/
	/* Below code demonstrates the usage of calibrate function. Calibrate    */
	/* function will wait untill user presses all the three dots. Only way to*/
	/* come out of this api is to reset the coprocessor bit.                 */
	/*************************************************************************/
	{
	
	FTImpl.DLStart();
	FTImpl.ColorRGB(64,64,64);
	FTImpl.Clear(1,1,1);
	FTImpl.ColorRGB(0xff,0xff,0xff);
	/* Draw number at 0,0 location */
	//FTImpl.ColorA(30);
	FTImpl.Cmd_Text((FT_DISPLAYWIDTH/2), (FT_DISPLAYHEIGHT/2), 27, FT_OPT_CENTER, "Please Tap on the dot");
	FTImpl.Cmd_Calibrate(0);

	FTImpl.Finish();

	}

}


int16_t linear(int16_t p1,int16_t p2,uint16_t t,uint16_t rate)
{
       float st  = (float)t/rate;
       return p1+(st*(p2-p1));
}

/* Startup Screen Information*/ 

PROGMEM char * const info[] = {  "FT801 Lift Application",
                          "API to demonstrate lift application,",
                          "with FTDI logo",
                          "& date and time display"
                       }; 


/********API to return the assigned TAG value when penup,for the primitives/widgets******/

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


/***********************API used to SET the ICON******************************************/
/*Refer the code flow in the flowchart availble in the Application Note */



void Info()
{
  uint16_t dloffset = 0,z;
// Touch Screen Calibration
  //Serial.println(sizeof(S_LiftAppCtxt_t));
  FTImpl.DLStart(); 
  FTImpl.Clear(1,1,1);
  FTImpl.ColorRGB(255,255,255);
  FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,FT_DISPLAYHEIGHT/2,28,FT_OPT_CENTERX|FT_OPT_CENTERY,"Please tap on a dot");
  FTImpl.Cmd_Calibrate(0);
  FTImpl.DLEnd();
  FTImpl.Finish();
// Ftdi Logo animation  
  //Serial.println("Before Logo");
  FTImpl.Cmd_Logo();
	while(FTImpl.CheckLogo() != FT_GE_FINISHED);
//Copy the Displaylist from DL RAM to GRAM
  dloffset = FTImpl.Read16(REG_CMD_DL);
  dloffset -=4; 
  FTImpl.Cmd_Memcpy(STARTUP_ADDRESS,FT_RAM_DL,dloffset);

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
	FTImpl.End();
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
/* wait until Play key is not pressed*/ 
  /* To cover up some distortion while demo is being loaded */
    FTImpl.DLStart();
  FTImpl.ColorRGB(255,255,255);
  FTImpl.Clear(1,1,1);
  FTImpl.DLEnd();
  FTImpl.Finish();
}

int32_t Ft_App_AudioPlay(S_LiftAppCtxt_t *pCtxt,int32_t DstAddr,int32_t BuffChunkSz)
{
  /* awitch according to the current audio state */	        
  switch(pCtxt->AudioState)
  {
    case LIFTAPPAUDIOSTATE_INIT:
    {
      /* Initialize all the parameters and start downloading */
        pCtxt->AudioFileIdx = 0;
        pCtxt->AudioCurrAddr = DstAddr;
        pCtxt->AudioCurrFileSz = 0;
        pCtxt->AudioState = LIFTAPPAUDIOSTATE_DOWNLOAD; 
        FTImpl.Write32( REG_PLAYBACK_LENGTH,0);
	FTImpl.Write( REG_PLAYBACK_LOOP,0);
	FTImpl.Write( REG_PLAYBACK_PLAY,1);		
        
      break;
    }
    case LIFTAPPAUDIOSTATE_DOWNLOAD:
    {
      uint8_t pbuff[512];//temp buffer for sd read data
      uint16_t blocklen;
      int16_t i,NumSectors;
      
      /* Initialize all the parameters and start downloading */
      NumSectors = BuffChunkSz/LIFTAPPCHUNKSIZE*1L;//hardcoding wrt sdcard sector size
      /* After downloading all the files into graphics RAM change the state to playback */  
      
      if((pCtxt->AudioFileIdx >= LIFTAPPMAXFILENAMEARRAY) || (0 == strlen(&pCtxt->AudioFileNames[pCtxt->AudioFileIdx])))
      {
        pCtxt->AudioState = LIFTAPPAUDIOSTATE_PLAYBACK;
      }            
      else if(pCtxt->AudioCurrFileSz <= 0)
      {
        /* open a new file and start downloading data into the current audio buffer */
        if(0 == strlen(&pCtxt->AudioFileNames[pCtxt->AudioFileIdx]))
        {
          pCtxt->AudioState = LIFTAPPAUDIOSTATE_PLAYBACK;
        }
        else
        {
          
          if(FtSd.OpenFile(g_reader,&pCtxt->AudioFileNames[pCtxt->AudioFileIdx]))
          {
            /* file open failure */
            pCtxt->AudioFileIdx += strlen(&pCtxt->AudioFileNames[pCtxt->AudioFileIdx]);
            pCtxt->AudioFileIdx++;
			Serial.println("Error opening file");
            break;
          }          
          /* file open success */
          
          g_reader.ReadSector(pbuff);
          pCtxt->AudioCurrFileSz = (uint32_t)((pbuff[45]<<24) | (pbuff[44]<<16) | (pbuff[43]<<8) | (pbuff[42]));
          //pCtxt->AudioCurrFileSz = g_reader.Size;
		 // Serial.println(g_reader.Size,DEC);
		  blocklen = pCtxt->AudioCurrFileSz>512?512:pCtxt->AudioCurrFileSz;
          FTImpl.Write(pCtxt->AudioCurrAddr,&pbuff[46],blocklen - 46);                
          pCtxt->AudioCurrAddr += (blocklen - 46);
          pCtxt->AudioCurrFileSz -= (blocklen - 46);
        }
      }
      else
      {
        /* Download data into audio buffer */
        for(i=0;i<NumSectors;i++)
        {
          g_reader.ReadSector(pbuff);
          //Serial.println(FileSaveLen,DEC);
          blocklen = pCtxt->AudioCurrFileSz>512?512:pCtxt->AudioCurrFileSz;
          FTImpl.Write(pCtxt->AudioCurrAddr,pbuff,blocklen);
          pCtxt->AudioCurrAddr += (blocklen);
          pCtxt->AudioCurrFileSz -= (blocklen);
         
          if(pCtxt->AudioCurrFileSz <= 0)
          {
            pCtxt->AudioFileIdx += strlen(&pCtxt->AudioFileNames[pCtxt->AudioFileIdx]);
            pCtxt->AudioFileIdx++;//increment the index to open a new file
            break;
          }
        }
      }
      break;
    }
    case LIFTAPPAUDIOSTATE_PLAYBACK:
    {
      /* Downloading is finished and start the playback */
		FTImpl.Write32( REG_PLAYBACK_START,DstAddr);//Audio playback start address 
		//FTImpl.Write32( REG_PLAYBACK_LENGTH,totalbufflen);//Length of raw data buffer in bytes		
		FTImpl.Write32( REG_PLAYBACK_LENGTH,(pCtxt->AudioCurrAddr - DstAddr));
		FTImpl.Write16( REG_PLAYBACK_FREQ,11025);//Frequency
		FTImpl.Write( REG_PLAYBACK_FORMAT,FT_ULAW_SAMPLES);//Current sampling frequency
		FTImpl.Write( REG_PLAYBACK_LOOP,0);
		FTImpl.Write( REG_VOL_PB,255);				
		FTImpl.Write( REG_PLAYBACK_PLAY,1);		

        pCtxt->AudioState = LIFTAPPAUDIOSTATE_STOP;

      break;
    }
    case LIFTAPPAUDIOSTATE_STOP:
    {
      int32_t rdptr;
      /* Stop the audio playback in case of one shot */
      
      /* Check wether the playback is finished - audio engine */
      rdptr = FTImpl.Read32(REG_PLAYBACK_READPTR) - DstAddr;
      if((pCtxt->AudioCurrAddr <= rdptr) || (0 == rdptr))
      {
        
        /* Reset the respective parameters before entering none state */
        pCtxt->AudioFileIdx = 0;
        pCtxt->AudioState = LIFTAPPAUDIOSTATE_NONE;
        pCtxt->AudioFileNames[0] = '\0';
        pCtxt->AudioFileNames[1] = '\0';
      }      
      break;
    }
    case LIFTAPPAUDIOSTATE_NONE:
    default:
    {
      /* Nothing done in this state */
      break;
    }
    
  }      
  return 0;
}



void font_display(int16_t BMoffsetx,int16_t BMoffsety,uint32_t stringlenghth,char *string_display,uint8_t opt_landscape)
{
        uint16_t k;
        
        if(0 == opt_landscape)
        {
	for(k=0;k<stringlenghth;k++)
	  {
	    FTImpl.Vertex2ii(BMoffsetx,BMoffsety,8,string_display[k]);
            BMoffsety -= pgm_read_byte(&g_Gpu_Fonts[0].FontWidth[string_display[k]]);
            
	  }
        }
       	else if(1 == opt_landscape)
	{
	  for(k=0;k<stringlenghth;k++)
        	{
		FTImpl.Vertex2ii(BMoffsetx,BMoffsety,28,string_display[k]);
		BMoffsetx += pgm_read_byte(&g_Gpu_Fonts[0].FontWidth[string_display[k]]);
		}
	
	}
}



/* Lift demo application demonstrates background animation and foreground digits and arrow direction */
void FT_LiftApp_Landscape()
{
	/* Download the raw data of custom fonts into memory */
	int16_t BMoffsetx,BMoffsety,BHt = 156,BWd = 80,BSt = 80,Bfmt = FT_L8,BArrowHt = 85,BArrowWd = 80,BArrowSt = 80,BArrowfmt = FT_L8;
	int16_t NumBallsRange = 6, NumBallsEach = 10,RandomVal = 16;
	int32_t Baddr2,i,SzInc = 16,SzFlag = 0;
	uint8_t fontr = 255,fontg = 255,fontb = 255;
	S_LiftAppRate_t *pLARate;
	S_LiftAppTrasParams_t *pLATParams;
	S_LiftAppBallsLinear_t S_LiftBallsArray[6*10],*pLiftBalls = NULL;//as of now 80 balls are been plotted
	float temptransx,temptransy,running_rate;
	S_LiftAppCtxt_t S_LACtxt;
        


	/* load the bitmap raw data */

	/* Initial setup code to setup all the required bitmap handles globally */
	pLARate = &S_LACtxt.SLARate;
	pLATParams = &S_LACtxt.SLATransPrms;
        uint32_t time =0;
        time = millis();

	//initialize all the rate parameters
	pLARate->CurrTime = 0;
	pLARate->IttrCount = 0;

	/* Initialize all the transition parameters - all the below are in terms of basic units of rate
	either they can be based on itterations or based on time giffies */
	pLATParams->FloorNumChangeRate = 64;
	pLATParams->MaxFloorNum = 7;
	pLATParams->MinFloorNum = -2;
	pLATParams->ResizeRate = 32;
	pLATParams->ResizeDimMax = 2*256;
	pLATParams->ResizeDimMin = 1*256;
	pLATParams->FloorNumStagnantRate = 128;

	/* Initialization of lift context parameters */
	S_LACtxt.ArrowDir = LIFTAPP_DIR_DOWN;//going down direction
	S_LACtxt.CurrFloorNum = 5;//current floor number to start with
	S_LACtxt.NextFloorNum = -2;//destination floor number as of now
	S_LACtxt.CurrArrowResizeRate = pLATParams->ResizeRate;
	S_LACtxt.CurrFloorNumResizeRate = pLATParams->ResizeRate;
	S_LACtxt.CurrFloorNumStagnantRate = 0;
	S_LACtxt.CurrFloorNumChangeRate = S_LACtxt.SLATransPrms.FloorNumChangeRate;
	S_LACtxt.AudioState = 0;
        S_LACtxt.AudioPlayFlag = 0;
        S_LACtxt.opt_orientation = 1;

	
	FTImpl.ClearColorRGB(255, 255, 255);
	FTImpl.Clear(1, 1, 1); // clear screen

	FTImpl.Begin(FT_BITMAPS);
	Baddr2 = FT_RAM_G + (20 *1024);
        if(FtSd.OpenFile(g_reader,"font.raw")==0)
	  Ft_App_LoadRawFromFile(g_reader,Baddr2);
	FTImpl.BitmapHandle(0);//handle 0 is used for all the characters
	FTImpl.BitmapSource(Baddr2);
	FTImpl.BitmapLayout(Bfmt, BSt, BHt);
	FTImpl.BitmapSize(FT_BILINEAR, FT_BORDER, FT_BORDER, BWd*2, BHt*2);

	Baddr2 = ((Baddr2 + (int32_t)BSt*BHt*11 + 15)&~15);
        if(0 == FtSd.OpenFile(g_reader,"arr.raw"))
  	    Ft_App_LoadRawFromFile(g_reader,Baddr2);
	FTImpl.BitmapHandle(1);//bitmap handle 1 is used for arrow
	FTImpl.BitmapSource(Baddr2);
	FTImpl.BitmapLayout(BArrowfmt, BArrowSt, BArrowHt);
	FTImpl.BitmapSize(FT_BILINEAR, FT_BORDER, FT_BORDER, BArrowWd*2, BArrowHt*2);//make sure the bitmap is displayed when rotation happens

	Baddr2 = Baddr2 + 80*85;
        if(0 == FtSd.OpenFile(g_reader,"bs6.raw"))
  	  Ft_App_LoadRawFromFile(g_reader,Baddr2);
	FTImpl.BitmapHandle(2);//bitmap handle 2 is used for background balls
	FTImpl.BitmapSource(Baddr2);
	FTImpl.BitmapLayout(FT_L8, 60, 60);
	FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, 60, 55);

	Baddr2 += 60*55;
        if(0 == FtSd.OpenFile(g_reader,"bs5.raw"))
	  Ft_App_LoadRawFromFile(g_reader,Baddr2);
	FTImpl.BitmapHandle(3);//bitmap handle 2 is used for background balls
	FTImpl.BitmapSource(Baddr2);
	FTImpl.BitmapLayout(FT_L8, 50, 46);
	FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, 50, 46);

	Baddr2 += 50*46;
        if(0 == FtSd.OpenFile(g_reader,"bs4.raw"))
	  Ft_App_LoadRawFromFile(g_reader,Baddr2);
	FTImpl.BitmapHandle(4);//bitmap handle 2 is used for background balls
	FTImpl.BitmapSource(Baddr2);
	FTImpl.BitmapLayout(FT_L8, 40, 37);
	FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, 40, 37);

	Baddr2 += 40*37;
        if(0 == FtSd.OpenFile(g_reader,"bs3.raw"))
	  Ft_App_LoadRawFromFile(g_reader,Baddr2);
	FTImpl.BitmapHandle(5);//bitmap handle 2 is used for background balls
	FTImpl.BitmapSource(Baddr2);
	FTImpl.BitmapLayout(FT_L8, 30, 27);
	FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, 30, 27);

	Baddr2 += 30*27;
        if(0 == FtSd.OpenFile(g_reader,"bs2.raw"))
	  Ft_App_LoadRawFromFile(g_reader,Baddr2);
	FTImpl.BitmapHandle(6);//bitmap handle 2 is used for background balls
	FTImpl.BitmapSource(Baddr2);
	FTImpl.BitmapLayout(FT_L8, 20, 18);
	FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, 20, 18);

	Baddr2 += 20*18;
        if(0 == FtSd.OpenFile(g_reader,"bs1.raw"))
	  Ft_App_LoadRawFromFile(g_reader,Baddr2);
	FTImpl.BitmapHandle(7);//bitmap handle 2 is used for background balls
	FTImpl.BitmapSource(Baddr2);
	FTImpl.BitmapLayout(FT_L8, 10, 10);
	FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, 10, 10);
	FTImpl.Vertex2ii(0,0,7,0);

	Baddr2 += 10*10;
        if(0 == FtSd.OpenFile(g_reader,"logo.raw"))
	  Ft_App_LoadRawFromFile(g_reader,Baddr2);
	FTImpl.BitmapHandle(11);//handle 11 is used for logo
	FTImpl.BitmapSource(Baddr2);
	FTImpl.BitmapLayout(FT_ARGB4, 129*2, 50);
	FTImpl.BitmapSize(FT_BILINEAR, FT_BORDER, FT_BORDER, 129*2, 50);//make sure whole bitmap is displayed after rotation - as height is greater than width
	Baddr2 += 129*2*50;



	
	//FTImpl.Finish();

	Baddr2 += 10*10;
	Baddr2 =  ((Baddr2 + 7)&~7);//audio engine's requirement
	

        
	FTImpl.DLEnd();
	FTImpl.Finish();


	delay(30);

	/* compute the random values at the starting */
	pLiftBalls = S_LiftBallsArray;
	for(i=0;i<(NumBallsRange*NumBallsEach);i++)
	{
		pLiftBalls->xOffset = random(FT_DISPLAYWIDTH*16);
		pLiftBalls->yOffset = random(FT_DISPLAYHEIGHT*16);
		pLiftBalls->dx = random(RandomVal*8) - RandomVal*4;
		pLiftBalls->dy = -1*random(RandomVal*8);
		pLiftBalls++;
	}

	while(1)
	{		
                //loop_start = millis();
  		/* Logic of user touch - change background or skin wrt use touch */
		/* Logic of transition - change of floor numbers and direction */
		FT_LiftApp_FTransition(&S_LACtxt,Baddr2);		

		FTImpl.DLStart();
		FTImpl.ClearColorRGB(255, 255, 255);
		FTImpl.Clear(1, 1, 1); // clear screen
		FTImpl.ColorRGB(255,255,255);
		/* Background gradient */
		FTImpl.Cmd_Gradient( 0,0,0x66B4E8,0,272,0x132B3B);

		/* Draw background bitmaps */
		FTImpl.ColorRGB(0,0,0);
		pLiftBalls = S_LiftBallsArray;
		FTImpl.Begin(FT_BITMAPS);
		
		FTImpl.ColorA(64);
		FTImpl.ColorRGB(255,255,255);
		for(i=0;i<(NumBallsRange*NumBallsEach);i++)
		{
			/* handle inst insertion - check for the index */
			if(0 == (i%NumBallsEach))
			{
				FTImpl.BitmapHandle(2 + (i/NumBallsEach));
			}
			/* recalculate the background balls offset and respective rate when ball moves out of the diaply area */
			if( (pLiftBalls->xOffset > ((FT_DISPLAYWIDTH + 60)*16)) || 
				(pLiftBalls->yOffset > ((FT_DISPLAYHEIGHT + 60) *16)) ||
				(pLiftBalls->xOffset < (-60*16)) || 
				(pLiftBalls->yOffset < (-60*16)) )
			{
				/* always offset starts from below the screen and moves upwards */
				pLiftBalls->xOffset = random(FT_DISPLAYWIDTH*16);
				pLiftBalls->yOffset = FT_DISPLAYHEIGHT*16 + random(60*16);

				pLiftBalls->dx = random(RandomVal*8) - RandomVal*4;
				pLiftBalls->dy = -1*random(RandomVal*8);
			}
			FTImpl.Vertex2f(pLiftBalls->xOffset, pLiftBalls->yOffset);
			pLiftBalls->xOffset += pLiftBalls->dx;
			pLiftBalls->yOffset += pLiftBalls->dy;
			pLiftBalls++;
		}
		FTImpl.End();
		FTImpl.AlphaFunc(FT_ALWAYS,0);
		FTImpl.ColorA(255);

		if(LIFTAPP_DIR_NONE != S_LACtxt.ArrowDir)//do not display the arrow in case of no direction, meaning stagnant
		{

			//calculation of size value based on the rate
			//the bitmaps are scaled from original resolution till 2 times the resolution on both x and y axis
			SzInc = 16 + (S_LACtxt.SLATransPrms.ResizeRate/2 - abs(S_LACtxt.CurrArrowResizeRate%S_LACtxt.SLATransPrms.ResizeRate - S_LACtxt.SLATransPrms.ResizeRate/2));
			FTImpl.ColorRGB(fontr, fontg, fontb);
			/* Draw the arrow first */
			FTImpl.BitmapHandle(1); 
			FTImpl.Begin(FT_BITMAPS); // start drawing bitmaps
			//BMoffsetx = ((FT_DISPLAYWIDTH/4) - (BArrowWd*SzInc/32));
			BMoffsetx = (BArrowWd - (BArrowWd*SzInc/32));
			BMoffsety = ((FT_DISPLAYHEIGHT/2) - (BArrowHt*SzInc/32));

			/* perform inplace flip and scale of bitmap in case of direction is up */
			if(LIFTAPP_DIR_UP == S_LACtxt.ArrowDir)
			{
                                Ft_App_GraphicsMatrix_t S_GPUMatrix;
				Ft_App_TrsMtrxLoadIdentity(&S_GPUMatrix);
				Ft_App_TrsMtrxTranslate(&S_GPUMatrix,80/2.0,85/2.0,&temptransx,&temptransy);
				Ft_App_TrsMtrxScale(&S_GPUMatrix,(SzInc/16.0),(SzInc/16.0));
				Ft_App_TrsMtrxFlip(&S_GPUMatrix,FT_APP_GRAPHICS_FLIP_BOTTOM);		
				Ft_App_TrsMtrxTranslate(&S_GPUMatrix,(-80*SzInc)/32.0,(-85*SzInc)/32.0,&temptransx,&temptransy);
		
				//Ft_App_TrsMtrxTranslate(&S_GPUMatrix,-80/2.0,-85/2.0,&temptransx,&temptransy);
                                {
                                  Ft_App_PostProcess_Transform_t S_GPUTrasMatrix;
				Ft_App_UpdateTrsMtrx(&S_GPUMatrix,&S_GPUTrasMatrix);
				FTImpl.BitmapTransformA(S_GPUTrasMatrix.Transforma);
				FTImpl.BitmapTransformB(S_GPUTrasMatrix.Transformb);
				FTImpl.BitmapTransformC(S_GPUTrasMatrix.Transformc);
				FTImpl.BitmapTransformD(S_GPUTrasMatrix.Transformd);
				FTImpl.BitmapTransformE(S_GPUTrasMatrix.Transforme);
				FTImpl.BitmapTransformF(S_GPUTrasMatrix.Transformf);
                                }
			}
			else if(LIFTAPP_DIR_DOWN == S_LACtxt.ArrowDir)
			{
				//perform only scaling as rotation is not required
				FTImpl.BitmapTransformA(256*16/SzInc);
				FTImpl.BitmapTransformE(256*16/SzInc);			
			}
			FT_LiftAppComputeBitmap(G_LiftAppFontArrayArrow,-1,1,SzInc*16,BArrowWd,(FT_DISPLAYHEIGHT/2));		
			FTImpl.Cmd_LoadIdentity();
			FTImpl.Cmd_SetMatrix();
		}
		/* Draw the font bitmaps */
		/* algorithm for animation - floor numbers will change in dimensions when stagnant at a perticular floor */
		/* arrow will change in case of movement of lift */
		/* display the bitmap with increased size */
		SzInc = 16 + (S_LACtxt.SLATransPrms.ResizeRate/2 - abs(S_LACtxt.CurrFloorNumResizeRate%S_LACtxt.SLATransPrms.ResizeRate - S_LACtxt.SLATransPrms.ResizeRate/2));

		BMoffsetx = FT_DISPLAYWIDTH - BWd*2 - (BWd*SzInc/32);
		BMoffsety = ((FT_DISPLAYHEIGHT/2) - (BHt*SzInc/32));
		/* calculate the resolution change based on the number of characters used as well as */
		FTImpl.ColorRGB(fontr, fontg, fontb);
		FTImpl.BitmapTransformA(256*16/SzInc);
		FTImpl.BitmapTransformE(256*16/SzInc);
		FTImpl.BitmapHandle(0); 
		FTImpl.Begin(FT_BITMAPS); // start drawing bitmaps
		FT_LiftAppComputeBitmap(G_LiftAppFontArrayNumbers,S_LACtxt.CurrFloorNum,0,SzInc*16,FT_DISPLAYWIDTH - BWd*2,(FT_DISPLAYHEIGHT/2));

		FTImpl.BitmapTransformA(256);
		FTImpl.BitmapTransformE(256);


                
		BMoffsety = 240;
                BMoffsetx = 0;
                
                FTImpl.Begin(FT_BITMAPS); 
                int16_t t;
                {
                char text_run_disp[] = "The running text is displayed here.";
                int16_t stringlen_text_run_disp;
		stringlen_text_run_disp = strlen(text_run_disp);
		if(t < 300)
			t++;
		BMoffsetx = linear(480,0,t,300);
		font_display(BMoffsetx,BMoffsety,stringlen_text_run_disp,text_run_disp, 1);
                }
                
                BMoffsety = 220;
                BMoffsetx = 0;	
                
                uint32_t disp =0,hr,minutes,sec ;
                uint32_t temp = millis()-time;
                hr = (temp/3600000L)%12;
                minutes = (temp/60000L)%60;
                sec = (temp/1000L)%60;
                 
                       
                FTImpl.Vertex2ii(BMoffsetx,BMoffsety,28,(hr/10)+'0');
                FTImpl.Vertex2ii(BMoffsetx + 10,BMoffsety,28,(hr%10)+'0');
                FTImpl.Vertex2ii(BMoffsetx + 22,BMoffsety,28,':');
                FTImpl.Vertex2ii(BMoffsetx + 30,BMoffsety,28,(minutes/10)+'0');
                FTImpl.Vertex2ii(BMoffsetx + 40,BMoffsety,28,(minutes%10)+'0');
                FTImpl.Vertex2ii(BMoffsetx + 52,BMoffsety,28,':');
                FTImpl.Vertex2ii(BMoffsetx+ 60,BMoffsety,28,(sec/10)+'0');
                FTImpl.Vertex2ii(BMoffsetx + 70,BMoffsety,28,(sec%10)+'0');

		FTImpl.Begin(FT_BITMAPS);
		FTImpl.BitmapHandle(11);
		FTImpl.Vertex2ii(0,0,11,0);

		t++;
		FTImpl.DLEnd();
		FTImpl.Finish();
		
                
		//rate count increment logic
		pLARate->IttrCount++;
             
	}

}



/*  API to construct the display list of the lift application*/
void FT_LiftApp_Portrait()
{
	/* Download the raw data of custom fonts into memory */
	int16_t BMoffsetx,BMoffsety,BHt = 156,BWd = 80,BSt = 80,Bfmt = FT_L8,BArrowHt = 85,BArrowWd = 80,BArrowSt = 80,BArrowfmt = FT_L8;
	int16_t NumBallsRange = 6, NumBallsEach = 10,RandomVal = 16;
	int32_t Baddr2,i,SzInc = 16,SzFlag = 0;
	uint8_t fontr = 255,fontg = 255,fontb = 255;
	S_LiftAppRate_t *pLARate;
	S_LiftAppTrasParams_t *pLATParams;
	S_LiftAppBallsLinear_t S_LiftBallsArray[6*10],*pLiftBalls = NULL;//as of now 80 stars are been plotted
	Ft_App_GraphicsMatrix_t S_GPUMatrix;
	//Ft_App_PostProcess_Transform_t S_GPUTrasMatrix;
	float temptransx,temptransy,running_rate;
	S_LiftAppCtxt_t S_LACtxt;

 

	/* load the bitmap raw data */

	/* Initial setup code to setup all the required bitmap handles globally */
	pLARate = &S_LACtxt.SLARate;
	pLATParams = &S_LACtxt.SLATransPrms;
        
	//initialize all the rate parameters
	pLARate->CurrTime = 0;
	pLARate->IttrCount = 0;

	/* Initialize all the transition parameters - all the below are in terms of basic units of rate
	either they can be based on itterations or based on time giffies */
	pLATParams->FloorNumChangeRate = 64;
	pLATParams->MaxFloorNum = 90;
	pLATParams->MinFloorNum = 70;
	pLATParams->ResizeRate = 32;
	pLATParams->ResizeDimMax = 2*256;
	pLATParams->ResizeDimMin = 1*256;
	pLATParams->FloorNumStagnantRate = 128;

	/* Initialization of lift context parameters */
	S_LACtxt.ArrowDir = LIFTAPP_DIR_DOWN;//going down direction
        S_LACtxt.CurrFloorNum = 80;//current floor number to start with
        S_LACtxt.NextFloorNum = 77;//destination floor number as of now
	S_LACtxt.CurrArrowResizeRate = pLATParams->ResizeRate;
	S_LACtxt.CurrFloorNumResizeRate = pLATParams->ResizeRate;
	S_LACtxt.CurrFloorNumStagnantRate = 0;
	S_LACtxt.CurrFloorNumChangeRate = S_LACtxt.SLATransPrms.FloorNumChangeRate;
        S_LACtxt.AudioState = LIFTAPPAUDIOSTATE_NONE;
        S_LACtxt.AudioFileIdx = 0;
        S_LACtxt.AudioCurrAddr = 0;
        S_LACtxt.AudioCurrFileSz = 0;
        S_LACtxt.AudioFileNames[0] = '\0';
        S_LACtxt.AudioPlayFlag = 0;
        S_LACtxt.opt_orientation = 0;
        
       	FTImpl.ColorRGB(255, 255, 255);
	FTImpl.Clear(1, 1, 1); // clear screen

	Baddr2 = FT_RAM_G + (20 *1024);
        if(0 == FtSd.OpenFile(g_reader,"font.raw"))
            Ft_App_LoadRawFromFile(g_reader,Baddr2);
        FTImpl.BitmapHandle(0);//handle 0 is used for all the characters
	FTImpl.BitmapSource(Baddr2);
	FTImpl.BitmapLayout(Bfmt, BSt, BHt);
	FTImpl.BitmapSize(FT_BILINEAR, FT_BORDER, FT_BORDER, BHt*2, BHt*2);//make sure whole bitmap is displayed after rotation - as height is greater than width


	Baddr2 = ((Baddr2 + (int32_t)BSt*BHt*11 + 15)&~15);
        if(0 == FtSd.OpenFile(g_reader,"arr.raw"))
  	    Ft_App_LoadRawFromFile(g_reader,Baddr2);
	FTImpl.BitmapHandle(1);//bitmap handle 1 is used for arrow
	FTImpl.BitmapSource(Baddr2);
	FTImpl.BitmapLayout(BArrowfmt, BArrowSt, BArrowHt);
	FTImpl.BitmapSize(FT_BILINEAR, FT_BORDER, FT_BORDER, BArrowHt*2, BArrowHt*2);//make sure whole bitmap is displayed after rotation - as height is greater than width

	Baddr2 = Baddr2 + 80*85;
        if(0 == FtSd.OpenFile(g_reader,"bs6.raw"))
	  Ft_App_LoadRawFromFile(g_reader,Baddr2);
	FTImpl.BitmapHandle(2);//bitmap handle 2 is used for background stars
	FTImpl.BitmapSource(Baddr2);
	FTImpl.BitmapLayout(FT_L8, 60, 55);
	FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, 60, 55);

	Baddr2 += 60*55;
        if(0 == FtSd.OpenFile(g_reader,"bs5.raw"))
	  Ft_App_LoadRawFromFile(g_reader,Baddr2);
	FTImpl.BitmapHandle(3);//bitmap handle 2 is used for background balls
	FTImpl.BitmapSource(Baddr2);
	FTImpl.BitmapLayout(FT_L8, 50, 46);
	FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, 50, 46);

	Baddr2 += 50*46;
        if(0 == FtSd.OpenFile(g_reader,"bs4.raw"))
	Ft_App_LoadRawFromFile(g_reader,Baddr2);
	FTImpl.BitmapHandle(4);//bitmap handle 2 is used for background balls
	FTImpl.BitmapSource(Baddr2);
	FTImpl.BitmapLayout(FT_L8,40,37);
	FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER,40,37);

        Baddr2 += 40*37;
	if(0 == FtSd.OpenFile(g_reader,"bs3.raw"))
	  Ft_App_LoadRawFromFile(g_reader,Baddr2);
        FTImpl.BitmapHandle(5);//bitmap handle 2 is used for background balls
	FTImpl.BitmapSource(Baddr2);
	FTImpl.BitmapLayout(FT_L8, 30, 27);
	FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, 30, 27);

	Baddr2 += 30*27;
	if(0 == FtSd.OpenFile(g_reader,"bs2.raw"))
	  Ft_App_LoadRawFromFile(g_reader,Baddr2);
	FTImpl.BitmapHandle(6);//bitmap handle 2 is used for background balls
	FTImpl.BitmapSource(Baddr2);
	FTImpl.BitmapLayout(FT_L8, 20, 18);
	FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, 20, 18);

	Baddr2 += 20*18;
        if(0 == FtSd.OpenFile(g_reader,"bs1.raw"))
	  Ft_App_LoadRawFromFile(g_reader,Baddr2);
        FTImpl.BitmapHandle(7);//bitmap handle 2 is used for background balls
	FTImpl.BitmapSource(Baddr2);
	FTImpl.BitmapLayout(FT_L8, 10, 10);
	FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, 10, 10);

	Baddr2 += 10*10;
        if(0 == FtSd.OpenFile(g_reader,"logo.raw"))
	  Ft_App_LoadRawFromFile(g_reader,Baddr2);
	FTImpl.BitmapHandle(11);//handle 11 is used for logo
	FTImpl.BitmapSource(Baddr2);
	FTImpl.BitmapLayout(FT_ARGB4, 129*2, 50);
	FTImpl.BitmapSize(FT_BILINEAR, FT_BORDER, FT_BORDER, 512, 512);//make sure whole bitmap is displayed after rotation - as height is greater than width
	Baddr2 += 129*2*50;

	
	FTImpl.Finish();

	FTImpl.BitmapHandle(8);
	FTImpl.BitmapLayout(FT_L4,9,25);
	FTImpl.BitmapSource(950172);
	FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, 25, 25);
	Baddr2 += 10*10;
	Baddr2 =  (int32_t)((Baddr2 + 7)&~7);//audio engine's requirement
	
        uint32_t time =0;
        time = millis();

	FTImpl.DLEnd();
	FTImpl.Finish();
	delay(30);

	/* compute the random values at the starting */
	pLiftBalls = S_LiftBallsArray;
	for(i=0;i<(NumBallsRange*NumBallsEach);i++)
	{
		//always start from the right and move towards left
		pLiftBalls->xOffset = random(FT_DISPLAYWIDTH*16);
		pLiftBalls->yOffset = random(FT_DISPLAYHEIGHT*16);
		pLiftBalls->dx = -1*random(RandomVal*8);//always -ve
		pLiftBalls->dy = random(RandomVal*8) - RandomVal*4;
		pLiftBalls++;
	}

	while(1)
	{		
		/* Logic of user touch - change background or skin wrt use touch */
		/* Logic of transition - change of floor numbers and direction */
		FT_LiftApp_FTransition(&S_LACtxt,Baddr2);		
		FTImpl.DLStart();
		FTImpl.ClearColorRGB(255, 255, 255);
		FTImpl.Clear(1, 1, 1); // clear screen
		FTImpl.ColorRGB(255,255,255);
		FTImpl.SaveContext();
		/* Background gradient */
		FTImpl.Cmd_Gradient( 0,0,0x66B4E8,480,0,0x132B3B);


		/* Draw background bitmaps */
		FTImpl.ColorRGB(0,0,0);
		pLiftBalls = S_LiftBallsArray;
		FTImpl.Begin(FT_BITMAPS);
		
		FTImpl.ColorA(64);
		FTImpl.ColorRGB(255,255,255);
		for(i=0;i<(NumBallsRange*NumBallsEach);i++)
		{			
			if(0 == i%NumBallsEach)
			{				
				FTImpl.BitmapHandle(2 + (i/NumBallsEach));
			}
			if( ( (pLiftBalls->xOffset > ((FT_DISPLAYWIDTH + 60)*16)) || (pLiftBalls->yOffset > ((FT_DISPLAYHEIGHT + 60) *16)) ) ||
				( (pLiftBalls->xOffset < (-60*16)) || (pLiftBalls->yOffset < (-60*16)) ))
			{
				pLiftBalls->xOffset = FT_DISPLAYWIDTH*16 + random(80*16);
				pLiftBalls->yOffset = random(FT_DISPLAYHEIGHT*16);
				pLiftBalls->dx = -1*random(RandomVal*8);
				pLiftBalls->dy = random(RandomVal*8) - RandomVal*4;

			}
			FTImpl.Vertex2f(pLiftBalls->xOffset, pLiftBalls->yOffset);
			pLiftBalls->xOffset += pLiftBalls->dx;
			pLiftBalls->yOffset += pLiftBalls->dy;
			pLiftBalls++;
		}
		FTImpl.End();
		FTImpl.AlphaFunc(FT_ALWAYS,0);
		FTImpl.ColorA(255);

		if(LIFTAPP_DIR_NONE != S_LACtxt.ArrowDir)//do not display the arrow in case of no direction, meaning stagnant
		{
                        Ft_App_GraphicsMatrix_t S_GPUMatrix;
			//calculation of size value based on the rate
			//the bitmaps are scaled from original resolution till 2 times the resolution on both x and y axis
			SzInc = 16 + (S_LACtxt.SLATransPrms.ResizeRate/2 - abs(S_LACtxt.CurrArrowResizeRate%S_LACtxt.SLATransPrms.ResizeRate - S_LACtxt.SLATransPrms.ResizeRate/2));
			FTImpl.ColorRGB(fontr, fontg, fontb);
			/* Draw the arrow first */
			FTImpl.BitmapHandle(1); 
			FTImpl.Begin(FT_BITMAPS); // start drawing bitmaps
			BMoffsetx = (BArrowWd - (BArrowWd*SzInc/32));
			BMoffsety = ((FT_DISPLAYHEIGHT/2) - (BArrowHt*SzInc/32));

			/* perform inplace flip and scale of bitmap in case of direction is up */
			Ft_App_TrsMtrxLoadIdentity(&S_GPUMatrix);
			Ft_App_TrsMtrxTranslate(&S_GPUMatrix,80/2.0,85/2.0,&temptransx,&temptransy);
			Ft_App_TrsMtrxScale(&S_GPUMatrix,(SzInc/16.0),(SzInc/16.0));
			Ft_App_TrsMtrxRotate(&S_GPUMatrix,90);
			if(LIFTAPP_DIR_UP == S_LACtxt.ArrowDir)
			{
				Ft_App_TrsMtrxFlip(&S_GPUMatrix,FT_APP_GRAPHICS_FLIP_RIGHT);		
			}

			Ft_App_TrsMtrxTranslate(&S_GPUMatrix,(-80*SzInc)/32.0,(-85*SzInc)/32.0,&temptransx,&temptransy);
                        {
                        Ft_App_PostProcess_Transform_t S_GPUTrasMatrix;
			Ft_App_UpdateTrsMtrx(&S_GPUMatrix,&S_GPUTrasMatrix);
			FTImpl.BitmapTransformA(S_GPUTrasMatrix.Transforma);
			FTImpl.BitmapTransformB(S_GPUTrasMatrix.Transformb);
			FTImpl.BitmapTransformC(S_GPUTrasMatrix.Transformc);
			FTImpl.BitmapTransformD(S_GPUTrasMatrix.Transformd);
			FTImpl.BitmapTransformE(S_GPUTrasMatrix.Transforme);
			FTImpl.BitmapTransformF(S_GPUTrasMatrix.Transformf);
                        }
			FT_LiftAppComputeBitmap_Single(G_LiftAppFontArrayArrow,0,SzInc*16,BArrowWd,(FT_DISPLAYHEIGHT/2));
			FTImpl.Cmd_LoadIdentity();
			FTImpl.Cmd_SetMatrix();
		}

		
		/* algorithm for animation - floor numbers will change in dimensions when stagnant at a perticular floor */
		/* arrow will change in case of movement of lift */
		/* display the bitmap with increased size */
		SzInc = 16 + (S_LACtxt.SLATransPrms.ResizeRate/2 - abs(S_LACtxt.CurrFloorNumResizeRate%S_LACtxt.SLATransPrms.ResizeRate - S_LACtxt.SLATransPrms.ResizeRate/2));
		BMoffsetx = FT_DISPLAYWIDTH - BWd*2 - (BWd*SzInc/32);
		BMoffsety = ((FT_DISPLAYHEIGHT/2) - (BHt*SzInc/32));

		/* calculate the resolution change based on the number of characters used as well as */
		FTImpl.ColorRGB(fontr, fontg, fontb);
		/* perform inplace flip and scale of bitmap in case of direction is up */
		Ft_App_TrsMtrxLoadIdentity(&S_GPUMatrix);
		Ft_App_TrsMtrxTranslate(&S_GPUMatrix,BWd/2.0,BHt/2.0,&temptransx,&temptransy);
		Ft_App_TrsMtrxScale(&S_GPUMatrix,(SzInc/16.0),(SzInc/16.0));
		Ft_App_TrsMtrxRotate(&S_GPUMatrix,90);
		Ft_App_TrsMtrxTranslate(&S_GPUMatrix,(-BHt*SzInc)/32.0,(-BWd*SzInc)/32.0,&temptransx,&temptransy);
                {
                  Ft_App_PostProcess_Transform_t S_GPUTrasMatrix;
		Ft_App_UpdateTrsMtrx(&S_GPUMatrix,&S_GPUTrasMatrix);
		FTImpl.BitmapTransformA(S_GPUTrasMatrix.Transforma);
		FTImpl.BitmapTransformB(S_GPUTrasMatrix.Transformb);
		FTImpl.BitmapTransformC(S_GPUTrasMatrix.Transformc);
		FTImpl.BitmapTransformD(S_GPUTrasMatrix.Transformd);
		FTImpl.BitmapTransformE(S_GPUTrasMatrix.Transforme);
		FTImpl.BitmapTransformF(S_GPUTrasMatrix.Transformf);
                }
		FTImpl.BitmapHandle(0); 
		FTImpl.Begin(FT_BITMAPS); // start drawing bitmaps
		FT_LiftAppComputeBitmapRowRotate(G_LiftAppFontArrayNumbers,S_LACtxt.CurrFloorNum,0,SzInc*16,FT_DISPLAYWIDTH - BHt,(FT_DISPLAYHEIGHT/2));
		
                

        	BMoffsetx = ((FT_DISPLAYWIDTH /2) + 210);
		BMoffsety = ((FT_DISPLAYHEIGHT /2) + 130);
                FTImpl.Begin(FT_BITMAPS);
                
        	FTImpl.BitmapHandle(28);
		FTImpl.BitmapSize(FT_NEAREST,FT_BORDER,FT_BORDER,25,25);

		FTImpl.Cmd_LoadIdentity();
		FTImpl.Cmd_Translate(18*65536/2,25*65536/2);
		FTImpl.Cmd_Rotate(-90*65536/360);
		FTImpl.Cmd_Translate(-25*65536/2,-18*65536/2);
		FTImpl.Cmd_SetMatrix();

                int16_t t;
                {
                char text_run_disp[] = "The running text is displayed here.";
                int16_t stringlen_text_run_disp;
		stringlen_text_run_disp = strlen(text_run_disp);
		if(t < 200)
			t++;
		BMoffsety = linear(0,272,t,200);
                
		font_display(BMoffsetx,BMoffsety,stringlen_text_run_disp,text_run_disp, 0);
                }
                BMoffsetx = ((FT_DISPLAYWIDTH /2) + 190);
		BMoffsety = ((FT_DISPLAYHEIGHT /2) + 105);	
                uint32_t disp =0,hr,minutes,sec ;
                uint32_t temp = millis()-time;
                hr = (temp/3600000L)%12;
                minutes = (temp/60000L)%60;
                sec = (temp/1000L)%60;
                                
                FTImpl.Vertex2ii(BMoffsetx,BMoffsety,8,(hr/10)+'0');
                FTImpl.Vertex2ii(BMoffsetx,BMoffsety-10,8,(hr%10)+'0');
                FTImpl.Vertex2ii(BMoffsetx,BMoffsety-23,8,':');
                FTImpl.Vertex2ii(BMoffsetx,BMoffsety-30,8,(minutes/10)+'0');
                FTImpl.Vertex2ii(BMoffsetx,BMoffsety-40,8,(minutes%10)+'0');
                FTImpl.Vertex2ii(BMoffsetx,BMoffsety-53,8,':');
                FTImpl.Vertex2ii(BMoffsetx,BMoffsety-60,8,(sec/10)+'0');
                FTImpl.Vertex2ii(BMoffsetx,BMoffsety-70,8,(sec%10)+'0');
		FTImpl.RestoreContext();
		FTImpl.Cmd_LoadIdentity();
		FTImpl.Cmd_Translate(129*65536,50*65536);
		FTImpl.Cmd_Rotate(270*65536/360);
		FTImpl.Cmd_Translate(-80*65536,-50*65536);
		FTImpl.Cmd_SetMatrix();

		FTImpl.Begin(FT_BITMAPS);
		FTImpl.BitmapHandle(11);
		FTImpl.Vertex2f(-80*16,150*16);
		
		

                
		t++;
		FTImpl.DLEnd();
		FTImpl.Finish();
		//rate count increment - 
		pLARate->IttrCount++;
	}

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
	/*Initialize the SD object. Screen error message can only be displayed when the FT801 is ready*/
	sd_present = FtSd.Init();
	
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
		CheckStorageDevicePrecence(); 
  Info();
  
  /* APP to demonstrate lift functionality */
#ifdef ORIENTATION_PORTRAIT

	FT_LiftApp_Portrait();

#endif

#ifdef ORIENTATION_LANDSCAPE

	FT_LiftApp_Landscape();

#endif
	FTImpl.Exit();  /*  Safely close all opened handles */
}
Serial.println("--End Application--");
}

void loop()
{
}



/* Nothing beyond this */





