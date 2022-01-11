//---------------------------------------------------------
/*

FT_App_CarbonationMachine.ino

Program for writing to Newhaven Displays NHD-4.3CTP-SHIELD based on the FT801 controller.

This code is written for the Arduino Mega.

Copyright (c) 2016 - Newhaven Display International, Inc.

Newhaven Display invests time and resources providing this open source code,
please support Newhaven Display by purchasing products from Newhaven Display!

*/
//---------------------------------------------------------

#include <EEPROM.h>
#include <SPI.h>
#include <Wire.h>
#include <FT_NHD_43CTP_SHIELD.h>

#define RAM_IMAGES_SODA1 26700
static PROGMEM prog_uchar IMAGES_SODA1[] = {
	#include "IMAGES_SODA1.h"
};

#define RAM_IMAGES_SODA2 89252
static PROGMEM prog_uchar IMAGES_SODA2[] = {
	#include "IMAGES_SODA2.h"
};

/*
#define RAM_IMAGES_SODA3 150904
static PROGMEM prog_uchar IMAGES_SODA3[] = {
	#include "IMAGES_SODA2.h"
};
*/

FT801IMPL_SPI FTImpl(FT_CS_PIN,FT_PDN_PIN,FT_INT_PIN);

int counter=6;
int previous;
int image_count;
unsigned long currentTime;
unsigned long loopTime;
const int pin_A = 7;  // pin 7
const int pin_B = 6;  // pin 6
const int pin_C = 5;  // pin 5
unsigned char encoder_A;
unsigned char encoder_B;
unsigned char encoder_button;
unsigned char encoder_A_prev=0;
int transformA;
int transformE;
int i;
int n;
int wait_time;
int spark_y;
int box_location;
int box_start = 120;
int box_end = 120;

void read_encoder()  {  
  currentTime = millis();                     // get the current elapsed time
  if(currentTime >= (loopTime + 5)){          // 5ms since last check of encoder = 200Hz      
    encoder_A = digitalRead(pin_A);           // Read encoder pins
    encoder_B = digitalRead(pin_B);   
    if((!encoder_A) && (encoder_A_prev)){     // A has gone from high to low      
      if(encoder_B) {                         // B is high so clockwise      
        counter--;
        if(counter==0){counter=13;}        
      }   
      else {                                  // B is low so counter-clockwise 
        counter++;
        if(counter==14){counter=1;}        
      }   

    }   
    encoder_A_prev = encoder_A;               // Store value of A for next time      
    loopTime = currentTime;  // Updates loopTime
  }                         
}

void encoder_click(){
  if(counter<13){
    FTImpl.DLStart();
    FTImpl.ClearColorRGB(255, 255, 255);
    FTImpl.Clear(1, 1, 1);
    FTImpl.End();
    FTImpl.Begin(FT_BITMAPS);
    FTImpl.Vertex2ii(293, 7, image_count, 0);
    FTImpl.End();
    FTImpl.ColorRGB(0, 0, 0);
    FTImpl.Cmd_Text(5, 73, 31, 0, "CARBONATING");
    FTImpl.ColorRGB(201, 132, 63);
    FTImpl.Cmd_Spinner(158, 156, 3, 1);
    FTImpl.DLEnd();
    FTImpl.Finish();
    delay(wait_time);
  }
  else{
        FTImpl.DLStart();
        FTImpl.ClearColorRGB(255, 255, 255);
        FTImpl.Clear(1, 1, 1);
        FTImpl.End();
        FTImpl.ColorRGB(0, 0, 0);
        FTImpl.Cmd_Text(178, 61, 29, 0, "CLEANING");
        FTImpl.ColorRGB(255, 255, 255);
        FTImpl.End();
        FTImpl.Begin(FT_BITMAPS);
        FTImpl.Vertex2ii(183, 95, 0, 0);
        FTImpl.End();
        FTImpl.ColorRGB(201, 132, 63);
        FTImpl.Cmd_Spinner(240, 210, 1, 1);	
        FTImpl.DLEnd();
        FTImpl.Finish();
        delay(5000);
      }          
}

void setup()
{
        pinMode(pin_A, INPUT);
        pinMode(pin_B, INPUT);
        pinMode(pin_C, INPUT);
        currentTime = millis();
        loopTime = currentTime; 
  
  
	FTImpl.Init(FT_DISPLAY_RESOLUTION);FTImpl.SetDisplayEnablePin(FT_DISPENABLE_PIN);FTImpl.SetAudioEnablePin(FT_AUDIOENABLE_PIN);
	FTImpl.DisplayOn();FTImpl.AudioOn();

	FTImpl.DLStart();
	FTImpl.Cmd_Inflate(RAM_IMAGES_SODA1);FTImpl.WriteCmdfromflash(IMAGES_SODA1, sizeof(IMAGES_SODA1));	
        FTImpl.Cmd_Inflate(RAM_IMAGES_SODA2);FTImpl.WriteCmdfromflash(IMAGES_SODA2, sizeof(IMAGES_SODA2));
        FTImpl.Finish();
	FTImpl.DLStart();
	FTImpl.BitmapHandle(1);FTImpl.BitmapSource(26700);FTImpl.BitmapLayout(FT_ARGB1555, 278, 225);FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, 139, 225);
        FTImpl.BitmapHandle(2);FTImpl.BitmapSource(89252);FTImpl.BitmapLayout(FT_ARGB1555, 268, 225);FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, 134, 225);
        FTImpl.BitmapHandle(0);FTImpl.BitmapSource(211204);FTImpl.BitmapLayout(FT_ARGB1555, 196, 97);FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, 98, 97);
        FTImpl.DLEnd();FTImpl.Finish();
        FTImpl.DLStart();
	FTImpl.ClearColorRGB(201, 132, 63);FTImpl.Clear(1, 1, 1);
	FTImpl.ClearColorRGB(0, 0, 0);FTImpl.Cmd_Text(80, 113, 30, 0, "Carbonation Machine");FTImpl.Cmd_Spinner(227, 205, 0, 0);	
	FTImpl.DLEnd();FTImpl.Finish();
        delay(1000);
}

void loop()
{       
       switch (counter){
          case 1:
            image_count=2;spark_y = 242;box_start = box_end;box_end = 2;wait_time = 1000;break; 
          case 2:
            image_count=2;spark_y = 242;box_start = box_end;box_end = 122;wait_time = 1500;break; 
          case 3:
            image_count=2;spark_y = 242;box_start = box_end;box_end = 243;wait_time = 2000;break; 
          case 4:
            image_count=2;spark_y = 242;box_start = box_end;box_end = 360;wait_time = 2500;break;  
          case 5:
            image_count=1;spark_y = 222;box_start = box_end;box_end = 2;wait_time = 1000;break;  
          case 6:
            image_count=1;spark_y = 222;box_start = box_end;box_end = 122;wait_time = 1500;break; 
          case 7:
            image_count=1;spark_y = 222;box_start = box_end;box_end = 243;wait_time = 2000;break; 
          case 8:
            image_count=1;spark_y = 222;box_start = box_end;box_end = 360;wait_time = 2500;break; 
          case 9:
            image_count=2;spark_y = 242;box_start = box_end;box_end = 2;wait_time = 1000;break; 
          case 10:
            image_count=2;spark_y = 242;box_start = box_end;box_end = 122;wait_time = 1500;break;  
          case 11:
            image_count=2;spark_y = 242;box_start = box_end;box_end = 243;wait_time = 2000;break;
          case 12:
            image_count=2;spark_y = 242;box_start = box_end;box_end = 360;wait_time = 2500;break;
       }              
       if(counter<13){
           for(i=20;i<98;i++){
                FTImpl.DLStart();
                FTImpl.ClearColorRGB(255, 255, 255);
                FTImpl.Clear(1, 1, 1);
                FTImpl.End();
        	FTImpl.Cmd_FGColor(0x1DAAEB); 
                box_location = box_end;
        	FTImpl.Cmd_Button(box_location, 40, 100, 155, 27, FT_OPT_FLAT, "");
        	FTImpl.ColorRGB(0, 0, 0);
                FTImpl.Cmd_Text(16, 195, 28, 0, "Small");
        	FTImpl.Cmd_Text(16, spark_y, 27, 0, "Sparkling");
             	FTImpl.Cmd_Text(138, 195, 28, 0, "Medium");
        	FTImpl.Cmd_Text(138, spark_y, 27, 0, "Sparkling");       	
        	FTImpl.Cmd_Text(260, 195, 28, 0, "Large");
        	FTImpl.Cmd_Text(260, spark_y, 27, 0, "Sparkling");
        	FTImpl.Cmd_Text(377, 195, 28, 0, "XL");
        	FTImpl.Cmd_Text(377, spark_y, 27, 0, "Sparkling");
        
                switch (counter){
                    case 1:
                      FTImpl.Cmd_Text(16, 220, 27, 0, "Light");
        	      FTImpl.Cmd_Text(138, 220, 27, 0, "Light");
        	      FTImpl.Cmd_Text(260, 220, 27, 0, "Light");
        	      FTImpl.Cmd_Text(377, 220, 27, 0, "Light");           
                    break; 
                    case 2:
                      FTImpl.Cmd_Text(16, 220, 27, 0, "Light");
        	      FTImpl.Cmd_Text(138, 220, 27, 0, "Light");
        	      FTImpl.Cmd_Text(260, 220, 27, 0, "Light");
        	      FTImpl.Cmd_Text(377, 220, 27, 0, "Light");             
                    break; 
                    case 3:
                      FTImpl.Cmd_Text(16, 220, 27, 0, "Light");
        	      FTImpl.Cmd_Text(138, 220, 27, 0, "Light");
        	      FTImpl.Cmd_Text(260, 220, 27, 0, "Light");
        	      FTImpl.Cmd_Text(377, 220, 27, 0, "Light");             
                    break; 
                    case 4:
                      FTImpl.Cmd_Text(16, 220, 27, 0, "Light");
        	      FTImpl.Cmd_Text(138, 220, 27, 0, "Light");
        	      FTImpl.Cmd_Text(260, 220, 27, 0, "Light");
        	      FTImpl.Cmd_Text(377, 220, 27, 0, "Light"); 
                    break; 
                    case 5:            
                    break; 
                    case 6:
                    break; 
                    case 7:            
                    break; 
                    case 8:
                    break; 
                    case 9:
                      FTImpl.Cmd_Text(16, 220, 27, 0, "Extra");
        	      FTImpl.Cmd_Text(138, 220, 27, 0, "Extra");
        	      FTImpl.Cmd_Text(260, 220, 27, 0, "Extra");
        	      FTImpl.Cmd_Text(377, 220, 27, 0, "Extra");
                    break; 
                    case 10:
                      FTImpl.Cmd_Text(16, 220, 27, 0, "Extra");
        	      FTImpl.Cmd_Text(138, 220, 27, 0, "Extra");
        	      FTImpl.Cmd_Text(260, 220, 27, 0, "Extra");
        	      FTImpl.Cmd_Text(377, 220, 27, 0, "Extra");          
                    break; 
                    case 11:
                      FTImpl.Cmd_Text(16, 220, 27, 0, "Extra");
        	      FTImpl.Cmd_Text(138, 220, 27, 0, "Extra");
        	      FTImpl.Cmd_Text(260, 220, 27, 0, "Extra");
        	      FTImpl.Cmd_Text(377, 220, 27, 0, "Extra");          
                    break;
                    case 12:
                      FTImpl.Cmd_Text(16, 220, 27, 0, "Extra");
        	      FTImpl.Cmd_Text(138, 220, 27, 0, "Extra");
        	      FTImpl.Cmd_Text(260, 220, 27, 0, "Extra");
        	      FTImpl.Cmd_Text(377, 220, 27, 0, "Extra");          
                    break; 
                    }
                                   	
        	FTImpl.ColorRGB(255, 255, 255);
        	FTImpl.Cmd_Progress(144, 10, 180, 12, 0, i, 100);
        	FTImpl.ColorRGB(255, 255, 255);	
        	FTImpl.Begin(FT_BITMAPS);
        	FTImpl.BitmapTransformA(440);
        	FTImpl.BitmapTransformE(420);
        	FTImpl.BitmapTransformB(0);
        	FTImpl.BitmapTransformC(0);
        	FTImpl.BitmapTransformD(0);
        	FTImpl.BitmapTransformF(0);
        	FTImpl.Vertex2ii(10, 50, image_count, 0);
        	FTImpl.End();
        	FTImpl.Begin(FT_BITMAPS);
        	FTImpl.BitmapTransformA(410);
        	FTImpl.BitmapTransformE(390);
        	FTImpl.BitmapTransformB(0);
        	FTImpl.BitmapTransformC(0);
        	FTImpl.BitmapTransformD(0);
        	FTImpl.BitmapTransformF(0);
        	FTImpl.Vertex2ii(130, 40, image_count, 0);
        	FTImpl.End();
        	FTImpl.Begin(FT_BITMAPS);
        	FTImpl.BitmapTransformA(380);
        	FTImpl.BitmapTransformE(360);
        	FTImpl.BitmapTransformB(0);
        	FTImpl.BitmapTransformC(0);
        	FTImpl.BitmapTransformD(0);
        	FTImpl.BitmapTransformF(0);
        	FTImpl.Vertex2ii(247, 30, image_count, 0);
        	FTImpl.End();
        	FTImpl.Begin(FT_BITMAPS);
        	FTImpl.BitmapTransformA(350);
        	FTImpl.BitmapTransformE(320);
        	FTImpl.BitmapTransformB(0);
        	FTImpl.BitmapTransformC(0);
        	FTImpl.BitmapTransformD(0);
        	FTImpl.BitmapTransformF(0);
        	FTImpl.Vertex2ii(360, 15, image_count, 0);
                if(i>90){FTImpl.ColorRGB(0, 0, 0);FTImpl.Cmd_Text(337, 7, 29, 0, "READY");FTImpl.ColorRGB(255, 255, 255);}
        	FTImpl.End();
        	FTImpl.DLEnd();
        	FTImpl.Finish();
                i++;i++;i++;i++;i++;
                }
         }      
       else{
        FTImpl.DLStart();
	FTImpl.ClearColorRGB(255, 255, 255);
	FTImpl.Clear(1, 1, 1);
	FTImpl.End();
	FTImpl.ColorRGB(0, 0, 0);
	FTImpl.ColorRGB(255, 255, 255);
	FTImpl.Cmd_Progress(144, 10, 180, 12, 0, 97, 100);
	FTImpl.ColorRGB(0, 0, 0);
	FTImpl.Cmd_Text(337, 7, 29, 0, "READY");
	FTImpl.Cmd_Text(337, 32, 29, 0, "TO CLEAN");
	FTImpl.ColorRGB(255, 255, 255);
	FTImpl.End();
	FTImpl.Begin(FT_BITMAPS);
	FTImpl.Vertex2ii(183, 95, 0, 0);
	FTImpl.End();	
	FTImpl.DLEnd();
	FTImpl.Finish();
  }         
        previous = counter;
        //delay(1000);
        //encoder_click();               
        while (counter == previous){         
          read_encoder();
          encoder_button = digitalRead(pin_C);
          if(!encoder_button){encoder_click();return;}
        }    
        if(counter==14){counter=1;}          
}
/* end of file */