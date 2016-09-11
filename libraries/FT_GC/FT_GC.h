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
* @file                           FT_GC.h
* @brief                          Contains FT graphics controller interface for arduino platform.
								  Tested platform version: Arduino 1.0.4 and later
* @version                        1.1.0
* @date                           2014/02/05
*
*/

#ifndef _FT_GC_H_
#define _FT_GC_H_

/* Definitions used for debug. Uncomment the below to enable debug from graphics controller library */
#define FT_GCDEBUG0 255 //switchoff debug
#define FT_GCDEBUG1 1 //most critical debug information
#define FT_GCDEBUG2 2 //mid critical debug information
#define FT_GCDEBUG3 3 //least critical debug information

/* Change the below statement wrt debug criticality */
#define FT_GCDEBUG FT_GCDEBUG0

/* Version number of FT_GC */
#define FT_GC_MAJOR				1
#define FT_GC_MINOR				2
#define FT_GC_BUILD				0

/* FT_GC status enum - used for api return type, error type etc */
typedef enum FT_Status
{	
	/* Common enums */
	FT_OK = 0,
	FT_ERROR = 1,
	FT_WARNING = 2,
	FT_ERROR_INIT = 3,
	FT_ERROR_CHIPID = 4,
	
	/* Library related enums */
	FT_ERROR_NOPINASSIGNED = 50,
}FT_Status;

/* Audio coprocessor related enums */
typedef enum FT_AEStatus
{		
	FT_AE_OK = 0,	
	FT_AE_ERROR_FORMAT = 1,
	FT_AE_ERROR_SAMPLINGFREQ_OUTOFRANGE = 2,	//assert for boundary
	FT_AE_PLAYBACK_STOPPED = 3,
	FT_AE_PLAYBACK_CONTINUE = 4,
}FT_AEStatus;

/* Status enums for graphics engine */
typedef enum FT_GEStatus
{	
	FT_GE_OK = 0,	
	FT_GE_BUSY = 1,
	FT_GE_FINISHED = 2,
	
	/* Graphics related error enums */
	FT_GE_ERROR_INVALID_PRIMITIVE = 20,
	FT_GE_ERROR_INVALID_BITMAP_FORMAT = 21,
	FT_GE_ERROR_INVALID_BITMAP_HANDLENUM = 22,
	FT_GE_ERROR_VERTEX_OUTOFRANGE = 23,

	/* Coprocessor related enums */
	FT_GE_ERROR = 50,						//undefined error
	FT_GE_ERROR_JPEG = 51,					//erranious jpeg data
	FT_GE_ERROR_DEFLATE = 52,				//erranious deflated data
	FT_GE_ERROR_DISPLAYLIST_OVERFLOW = 53,	//DL buffer overflow
	FT_GE_ERROR_INVALID_WIDGET_PARAMS = 54,	//invalid input parameters - out of bound
	
	/* Display parameters error */
	FT_GE_ERROR_DISPLAYPARAMS = 100,//error in the display parameters
}FT_GEStatus;

/* Touch coprocessor related enums */
typedef enum FT_TEStatus
{	
	FT_TE_OK = 0,	
	FT_TE_ERROR_RZTHRESHOLD = 1,		//threshold out of bound
	FT_TE_ERROR_FILTERPARAM = 2,		//filter out of bound
	FT_TE_ERROR_MODE = 3,				//mode out of range
	FT_TE_ERROR_INVALIDPARAM = 4,		//generic invalid param
}FT_TEStatus;

/* FT_GC options */
typedef enum FT_GCOptions{
	FT_INTERNAL_CLOCK_SOURCE = 1,
}FT_GCOptions;

 
/************************************************************************************************************************************************************
Display parameters used for various options are

FT_GC_DisplayResolution			Width 	Height	Swizzle	Polarity	PClk	HCycle	Hoffset		Hsync0		Hsync1		VCycle	Voffset		Vsync0	Vsync1	
FT_DISPLAY_QVGA_320x240   		320		240		3		0			8		408		70			0			10			263			13		0		2
FT_DISPLAY_WQVGA_480x272		480		272		0		1			5		548		43			0			41			292			12		0		10
*************************************************************************************************************************************************************/
/*
typedef enum FT_GC_DispRes
{
	FT_DISPLAY_QVGA_320x240 = 0,
	FT_DISPLAY_WQVGA_480x272 = 1,
}FT_GC_DispRes;
*/
#define		FT_DISPLAY_QVGA_320x240		0UL
#define		FT_DISPLAY_WQVGA_480x272 	1UL
/* Structure definitions */

typedef struct sTagXY
{
	int16_t y;		//y coordinate of touch object
	int16_t x;		//x coordinate of touch object
	uint16_t tag;	//TAG value of the object
}sTagXY;

typedef struct sTrackTag
{
	uint16_t tag;	//TAG value of the object
	uint16_t track;	//track value of the object	
}sTrackTag;

/* FT80x font table structure */
/* Font table address in ROM can be found by reading 32bit value from FT_FONT_TABLE_POINTER location. */
/* 16 font tables are present at the address read from location FT_FONT_TABLE_POINTER */
typedef struct FT_Fonts
{
	/* All the values are in bytes */
	/* Width of each character font from 0 to 127 */
	uint8_t	FontWidth[FT_NUMCHAR_PERFONT];
	/* Bitmap format of font wrt bitmap formats supported by FT800 - L1, L4, L8 */
	uint32_t	FontBitmapFormat;
	/* Font line stride in FT800 ROM */
	uint32_t	FontLineStride;
	/* Font width in pixels */
	uint32_t	FontWidthInPixels;
	/* Font height in pixels */
	uint32_t	FontHeightInPixels;
	/* Pointer to font graphics raw data */
	uint32_t	PointerToFontGraphicsData;
}FT_Fonts_t;


 /*
 Following are the assumptions made for FT_GC library
 1. SPI library from arduino is included and working fine on this platform
 2. Output parameters are wrt LCD display specification
 3. 
 */
template<class FT_Trans>
class FT_GC : public FT_Trans
{
	public:	
	/* Api to set cs, pdn, int pin. CS pin is mandatory. If PDN and INT pins are set then gpios are initialized */	
	FT_GC();/* base class constructor */	
	~FT_GC();/* base class destructor */
	FT_Status Init(uint8_t ResType,uint16_t options1);//API to set the resolution of output display	
	FT_Status Init(uint16_t hperiod,uint16_t vperiod,uint16_t hfrontporch,uint16_t hbackporch,uint16_t hpulsewidth,
				   uint16_t vfrontporch,uint16_t vbackporch,uint16_t vpulsewidth,
				   uint8_t polarity,uint8_t swizzle,uint8_t fps, uint16_t options1);/* Api to program the output display wrt display parameters */
	FT_Status Exit();//exit state of graphics processor
	void GetVersion(uint8_t &Major, uint8_t &Minor, uint8_t &Build);/* Api to get version of the library */
	void SetDisplayEnablePin(uint8_t GpioBit);//FT_GC gpio bit for display enable/disable
	void SetAudioEnablePin(uint8_t GpioBit);//FT_GC gpio bit for audio enable/disable
	void DisplayOn(void);/* Apis to enable/disable backlight */
	void DisplayOff(void);
	void AudioOn(void);
	void AudioOff(void);
	void SetInterruptPin(uint16_t Intpin);//apis to set interrupt pin
	void ResetCopro(void);//api to reset only coprocessor	
	void Reset(void);//api to reset whole FT_GC via pdn - if pdn is not assigned then reset is not successful
	/* Apis related to power up/power down funcationality */
	void DisplayConfigExternalClock(uint8_t ResType);
	void ActiveInternalClock(void);
	void PDN_Cycle(void);
	
	/* Apis related to graphics processor */	
	//enable or disable interrupts
	void EnableInterrupts(uint8_t GEnable,uint8_t Mask);
	uint8_t ReadIntReg(void);//read the interrupt flag register - note that on FT_GC the interrupts are clear by read
	//void AssignCBFunPtr(void *CBFunPtr);//api to register callback function from application using interrupt mechanism	
	
	/* APIs related to graphics engine */
	FT_GEStatus AlphaFunc(uint8_t Func, uint8_t Ref);
	FT_GEStatus Begin(uint8_t Prim);
	FT_GEStatus BitmapHandle(uint8_t Handle);
	FT_GEStatus BitmapLayout(uint8_t Format, uint16_t Linestride, uint16_t Height);
	FT_GEStatus BitmapSize(uint8_t Filter, uint8_t wrapx, uint8_t wrapy, uint16_t width, uint16_t height);
	FT_GEStatus BitmapSource(uint32_t Addr);
	FT_GEStatus BitmapTransformA(int32_t A);
	FT_GEStatus BitmapTransformB(int32_t B);
	FT_GEStatus BitmapTransformC(int32_t C);
	FT_GEStatus BitmapTransformD(int32_t D);
	FT_GEStatus BitmapTransformE(int32_t E);
	FT_GEStatus BitmapTransformF(int32_t F);
	FT_GEStatus BlendFunc(uint8_t Src, uint8_t Dst);
	FT_GEStatus Call(uint16_t Dest);
	FT_GEStatus Cell(uint8_t Cell);
	FT_GEStatus ClearColorA(uint8_t Alpha);
	FT_GEStatus ClearColorRGB(uint8_t red, uint8_t green, uint8_t blue);
	FT_GEStatus ClearColorRGB(uint32_t rgb);
	FT_GEStatus Clear(uint8_t c, uint8_t s, uint8_t t);
	FT_GEStatus Clear(void);
	FT_GEStatus ClearStencil(uint8_t s);
	FT_GEStatus ClearTag(uint8_t s);
	FT_GEStatus ColorA(uint8_t Alpha);
	FT_GEStatus ColorMask(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
	FT_GEStatus ColorRGB(uint8_t red, uint8_t green, uint8_t blue);
	FT_GEStatus Display(void);
	FT_GEStatus End(void);
	FT_GEStatus Jump(uint16_t Dest);
	FT_GEStatus LineWidth(uint16_t Width);
	FT_GEStatus Macro(uint8_t m);
	FT_GEStatus PointSize(uint16_t Size);
	FT_GEStatus RestoreContext(void);
	FT_GEStatus Return(void);
	FT_GEStatus SaveContext(void);
	FT_GEStatus ScissorSize(uint16_t width, uint16_t height);
	FT_GEStatus ScissorXY(uint16_t x, uint16_t y);
	FT_GEStatus StencilFunc(uint8_t Func, uint8_t Ref, uint8_t Mask);
	FT_GEStatus StencilMask(uint8_t Mask);
	FT_GEStatus StencilOp(uint8_t Sfail, uint8_t Spass);
	FT_GEStatus TagMask(uint8_t Mask);
	FT_GEStatus Tag(uint8_t s);
	FT_GEStatus Vertex2f(int16_t x, int16_t y);
	FT_GEStatus Vertex2ii(uint16_t x, uint16_t y, uint8_t Handle = 0, uint8_t Cell = 0);

	/* graphics helper apis */
	FT_GEStatus ColorRGB(uint32_t rgb);
	FT_GEStatus ColorARGB(uint32_t argb);

	/* APIs related to coprocessor commands, widgets etc */
	FT_GEStatus Cmd_Logo(void);
	FT_GEStatus Cmd_Append(uint32_t Ptr, uint32_t Num);
	FT_GEStatus Cmd_BGColor(uint32_t c);
	FT_GEStatus Cmd_Button(int16_t x, int16_t y, uint16_t w, uint16_t h, uint8_t Font, uint16_t Options, const char *s);
	FT_GEStatus Cmd_Calibrate(uint32_t Result);
	FT_GEStatus Cmd_Clock(int16_t x, int16_t y, int16_t r, uint16_t Options, uint16_t h, uint16_t m, uint16_t s, uint16_t ms);
	FT_GEStatus Cmd_ColdStart(void);
	FT_GEStatus Cmd_Dial(int16_t x, int16_t y, int16_t r, uint16_t Options, uint16_t Val);
	FT_GEStatus Cmd_DLStart(void);
	FT_GEStatus Cmd_FGColor(uint32_t c);
	FT_GEStatus Cmd_Gauge(int16_t x, int16_t y, int16_t r, uint16_t Options, uint16_t Major, uint16_t Minor, uint16_t Val, uint16_t Range);
	FT_GEStatus Cmd_GetMatrix(void);
	FT_GEStatus Cmd_GetProps(uint32_t &Ptr, uint32_t &w, uint32_t &h);
	FT_GEStatus Cmd_GetPtr(uint32_t Result);
	FT_GEStatus Cmd_GradColor(uint32_t c);
	FT_GEStatus Cmd_Gradient(int16_t x0, int16_t y0, uint32_t rgb0, int16_t x1, int16_t y1, uint32_t rgb1);
	FT_GEStatus Cmd_Inflate(uint32_t Ptr);
	FT_GEStatus Cmd_Interrupt(uint32_t ms);
	FT_GEStatus Cmd_Keys(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t Font, uint16_t Options, const char *s);
	FT_GEStatus Cmd_LoadIdentity(void);
	FT_GEStatus Cmd_LoadImage(uint32_t Ptr, int32_t Options);
	FT_GEStatus Cmd_Memcpy(uint32_t Dest, uint32_t Src, uint32_t Num);
	FT_GEStatus Cmd_Memset(uint32_t Ptr, uint8_t Value, uint32_t Num);
	FT_GEStatus Cmd_Memcrc(uint32_t Ptr, uint32_t Num,uint32_t &Result);
	FT_GEStatus Cmd_Memwrite(uint32_t Ptr, uint32_t Num);
	FT_GEStatus Cmd_Memzero(uint32_t Ptr, uint32_t Num);
	FT_GEStatus Cmd_Number(int16_t x, int16_t y, uint8_t Font, uint16_t Options, uint32_t n);
	FT_GEStatus Cmd_Progress(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t Options, uint16_t Val, uint16_t Range);
	FT_GEStatus Cmd_RegRead(uint32_t Ptr,uint32_t Result);
	FT_GEStatus Cmd_Rotate(int32_t a);
	FT_GEStatus Cmd_Scale(int32_t sx, int32_t sy);
	FT_GEStatus Cmd_ScreenSaver(void);
	FT_GEStatus Cmd_Scrollbar(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t Options, uint16_t Val, uint16_t Size, uint16_t Range);
	FT_GEStatus Cmd_SetFont(uint8_t Font, uint32_t Ptr);
	FT_GEStatus Cmd_SetMatrix(void);
	FT_GEStatus Cmd_Sketch(int16_t x, int16_t y, uint16_t w, uint16_t h, uint32_t Ptr, uint16_t Format);
	FT_GEStatus Cmd_Slider(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t Options, uint16_t val, uint16_t Range);
	FT_GEStatus Cmd_Snapshot(uint32_t OutputAddr);
	FT_GEStatus Cmd_Spinner(int16_t x, int16_t y, uint8_t Style, uint8_t Scale);
	FT_GEStatus Cmd_Stop(void);
	FT_GEStatus Cmd_Swap(void);
	FT_GEStatus Cmd_Text(int16_t x, int16_t y, uint8_t font, uint16_t Options, const char *s);
	FT_GEStatus Cmd_Toggle(int16_t x, int16_t y, int16_t w, uint8_t font, uint16_t Options, uint16_t State, const char *s);
	FT_GEStatus Cmd_Track(int16_t x, int16_t y, uint16_t w, uint16_t h, uint8_t Tag);
	FT_GEStatus Cmd_Translate(int32_t tx, int32_t ty);  

	/* Apis related to audio engine */
	FT_AEStatus PlaySound(uint8_t Volume,uint16_t SoundNote);
	FT_AEStatus PlaySound(uint16_t SoundNote);//higher byte is the note and lower byte is the sound
	void StopSound(void);//volume will not be modified
	void SetSoundVolume(uint8_t Volume);
	uint8_t GetSoundVolume(void);

	FT_AEStatus PlayAudio(uint8_t Volume,uint8_t Format,uint16_t SamplingFreq,uint32_t BufferAddr,uint32_t BufferSize,uint8_t Loop);//one shot or continuous, sampling frequency is from 8k to 48k
	void SetAudioVolume(uint8_t Volume);
	FT_AEStatus GetAudioStats(uint32_t &CurrPlayAddr);//returns playback stopped or continue
	uint8_t GetAudioVolume(void);
	void StopAudio(void);//volume will not be modified

	/* Apis related to touch engine */
	void SetTouchMode(uint8_t TMode);//one of 0ff/oneshot/frame/continuous. default being continuous
	void SetHostTagXY(uint16_t xoffset,uint16_t yoffset);//api to set coordinates for host specific tag query
	uint8_t GetHostTagXY(void);//api to get TAG from FT_GC for coordinates set by  SetHostTagXY() api - host needs to wait for at least 1 frame to get these query values
	void GetTagXY(sTagXY &sTagxy);//get the touched object tag and repective xy coordinates
	void GetTrackTag(sTrackTag &sTracktag);//get the track value and the tag value
	
	/* APIS related to power modes */
	void HostCommand(uint32_t HostCommand);	
	//void CheckIntFlag(uint8_t IntFlag);	

	/* Special apis */
	//apis for ease of usage in FT_GC
	void DLStart(void);//inserts cmd_dlstart() followed by clear(1,1,1) graphics command
	void DLEnd(void);//inserts display() gpu instruction at the end and inserts cmd_swap() command
	FT_GEStatus CheckLogo(void);//special api to check logo completion

	//apis to render all the commands to hardware
	FT_GEStatus Flush(void);//api to flush out all the commands to FT_GC, does not wait for the completion of the rendering
	FT_GEStatus Finish(void);//flushes out all the commands to FT_GC and waits for the completion of execution
	FT_GEStatus CheckFinish(void);//checks fifo and returns the status	
	uint32_t GetError(void);//error from graphics controller library

	/* API related to coprocessor fifo buffer management */
	//please note that all the below apis are transfer commands
	FT_GEStatus WriteCmd(uint32_t Cmd);
	FT_GEStatus WriteCmd(uint8_t *Src,uint32_t NBytes);//api to send N bytes to command 	
	FT_GEStatus WriteCmdfromflash(prog_uchar *Src,uint32_t NBytes);	
	//void StartTransferRCmd(uint32_t Addr);//assert CSpin and send read command
	FT_GEStatus TransferCmd(uint32_t Cmd);
	FT_GEStatus TransferCmd(uint8_t *Src,uint32_t NBytes);	
	FT_GEStatus TransferCmdfromflash(prog_uchar *Src,uint32_t NBytes);	
	void EndTransferCmd();//de assert CSpin
	FT_GEStatus Cmd_GetResult(uint32_t &Result);//reads the result of the previous commands such as cmd_memcrc,cmd_calibration, cmd_regread which has return values. if busy returns busy status
	FT_GEStatus Cmd_GetResults(int8_t *pA,uint16_t NBytes);//reads N bytes of result bytes from current write pointer
	FT_GEStatus UpdateFreeSpace();
	FT_GEStatus ChkGetFreeSpace(uint16_t NBytes);
	FT_GEStatus StartTransferCmd();
	
	
	/* Global variables */	
	uint8_t  GInit,PDNPin,TrnsFlag;//Global flag to indicate that initialization is done
	int32_t  GError;//Global error flag
	uint16_t IntPin;//interrupt pin	
	uint8_t  DispGpioPin,AudioGpioPin;//default FT_GC pin assignments for diaplay and audio control	
	
	uint16_t CmdFifoWp,FreeSpace;//command fifo write pointer
};
/* Api to set cs, pdn, int pin. CS pin is mandatory. If PDN and INT pins are set then gpios are initialized */		
template<class FT_Trans>
FT_GC<FT_Trans>::FT_GC()
{
	/* initialize context parameters - update with the default values */
	GInit = 0;/*CSPin = FT_CS_PIN;*/PDNPin = FT_PDN_PIN;GError = FT_OK;
	IntPin = -1;CmdFifoWp = 0;FreeSpace = FT_CMDFIFO_SIZE - 4;
	DispGpioPin = FT_GPIO7;AudioGpioPin = FT_GPIO1;	
	TrnsFlag = 0;
	
	//redundant call
	FT_GC<FT_Trans>::SetCSpin(FT_CS_PIN);
}

template<class FT_Trans>
FT_GC<FT_Trans>::~FT_GC()
{
}//do nothing

/* API to initialize the display wrt input configuration */
template<class FT_Trans>
FT_Status FT_GC<FT_Trans>::Init(uint8_t ResType, uint16_t options1 = 0)
{
	/* assign the pdn */
	pinMode(PDNPin, OUTPUT);
	digitalWrite(PDNPin, HIGH);    	
	
	/*attempt to initialize interrupt pin*/
	if(IntPin != -1){
		pinMode(IntPin, INPUT);
	}
	
	/* Initialize SPI channel */
	FT_Trans::Init();
	
	/* Bootup of graphics controller */
	Reset();
	/* Set the display configurations followed by external clock set, spi clock change wrt FT80x */
	DisplayConfigExternalClock(ResType);
	
	

	if((options1&FT_INTERNAL_CLOCK_SOURCE) == FT_INTERNAL_CLOCK_SOURCE){
	/* Set to use internal clock source */
		HostCommand(FT_CLKINT);
	}
	else{
	/* Set to use external clock */
		HostCommand(FT_CLKEXT);	
	}
	
	
	/* change the clock to maximum SPI operating frequency */
	FT_Trans::ChangeClock(FT_SPI_CLK_FREQ_MAX);//change the clock to normal operating frequency - harcoded wrt due	
	
	
	return FT_OK;
}
template<class FT_Trans>
FT_Status FT_GC<FT_Trans>::Init(uint16_t hperiod,uint16_t vperiod,uint16_t hfrontporch,uint16_t hbackporch,uint16_t hpulsewidth,uint16_t vfrontporch,uint16_t vbackporch,uint16_t vpulsewidth,uint8_t polarity,uint8_t swizzle,uint8_t fps, uint16_t options1 = 0)
{
	/* based on the input arguments compute the scan out register values */
	return FT_OK;
}
template<class FT_Trans>
/* Exit SPI channel */			   
FT_Status FT_GC<FT_Trans>::Exit(void)
{
	/* Cross check whether SPI channel is used by others modules */
	
	SPI.end();
	return FT_OK;
}
/* Api to get version of the library */
/* Good to have compile date as well */
template<class FT_Trans>
void FT_GC<FT_Trans>::GetVersion(uint8_t &Major, uint8_t &Minor, uint8_t &Build)
{
	/* Update the version numbers */
	Major = FT_GC_MAJOR;
	Minor = FT_GC_MINOR;
	Build = FT_GC_BUILD;
}
//FT_GC gpio bit for display enable/disable
template<class FT_Trans>
void FT_GC<FT_Trans>::SetDisplayEnablePin(uint8_t GpioBit)
{
	/* update the display enable pin gpio bit number */
	DispGpioPin = GpioBit;
	
	/* set the direction of this bit to output. 1 is output and 0 is input */
	FT_Trans::Write(REG_GPIO_DIR,(1 << DispGpioPin) | FT_Trans::Read(REG_GPIO_DIR));
}
template<class FT_Trans>
void FT_GC<FT_Trans>::SetAudioEnablePin(uint8_t GpioBit)
{
	//FT_GC gpio bit for audio enable/disable
	/* update the audio enable pin gpio bit number */
	AudioGpioPin = GpioBit;
	/* set the direction of this bit to output. 1 is output and 0 is input */
	FT_Trans::Write(REG_GPIO_DIR,(1 << AudioGpioPin) | FT_Trans::Read(REG_GPIO_DIR));
}

template<class FT_Trans>
/* Apis to enable/disable backlight */
void FT_GC<FT_Trans>::DisplayOn(void)
{
	/* switch on the display, 1 means enable and 0 means disable */
	FT_Trans::Write(REG_GPIO,(1 << DispGpioPin) | FT_Trans::Read(REG_GPIO));
	
}
template<class FT_Trans>
void FT_GC<FT_Trans>::DisplayOff(void)
{
	/* switch off the display */
	FT_Trans::Write(REG_GPIO,(~(1 << DispGpioPin)) & FT_Trans::Read(REG_GPIO));
}
template<class FT_Trans>
//apis to set interrupt pin
void FT_GC<FT_Trans>::SetInterruptPin(uint16_t Intpin)
{
	/* update the interrupt pin */
	IntPin = Intpin;
}
template<class FT_Trans>
void FT_GC<FT_Trans>::AudioOn(void)
{
	/* switch on the audio , 1 means enable and 0 means disable*/
	FT_Trans::Write(REG_GPIO,(1 << AudioGpioPin) | FT_Trans::Read(REG_GPIO));
}
template<class FT_Trans>
void FT_GC<FT_Trans>::AudioOff(void)
{
	/* switch off the audio , 1 means enable and 0 means disable*/
	FT_Trans::Write(REG_GPIO,(~(1 << AudioGpioPin)) & FT_Trans::Read(REG_GPIO));
}

template<class FT_Trans>
//api to reset coprocessor only - do this only when coprocessor returns error. for graphic processor error, utilize reset() api
void FT_GC<FT_Trans>::ResetCopro(void)
{
	/* first set the reset bit high */
	FT_Trans::Write(REG_CPURESET,FT_RESET_HOLD_COPROCESSOR);//first hold the coprocessor in reset
	//make the cmd read write pointers to 0
	CmdFifoWp = 0;
	FreeSpace = FT_CMDFIFO_SIZE - 4;
	FT_Trans::Write16(REG_CMD_READ,0);
	FT_Trans::Write16(REG_CMD_WRITE,0);
	delay(10);//just to make sure reset is fine
	/* release the coprocessors from reset */
	FT_Trans::Write(REG_CPURESET,FT_RESET_RELEASE_COPROCESSOR);
	/* ideally delay of 25ms is required for audio engine to playback mute sound to avoid pop sound */
}
template<class FT_Trans>
//api to reset whole FT_GC via pdn - if pdn is not assigned then reset is not successful
void FT_GC<FT_Trans>::Reset(void)
{

	/* Reset of whole graphics controller */
	PDN_Cycle();
	/* send active command to enable SPI, followed by download of default DL into graphics engine, followed by SPI settings wrt internal clock requirements during bootup stage */
	ActiveInternalClock();
}
template<class FT_Trans>
/* API to do power down cycle based on PDN gpio pin */
void FT_GC<FT_Trans>::PDN_Cycle(void)
{
	/* do a power cycle by toggling power down pin followed by active command followed by display list initialization sequence */
	digitalWrite(PDNPin, HIGH);
	delay(20);
	digitalWrite(PDNPin, LOW);
	delay(20);
	digitalWrite(PDNPin, HIGH);
	delay(20);
	
}
template<class FT_Trans>
/* API to set active command, set internal clock and download first DL */
void FT_GC<FT_Trans>::ActiveInternalClock(void)
{
	uint8_t FT_DLCODE_BOOTUP[12] =
	{
	  0,0,0,2,//GPU instruction CLEAR_COLOR_RGB - black color
	  7,0,0,38, //GPU instruction CLEAR
	  0,0,0,0,  //GPU instruction DISPLAY
	};
	/* change the SPI clock to <11MHz */
	FT_Trans::ChangeClock(FT_SPI_CLK_FREQ_MIN);
	HostCommand(FT_ACTIVE);//wake up the processor from sleep state
	delay(20);
	/* download the first display list */
	FT_Trans::Write(FT_RAM_DL,FT_DLCODE_BOOTUP,12);
	/* perform first swap command */
	FT_Trans::Write(REG_DLSWAP,FT_DLSWAP_FRAME);
	//make the cmd read write pointers to 0
	CmdFifoWp = 0;
	FreeSpace = FT_CMDFIFO_SIZE - 4;	
}
template<class FT_Trans>
/* API to configure the display and set to external clock */
void FT_GC<FT_Trans>::DisplayConfigExternalClock(uint8_t ResType)
{
	/* Presently supporting two types of resolutions, qvga and wqvga */
	if(ResType == FT_DISPLAY_QVGA_320x240)
	{
		FT_Trans::Write16(REG_VSYNC0, 		FT_DISPLAY_VSYNC0_QVGA );
		FT_Trans::Write16(REG_VSYNC1, 		FT_DISPLAY_VSYNC1_QVGA );
		FT_Trans::Write16(REG_VOFFSET, 		FT_DISPLAY_VOFFSET_QVGA);
		FT_Trans::Write16(REG_VCYCLE, 		FT_DISPLAY_VCYCLE_QVGA );
		FT_Trans::Write16(REG_HSYNC0, 		FT_DISPLAY_HSYNC0_QVGA );
		FT_Trans::Write16(REG_HSYNC1, 		FT_DISPLAY_HSYNC1_QVGA );
		FT_Trans::Write16(REG_HOFFSET, 		FT_DISPLAY_HOFFSET_QVGA);
		FT_Trans::Write16(REG_HCYCLE, 		FT_DISPLAY_HCYCLE_QVGA );
		FT_Trans::Write16(REG_HSIZE,			FT_DISPLAY_HSIZE_QVGA  );
		FT_Trans::Write16(REG_VSIZE, 			FT_DISPLAY_VSIZE_QVGA  );		 		
		FT_Trans::Write16(REG_PCLK_POL, 		FT_DISPLAY_PCLKPOL_QVGA);
        FT_Trans::Write16(REG_SWIZZLE, 		FT_DISPLAY_SWIZZLE_QVGA);
		/* after configuring display parameters, configure pclk */
		FT_Trans::Write16(REG_PCLK,			FT_DISPLAY_PCLK_QVGA   );
	}
	else if(ResType == FT_DISPLAY_WQVGA_480x272)
	{
		FT_Trans::Write16(REG_VSYNC0, 		FT_DISPLAY_VSYNC0_WQVGA );
		FT_Trans::Write16(REG_VSYNC1, 		FT_DISPLAY_VSYNC1_WQVGA );
		FT_Trans::Write16(REG_VOFFSET, 		FT_DISPLAY_VOFFSET_WQVGA);
		FT_Trans::Write16(REG_VCYCLE, 		FT_DISPLAY_VCYCLE_WQVGA );
		FT_Trans::Write16(REG_HSYNC0, 		FT_DISPLAY_HSYNC0_WQVGA );
		FT_Trans::Write16(REG_HSYNC1, 		FT_DISPLAY_HSYNC1_WQVGA );
		FT_Trans::Write16(REG_HOFFSET, 		FT_DISPLAY_HOFFSET_WQVGA);
		FT_Trans::Write16(REG_HCYCLE, 		FT_DISPLAY_HCYCLE_WQVGA );
		FT_Trans::Write16(REG_HSIZE,			FT_DISPLAY_HSIZE_WQVGA 	);
		FT_Trans::Write16(REG_VSIZE, 			FT_DISPLAY_VSIZE_WQVGA 	);		 		
		FT_Trans::Write16(REG_PCLK_POL, 		FT_DISPLAY_PCLKPOL_WQVGA);
        FT_Trans::Write16(REG_SWIZZLE, 		FT_DISPLAY_SWIZZLE_WQVGA);
		/* after configuring display parameters, configure pclk */
		FT_Trans::Write16(REG_PCLK,			FT_DISPLAY_PCLK_WQVGA	);	
	}
	else
	{
		FT_Trans::Write16(REG_VSYNC0, 		FT_DISPLAY_VSYNC0 );
		FT_Trans::Write16(REG_VSYNC1, 		FT_DISPLAY_VSYNC1 );
		FT_Trans::Write16(REG_VOFFSET, 		FT_DISPLAY_VOFFSET);
		FT_Trans::Write16(REG_VCYCLE, 		FT_DISPLAY_VCYCLE );
		FT_Trans::Write16(REG_HSYNC0, 		FT_DISPLAY_HSYNC0 );
		FT_Trans::Write16(REG_HSYNC1, 		FT_DISPLAY_HSYNC1 );
		FT_Trans::Write16(REG_HOFFSET, 		FT_DISPLAY_HOFFSET);
		FT_Trans::Write16(REG_HCYCLE, 		FT_DISPLAY_HCYCLE );
		FT_Trans::Write16(REG_HSIZE,			FT_DISPLAY_HSIZE  );
		FT_Trans::Write16(REG_VSIZE, 			FT_DISPLAY_VSIZE  );		 		
		FT_Trans::Write16(REG_PCLK_POL, 		FT_DISPLAY_PCLKPOL);
        FT_Trans::Write16(REG_SWIZZLE, 		FT_DISPLAY_SWIZZLE);
		/* after configuring display parameters, configure pclk */
		FT_Trans::Write16(REG_PCLK,			FT_DISPLAY_PCLK   );
	}
	
}
template<class FT_Trans>
/* Apis related to graphics processor */	
//api to enable or disable interrupts
void FT_GC<FT_Trans>::EnableInterrupts(uint8_t GEnable,uint8_t Mask)
{
	FT_Trans::Write(REG_INT_EN,GEnable);//1 means enable global interrupts, 0 means disable global interrupts
	FT_Trans::Write(REG_INT_MASK,Mask);//0 means interrupts are masked, 1 means interrupts are not masked
}
template<class FT_Trans>
//read the interrupt flag register - note that on ft800/ft801 the interrupts are clear by read
uint8_t FT_GC<FT_Trans>::ReadIntReg(void)
{
	return (FT_Trans::Read(REG_INT_FLAGS));
}
template<class FT_Trans>
//void AssignCBFunptr(void *CBFunPtr){}//api to register callback function from application using interrupt mechanism	

/* APIs related to graphics engine */
FT_GEStatus FT_GC<FT_Trans>::AlphaFunc(uint8_t Func, uint8_t Ref) 
{
  return ( WriteCmd((9UL << 24) | ((Func & 7L) << 8) | ((Ref & 0xFFL) << 0)) );
}
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::Begin(uint8_t Prim) 
{
  return ( WriteCmd((31UL << 24) | Prim) );
}
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::BitmapHandle(uint8_t Handle)
{
  return ( WriteCmd((5UL << 24) | Handle) );
}
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::BitmapLayout(uint8_t Format, uint16_t Linestride, uint16_t Height) 
{
  // WriteCmd((7UL << 24) | ((format & 0x1FL) << 19) | ((linestride & 0x3FFL) << 9) | ((height & 0x1FFL) << 0));
  union {
    uint32_t UUInt32;
    uint8_t A[4];
  };
  A[0] = Height;
  A[1] = (1 & (Height >> 8)) | (Linestride << 1);
  A[2] = (7 & (Linestride >> 7)) | (Format << 3);
  A[3] = 7;
  return ( WriteCmd(UUInt32) );
}
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::BitmapSize(uint8_t filter, uint8_t wrapx, uint8_t wrapy, uint16_t width, uint16_t height) 
{
  uint8_t fxy = (filter << 2) | (wrapx << 1) | (wrapy);
  // WriteCmd((8UL << 24) | ((uint32_t)fxy << 18) | ((width & 0x1FFL) << 9) | ((height & 0x1FFL) << 0));
  union {
    uint32_t UUInt32;
    uint8_t A[4];
  };
  A[0] = height;
  A[1] = (1 & (height >> 8)) | (width << 1);
  A[2] = (3 & (width >> 7)) | (fxy << 2);
  A[3] = 8;
  return ( WriteCmd(UUInt32) );
}
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::BitmapSource(uint32_t Addr) 
{
  return ( WriteCmd((1UL << 24) | ((Addr & 0xFFFFFL) << 0)) );
}
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::BitmapTransformA(int32_t A) 
{
  return ( WriteCmd((21UL << 24) | ((A & 0x1FFFFL) << 0)) );
}
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::BitmapTransformB(int32_t B) 
{
  return ( WriteCmd((22UL << 24) | ((B & 0x1FFFFL) << 0)) );
}
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::BitmapTransformC(int32_t C) 
{
  return ( WriteCmd((23UL << 24) | ((C & 0xFFFFFFL) << 0)) );
}
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::BitmapTransformD(int32_t D) 
{
  return ( WriteCmd((24UL << 24) | ((D & 0x1FFFFL) << 0)) );
}
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::BitmapTransformE(int32_t E) 
{
  return ( WriteCmd((25UL << 24) | ((E & 0x1FFFFL) << 0)) );
}
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::BitmapTransformF(int32_t F) 
{
  return ( WriteCmd((26UL << 24) | ((F & 0xFFFFFFL) << 0)) );
}
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::BlendFunc(uint8_t Src, uint8_t Dst) 
{
  return ( WriteCmd((11UL << 24) | ((Src & 7L) << 3) | ((Dst & 7L) << 0)) );
}
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::Call(uint16_t Dest) 
{
  return ( WriteCmd((29UL << 24) | ((Dest & 0xFFFFL) << 0)) );
}
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::Cell(uint8_t Cell) 
{
  return ( WriteCmd((6UL << 24) | ((Cell & 0x7FL) << 0)) );
}
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::ClearColorA(uint8_t Alpha) 
{
  return ( WriteCmd((15UL << 24) | ((Alpha & 0xFFL) << 0)) );
}
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::ClearColorRGB(uint8_t red, uint8_t green, uint8_t blue) 
{
  return ( WriteCmd((2UL << 24) | ((red & 0xFFL) << 16) | ((green & 0xFFL) << 8) | ((blue & 0xFFL) << 0)) );
}
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::ClearColorRGB(uint32_t rgb) 
{
  return ( WriteCmd((2UL << 24) | (rgb & 0xFFFFFFL)) );
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::Clear(uint8_t c, uint8_t s, uint8_t t) 
{
  uint8_t m = (c << 2) | (s << 1) | t;
  return ( WriteCmd((38UL << 24) | m) );
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::Clear(void) 
{
  return ( WriteCmd((38UL << 24) | 7) );
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::ClearStencil(uint8_t s) 
{
  return ( WriteCmd((17UL << 24) | ((s & 0xFFL) << 0)) );
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::ClearTag(uint8_t s) 
{
  return ( WriteCmd((18UL << 24) | ((s & 0xFFL) << 0)) );
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::ColorA(uint8_t Alpha) 
{
  return ( WriteCmd((16UL << 24) | ((Alpha & 0xFFL) << 0)) );
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::ColorMask(uint8_t r, uint8_t g, uint8_t b, uint8_t a) 
{
  return ( WriteCmd((32UL << 24) | ((r & 1L) << 3) | ((g & 1L) << 2) | ((b & 1L) << 1) | ((a & 1L) << 0)) );
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::ColorRGB(uint8_t red, uint8_t green, uint8_t blue) {
  // WriteCmd((4UL << 24) | ((red & 0xFFL) << 16) | ((green & 0xFFL) << 8) | ((blue & 0xFFL) << 0)) );
  union {
    uint32_t UUInt32;
    uint8_t A[4];
  };
  A[0] = blue;
  A[1] = green;
  A[2] = red;
  A[3] = 4;
  return ( WriteCmd(UUInt32) );
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::Display(void) 
{
  return ( WriteCmd((0UL << 24)) );
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::End(void) {
  return ( WriteCmd((33UL << 24)) );
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::Jump(uint16_t Dest) 
{
  return ( WriteCmd((30UL << 24) | ((Dest & 0x7FFL) << 0)) );
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::LineWidth(uint16_t Width) 
{
  return ( WriteCmd((14UL << 24) | ((Width & 0xFFFL) << 0)) );
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::Macro(uint8_t m) 
{
  return ( WriteCmd((37UL << 24) | ((m & 1L) << 0)) );
}   
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::PointSize(uint16_t Size) 
{
  return ( WriteCmd((13UL << 24) | ((Size & 0x1FFFL) << 0)) );
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::RestoreContext(void) 
{
  return ( WriteCmd((35UL << 24)) );
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::Return(void) 
{
  return ( WriteCmd((36UL << 24)) );
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::SaveContext(void) 
{
  return ( WriteCmd((34UL << 24)) );
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::ScissorSize(uint16_t Width, uint16_t Height) 
{
  return ( WriteCmd((28UL << 24) | ((Width & 0x3FFL) << 10) | ((Height & 0x3FFL) << 0)) );
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::ScissorXY(uint16_t x, uint16_t y)
{
  return ( WriteCmd((27UL << 24) | ((x & 0x1FFL) << 9) | ((y & 0x1FFL) << 0)) );
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::StencilFunc(uint8_t Func, uint8_t Ref, uint8_t Mask) 
{
  return ( WriteCmd((10UL << 24) | ((Func & 7L) << 16) | ((Ref & 0xFFL) << 8) | ((Mask & 0xFFL) << 0)) );
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::StencilMask(uint8_t Mask) 
{
  return ( WriteCmd((19UL << 24) | ((Mask & 0xFFL) << 0)) );
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::StencilOp(uint8_t Sfail, uint8_t Spass) 
{
  return ( WriteCmd((12UL << 24) | ((Sfail & 7L) << 3) | ((Spass & 7L) << 0)) );
}   
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::TagMask(uint8_t Mask) 
{
  return ( WriteCmd((20UL << 24) | ((Mask & 1L) << 0)) );
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::Tag(uint8_t s) 
{
  return ( WriteCmd((3UL << 24) | ((s & 0xFFL) << 0)) );
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::Vertex2f(int16_t x, int16_t y) 
{
  // x = int(16 * x);
  // y = int(16 * y);
  return ( WriteCmd((1UL << 30) | ((x & 0x7FFFL) << 15) | ((y & 0x7FFFL) << 0)) );
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::Vertex2ii(uint16_t x, uint16_t y, uint8_t Handle, uint8_t Cell) 
{
  //return (WriteCmd((2UL << 30) | ((x & 0x1FFL) << 21) | ((y & 0x1FFL) << 12) | ((handle & 0x1FL) << 7) | ((cell & 0x7FL) << 0)) ));
   /* Generates invalid graphics instruction due to shift operation of -ve coefficient */
  x &= 0x1FFL;//error handling of -ve coefficients
  y &= 0x1FFL;
  union {
    uint32_t UUInt32;
    uint8_t A[4];
  };
  A[0] = Cell | ((Handle & 1) << 7);
  A[1] = (Handle >> 1) | (y << 4);
  A[2] = (y >> 4) | (x << 5);
  A[3] = (2 << 6) | (x >> 3);
  return ( WriteCmd(UUInt32) );
}    
template<class FT_Trans>

/* graphics helper apis */
FT_GEStatus FT_GC<FT_Trans>::ColorRGB(uint32_t rgb) 
{
  return ( WriteCmd((4UL << 24) | (rgb & 0xFFFFFFL)) );
}    
template<class FT_Trans>
/* Form two commands, one for rgb and the other for a */
FT_GEStatus FT_GC<FT_Trans>::ColorARGB(uint32_t argb) 
{
	FT_GEStatus Status;
	WriteCmd((4UL << 24) | (argb & 0xFFFFFFL));
	Status = WriteCmd((16UL << 24) | ((argb>>24) & 0xFFL) );
  
  return Status;
}   
template<class FT_Trans>


FT_GEStatus FT_GC<FT_Trans>::Cmd_Logo(void)
{
	FT_GEStatus Status;	
	Status = WriteCmd(CMD_LOGO);		
	return Status;
}    
template<class FT_Trans>

FT_GEStatus FT_GC<FT_Trans>::Cmd_Append(uint32_t Ptr, uint32_t Num)
{
	FT_GEStatus Status;
	WriteCmd(CMD_APPEND);
	WriteCmd(Ptr);
	Status = WriteCmd(Num);//checking only for the last command
	
	return Status;
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::Cmd_BGColor(uint32_t c)
{
	FT_GEStatus Status;
	WriteCmd(CMD_BGCOLOR);
	Status = WriteCmd(c);
	
	return Status;
}    
template<class FT_Trans>

/* API for touch transform calculation and set the transform registers */
/*FT_GEStatus FT_GC<FT_Trans>::Cmd_Touch_Transform(int32_t x0,int32_t y0,int32_t x1,int32_t y1,int32_t x2,int32_t y2,int32_t tx0,int32_t ty0,int32_t tx1,int32_t ty1,int32_t tx2,int32_t ty2,uint32_t Result)
{
	FT_GEStatus Status;
	
	WriteCmd(CMD_BITMAP_TRANSFORM);
	WriteCmd(x0);
	WriteCmd(y0);
	WriteCmd(x1);
	WriteCmd(y1);
	WriteCmd(x2);
	WriteCmd(y2);
	WriteCmd(tx0);
	WriteCmd(ty0);
	WriteCmd(tx1);
	WriteCmd(ty1);
	WriteCmd(tx2);
	WriteCmd(ty2);
	Status = WriteCmd(Result);

	return Status;
}
*/
/* make sure the string is in ram instead of program memory. it is assumed that *s is valid string and if null then it should contain \0 */
FT_GEStatus FT_GC<FT_Trans>::Cmd_Button(int16_t x, int16_t y, uint16_t w, uint16_t h, uint8_t Font, uint16_t Options, const char *s)
{
	FT_GEStatus Status;
	
	WriteCmd(CMD_BUTTON);
	WriteCmd(((y & 0xFFFFL) <<16) | (x & 0xFFFFL));
	WriteCmd(((h & 0xFFFFL)<<16) | (w & 0xFFFFL));
	WriteCmd(((Options & 0xFFFFL)<<16) | (Font & 0xFFFFL));
	Status = WriteCmd((uint8_t *)s,strlen((const char *)s) + 1);//make sure last byte is added into the
	
	return Status;
}    
template<class FT_Trans>

/* Check the result of command calibrate by cmd_GetResult */
FT_GEStatus FT_GC<FT_Trans>::Cmd_Calibrate(uint32_t Result)
{
	FT_GEStatus Status;
	
	WriteCmd(CMD_CALIBRATE);
	Status = WriteCmd(Result);//write extra word for result
	
	return Status;
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::Cmd_Clock(int16_t x, int16_t y, int16_t r, uint16_t Options, uint16_t h, uint16_t m, uint16_t s, uint16_t ms)
{
	FT_GEStatus Status;
	
	WriteCmd(CMD_CLOCK);
	WriteCmd(((y & 0xFFFFL)<<16) | (x & 0xFFFFL));
	WriteCmd(((Options & 0xFFFFL)<<16) | (r & 0xFFFFL));
	WriteCmd(((m & 0xFFFFL)<<16) | (h & 0xFFFFL));
	Status = WriteCmd(((ms & 0xFFFFL)<<16) | (s & 0xFFFFL));
	
	return Status;
}    
template<class FT_Trans>

FT_GEStatus FT_GC<FT_Trans>::Cmd_ColdStart(void)
{
	//should we even change command read and write pointers
	return ( WriteCmd(CMD_COLDSTART) );
}   
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::Cmd_Dial(int16_t x, int16_t y, int16_t r, uint16_t Options, uint16_t Val)
{
	FT_GEStatus Status;
	
	WriteCmd(CMD_DIAL);
	WriteCmd(((y & 0xFFFFL)<<16) | (x & 0xFFFFL));
	WriteCmd(((Options & 0xFFFFL)<<16) | (r & 0xFFFFL));
	Status = WriteCmd(Val);
	
	return Status;
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::Cmd_DLStart(void)
{
	return ( WriteCmd(CMD_DLSTART) );
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::Cmd_FGColor(uint32_t c)
{
	FT_GEStatus Status;
	
	WriteCmd(CMD_FGCOLOR);
	Status = WriteCmd(c);
	
	return Status;	
}    
template<class FT_Trans>

FT_GEStatus FT_GC<FT_Trans>::Cmd_Gauge(int16_t x, int16_t y, int16_t r, uint16_t Options, uint16_t Major, uint16_t Minor, uint16_t Val, uint16_t Range)
{
	FT_GEStatus Status;
	
	WriteCmd(CMD_GAUGE);
	WriteCmd(((y & 0xFFFFL)<<16) | (x & 0xFFFFL));
	WriteCmd(((Options & 0xFFFFL)<<16) | (r & 0xFFFFL));
	WriteCmd(((Minor & 0xFFFFL)<<16) | (Major & 0xFFFFL));
	Status = WriteCmd(((Range & 0xFFFFL)<<16) | (Val & 0xFFFFL));
	
	return Status;	
}    
template<class FT_Trans>
/* Results are available from getresults api */
FT_GEStatus FT_GC<FT_Trans>::Cmd_GetMatrix(void)
{
	FT_GEStatus Status;
	
	WriteCmd(CMD_GETMATRIX);
	WriteCmd(0);
	WriteCmd(0);
	WriteCmd(0);
	WriteCmd(0);
	WriteCmd(0);
	Status = WriteCmd(0);
	
	return Status;	
}    
template<class FT_Trans>
/* perform this api and wait for the completion by finish and use getresults api  for the result*/
FT_GEStatus FT_GC<FT_Trans>::Cmd_GetProps(uint32_t &Ptr, uint32_t &w, uint32_t &h)
{
	FT_GEStatus Status;
	
	WriteCmd(CMD_GETPROPS);
	WriteCmd(0);
	WriteCmd(0);
	Status = WriteCmd(0);
	
	return Status;	
}    
template<class FT_Trans>
//perform this api and wait for the completion by finish and use cmd_getresult for the result
FT_GEStatus FT_GC<FT_Trans>::Cmd_GetPtr(uint32_t Result)
{
	FT_GEStatus Status;
	
	WriteCmd(CMD_GETPTR);
	Status = WriteCmd(Result);
	
	return Status;	
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::Cmd_GradColor(uint32_t c)
{
	FT_GEStatus Status;
	
	WriteCmd(CMD_GRADCOLOR);
	Status = WriteCmd(c);
	
	return Status;	
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::Cmd_Gradient(int16_t x0, int16_t y0, uint32_t rgb0, int16_t x1, int16_t y1, uint32_t rgb1)
{
	FT_GEStatus Status;
	
	WriteCmd(CMD_GRADIENT);
	WriteCmd(((y0 & 0xFFFFL)<<16)|(x0 & 0xFFFFL));
	WriteCmd(rgb0);
	WriteCmd(((y1 & 0xFFFFL)<<16)|(x1 & 0xFFFFL));
	Status = WriteCmd(rgb1);
	
	return Status;	
}    
template<class FT_Trans>
//after calling this api copy the raw content which is output from deflate
FT_GEStatus FT_GC<FT_Trans>::Cmd_Inflate(uint32_t Ptr)
{
	FT_GEStatus Status;
	
	WriteCmd(CMD_INFLATE);
	Status = WriteCmd(Ptr);
	
	return Status;	
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::Cmd_Interrupt(uint32_t ms)
{
	FT_GEStatus Status;
	
	WriteCmd(CMD_INTERRUPT);
	Status = WriteCmd(ms);
	
	return Status;	
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::Cmd_Keys(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t Font, uint16_t Options, const char *s)
{
	FT_GEStatus Status;
	
	WriteCmd(CMD_KEYS);
	WriteCmd(((y & 0xFFFFL)<<16) | (x & 0xFFFFL));
	WriteCmd(((h & 0xFFFFL)<<16) | (w & 0xFFFFL));
	WriteCmd(((Options & 0xFFFFL)<<16) | (Font & 0xFFFFL));
	Status = WriteCmd((uint8_t *)s,strlen((const char *)s) + 1);	
	
	return Status;	
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::Cmd_LoadIdentity(void)
{
	return ( WriteCmd(CMD_LOADIDENTITY) );
}    
template<class FT_Trans>
//after this api, copy the jpeg data into fifo
FT_GEStatus FT_GC<FT_Trans>::Cmd_LoadImage(uint32_t Ptr, int32_t Options)
{
	FT_GEStatus Status;
	
	WriteCmd(CMD_LOADIMAGE);
	WriteCmd(Ptr);
	Status = WriteCmd(Options);
	
	return Status;	
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::Cmd_Memcpy(uint32_t Dest, uint32_t Src, uint32_t Num)
{
	FT_GEStatus Status;
	
	WriteCmd(CMD_MEMCPY);
	WriteCmd(Dest);
	WriteCmd(Src);
	Status = WriteCmd(Num);
	
	return Status;	
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::Cmd_Memset(uint32_t Ptr, uint8_t Value, uint32_t Num)
{
	FT_GEStatus Status;
	
	WriteCmd(CMD_MEMSET);
	WriteCmd(Ptr);
	WriteCmd(Value);	
	Status = WriteCmd(Num);
	
	return Status;	
}    
template<class FT_Trans>
//perform this api, wait for the completion and use cmd_getresult for the result
FT_GEStatus FT_GC<FT_Trans>::Cmd_Memcrc(uint32_t Ptr, uint32_t Num,uint32_t &Result)
{
	FT_GEStatus Status;
	
	WriteCmd(CMD_MEMCRC);
	WriteCmd(Ptr);
	WriteCmd(Num);
	Status = WriteCmd(Result);
	
	return Status;	
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::Cmd_Memwrite(uint32_t Ptr, uint32_t Num)
{
	FT_GEStatus Status;
	
	WriteCmd(CMD_MEMWRITE);
	WriteCmd(Ptr);
	Status = WriteCmd(Num);
	
	return Status;	
}    
template<class FT_Trans>

FT_GEStatus FT_GC<FT_Trans>::Cmd_Memzero(uint32_t Ptr, uint32_t Num)
{
	FT_GEStatus Status;
	
	WriteCmd(CMD_MEMZERO);
	WriteCmd(Ptr);
	Status = WriteCmd(Num);
	return Status;	
}    
template<class FT_Trans>

FT_GEStatus FT_GC<FT_Trans>::Cmd_Number(int16_t x, int16_t y, uint8_t Font, uint16_t Options, uint32_t n)
{
	FT_GEStatus Status;
	
	WriteCmd(CMD_NUMBER);
	WriteCmd(((y & 0xFFFFL)<<16) | (x & 0xFFFFL));
	WriteCmd(((Options & 0xFFFFL)<<16) | (Font & 0xFFFFL));
	Status = WriteCmd(n);
	
	return Status;	
}   
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::Cmd_Progress(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t Options, uint16_t Val, uint16_t Range)
{
	FT_GEStatus Status;
	
	WriteCmd(CMD_PROGRESS);
	WriteCmd(((y & 0xFFFFL)<<16) | (x & 0xFFFFL));
	WriteCmd(((h & 0xFFFFL)<<16) | (w & 0xFFFFL));
	WriteCmd(((Val & 0xFFFFL)<<16) | (Options & 0xFFFFL));
	Status = WriteCmd(Range);
	
	return Status;	
}    
template<class FT_Trans>
//perform this api, wait for the completion and use cmd_getresult for the result
FT_GEStatus FT_GC<FT_Trans>::Cmd_RegRead(uint32_t Ptr,uint32_t Result)
{
	FT_GEStatus Status;
	
	WriteCmd(CMD_REGREAD);
	WriteCmd(Ptr);
	Status = WriteCmd(Result);
	
	return Status;	
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::Cmd_Rotate(int32_t a)
{
	FT_GEStatus Status;
	
	WriteCmd(CMD_ROTATE);
	Status = WriteCmd(a);
	
	return Status;	
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::Cmd_Scale(int32_t sx, int32_t sy)
{
	FT_GEStatus Status;
	
	WriteCmd(CMD_SCALE);
	WriteCmd(sx);
	Status = WriteCmd(sy);
	
	return Status;	
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::Cmd_ScreenSaver(void)
{
	return ( WriteCmd(CMD_SCREENSAVER) );
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::Cmd_Scrollbar(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t Options, uint16_t Val, uint16_t Size, uint16_t Range)
{
	FT_GEStatus Status;
	
	WriteCmd(CMD_SCROLLBAR);
	WriteCmd(((y & 0xFFFFL)<<16) | (x & 0xFFFFL));
	WriteCmd(((h & 0xFFFFL)<<16) | (w & 0xFFFFL));
	WriteCmd(((Val & 0xFFFFL)<<16) | (Options & 0xFFFFL));
	Status = WriteCmd(((Range & 0xFFFFL)<<16) | (Size & 0xFFFFL));
	
	return Status;	
}    
template<class FT_Trans>
//make sure ptr is pointing to table and in turn table has pointer to the actual bitmap data
FT_GEStatus FT_GC<FT_Trans>::Cmd_SetFont(uint8_t Font, uint32_t Ptr)
{
	FT_GEStatus Status;
	
	WriteCmd(CMD_SETFONT);
	WriteCmd(Font);
	Status = WriteCmd(Ptr);
	
	return Status;	
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::Cmd_SetMatrix(void)
{
	return ( WriteCmd(CMD_SETMATRIX) );	
}    
template<class FT_Trans>

FT_GEStatus FT_GC<FT_Trans>::Cmd_Sketch(int16_t x, int16_t y, uint16_t w, uint16_t h, uint32_t Ptr, uint16_t Format)
{
	FT_GEStatus Status;
	
	WriteCmd(CMD_SKETCH);
	WriteCmd(((y & 0xFFFFL)<<16) | (x & 0xFFFFL));
	WriteCmd(((h & 0xFFFFL)<<16) | (w & 0xFFFFL));
	WriteCmd(Ptr);
	Status = WriteCmd(Format);
	
	return Status;	
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::Cmd_Slider(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t Options, uint16_t Val, uint16_t Range)
{
	FT_GEStatus Status;
	
	WriteCmd(CMD_SLIDER);
	WriteCmd(((y & 0xFFFFL)<<16) | (x & 0xFFFFL));
	WriteCmd(((h & 0xFFFFL)<<16) | (w & 0xFFFFL));
	WriteCmd(((Val & 0xFFFFL)<<16) | (Options & 0xFFFFL));
	Status = WriteCmd(Range);
	
	return Status;	
}    
template<class FT_Trans>
//perform this api and wait for the completion
FT_GEStatus FT_GC<FT_Trans>::Cmd_Snapshot(uint32_t OutputAddr)
{
	FT_GEStatus Status;
	
	WriteCmd(CMD_SNAPSHOT);
	Status = WriteCmd(OutputAddr);
	
	return Status;	
}    
template<class FT_Trans>
//note that macro 0 and macro 1 are modified by the coprocessor when performing this function
FT_GEStatus FT_GC<FT_Trans>::Cmd_Spinner(int16_t x, int16_t y, uint8_t Style, uint8_t Scale)
{
	FT_GEStatus Status;
	
	WriteCmd(CMD_SPINNER);
	WriteCmd(((y & 0xFFFFL)<<16) | (x & 0xFFFFL));
	Status = WriteCmd(((Scale & 0xFFFFL)<<16) | (Style & 0xFFFFL));
	
	return Status;	
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::Cmd_Stop(void)
{
	return ( WriteCmd(CMD_STOP) );
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::Cmd_Swap(void)
{
	return ( WriteCmd(CMD_SWAP) );
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::Cmd_Text(int16_t x, int16_t y, uint8_t Font, uint16_t Options, const char *s)
{
	FT_GEStatus Status;
	WriteCmd(CMD_TEXT);
	WriteCmd(((y & 0xFFFFL)<<16) | (x & 0xFFFFL));
	WriteCmd(((Options & 0xFFFFL)<<16) | (Font & 0xFFFFL));
	Status = WriteCmd((uint8_t *)s,strlen((const char *)s) + 1);
	
	return Status;	
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::Cmd_Toggle(int16_t x, int16_t y, int16_t w, uint8_t Font, uint16_t Options, uint16_t State, const char *s)
{
	FT_GEStatus Status;
	WriteCmd(CMD_TOGGLE);
	WriteCmd(((y & 0xFFFFL)<<16) | (x & 0xFFFFL));
	WriteCmd(((Font & 0xFFFFL)<<16) | (w & 0xFFFFL));
	WriteCmd(((State & 0xFFFFL)<<16) | (Options & 0xFFFFL));
	Status = WriteCmd((uint8_t *)s,strlen((const char *)s) + 1);
	
	return Status;	
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::Cmd_Track(int16_t x, int16_t y, uint16_t w, uint16_t h, uint8_t Tag)
{
	FT_GEStatus Status;
	WriteCmd(CMD_TRACK);
	WriteCmd(((y & 0xFFFFL)<<16) | (x & 0xFFFFL));
	WriteCmd(((h & 0xFFFFL)<<16) | (w & 0xFFFFL));
	Status = WriteCmd(Tag);
	
	return Status;	
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::Cmd_Translate(int32_t tx, int32_t ty)
{
	FT_GEStatus Status;
	
	WriteCmd(CMD_TRANSLATE);
	WriteCmd(tx);
	Status = WriteCmd(ty);
	
	return Status;	
}    

template<class FT_Trans>
/* Apis related to audio engine */
FT_AEStatus FT_GC<FT_Trans>::PlaySound(uint8_t Volume,uint16_t SoundNote)
{
	FT_Trans::Write(REG_VOL_SOUND,Volume);//change the volume of synthesized sound, 0 means off, 255 means max on
	FT_Trans::Write16(REG_SOUND,SoundNote);
	FT_Trans::Write(REG_PLAY,FT_SOUND_PLAY);
	
	return FT_AE_OK;
}    template<class FT_Trans>
//higher byte is the note and lower byte is the sound
FT_AEStatus FT_GC<FT_Trans>::PlaySound(uint16_t SoundNote)
{
	FT_Trans::Write16(REG_SOUND,SoundNote);
	FT_Trans::Write(REG_PLAY,FT_SOUND_PLAY);
	return FT_AE_OK;
}    
template<class FT_Trans>
//volume will not be modified
void FT_GC<FT_Trans>::StopSound(void)
{
	FT_Trans::Write16(REG_SOUND,FT_SILENCE);//configure silence
	FT_Trans::Write(REG_PLAY,FT_SOUND_PLAY);//play the silence
}    
template<class FT_Trans>
void FT_GC<FT_Trans>::SetSoundVolume(uint8_t Volume)
{
	FT_Trans::Write(REG_VOL_SOUND,Volume);
}    
template<class FT_Trans>
uint8_t FT_GC<FT_Trans>::GetSoundVolume(void)
{
	return FT_Trans::Read(REG_VOL_SOUND);
}    
template<class FT_Trans>
//one shot or continuous, sampling frequency is from 8k to 48k
FT_AEStatus FT_GC<FT_Trans>::PlayAudio(uint8_t Volume,uint8_t Format,uint16_t SamplingFreq,uint32_t BufferAddr,uint32_t BufferSize,uint8_t Loop)
{
	if((SamplingFreq*1L < FT_AUDIO_SAMPLINGFREQ_MIN*1L) | (SamplingFreq*1L > FT_AUDIO_SAMPLINGFREQ_MAX*1L))
	{
		return FT_AE_ERROR_SAMPLINGFREQ_OUTOFRANGE;
	}
	if(Format > FT_ADPCM_SAMPLES)
	{
		return FT_AE_ERROR_FORMAT;
	}
	FT_Trans::Write(REG_VOL_PB,Volume);
	FT_Trans::Write32(REG_PLAYBACK_START,BufferAddr);
	FT_Trans::Write32(REG_PLAYBACK_LENGTH,BufferSize);
	FT_Trans::Write32(REG_PLAYBACK_FREQ,SamplingFreq);
	FT_Trans::Write(REG_PLAYBACK_FORMAT,Format);
	FT_Trans::Write(REG_PLAYBACK_LOOP,Loop);//0 means one shot and 1 means loop
	
	FT_Trans::Write(REG_PLAYBACK_PLAY,FT_AUDIO_PLAY);

	return FT_AE_OK;
}    
template<class FT_Trans>
void FT_GC<FT_Trans>::SetAudioVolume(uint8_t Volume)
{
	FT_Trans::Write(REG_VOL_PB,Volume);
}    
template<class FT_Trans>
//returns playback stopped or continue
FT_AEStatus FT_GC<FT_Trans>::GetAudioStats(uint32_t &CurrPlayAddr)
{
	CurrPlayAddr = FT_Trans::Read32(REG_PLAYBACK_READPTR);
	
	//in case of loop, check for the playback status
	if(0 == FT_Trans::Read(REG_PLAYBACK_PLAY))
	{
		return FT_AE_PLAYBACK_STOPPED;
	}

	return FT_AE_PLAYBACK_CONTINUE;
}    
template<class FT_Trans>
uint8_t FT_GC<FT_Trans>::GetAudioVolume(void)
{
	return  (FT_Trans::Read(REG_VOL_PB));
}    
template<class FT_Trans>
//volume will not be modified
void FT_GC<FT_Trans>::StopAudio(void)
{
	//configure audio with length o and play
	FT_Trans::Write32(REG_PLAYBACK_LENGTH,0);
	FT_Trans::Write(REG_PLAYBACK_LOOP,0);//0 means one shot and 1 means loop
	
	FT_Trans::Write(REG_PLAYBACK_PLAY,FT_AUDIO_PLAY);	
}    

template<class FT_Trans>
/* Apis related to touch engine */
//one of 0ff/oneshot/frame/continuous. default being continuous
void FT_GC<FT_Trans>::SetTouchMode(uint8_t TMode)
{
	FT_Trans::Write(REG_TOUCH_MODE,TMode);
}    
template<class FT_Trans>
//api to set coordinates for host specific tag query
void FT_GC<FT_Trans>::SetHostTagXY(uint16_t xoffset,uint16_t yoffset)
{
	uint8_t A[6];
	//little endian specific
	A[0] = xoffset & 0xFF;
	A[1] = xoffset >> 8;
	A[4] = yoffset & 0xFF;
	A[5] = yoffset >> 8;
	FT_Trans::Write(REG_TAG_X,A,6);
}    
template<class FT_Trans>
//api to get TAG from FT_GC for coordinates set by  SetHostTagXY() api - host needs to wait for at least 1 frame to get these query values
uint8_t FT_GC<FT_Trans>::GetHostTagXY(void)
{
	return ( FT_Trans::Read(REG_TAG) );
}   
template<class FT_Trans>
//get the touched object tag and repective xy coordinates
void FT_GC<FT_Trans>::GetTagXY(sTagXY &sTagxy)
{
	FT_Trans::Read(REG_TOUCH_TAG_XY,(uint8_t *)&sTagxy,6);
}    
template<class FT_Trans>
//get the track value and the tag value
void FT_GC<FT_Trans>::GetTrackTag(sTrackTag &sTracktag)
{
	uint32_t *ptr = (uint32_t *)&sTracktag;
	*ptr = FT_Trans::Read32(REG_TRACKER);
}    

template<class FT_Trans>
/* APIS related to power modes */
void FT_GC<FT_Trans>::HostCommand(uint32_t HostCommand)
{
	uint32_t Addr;
	/* construct host command and send to graphics controller */
	Addr = HostCommand<<16;
	FT_Trans::Read(Addr);//ideally sending 3 bytes is sufficient
	delay(20);//worst scenario
}    
template<class FT_Trans>	
//void FT_GC<FT_Trans>::CheckIntFlag(uint8_t IntFlag){}
FT_GEStatus FT_GC<FT_Trans>::UpdateFreeSpace()
{	
	if(TrnsFlag)
	{
		EndTransferCmd();
		//update the write pointer
		FT_Trans::Write16(REG_CMD_WRITE,CmdFifoWp);
		StartTransferCmd();
	}
	else
	{
		//update the write pointer
		FT_Trans::Write16(REG_CMD_WRITE,CmdFifoWp);
	}
	return FT_GE_OK;
}    
template<class FT_Trans>
/* API related to coprocessor fifo write command */
FT_GEStatus FT_GC<FT_Trans>::ChkGetFreeSpace(uint16_t NBytes)
{	
	//return busy if no space
	if(FreeSpace < NBytes)
	{
		if(TrnsFlag)
		{
			EndTransferCmd();
			TrnsFlag = 1;//coz EndTransferCmd will make it 0
		}
		//update the write pointer
		FT_Trans::Write16(REG_CMD_WRITE,CmdFifoWp);
		while(FreeSpace < NBytes)
		{
			uint16_t rdptr = FT_Trans::Read16(REG_CMD_READ);
			if(rdptr == FT_COPRO_ERROR)
			{
				return FT_GE_ERROR;
			}
			//update the freespace by reading the register
			FreeSpace = ((CmdFifoWp - rdptr)&0xffc);
			FreeSpace = (FT_CMDFIFO_SIZE - 4) - FreeSpace;
		}		
		if(TrnsFlag)
		{
			StartTransferCmd();
		}
	}
	return FT_GE_OK;
}    
template<class FT_Trans>

//assert CSpin and send write command
FT_GEStatus FT_GC<FT_Trans>::StartTransferCmd()
{
	//start write transaction
	FT_Trans::StartWrite(FT_RAM_CMD + CmdFifoWp);
	TrnsFlag = 1;
	return FT_GE_OK;
}    
template<class FT_Trans>

FT_GEStatus FT_GC<FT_Trans>::TransferCmd(uint32_t Cmd)
{
	if(FreeSpace <4)
	{
		//blocking call till freespace is available
		if(FT_GE_ERROR == ChkGetFreeSpace(4))
		{
			return FT_GE_ERROR;
		}
	}
	FT_Trans::Transfer32(Cmd);
	CmdFifoWp = (CmdFifoWp + 4)&0xfff;
	FreeSpace -= 4;
	
	return FT_GE_OK;
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::TransferCmd(uint8_t *Src,uint32_t NBytes)
{
	uint32_t i,Count;
	//align the NBytes to multiple of 4
	NBytes = (NBytes + 3)&(~3);
	//transfer the whole buffer into command buffer
	while(NBytes)
	{
		Count = NBytes;
		if(Count > FreeSpace)
		{
			//first update the free space
			UpdateFreeSpace();
			//then transfer the data
			Count = min(FreeSpace,Count);
			for(i = 0;i<Count;i++)	
				FT_Trans::Transfer(*Src++);
			CmdFifoWp = (CmdFifoWp + Count)&0xfff;
			FreeSpace -= Count;
			//get the free space
			NBytes -= Count;
			Count = min(NBytes,FT_CMDFIFO_SIZE/2);//atleast wait for half the buffer completion
			if(FT_GE_ERROR == ChkGetFreeSpace(Count))
			{
				return FT_GE_ERROR;
			}
		}
		else
		{
			//transfer of data to command buffer
			for(i = 0;i<Count;i++)	
				FT_Trans::Transfer(*Src++);
			CmdFifoWp = (CmdFifoWp + Count)&0xfff;
			FreeSpace -= Count;
			NBytes -= Count;
		}		
	}
	return FT_GE_OK;
}    
template<class FT_Trans>
FT_GEStatus FT_GC<FT_Trans>::TransferCmdfromflash( prog_uchar *Src,uint32_t NBytes)
{
	uint32_t i,Count;
	//align the NBytes to multiple of 4
	NBytes = (NBytes + 3)&(~3);
	//transfer the whole buffer into command buffer
	while(NBytes)
	{
		Count = NBytes;
		if(Count > FreeSpace)
		{
			//first update the free space
			UpdateFreeSpace();
			//then transfer the data
			Count = min(FreeSpace,Count);
			for(i = 0;i<Count;i++)	
			{
				FT_Trans::Transfer(pgm_read_byte_near(Src));
				Src++;
			}	
			CmdFifoWp = (CmdFifoWp + Count)&0xfff;
			FreeSpace -= Count;
			//get the free space
			NBytes -= Count;
			Count = min(NBytes,FT_CMDFIFO_SIZE/2);//atleast wait for half the buffer completion
			if(FT_GE_ERROR == ChkGetFreeSpace(Count))
			{
				return FT_GE_ERROR;
			}
		}
		else
		{
			//transfer of data to command buffer
			for(i = 0;i<Count;i++)	
			{
				FT_Trans::Transfer(pgm_read_byte_near(Src));
				Src++;
			}	
			CmdFifoWp = (CmdFifoWp + Count)&0xfff;
			FreeSpace -= Count;
			NBytes -= Count;
		}		
	}
	return FT_GE_OK;
}    
template<class FT_Trans>
//end the command transfer
void FT_GC<FT_Trans>::EndTransferCmd(void)
{	
	FT_Trans::EndTransfer();
	//update the write pointer of fifo
	FT_Trans::Write32(REG_CMD_WRITE,CmdFifoWp);
	TrnsFlag = 0;
}    
template<class FT_Trans>

FT_GEStatus FT_GC<FT_Trans>::WriteCmd(uint32_t Cmd)
{
	FT_GEStatus Status;
#if 0	
	if(0 == TrnsFlag)
		StartTransferCmd();
	Status = TransferCmd(Cmd);
	if(0 == TrnsFlag)
		EndTransferCmd();
#else
	if(0 == TrnsFlag)
	{
		StartTransferCmd();
		Status = TransferCmd(Cmd);
		EndTransferCmd();
	}
	else
	{
		Status = TransferCmd(Cmd);
	}

#endif
	return Status;
}

/*
void FT_GC<FT_Trans>::TransferCmdStr(const char *Str)
{
	TransferCmd(Str,strlen(Str)+1);
}
FT_GEStatus FT_GC<FT_Trans>::WriteCmdStr(const char* Str)
{
	StartTransferCmd();
	TransferCmd(Str,strlen(Str)+1);
	EndTransferCmd();
}*/
template<class FT_Trans>
//api to send N bytes to command buffer, this api pads with 0's at the end 
FT_GEStatus FT_GC<FT_Trans>::WriteCmd(uint8_t *Src,uint32_t NBytes)
{
	FT_GEStatus Status;
	if(0 == TrnsFlag)
	{
		StartTransferCmd();
		Status = TransferCmd(Src,NBytes);
		EndTransferCmd();//here transflag is made to 0
	}
	else
		Status = TransferCmd(Src,NBytes);
		
	return Status;
}   
template<class FT_Trans>

FT_GEStatus FT_GC<FT_Trans>::WriteCmdfromflash( prog_uchar *Src,uint32_t NBytes)
{
	FT_GEStatus Status;
	if(0 == TrnsFlag)
	{
		StartTransferCmd();
		Status =  TransferCmdfromflash(Src,NBytes);		
		EndTransferCmd();//here transflag is made to 0
	}
	else
		Status = TransferCmdfromflash(Src,NBytes);
		
	return Status;
}    
template<class FT_Trans>		
//reads the result of the previous commands such as cmd_memcrc,cmd_calibration, cmd_regread which has return values. if busy returns busy status
FT_GEStatus FT_GC<FT_Trans>::Cmd_GetResult(uint32_t &Result)
{
	Result = FT_Trans::Read32(FT_RAM_CMD + ((CmdFifoWp - 4)&0xFFC));//make sure no other commands are issued after cmd_memcrc/cmd_calibration/cmd_regread
	
	return FT_GE_OK;
}    
template<class FT_Trans>
//api to read n bytes from the current write pointer location
//make sure no other commands are issued after cmd_memcrc/cmd_calibration/cmd_regread
//assumed that number of bytes in array is allocated/managed by application
FT_GEStatus FT_GC<FT_Trans>::Cmd_GetResults(int8_t *pA,uint16_t NBytes)
{
	//handling of circular buffer
	if((CmdFifoWp - NBytes) < 0)
	{
		uint16_t ReadLen = NBytes - CmdFifoWp;
		FT_Trans::Read((FT_RAM_CMD + (FT_CMDFIFO_SIZE - ReadLen)),(uint8_t *)pA,ReadLen);//first read the end bytes
		pA += ReadLen;
		ReadLen = NBytes - ReadLen;
		FT_Trans::Read(FT_RAM_CMD,(uint8_t *)pA,ReadLen);//later read the starting bytes
	}
	else
	{
		FT_Trans::Read(FT_RAM_CMD + (CmdFifoWp - NBytes),(uint8_t *)pA,NBytes);
	}
	
	return FT_GE_OK;
}    

template<class FT_Trans>
/* Special apis */
//apis for ease of usage in FT_GC
//inserts cmd_dlstart() followed by clear(1,1,1) graphics command
void FT_GC<FT_Trans>::DLStart(void)
{
	Cmd_DLStart();
	Clear(1,1,1);
}    

template<class FT_Trans>
//inserts display() gpu instruction at the end and inserts cmd_swap() command
void FT_GC<FT_Trans>::DLEnd(void)
{
	Display();
	Cmd_Swap();
}    

template<class FT_Trans>
//special api to check logo completion
FT_GEStatus FT_GC<FT_Trans>::CheckLogo(void)
{
	uint16_t ReadCmdPtr = FT_Trans::Read16(REG_CMD_READ) ;
	if((ReadCmdPtr == FT_Trans::Read16(REG_CMD_WRITE)) && (ReadCmdPtr == 0))
	{
		CmdFifoWp = 0;
		return FT_GE_FINISHED;
	}
	else if(FT_COPRO_ERROR == ReadCmdPtr)
	{
		return FT_GE_ERROR;
	}
	else
	{
		return FT_GE_BUSY;
	}
	
}   

template<class FT_Trans>
//apis to render all the commands to hardware
//api to flush out all the commands to FT_GC, does not wait for the completion of the rendering
FT_GEStatus FT_GC<FT_Trans>::Flush(void)
{
	if(TrnsFlag)
	{
		EndTransferCmd();
	}	
	FT_Trans::Write16(REG_CMD_WRITE,CmdFifoWp);
	
	return FT_GE_OK;
}    

template<class FT_Trans>
//flushes out all the commands to FT_GC and waits for the completion of execution
FT_GEStatus FT_GC<FT_Trans>::Finish(void)
{
	uint16_t ReadPrt;
	
	if(TrnsFlag)
	{
		EndTransferCmd();
	}
	FT_Trans::Write16(REG_CMD_WRITE,CmdFifoWp);
	
	while((ReadPrt = FT_Trans::Read16(REG_CMD_READ)) != CmdFifoWp)
	{
		if(FT_COPRO_ERROR == ReadPrt)
		{
			return FT_GE_ERROR;
		}
	}	
	return FT_GE_OK;
}    

template<class FT_Trans>
//checks fifo and returns the status	
FT_GEStatus FT_GC<FT_Trans>::CheckFinish(void)
{
	uint16_t ReadPrt = FT_Trans::Read16(REG_CMD_READ);
	

	if (FT_COPRO_ERROR == ReadPrt)
	{
		return FT_GE_ERROR;
	}
	else if(ReadPrt != CmdFifoWp)
	{
		return FT_GE_BUSY;
	}	//success case return finished
	return FT_GE_FINISHED;
}    

template<class FT_Trans>
//error from graphics controller library
uint32_t FT_GC<FT_Trans>::GetError(void)
{
	if(FT_COPRO_ERROR == FT_Trans::Read16(REG_CMD_READ))
	{
		return FT_GE_ERROR;
	}
	return FT_GE_OK;	
}


#endif /* _FT_GC_H_ */


