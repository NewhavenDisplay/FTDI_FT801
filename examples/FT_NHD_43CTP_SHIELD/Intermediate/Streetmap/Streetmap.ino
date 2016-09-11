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
* @file                           Streetmap.ino
* @brief                          Sketch to lines, font index, and call feature of the FT801 series graphics controller.
								  Tested platform version: Arduino 1.0.4 and later
* @version                        0.1.0
* @date                           2014/17/05
*
*/



/* Arduino standard includes */
#include "SPI.h"
#include "Wire.h"

/* Platform specific includes */
#include "FT_NHD_43CTP_SHIELD.h"

/* Global object for FT801 Implementation */
FT801IMPL_SPI FTImpl(FT_CS_PIN,FT_PDN_PIN,FT_INT_PIN);

/* API to boot up the graphics controller, verify the hardware and configure the display/audio pins */
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
	
	/* Set the Display & audio pins */
	FTImpl.SetDisplayEnablePin(FT_DISPENABLE_PIN);
    FTImpl.SetAudioEnablePin(FT_AUDIOENABLE_PIN); 
	FTImpl.DisplayOn(); 
	FTImpl.AudioOn();  	
	return 0;
}

/*display predefined lines to represent streets utilizing the Call command*/
void Streetmap()
{
	FTImpl.DLStart();//start the display list. Note DLStart and DLEnd are helper apis, Cmd_DLStart() and Display() can also be utilized.
	FTImpl.ClearColorRGB(236,232,224); // clear the background to light gray
	FTImpl.Clear(1,1,1);
	
	FTImpl.ColorRGB(170,157,136);//subsequent drawing operations will be medium gray
	FTImpl.LineWidth(63); //line width at 16th pixel precision
	FTImpl.Call(20);//draw the streets, 20 is the number of commands after Cmd_DLStart to the 'FTImpl.Begin(FT_LINES);' command.
	FTImpl.ColorRGB(250,250,250);//change color
	FTImpl.LineWidth(48); //draw the streets, 20 is the number of commands after Cmd_DLStart to the 'FTImpl.Begin(FT_LINES);' command.
	FTImpl.Call(20);//draw the streets on top of the existing gray streets. 
	FTImpl.ColorRGB(0,0,0);  //change color for the "Main st" text
	FTImpl.Begin(FT_BITMAPS);
	FTImpl.Vertex2ii(240,91,27,77  );//draw the letter 'M' at (240,91) position.  27 is the font handle and 77 is the ascii value.
	FTImpl.Vertex2ii(252,91,27,97	);//draw the letter 'a' at (252,91) position.  27 is the font handle and 97 is the ascii value.
	FTImpl.Vertex2ii(260,91,27,105	);//draw the letter 'i' at (260,91) position.  27 is the font handle and 105 is the ascii value.
	FTImpl.Vertex2ii(260,91,27,110	);//draw the letter 'n' at (260,91) position.  27 is the font handle and 110 is the ascii value.
	FTImpl.Vertex2ii(275,91,27,115	);//draw the letter ' ' at (275,91) position.  27 is the font handle and 115 is the ascii value.
	FTImpl.Vertex2ii(282,91,27,116	);//draw the letter 's' at (282,91) position.  27 is the font handle and 116 is the ascii value.
	FTImpl.Vertex2ii(286,91,27,46	);//draw the letter 't' at (286,91) position.  27 is the font handle and 46 is the ascii value.
	FTImpl.End();
	FTImpl.Display();
	/*draw lines at a pre-specified coordinates, two vertexes define a line*/
	FTImpl.Begin(FT_LINES);  //19th command
	FTImpl.Vertex2f(-160,-20  );
	FTImpl.Vertex2f(320,4160  );
	FTImpl.Vertex2f(800,-20   );
	FTImpl.Vertex2f(1280,4160 );
	FTImpl.Vertex2f(1920,-20  );
	FTImpl.Vertex2f(2400,4160 );
	FTImpl.Vertex2f(2560,-20  );
	FTImpl.Vertex2f(3040,4160 );
	FTImpl.Vertex2f(3200,-20  );
	FTImpl.Vertex2f(3680,4160 );
	FTImpl.Vertex2f(2880,-20  );
	FTImpl.Vertex2f(3360,4160 );
	FTImpl.Vertex2f(-20,0	   );
	FTImpl.Vertex2f(5440,-480 );
	FTImpl.Vertex2f(-20,960   );
	FTImpl.Vertex2f(5440,480  );
	FTImpl.Vertex2f(-20,1920  );
	FTImpl.Vertex2f(5440,1440 );
	FTImpl.Vertex2f(-20,2880  );
	FTImpl.Vertex2f(5440,2400 );
	FTImpl.End();
	FTImpl.Return();  /*continue where Call command was issued*/
	FTImpl.DLEnd();//end the display list
	FTImpl.Finish();//render the display list and wait for the completion of the DL
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
		Streetmap();
	}
	Serial.println("--End Application--");
}

/* Nothing in loop api */
void loop()
{
}
