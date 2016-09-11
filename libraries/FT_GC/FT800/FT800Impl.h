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
* @file                           FT800Impl.h
* @brief                          Contains FT800 graphics controller implementation for arduino platform.
								  Tested platform version: Arduino 1.0.4 and later
* @version                        1.1.0
* @date                           2014/02/05
*
*/

#ifndef _FT800IMPL_H_
#define _FT800IMPL_H_

/* Macros for FT800Impl library version */
#define FT800IMPL_MAJOR 				(1)
#define FT800IMPL_MINOR 				(0)
#define FT800IMPL_BUILD 				(0)

/* FT800Impl status enum - used for api return type, error type etc */

 
/************************************************************************************************************************************************************
Display parameters used for various options are

FT800_DisplayResolution			Width 	Height	Swizzle	Polarity	PClk	HCycle	Hoffset		Hsync0		Hsync1		VCycle	Voffset		Vsync0	Vsync1	
FT_DISPLAY_QVGA_320x240   		320		240		3		0			8		408		70			0			10			263			13		0		2
FT_DISPLAY_WQVGA_480x272		480		272		0		1			5		548		43			0			41			292			12		0		10
*************************************************************************************************************************************************************/



 /*
 Following are the assumptions made for FT800Impl library
 1. SPI library from arduino is included and working fine on this platform
 2. 
 3. 
 */
	template<class FT_Trans>
class FT800Impl : public FT_GC<FT_Trans>
 {
	public:
	
	/* Api to set cs, pdn, int pin. CS pin is mandatory. If PDN and INT pins are set then gpios are initialized */	
	FT800Impl(void);/* API utilizing default pin for cs, pdn. For interrupt pin - no pin will be assigned */	
	FT800Impl(uint8_t CSPin,uint8_t PDNPin);/* no interrupt pin will be assigned */	
	FT800Impl(uint8_t CSPin,uint8_t PDNPin,uint8_t IntPin);//all the three pins will be utilized
	~FT800Impl();
	FT_Status Init(uint8_t ResType, uint16_t options1);//api to set the resolution of output display	
	FT_Status Init(uint16_t hperiod,uint16_t vperiod,uint16_t hfrontporch,uint16_t hbackporch,uint16_t hpulsewidth,
				   uint16_t vfrontporch,uint16_t vbackporch,uint16_t vpulsewidth,
				   uint8_t polarity,uint8_t swizzle,uint8_t fps, uint16_t options1);/* Api to program the output display wrt display parameters */
	FT_Status Exit();
	void GetVersion(uint8_t &Major, uint8_t &Minor, uint8_t &Build);/* Api to get version of the library */
	uint8_t IsPendown();//API to return the status of pendown. 0 - penup, 1 - pendown
	/* Apis related to graphics processor */	
	uint8_t SetIntMask(uint8_t IntMask);
	//void AssignCBFunptr(void *CBFunPtr);//api to register callback function from application using interrupt mechanism
	//api to set the resistive touch sensor parameters
	void SetTouch(uint8_t AdcMode,uint16_t Charge,uint8_t Settle,uint16_t Oversample, uint16_t RZTreshold);
};



/* Api to set cs, pdn, int pin. CS pin is mandatory. If PDN and INT pins are set then gpios are initialized */	
/* API utilizing default pin for cs, pdn. For interrupt pin - no pin will be assigned */	
template<class FT_Trans>
FT800Impl<FT_Trans>::FT800Impl(void)
{
	
}
template<class FT_Trans>
FT800Impl<FT_Trans>::FT800Impl(uint8_t csPin,uint8_t pdnPin)
{
	/* no interrupt pin will be assigned */	
	FT_GC<FT_Trans>::CSPin = csPin;
	FT_GC<FT_Trans>::PDNPin = pdnPin;
}
template<class FT_Trans>
FT800Impl<FT_Trans>::FT800Impl(uint8_t csPin,uint8_t pdnPin,uint8_t IntPin)
{
	//all the three pins will be utilized
	FT_GC<FT_Trans>::CSPin = csPin;
	FT_GC<FT_Trans>::PDNPin = pdnPin;
	FT_GC<FT_Trans>::IntPin = IntPin;
}
template<class FT_Trans>
FT800Impl<FT_Trans>::~FT800Impl()
{
	//do nothing
}
/* api to set the resolution of output display */
template<class FT_Trans>
FT_Status FT800Impl<FT_Trans>::Init(uint8_t ResType, uint16_t options1=0)
{
	return ( FT_GC<FT_Trans>::Init(ResType,options1) );
	
	
}

/* Api to program the output display wrt display parameters */
template<class FT_Trans>
FT_Status FT800Impl<FT_Trans>::Init(uint16_t hperiod,uint16_t vperiod,uint16_t hfrontporch,uint16_t hbackporch,uint16_t hpulsewidth,
			   uint16_t vfrontporch,uint16_t vbackporch,uint16_t vpulsewidth,
			   uint8_t polarity,uint8_t swizzle,uint8_t fps, uint16_t options1=0)
{
	FT_Status Status;
	Status = FT_GC<FT_Trans>::Init(hperiod,vperiod,hfrontporch,hbackporch,hpulsewidth,vfrontporch,vbackporch,vpulsewidth,polarity,swizzle,fps);
	
	return Status;	
}

template<class FT_Trans>
FT_Status FT800Impl<FT_Trans>::Exit()
{
	return ( FT_GC<FT_Trans>::Exit() );
	
}
/* Api to get version of the library */
template<class FT_Trans>
void FT800Impl<FT_Trans>::GetVersion(uint8_t &Major, uint8_t &Minor, uint8_t &Build)
{
	/* Update the arguments with version numbers */
	Major = FT800IMPL_MAJOR;
	Minor = FT800IMPL_MINOR;
	Build = FT800IMPL_BUILD;
}
/* Apis related to graphics processor */	
template<class FT_Trans>
uint8_t FT800Impl<FT_Trans>::SetIntMask(uint8_t IntMask)
{
	/* Write the interrupt mask value to register */
	FT_Trans::Write(REG_INT_MASK,IntMask);	
	
	return 0;
}
/* Api to detect pen down or pen up from the hardware */
template<class FT_Trans>
uint8_t FT800Impl<FT_Trans>::IsPendown()
{
	//return 0 if pen up and return 1 if pen down
	if(FT_Trans::Read32(REG_TOUCH_DIRECT_XY)&0x80000000)//bit31 indicates whether pen down or pen up. 0 - touch is sensed, 1 - no touch
	{
		return 0;//pen up
	}
	
	return 1;//pen down
}
//set touch parameters
template<class FT_Trans>
void FT800Impl<FT_Trans>::SetTouch(uint8_t AdcMode,uint16_t Charge,uint8_t Settle,uint16_t Oversample, uint16_t RZTreshold)
{
	FT_Trans::Write(REG_TOUCH_ADC_MODE,AdcMode);
	FT_Trans::Write16(REG_TOUCH_CHARGE,Charge);
	FT_Trans::Write(REG_TOUCH_SETTLE,Settle);
	FT_Trans::Write16(REG_TOUCH_OVERSAMPLE,Oversample);
	FT_Trans::Write(REG_TOUCH_RZTHRESH,RZTreshold);	
}

#endif /* _FT800IMPL_H_ */


