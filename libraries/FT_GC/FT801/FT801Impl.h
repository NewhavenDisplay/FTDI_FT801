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
* @file                           FT801Impl.h
* @brief                          Contains FT800 graphics controller implementation for arduino platform.
								  Tested platform version: Arduino 1.0.4 and later
* @version                        1.1.0
* @date                           2014/02/05
*
*/

#ifndef _FT801IMPL_H_
#define _FT801IMPL_H_

/* Macros for FT800 library version */
#define FT801IMPL_MAJOR 				(1)
#define FT801IMPL_MINOR 				(0)
#define FT801IMPL_BUILD 				(0)

/* FT801 status enum - used for api return type, error type etc */

 
/************************************************************************************************************************************************************
Display parameters used for various options are

FT801_DisplayResolution			Width 	Height	Swizzle	Polarity	PClk	HCycle	Hoffset		Hsync0		Hsync1		VCycle	Voffset		Vsync0	Vsync1	
FT_DISPLAY_QVGA_320x240   		320		240		3		0			8		408		70			0			10			263			13		0		2
FT_DISPLAY_WQVGA_480x272		480		272		0		1			5		548		43			0			41			292			12		0		10
*************************************************************************************************************************************************************/


/* Structure definitions */


typedef struct sCTouchXY
{
	int16_t y0;		//y coordinate of touch0
	int16_t x0;		//x coordinate of touch0
	int16_t y1;		//y coordinate of touch1
	int16_t x1;		//x coordinate of touch1
	int16_t y2;		//y coordinate of touch2
	int16_t x2;		//x coordinate of touch2
	int16_t y3;		//y coordinate of touch3
	int16_t x3;		//x coordinate of touch3
	int16_t y4;		//y coordinate of touch4
	int16_t x4;		//x coordinate of touch4	
}sCTouchXY;

 /*
 Following are the assumptions made for FT801 library
 1. SPI library from arduino is included and working fine on this platform
 2. APIs to be called before bootup/initialization
 3. APIs that can be called at run time
 */
template<class FT_Trans>
class FT801Impl: public FT_GC<FT_Trans>
 {
	public:
	
	/* Api to set cs, pdn, int pin. CS pin is mandatory. If PDN and INT pins are set then gpios are initialized */	
	FT801Impl(void);/* API utilizing default pin for cs, pdn. For interrupt pin - no pin will be assigned */	
	FT801Impl(uint8_t CSPin,uint8_t PDNPin);/* no interrupt pin will be assigned */	
	FT801Impl(uint8_t CSPin,uint8_t PDNPin,uint8_t IntPin);//all the three pins will be utilized
	~FT801Impl();
	FT_Status Init(uint8_t ResType,uint16_t options1);//api to set the resolution of output display	
	FT_Status Init(uint16_t hperiod,uint16_t vperiod,uint16_t hfrontporch,uint16_t hbackporch,uint16_t hpulsewidth,
				   uint16_t vfrontporch,uint16_t vbackporch,uint16_t vpulsewidth,
				   uint8_t polarity,uint8_t swizzle,uint8_t fps, uint16_t options1);/* Api to program the output display wrt display parameters */
	FT_Status Exit();
	void GetVersion(uint8_t &Major, uint8_t &Minor, uint8_t &Build);/* Api to get version of the library */
	uint8_t IsPendown();//API to return the status of pendown. 0 - penup, 1 - pendown
	/* Apis related to graphics processor */	
	uint8_t SetIntMask(uint8_t IntMask);
	//void AssignCBFunptr(void *CBFunPtr);//api to register callback function from application using interrupt mechanism	
	
	/* APIs related to graphics engine */
	void SetCTouchMode(uint8_t CTPMode);//compatibility or extended more - by default the init value is compatibility mode
	void GetCTouchXY(sCTouchXY &sCTouchxy);//get the coordinates of all the 5 touch points
	FT_GEStatus Cmd_CSketch(int16_t x, int16_t y, uint16_t w, uint16_t h, uint32_t Ptr, uint16_t Format, uint16_t Freq);//api to send csketch command
};




/* Api to set cs, pdn, int pin. CS pin is mandatory. If PDN and INT pins are set then gpios are initialized */	
/* API utilizing default pin for cs, pdn. For interrupt pin - no pin will be assigned */	
template<class FT_Trans>
FT801Impl<FT_Trans>::FT801Impl(void)
{
	
}
template<class FT_Trans>
FT801Impl<FT_Trans>::FT801Impl(uint8_t csPin,uint8_t pdnPin)
{
	/* no interrupt pin will be assigned */	
	FT_GC<FT_Trans>::CSPin = csPin;
	FT_GC<FT_Trans>::PDNPin = pdnPin;
}
template<class FT_Trans>
FT801Impl<FT_Trans>::FT801Impl(uint8_t csPin,uint8_t pdnPin,uint8_t IntPin)
{
	//all the three pins will be utilized
	FT_GC<FT_Trans>::CSPin = csPin;
	FT_GC<FT_Trans>::PDNPin = pdnPin;
	FT_GC<FT_Trans>::IntPin = IntPin;
}
template<class FT_Trans>
FT801Impl<FT_Trans>::~FT801Impl()
{
	//do nothing
}
/* api to set the resolution of output display */
template<class FT_Trans>
FT_Status FT801Impl<FT_Trans>::Init(uint8_t ResType, uint16_t options1=0)
{
	return (FT_GC<FT_Trans>::Init(ResType,options1) );
}

/* Api to program the output display wrt display parameters */
template<class FT_Trans>
FT_Status FT801Impl<FT_Trans>::Init(uint16_t hperiod,uint16_t vperiod,uint16_t hfrontporch,uint16_t hbackporch,uint16_t hpulsewidth,
			   uint16_t vfrontporch,uint16_t vbackporch,uint16_t vpulsewidth,
			   uint8_t polarity,uint8_t swizzle,uint8_t fps,uint16_t options1=0)
{
	FT_Status Status;
	Status = FT_GC<FT_Trans>::Init(hperiod,vperiod,hfrontporch,hbackporch,hpulsewidth,vfrontporch,vbackporch,vpulsewidth,polarity,swizzle,fps,options1);
	
	return Status;	
}
template<class FT_Trans>
FT_Status FT801Impl<FT_Trans>::Exit()
{
	return (FT_GC<FT_Trans>::Exit() );
}
/* Api to get version of the library */
template<class FT_Trans>
void FT801Impl<FT_Trans>::GetVersion(uint8_t &Major, uint8_t &Minor, uint8_t &Build)
{
	/* Update the arguments with version numbers */
	Major = FT801IMPL_MAJOR;
	Minor = FT801IMPL_MINOR;
	Build = FT801IMPL_BUILD;
}

/* Apis related to graphics processor */	
template<class FT_Trans>
uint8_t FT801Impl<FT_Trans>::SetIntMask(uint8_t IntMask)
{
	/* Write the interrupt mask value to register */
	FT_Trans::Write(REG_INT_MASK,IntMask);	
	
	return 0;
}
/* Api to detect pen down or pen up from the hardware - compatibility mode */
template<class FT_Trans>
uint8_t FT801Impl<FT_Trans>::IsPendown()
{
	//return 0 if pen up and return 1 if pen down
	if(FT_Trans::Read32(REG_CTOUCH_TOUCH2_XY)&0x80000000)//bit31 indicates whether pen down or pen up. 0 - touch is sensed, 1 - no touch
	{
		return 0;//pen up
	}
	
	return 1;//pen down
}

/* APIs related to graphics engine */
//compatibility or extended more - by default the init value is compatibility mode
//CTPMode - FT_CTOUCH_MODE_COMPATIBILITY means compatibility and FT_CTOUCH_MODE_EXTENDED means extended
template<class FT_Trans>
void FT801Impl<FT_Trans>::SetCTouchMode(uint8_t CTPMode)
{
	FT_Trans::Write(REG_CTOUCH_EXTENDED,CTPMode);
}

//get the coordinates of all the 5 touch points
template<class FT_Trans>
void FT801Impl<FT_Trans>::GetCTouchXY(sCTouchXY &sCTouchxy)
{
	/* Read all the touch registers */
	FT_Trans::Read(REG_CTOUCH_TOUCH2_XY,(uint8_t *)&sCTouchxy.y2,8);//2 and 3 can be read one shot
	(*(uint32_t *)(&sCTouchxy.y0)) = FT_Trans::Read32(REG_CTOUCH_TOUCH0_XY);
	(*(uint32_t *)(&sCTouchxy.y1)) = FT_Trans::Read32(REG_CTOUCH_TOUCH1_XY);
	sCTouchxy.x4 = FT_Trans::Read16(REG_CTOUCH_TOUCH4_X);
	sCTouchxy.y4 = FT_Trans::Read16(REG_CTOUCH_TOUCH4_Y);
}

//api to do a new csketch - only used in ft801. Typical value for Freq parameter is 1500
template<class FT_Trans>
FT_GEStatus FT801Impl<FT_Trans>::Cmd_CSketch(int16_t x, int16_t y, uint16_t w, uint16_t h, uint32_t Ptr, uint16_t Format, uint16_t Freq)
{
	FT_GEStatus Status;
	
	FT_GC<FT_Trans>::WriteCmd(CMD_CSKETCH);
	FT_GC<FT_Trans>::WriteCmd(((y & 0xFFFFL)<<16) | (x & 0xFFFFL));
	FT_GC<FT_Trans>::WriteCmd(((h & 0xFFFFL)<<16) | (w & 0xFFFFL));
	FT_GC<FT_Trans>::WriteCmd(Ptr);
	Status = FT_GC<FT_Trans>::WriteCmd(((Freq & 0xFFFFL)<<16) | (Format & 0xFFFFL));
	
	return Status;	

}
#endif /* _FT801IMPL_H_ */


