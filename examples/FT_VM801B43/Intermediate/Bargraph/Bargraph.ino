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
* @file       Bargraph.ino
* @brief      Sketch to demonstrate the bargraph bitmap drawing format on FT801.  Bargraph render data as a bar graph.  It looks up the x coordinate in a byte array, then gives an opaque pixel if the byte value is less than y, otherwise a transparent pixel. The result is a bar graph of the bitmap data.  A maximum of 256x256 size bitmap can be drawn using the BARGRAPH  format.  Orientation, width and height of the graph can be altered using the bitmap transform matrix.      
								  Tested platform version: Arduino 1.0.4 and later
* @version                        0.1.0
* @date                           2014/17/05
*
*/



/* Arduino standard includes */
#include "SPI.h"
#include "Wire.h"

/* Platform specific includes */
#include "FT_VM801B43.h"

#define FT_CLAM(value,mininum,maximum) (max((mininum),min((value),(maximum))))
#define WRAP_AROUND 512
#define UPDATE_MID 1024
#define UPDATE_LEFT 2048
#define UPDATE_MID_TAG 100
#define UPDATE_LEFT_TAG 101
#define WRAP_AROUND_TAG 102
#define SELECTION_POINT_SIZE 10

#ifdef FT800_CHIPID
	#define MAX 1200
#else
	#define MAX FT_DISPLAYWIDTH
#endif


/* Global object for FT801 Implementation */
FT801IMPL_SPI FTImpl(FT_CS_PIN,FT_PDN_PIN,FT_INT_PIN);

/*Draw the bar graph based on the position, options, new value, and range of the data.  The data represented is located in RAM_G and it's the size is 'w' bytes*/
void Bargraph(int16_t x,int16_t y,int16_t w,int16_t h,uint16_t options, uint16_t val,uint16_t range)
{
			uint16_t tempVal=0, seg1=0, seg2=0, seg3=0;
			uint16_t midPoint=w/2;
			static int16_t index=0,ramOffset=0;
			uint8_t removePix = 5;

			/*update the supplied value to the correct location in RAM_G.  Data wrapping around is done by updating the RAM_G index location only if the value is greater than 0.*/
			if((options&WRAP_AROUND)==WRAP_AROUND){
				if(val>0){
					tempVal = h - ((h * val) / range);
					/*This function set one or more bytes to the speficied value.  Alternatively, the overloaded Write() function can be used to write data to the RAM_G*/
					FTImpl.Cmd_Memset(ramOffset+index,tempVal, 1); 
				}
			}
			/*No wrap around means update the RAM_G with all values.*/
			else{ 
				tempVal = h - ((h * val) / range);
				FTImpl.Cmd_Memset(ramOffset+index,tempVal, 1);
			}
 
			FTImpl.ColorRGB(255,25,25);
			FTImpl.LineWidth(10);
			FTImpl.Begin(FT_BITMAPS);  
			
			/*New value shows up in the middle.  The bargraph is drawn in section because the most recent data should be shown at beginning.*/
			if((options&UPDATE_MID)==UPDATE_MID){  //draw updates from the middle
			/*new value is added at the beginning of the RAM_G memory section*/
				if(index==0){ 
					FTImpl.BitmapSource(ramOffset);
					FTImpl.BitmapLayout(FT_BARGRAPH, w-midPoint,1);
					FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, w-midPoint, h);
					FTImpl.Vertex2f((x+midPoint)*16,(y)*16);

					FTImpl.BitmapSource(ramOffset+midPoint);
					FTImpl.BitmapLayout(FT_BARGRAPH, midPoint-removePix,1);
					FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, midPoint-removePix, h);
					FTImpl.Vertex2f((x)*16,(y)*16);	
				}
				/*new value is added at the end of the RAM_G memory section*/
				else if(index==w-1){
					FTImpl.BitmapSource(ramOffset+index-1);
					FTImpl.BitmapLayout(FT_BARGRAPH, 1,1);
					FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, 1, h);
					FTImpl.Vertex2f((x+midPoint)*16,(y)*16);

					FTImpl.BitmapSource(ramOffset);
					FTImpl.BitmapLayout(FT_BARGRAPH, w-(midPoint+1),1);
					FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, w-(midPoint+1), h);
					FTImpl.Vertex2f((x+midPoint+1)*16,(y)*16);

					FTImpl.BitmapSource(ramOffset+(w-(midPoint+1)));
					FTImpl.BitmapLayout(FT_BARGRAPH, midPoint-removePix,1);
					FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, midPoint-removePix, h);
					FTImpl.Vertex2f((x)*16,(y)*16);
				}
				/*new value is added in the middle*/
				else{ 
					/*new value is added before the mid point of the RAM_G memory section*/
					if((index+(w-midPoint))<w){
						FTImpl.BitmapSource(ramOffset+index);
						FTImpl.BitmapLayout(FT_BARGRAPH, w-midPoint,1);
						FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, w-midPoint, h);
						FTImpl.Vertex2f((x+midPoint)*16,(y)*16);
						seg1=x+midPoint;

						tempVal=FT_CLAM(w-(index+(w-midPoint)),1,midPoint-removePix);
						FTImpl.BitmapSource(index+(w-midPoint));
						FTImpl.BitmapLayout(FT_BARGRAPH, tempVal,1);
						FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, tempVal, h);
						FTImpl.Vertex2f((x)*16,(y)*16);
						seg2=x;

						if(index>removePix){
						FTImpl.BitmapSource(ramOffset);
						FTImpl.BitmapLayout(FT_BARGRAPH, index-removePix,1);
						FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, index-removePix, h);
						FTImpl.Vertex2f((x+tempVal)*16,(y)*16);
						seg3=x+tempVal;
						}
					}
					/*new value is added after the mid point of the RAM_G memory section*/
					else if(index+midPoint>w){
						tempVal=w-index;
						FTImpl.BitmapSource(ramOffset+index);
						FTImpl.BitmapLayout(FT_BARGRAPH, tempVal,1);
						FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, tempVal, h);
						FTImpl.Vertex2f((x+midPoint)*16,(y)*16);
						
						tempVal=(index+midPoint)-w;
						FTImpl.BitmapSource(ramOffset);
						FTImpl.BitmapLayout(FT_BARGRAPH, tempVal,1);
						FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, tempVal, h);
						FTImpl.Vertex2f((x+midPoint+(w-index))*16,(y)*16);

						FTImpl.BitmapSource(ramOffset+tempVal);
						FTImpl.BitmapLayout(FT_BARGRAPH, midPoint-removePix,1);
						FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, midPoint-removePix, h);
						FTImpl.Vertex2f((x)*16,(y)*16);
					}
					/*new value is added at the midpoint of the RAM_G memory section*/
					else{  
						FTImpl.BitmapSource(ramOffset+index);
						FTImpl.BitmapLayout(FT_BARGRAPH, w-midPoint,1);
						FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, w-midPoint, h);
						FTImpl.Vertex2f((x+midPoint)*16,(y)*16);

						FTImpl.BitmapSource(ramOffset);
						FTImpl.BitmapLayout(FT_BARGRAPH, midPoint-removePix,1);
						FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, midPoint-removePix, h);
						FTImpl.Vertex2f((x)*16,(y)*16);
					}
					
				}
			}
			/*New value shows up in the left side.  The bargraph is drawn in section because the most recent data should be shown at beginning.*/
			else if((options&UPDATE_LEFT)==UPDATE_LEFT){//draw updates from the left side
				/*new value is added at the beginning of the RAM_G memory section*/
				if(index==0){
					FTImpl.BitmapSource(ramOffset);
					FTImpl.BitmapLayout(FT_BARGRAPH, 1,1);
					FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, 1, h);
					FTImpl.Vertex2f((x)*16,(y)*16);

					FTImpl.BitmapSource(ramOffset+1);
					FTImpl.BitmapLayout(FT_BARGRAPH, w-1,1);
					FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, w-1, h);
					FTImpl.Vertex2f((x+1)*16,(y)*16);	
				}
				/*new value is added at the end of the RAM_G memory section*/
				else if(index==(w-1)){
					FTImpl.BitmapSource(ramOffset+index);
					FTImpl.BitmapLayout(FT_BARGRAPH, 1,1);
					FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, 1, h);
					FTImpl.Vertex2f((x)*16,(y)*16);

					FTImpl.BitmapSource(ramOffset);
					FTImpl.BitmapLayout(FT_BARGRAPH, w-1,1);
					FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, w-1, h);
					FTImpl.Vertex2f((x+1)*16,(y)*16);
				}
				/*new value is added at the middle of the RAM_G memory section*/
				else{
					FTImpl.BitmapSource(ramOffset+index);
					FTImpl.BitmapLayout(FT_BARGRAPH, w-index,1);
					FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, w-index, h);
					FTImpl.Vertex2f((x)*16,(y)*16);
						if(index>1){
							FTImpl.BitmapSource(ramOffset);
							FTImpl.BitmapLayout(FT_BARGRAPH, index-1,1);
							FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, index-1, h);
							FTImpl.Vertex2f((x+w-index)*16,(y)*16);
						}
						else{
							FTImpl.BitmapSource(ramOffset);
							FTImpl.BitmapLayout(FT_BARGRAPH, 1,1);
							FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, 1, h);
							FTImpl.Vertex2f((x+w-1)*16,(y)*16);
						}

				
					}
			}
			
	FTImpl.Begin(FT_LINES);
	FTImpl.LineWidth(10);
	FTImpl.ColorRGB(255,255,255);
	FTImpl.Vertex2f((x)*16,y*16);
	FTImpl.Vertex2f((x)*16,(y+h)*16);
	
	FTImpl.Vertex2f((x)*16,(y+h)*16);
	FTImpl.Vertex2f((x+w)*16,(y+h)*16);
	
	
	index--;
			if(index<0){
				index=w-1;
			}
}

/*returns the tag value on touch release*/
uint8_t Read_Keys()
{
  static uint8_t Read_tag=0,Last_tag=0,Ret_tag=0;	
  Read_tag = FTImpl.Read(REG_TOUCH_TAG);
  Ret_tag = 0;
  if(Read_tag!=0)	//touch an object for the first time
  {
    if(Last_tag!=Read_tag) //note the tag value if it's different
    {
      Last_tag = Read_tag;	
    }  
  }
  else
  {
    if(Last_tag!=0) //the touch has just been release on a tagged object
    {
      Ret_tag = Last_tag;
	  Last_tag=0;
    }  
  }
  return Ret_tag;
}

/*selected option*/
void selectedPoint(int16_t x, int16_t y){
	FTImpl.ColorRGB(255,255,255);
	FTImpl.PointSize(SELECTION_POINT_SIZE*16);
	FTImpl.Vertex2f(x*16,y*16);
	FTImpl.ColorRGB(50,200,50);
	FTImpl.PointSize((SELECTION_POINT_SIZE-2)*16);
	FTImpl.Vertex2f(x*16,y*16);
}

/*unselected option*/
void unselectedPoint(int16_t x, int16_t y){
	FTImpl.ColorRGB(255,255,255);
	FTImpl.PointSize(SELECTION_POINT_SIZE*16);
	FTImpl.Vertex2f(x*16,y*16);
	FTImpl.ColorRGB(0,0,0);
	FTImpl.PointSize((SELECTION_POINT_SIZE-2)*16);
	FTImpl.Vertex2f(x*16,y*16);
}

/*update and return the selected options from the user input*/
uint16_t options(int16_t x, int16_t y){
	uint8_t readkey=0;
	static uint16_t updateMethod = UPDATE_MID, wrapAround=WRAP_AROUND;
	readkey=Read_Keys();
	/*update options variable*/
	switch(readkey){
		case UPDATE_LEFT_TAG:
			updateMethod = UPDATE_LEFT;
			break;
		case UPDATE_MID_TAG:
			updateMethod = UPDATE_MID;
			break;
		case WRAP_AROUND_TAG:
			if(wrapAround) wrapAround=0;
			else wrapAround=WRAP_AROUND;
			break;
	}
	
	/*update options on the display*/
		FTImpl.TagMask(1);
		FTImpl.Begin(FT_POINTS);
		
		FTImpl.Tag(UPDATE_LEFT_TAG);
		if(updateMethod==UPDATE_LEFT)selectedPoint(x,y);
		else unselectedPoint(x,y);
		
		FTImpl.Tag(UPDATE_MID_TAG);
		if(updateMethod == UPDATE_MID)selectedPoint(x,y+23);
		else unselectedPoint(x,y+23);
		
		FTImpl.Tag(WRAP_AROUND_TAG);
		if(wrapAround==WRAP_AROUND) selectedPoint(x,y+46);
		else unselectedPoint(x,y+46);
		FTImpl.TagMask(0);
		
		FTImpl.ColorRGB(255,255,255);
		FTImpl.Cmd_Text(x+15,y,26,FT_OPT_CENTERY,"Update From Left");
		FTImpl.Cmd_Text(x+15,y+23,26,FT_OPT_CENTERY,"Update From Middle");
		FTImpl.Cmd_Text(x+15,y+46,26,FT_OPT_CENTERY,"Data Wrap Around");
	
	return (wrapAround|updateMethod);
}

/*get a value for the bargraph*/
uint16_t getValue(){
	/*platforms utilize the FT800 graphics controller*/
	#ifdef FT800_CHIPID
			return (MAX-min(FTImpl.Read16(REG_TOUCH_RZ),MAX));
	#else
			/*platforms utilize the FT801 graphics controller*/
			#ifdef FT801_CHIPID
				sTagXY sTagxy;
				if(FTImpl.IsPendown()){
					FTImpl.GetTagXY(sTagxy);
					return sTagxy.x;
				}
				else{
					return 0;
				}
			#endif
	#endif	
			
}

/*************************************************************************/
/* Below code demonstrates the usage of calibrate function. Calibrate    */
/* function will wait till user have pressed all three dots.  Only way to*/
/* come out of this API is to reset the coprocessor.                     */
/*************************************************************************/
void Calibrate()
{
  FTImpl.DLStart(); //start the display list. Note DLStart and DLEnd are helper apis, Cmd_DLStart() and Display() can also be utilized.
  FTImpl.ClearColorRGB(64,64,64); 
  FTImpl.Clear(1,1,1);    
  FTImpl.ColorRGB(0xff, 0xff, 0xff);
  FTImpl.Cmd_Text((FT_DISPLAYWIDTH/2), (FT_DISPLAYHEIGHT/2), 27, FT_OPT_CENTER, "Please Tap on the dot"); //display string at the center of the screen using inbuilt font handle 29 
  FTImpl.Cmd_Calibrate(0);
  FTImpl.Finish(); //render the display list and wait for the completion of the DL
}

/* Api to boot up the graphics controller, verify the hardware and configure the display/audio pins */
/* Returns 0 in case of success and 1 in case of failure */
int16_t BootupConfigure()
{
	uint32_t chipid = 0;
	FTImpl.Init(FT_DISPLAY_RESOLUTION);

	delay(20);//for safer side
	chipid = FTImpl.Read32(FT_ROM_CHIPID);
	
	/* Identify the chip */
	if(FT801_CHIPID != chipid)
	{
		Serial.print("Error in chip id read ");
		Serial.println(chipid,HEX);
		return 1;
	}
	
	#ifdef FT800_CHIPID
	/*Touch screen pressure sensitivity adjustment*/
	FTImpl.Write16(REG_TOUCH_RZTHRESH,1200); 
	#endif 
	
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
		uint16_t selectedOptions,value,width=200,height=100,xPos=FT_DISPLAYWIDTH/2-width/2,yPos=FT_DISPLAYHEIGHT/2-height/2 - 20;
		
		FTImpl.Cmd_Memset(0,255,width);  //clear bargraph memory location in RAM_G. 
		Calibrate(); //calibrate the touch screen.
		while(1){
			FTImpl.DLStart(); 		
			selectedOptions = options(xPos+10, yPos+height+20); //update and check user inputs
			value = getValue(); //get a value
			Bargraph(xPos,yPos,width,height,selectedOptions,value,MAX);  //draw the bargraph
			FTImpl.DLEnd();
			FTImpl.Finish();
		}
	}
	Serial.println("--End Application--");
}

/* Nothing in loop api */
void loop()
{
}
