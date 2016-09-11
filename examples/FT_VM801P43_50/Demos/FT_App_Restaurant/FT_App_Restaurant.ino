
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
* @file                           FT_App_Restaurant.ino
* @brief                          Sample application to demonstrate FT801 primitives, widgets and customized screen shots. 
* @version                        0.1.0
* @date                           2014/03/21
*
*/


#include "SPI.h"
#include "Wire.h"
#include "FT_VM801P43_50.h"

#define imw  (80)
#define imh  (53)

/* SD card object*/
FT_SD FtSd(FT_SD_CSPIN);

/* sd_present holds error values during initialization.  0 means no error and all other errors are non zero value */
FT_SDStatus sd_present; 

/* Global object for transport */
FT801IMPL_SPI FTImpl(FT_CS_PIN,FT_PDN_PIN,FT_INT_PIN);


int16_t CartX = 480;


 typedef struct logoim
{
  prog_char name[10];
  uint16_t image_height;
  uint8_t image_format;
  uint8_t filter;
  uint16_t sizex;
  uint16_t sizey;
  uint16_t linestride;
  uint16_t gram_address;
}t_imageprp;


PROGMEM t_imageprp Main_Icons[7] = {
                             {"Menu.BIN",  53,  FT_RGB565,  FT_BILINEAR,    114,   76,  80*2,  0L     },    //handle 0 
                             {"BB.BIN",    50,  FT_ARGB4,   FT_NEAREST,    50,    50,  50*2,  153},    //handle 1
                             {"HB.BIN",    50,  FT_ARGB4,   FT_NEAREST,    50,    50,  50*2,  159},    //handle 2
                             {"CB.BIN",    50,  FT_ARGB4,   FT_NEAREST,    50,    51,  51*2,  165},    //handle 3
                             {"OB.BIN",    50,  FT_RGB565,   FT_NEAREST,    50,    50,  50*2,  171},    //handle 4
                             {"PB.BIN",    26,  FT_ARGB4,   FT_NEAREST,    26,    26,  26*2,  177},    //handle 5
                             {"MB.BIN",    6,  FT_ARGB4,   FT_NEAREST,    26,    26,  26*2,  180},    //handle 6
};

/*****************************************************************************/
/* Example code to display few points at various offsets with various colors */ 

PROGMEM prog_char *info[] =  { 
					  "FT801 Restaurant Demo Application",
          			  "APP to demonstrate interactive Menu,",
					  "and animation,",	 
					  "using Bitmaps & Track",
};


static uint8_t key_pressed = 0;

//static int16_t trackval = 0;
static float trackval=0;
uint8_t item_no = 0,item_selection = 0,item_selected_counts = 0;

uint8_t description_no = 0;
int32_t StoreTag[16],CheckTag[16];

typedef struct Item_Prop
{
        prog_char ItemName[20];
        prog_char ItemPrice[7];
        prog_char ItemDescription[100];
        uint8_t ItemPriceinVal;
}Item_Properties;

 Item_Properties Item_Property[14] PROGMEM= 
{
	{0,0,0},//0
	{"Espresso\0", "$3.00\0", "Italian Espresso Coffee",3},//1
	{"Black Coffee\0", "$2.00\0", "Freshly Brewed Coffee",2},//2
	{"Hot Choclate\0", "$3.00\0", "The Finest cocoa blended with hot milk",3},//3
	{"Tea\0", "$2.00\0", "Hot Tea",2},//4
	{"Blueberry Muffin\0", "$2.00\0", "A moist, flavorful muffin packed with juicy blueberries",2},//5
	{"Choclate Muffin\0", "$2.00\0", "A delicious muffin packed with rich choclate",2},//6
	{"Choclate Donut\0", "$2.00\0", "A classic glazed chocolate doughnut",2},//7
	{"Pain au Choclat\0", "$2.00\0", "A light, flaky pastry with chocolate",2},//8
	{"Tiramisu\0", "$2.00\0", "Creamy traditional tiramisu",2},//9
	{"Walnut Frappe\0", "$3.00\0","Coffee blended with Walnut drizzle",3},//10
	{"Choclate Frappe\0", "$3.00","A creamy blend of chocolaty chips, milk and ice",3},//11
	{"Strawberry Frappe\0", "$3.00","Strawberries and milk blended with ice",3},//12
	{"Vanilla Frappe\0", "$3.00","A rich and creamy blend of vanilla bean, milk and ice",3},//13
};

PROGMEM prog_char Array_MainMenu_name[4][12] = {"\0", "Beverages", "Pastries", "Frappe"};
PROGMEM prog_char Array_Beverage_name[5][14] = {"\0","Espresso", "Black Coffee", "Hot Choclate","Tea"};
PROGMEM prog_char Array_Frappe_name[5][12] = {"\0","Walnut", "Choclate", "Strawberry","Vanilla"};
PROGMEM prog_char Array_Pastry_name[6][18] = {"\0", "Blueberry Muffin", "Choc Muffin", "Choc Donut", "Pain au Choclat", "Tiramisu"};

uint8_t Read_Keys()
{
	static uint8_t Read_tag=0,temp_tag=0,ret_tag=0;	
	ret_tag = NULL;		
	Read_tag = FTImpl.Read(REG_TOUCH_TAG);						
	if(Read_tag!=NULL)											// Allow if the Key is released
	{
		if(temp_tag!=Read_tag)
		{
			temp_tag = Read_tag;	
			key_pressed = Read_tag;	
			// Load the Read tag to temp variable	
		}  
	}
	else
	{
		key_pressed = 0;
		if(temp_tag!=0)
		{
			ret_tag = temp_tag;
			temp_tag = 0;
		}  
	}
	return ret_tag;
}

void Info()
{
  uint16_t dloffset = 0,z;
// Touch Screen Calibration
  FTImpl.DLStart(); 
  FTImpl.ColorRGB(255,255,255);
  FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,FT_DISPLAYHEIGHT/2,28,FT_OPT_CENTERX|FT_OPT_CENTERY,"Please tap on a dot");
  FTImpl.Cmd_Calibrate(0);
  FTImpl.DLEnd();
  FTImpl.Finish();
// Ftdi Logo animation  
  FTImpl.Cmd_Logo();
  while(FTImpl.CheckLogo() != FT_GE_FINISHED);
//Copy the Displaylist from DL RAM to GRAM
  dloffset = FTImpl.Read16(REG_CMD_DL);
  dloffset -=4; 
  FTImpl.WriteCmd(CMD_MEMCPY);
  FTImpl.WriteCmd(100*1024L);
  FTImpl.WriteCmd(FT_RAM_DL);
  FTImpl.WriteCmd(dloffset);
//Enter into Info Screen
  do
  {
    FTImpl.DLStart();   
    FTImpl.Cmd_Append(100*1024L,dloffset);
//Reset the BITMAP properties used during Logo animation
    FTImpl.BitmapTransformA(256);
    FTImpl.BitmapTransformB(0);
    FTImpl.BitmapTransformC(0);
    FTImpl.BitmapTransformD(0);
    FTImpl.BitmapTransformE(256);
    FTImpl.BitmapTransformF(0);  
//Display the information with transparent Logo using Edge Strip
    FTImpl.SaveContext();  
    FTImpl.ColorRGB(219,180,150);
    FTImpl.ColorA(220);
    FTImpl.Begin(FT_EDGE_STRIP_A);
    FTImpl.Vertex2f(0,FT_DISPLAYHEIGHT*16);
    FTImpl.Vertex2f(FT_DISPLAYWIDTH*16,FT_DISPLAYHEIGHT*16);
    FTImpl.ColorA(255);
	FTImpl.RestoreContext();
    FTImpl.ColorRGB(0,0,0);
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,20,28,FT_OPT_CENTERX|FT_OPT_CENTERY,info[0]);
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,60,26,FT_OPT_CENTERX|FT_OPT_CENTERY,info[1]);
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,90,26,FT_OPT_CENTERX|FT_OPT_CENTERY,info[2]);  
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,120,26,FT_OPT_CENTERX|FT_OPT_CENTERY,info[3]);  
    FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2,FT_DISPLAYHEIGHT-30,26,FT_OPT_CENTERX|FT_OPT_CENTERY,"Click to play");
//Check if the Play key and change the color
    if(key_pressed!='P')

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
  
  FTImpl.DLStart();   
  FTImpl.ColorRGB(255,255,255);
  FTImpl.Clear(1,1,1);   
  FTImpl.ColorRGB(0,0,0);
  FTImpl.Cmd_Text(FT_DISPLAYWIDTH/2-80,FT_DISPLAYHEIGHT/2,29,FT_OPT_CENTERX|FT_OPT_CENTERY,"Data Loading");
  FTImpl.Cmd_Spinner(FT_DISPLAYWIDTH/2+40,FT_DISPLAYHEIGHT/2,1,0);
  FTImpl.DLEnd();
  FTImpl.Finish();
  FTImpl.PlaySound(255,0x50);
}

void Back_Home(uint8_t alpha,uint8_t OrderFlag)
{
	uint8_t TagValue=0,Touch=0;
	int16_t IconCt=0;
	sTagXY sTagxy;
	FTImpl.GetTagXY(sTagxy);
	Touch = sTagxy.y;
	//TagValue = sTagxy.tag;
	TagValue = FTImpl.Read( REG_TOUCH_TAG);
   FTImpl.ColorA(0);
   FTImpl.TagMask(1);
   FTImpl.Tag('T');
   FTImpl.Cmd_Button(10,200,470,70,16,0,"");
   FTImpl.RestoreContext();  
   FTImpl.ColorA(alpha);  
   FTImpl.Tag('B');
   FTImpl.Vertex2ii(10,220,1,0);
   FTImpl.Tag('H');
   FTImpl.Vertex2ii(70,220,2,0);
	if(OrderFlag ==0)
	   {
	   FTImpl.Tag('O');
	   FTImpl.ColorRGB(255,255,255);						
	   FTImpl.Vertex2ii(430,220,4,0);	
	   }
   FTImpl.RestoreContext();  
   FTImpl.TagMask(0);

}

uint8_t Selection_Menu(uint8_t NoItems,uint8_t start_tag)
{ 
    float zinout_f = 0; 
    uint8_t Menu=0,temp_Menu=0,x,rotation_flag=0;    
    int16_t prevth=0,currth=0,key_deteted_cts=0,/*xoff,yoff,*/rotation_cts=0,deg = 0/*,theta,zinout*/; 
    sTagXY sTagxy;
	sTrackTag sTracktag;
    char buffer[20];
    FTImpl.Cmd_Track(240,0,1,1,1);
    FTImpl.Cmd_Track(240,0,1,1,2);
    FTImpl.Cmd_Track(240,0,1,1,3);
    FTImpl.Cmd_Track(240,0,1,1,4);
    FTImpl.Finish();
    deg = 360/6;
	
    do
    {
	FTImpl.GetTagXY(sTagxy);
	FTImpl.GetTrackTag(sTracktag);

    uint32_t sy = FTImpl.Read32(REG_TRACKER);
    uint8_t key_detected = sTracktag.tag;    

    int16_t TouchTag = sTagxy.y; 
    if(key_detected>0)
    { 
        key_deteted_cts++;
        if(key_detected<4)
    	temp_Menu = key_detected; else temp_Menu = 0;
    	Menu = 0;			
    	rotation_cts = 0;
    }   
         
	if(key_detected==0)
    {
    	if(key_deteted_cts>0&&key_deteted_cts<15)
    	{
    		if(!rotation_flag)
				Menu = temp_Menu; else rotation_flag = 0;
    	}			
            key_deteted_cts = 0;
            
    	if(rotation_cts>100)
    	{	
            trackval+=0.5;
    		rotation_flag = 1;
    	}	
    	else 
            rotation_cts++;
    }     

        if(key_deteted_cts>15)
        {  
      	float tempval;
		currth = sTracktag.track;
      	tempval = ((currth-prevth));
      		
      	if((currth-prevth) > 32000)
      	{
      		tempval = 65536 - currth + prevth;
      	}
      	else if((currth-prevth) < -32000)
      	{
      		tempval = 65536 - prevth + currth;
      		tempval = -tempval;
      	}
		
      	tempval = (tempval*360)/65536;
      	rotation_flag = 0;
      
      	if(prevth!=0)
      	{
      		trackval += tempval;
      	}
		
      	prevth = currth;
      	rotation_cts = 0;
        }
      	else
           {
              prevth=0;    
           }  
     

      	FTImpl.DLStart();   
        /* Background Effect */
      	FTImpl.TagMask(1);
      	FTImpl.Tag(4);      	
      	FTImpl.Cmd_Gradient(0, 0, 0x000000, 0, 180, 0xFF0000);
      	FTImpl.Begin(FT_POINTS);
      	FTImpl.ColorA(50);
      	FTImpl.ColorRGB(0,0,0);
      	FTImpl.PointSize(250*16);
      	FTImpl.Vertex2f((240)*16,-150);
      	FTImpl.ColorA(60);
      	FTImpl.ColorRGB(255,0,0);
      	FTImpl.PointSize(135*16);
      	FTImpl.Vertex2f((240)*16,-150);
      	FTImpl.RestoreContext();
        FTImpl.TagMask(0);
        /* Main Menu Items Display */
      	FTImpl.Begin(FT_BITMAPS);
        for(x=1;x<=NoItems;x++)
  	{
            float tempval;
	    int16_t theta = deg*x;		
  	    tempval = deg*x + trackval+deg;

	    theta = (int16_t)((int32_t)tempval%220); 
		
	    int16_t xoff = 200+(int16_t)(190*cos(theta*0.0174));
  	    int16_t yoff = -20+(int16_t)(150*sin(theta*0.0174)); 
  
  	    int16_t zinout = 310- yoff;           
  	    FTImpl.BitmapHandle(0);
            FTImpl.Cell(x-1);
            FTImpl.BitmapTransformA(zinout);
  	    FTImpl.BitmapTransformE(zinout );
            FTImpl.TagMask(1);
  	    FTImpl.Tag(x);
            FTImpl.BitmapSize(FT_BILINEAR, FT_BORDER, FT_BORDER, 256*80/zinout, 256*53/zinout  );
  	    FTImpl.Vertex2f(xoff*16,yoff*16);
  	    FTImpl.TagMask(0);
       }

       FTImpl.RestoreContext();
       FTImpl.Cmd_Text(150,50,26,0,"Please select one of the Options");
       
       for(x=1;x<=NoItems;x++)
       {
         
            float tempval1 ;
	    int16_t theta = deg*x;		
	    tempval1 = deg*x + trackval+deg;

	    theta = (int16_t)((int32_t)tempval1%220); 

	    int16_t xoff = 200+(int16_t)(190*cos(theta*0.0174));
	    int16_t yoff = -20+(int16_t)(150*sin(theta*0.0174)); 
            zinout_f  =  310-yoff;
 	    zinout_f  =  255/zinout_f;
	    int16_t zinout = 5+yoff+(int16_t)(53*zinout_f);
            strcpy_P(buffer, Array_MainMenu_name[x]);           
            FTImpl.Cmd_Text(xoff+5,(zinout),16,FT_OPT_CENTERY,buffer);
       }
       FTImpl.DLEnd();
       FTImpl.Finish();       
    }while(Menu==0);
    return Menu;  
}

static struct {
  signed short dragprev;
  int vel;      // velocity
  long base;    // screen x coordinate, in 1/16ths pixel
  long limit;
} scroller;

static void scroller_init(uint32_t limit)
{
  scroller.dragprev = -32768;
  scroller.vel = 0;      // velocity
  scroller.base = 0;     // screen x coordinate, in 1/16ths pixel
  scroller.limit = limit;
}

static void scroller_run()
{

sTagXY sTagxy;
FTImpl.GetTagXY(sTagxy);

  signed short sy = sTagxy.y;
  
  if ((sy != -32768) & (scroller.dragprev != -32768)) {
    scroller.vel = (scroller.dragprev - sy) << 4;
  } else {
    int change = max(1, abs(scroller.vel) >> 5);
    if (scroller.vel < 0)
      scroller.vel += change;
    if (scroller.vel > 0)
      scroller.vel -= change;
  }
  scroller.dragprev = sy;
  scroller.base += scroller.vel;
  scroller.base = max(0, min(scroller.base, scroller.limit));
}

uint8_t Order_list()
{
	uint8_t dg_count=0,i=0,Key_counts=0;
	uint8_t  z,Read_tag,temp = 0,TagVal,y=0,BHFlag=0;
	int16_t sy = 0,	drag = 0,PlusTag = 101,MinusTag = 201,QtyNo=0,TotPrice=0,TotItems=0,PosY=0,Touch=0,IconCt=0,alpha=255;
	uint16_t yoff = 40;
        char MenuItemname[20];       
sTagXY sTagxy;
        scroller_init((FT_DISPLAYHEIGHT*2)*16);
	do
	{
	FTImpl.GetTagXY(sTagxy);
	sy = sTagxy.y;
		//TagVal = sTagxy.tag;
	TagVal = FTImpl.Read(REG_TOUCH_TAG);
		Read_tag = Read_Keys();
		if(TagVal != 0)	Key_counts++;
		if(sy != -32768)	
		{
      		  dg_count++;     	
      		  temp = Read_tag;      
		}
		/*Move into the particular frame based on dragdt now 30pixels*/
		if(sy==-32768)
		{
		  dg_count = 0;			 
		  drag = scroller.base>>4; 	
		}
		/*if tag in but still pendown take a scroller basevalue*/
		else if(dg_count>15)
		{
		  Read_tag = 0;
		  temp = Read_tag;
		  drag = scroller.base>>4;
		}
                if(drag >=282)drag = 282;
		if(Read_tag !='H' && Read_tag !='B'&& drag <= 282 && Key_counts > 15)
		{
		  scroller_run();
		}
		FTImpl.DLStart();        // start
		FTImpl.SaveContext();	
		FTImpl.Cmd_Gradient(0, 0, 0x000000, 0, 180, 0xFF0000);
		
		yoff = 50;
		yoff-=drag;
			
		TotPrice =0;
                TotItems=0;
                for(z=1,y=1;y<16,z<16;z++)
		{

			FTImpl.ColorRGB(255,255,255);

			if((CheckTag[z]!=0) && (StoreTag[z]>=0))
			{
                                PosY = (yoff+(y-1)*30);
                                strcpy_P(MenuItemname, Item_Property[z].ItemName);

                                FTImpl.Cmd_Text(20,PosY,27,FT_OPT_CENTERY,MenuItemname);
                                
              			if((Read_tag>0)&&(Read_tag == z))
              			{             

              			    FTImpl.PlaySound(255,0x56);
              			    StoreTag[z]++;
              			    Read_tag = 0;
                                    Key_counts = 0;
              			}
              			if((Read_tag>0)&&(Read_tag==z+20))
              			{

                                   FTImpl.PlaySound(255,0x56);
              			    StoreTag[z]--;
              			    Read_tag = 0;
                                    Key_counts = 0;  
              			}
              			if(StoreTag[z] < 0 )StoreTag[z] = 0;
              			FTImpl.Cmd_Number(210,PosY,27,FT_OPT_CENTERY,StoreTag[z]);
                                TotItems+= StoreTag[z];
              

              			FTImpl.TagMask(1);
              			FTImpl.Tag(z);
                                FTImpl.ColorRGB(0,0,0);

                                FTImpl.Vertex2ii(165,PosY-18,30,'+');

              			FTImpl.Tag(z+20);



              			FTImpl.Vertex2ii(245,PosY-25,31,'-');	
              			FTImpl.TagMask(0);
              

              			FTImpl.ColorRGB(255,255,255);
              
                                uint8_t Value = StoreTag[z]* (pgm_read_byte(&Item_Property[z].ItemPriceinVal));
                                FTImpl.Cmd_Number(340,PosY,27,FT_OPT_CENTERY,Value);
                                                              
                                TotPrice+=  Value;

              			y++;

			}				
		}	
                // to reduce points in the cart if the total number of items is less than 15	
		if(TotItems < 15) 
		{
			item_selected_counts = TotItems;
		}



		FTImpl.Cmd_Text(20,yoff+(y-1)*30,29,FT_OPT_CENTERY,"Total");
		FTImpl.Cmd_Number(340,yoff+(y-1)*30,29,FT_OPT_CENTERY,TotPrice);
		FTImpl.ColorRGB(150,150,150);
		FTImpl.Begin(FT_RECTS);
		FTImpl.Vertex2f(10*16,5*16);
		FTImpl.Vertex2f(460*16,30*16);
		FTImpl.ColorRGB(0,0,0);
		FTImpl.Cmd_Text(30,15,27,FT_OPT_CENTERY,"Item");
		FTImpl.Cmd_Text(190,15,27,FT_OPT_CENTERY,"Quantity");
		FTImpl.Cmd_Text(340,15,27,FT_OPT_CENTERY,"Price($)");
		FTImpl.Begin(FT_BITMAPS);
		FTImpl.TagMask(1);
		  FTImpl.ColorA(0);
               FTImpl.Tag('T');
               FTImpl.Cmd_Button(10,200,470,70,16,0,"");
               FTImpl.RestoreContext();
               	FTImpl.TagMask(0);
	      if(Read_tag == 'T'||IconCt ==0)
              {
		   BHFlag =1;
		   alpha = 255;
	      }
		  if(BHFlag == 1)
		  {
		if((alpha>0)&&(IconCt > 150)&&(sy== -32768)) 
			alpha--;
	  	Back_Home(alpha,1);
		   }
      		//FTImpl.Begin(FT_BITMAPS);
		FTImpl.TagMask(1);
		FTImpl.Tag('P');
		FTImpl.Begin(FT_POINTS);
		FTImpl.ColorRGB(0,0,0);
		FTImpl.PointSize(22*16);
	        FTImpl.Vertex2ii(430,243,0,0);
		FTImpl.ColorRGB(255,255,255);
		FTImpl.PointSize(16*16);
	        FTImpl.Vertex2ii(430,243,0,0);
		FTImpl.ColorRGB(0,0,0);
		FTImpl.Cmd_Text(423,242,29,FT_OPT_CENTERY,"$");


		FTImpl.End();
	        FTImpl.TagMask(0);

		if(Read_tag == 'P' && Key_counts > 15)i=1;
		if((i>0)&&( i <100))
		{
			FTImpl.ColorRGB(0,0,0);
			FTImpl.Begin(FT_RECTS);
			FTImpl.Vertex2f(110*16,115*16);
			FTImpl.Vertex2f(410*16,180*16);
			FTImpl.ColorRGB(255,255,255);
			FTImpl.Cmd_Text(120,125,27,FT_OPT_CENTERY,"Please make payment at the counter.");
			FTImpl.Cmd_Text(200,150,27,FT_OPT_CENTERY,"Thank You.");	
		}
		if(i>=100) i =0;
		FTImpl.DLEnd();
		FTImpl.Finish();      
		if(i>=1)i++;
              IconCt++;
		if(Read_tag == 'P'||Read_tag == 255) Read_tag=0;
	}while((Read_tag==0)||(Read_tag < 50 )||Read_tag =='T' );
if(Read_tag == 'H')	 FTImpl.PlaySound(255,0x51);
return Read_tag;
}
uint8_t ItemSelection(uint8_t NoItems,uint8_t start_cell/*char **Menu_items*/)
{ 
        float zinout_f=0;
        uint8_t StCell=0,BHFlag=0;
        int16_t k,points_cdsX[16],points_cdsY[16];
        uint8_t x,rotation_flag=0,Menu=0,temp_Menu=0,Read_tag,CartFlag=0,i,TouchTag=0;
        int16_t prevth=0,currth=0,key_deteted_cts=0,rotation_cts=0,deg = 0,theta,scroller_vel=0,sx=0,Touch,alpha=255,IconCt=0; 
        char buffer[20];
      sTagXY sTagxy;
		sTrackTag sTracktag;
        FTImpl.Cmd_Track(240,0,1,1,1);
        FTImpl.Cmd_Track(240,0,1,1,2);
        FTImpl.Cmd_Track(240,0,1,1,3);
        FTImpl.Cmd_Track(240,0,1,1,4);
        FTImpl.Cmd_Track(240,0,1,1,5);
        // for gradient
        FTImpl.Cmd_Track(240,0,1,1,90);
        FTImpl.Finish();
        deg = 360/6;
        /* Calculate points position to place inside Cart */
  	points_cdsX[0] = 247;
	for(k=1; k<=15;k++)
	{
		  points_cdsX[k] = points_cdsX[k-1] -5;		
		  points_cdsX[0] = 248;
		  points_cdsX[6] = 248;
		  points_cdsX[11] = 248;
	}

	for(k=1; k<=15;k++)
	{
		if(k < 6)	points_cdsY[k] =  68;
		if((k >= 6) && (k < 11))	points_cdsY[k] =  64;
		if((k >= 11) && (k < 16))	points_cdsY[k] =  60;
	}

        StCell = start_cell;
	
	do
	{

			FTImpl.GetTrackTag(sTracktag);
			uint8_t key_detected = sTracktag.tag;
        	 TouchTag = FTImpl.Read( REG_TOUCH_TAG);
			 FTImpl.GetTagXY(sTagxy);
			 Touch = sTagxy.y;			 
			 Read_tag = Read_Keys();

        	 FTImpl.DLStart();        // start
        	 FTImpl.SaveContext();
        	 FTImpl.Cmd_Gradient(0, 0, 0x000000, 0, 180, 0xFF0000);
             /* Background Effect */
        	 FTImpl.Begin(FT_POINTS);
        	 FTImpl.ColorA(50);
        	 FTImpl.ColorRGB(0,0,0);
        	 FTImpl.TagMask(1);
        	 FTImpl.Tag(90);
        	 FTImpl.PointSize(250*16);
        	 FTImpl.Vertex2f((240)*16,-150);
        	 FTImpl.ColorA(60);
        	 FTImpl.ColorRGB(255,0,0);
        	 FTImpl.Tag(91);
        	 FTImpl.PointSize(135*16);
        	 FTImpl.Vertex2f((240)*16,-150);
        	 FTImpl.TagMask(0);
        	 FTImpl.RestoreContext();
	 
	         if(key_pressed > 0 && key_pressed < 20)
	 	    description_no = key_pressed+item_no;
		  
                 if(key_detected>0 && key_detected!='T')
                 { 
                     key_deteted_cts++;
                     if(key_detected<NoItems+1)
	                 temp_Menu = key_detected; 
	             else
		         temp_Menu = 0;
	             Menu = 0;			
	             rotation_cts = 0;
                 }   
                 if(key_detected==0)
                 {	   
                     if(key_deteted_cts>=0&&key_deteted_cts<15)
            	     {
            		if(!rotation_flag)
            		Menu = temp_Menu; else rotation_flag = 0;
            	     }			
                     key_deteted_cts = 0;
        	     if(rotation_cts>100)
        	     {	
        		  //trackval++; 
                          trackval+=0.5;
        		  item_selection=0;
        		  rotation_flag = 1;
        	     }	
        	     else 
		       rotation_cts++;
               }     

              	if(key_deteted_cts>15 && item_selection==0&&TouchTag!=91)
                {  
              		 float tempval;
					//currth = (ssy>>16);
              		 currth = sTracktag.track;
					 tempval = ((currth-prevth));
              		 
              		 if((currth-prevth) > 32000)
              		 {
              			 tempval = 65536 - currth + prevth;
              		 }
              		 else if((currth-prevth) < -32000)
              		 {
              			 tempval = 65536 - prevth + currth;
              			 tempval = -tempval;
              		 }
              		 tempval = (tempval*360)/65536;
              		 rotation_flag = 0;
              	   //currth = (sy>>16)/182;
              
              	       if(prevth!=0)
              	       {
              	          //trackval += (currth-prevth);
              		  trackval += tempval;
              	       }
              		prevth = currth;
              		rotation_cts = 0;
                }
              	else
                {
                      prevth=0;    
                }  
              if(key_detected=='T')
              {
          	alpha = 255;
          	IconCt = 0;
              }	
              if(key_pressed >0 && key_pressed <6)
          		i = key_pressed;
              /* To avoid items being put in cart even after PenUp */
          	if(Touch == -32768)
		i=0;
          
              if(i!=0 && key_pressed == 'S')
          		item_selection=1;
          	else
          		item_selection =0;

              if(item_selection==1 && key_pressed=='S')
              {
          	 item_selection = 0;		 

                  FTImpl.PlaySound(255,0x54);  
          	 if(item_selected_counts<17)
          	 {
          		 StoreTag[description_no-1]++;
          		 CheckTag[description_no-1] =1;
          	         item_selected_counts++;
                         
          	 }
          	 key_pressed = 0;
              }
 
              FTImpl.SaveContext();
              /* Move Images in a circular path */

              FTImpl.Begin(FT_BITMAPS);
              
              start_cell = StCell;
              for(x=1;x<=NoItems;x++)
              {
          
                  float tempval;
		  theta = deg*x;		
		  tempval = deg*x + trackval+deg;
			if(NoItems == 4)
		  theta = (int16_t)((int32_t)tempval%250); 
			else
		  theta = (int16_t)((int32_t)tempval%300); 
         	  int16_t xoff = 200+(int16_t)(190*cos(theta*0.0174));
		  int16_t yoff = -20+(int16_t)(150*sin(theta*0.0174)); 
          	  int16_t zinout = 310- yoff;
                  FTImpl.BitmapHandle(0);              
                  FTImpl.Cell(start_cell);
          	  FTImpl.TagMask(1);
          	  FTImpl.Tag(x);
              FTImpl.BitmapTransformA(zinout);
        	  FTImpl.BitmapTransformE(zinout);
              FTImpl.BitmapSize(FT_BILINEAR, FT_BORDER, FT_BORDER, 256*80/zinout, 256*53/zinout  );



          	  FTImpl.Vertex2f(xoff*16,yoff*16);
          	  FTImpl.TagMask(0);
                  start_cell++;                  
              }
              /* Set the bitmap properties to default, so it doesnt interfere with text display */

              FTImpl.RestoreContext();
          
              for(x=1;x<=NoItems;x++)
              {
		  float tempval1 ;
		  theta = deg*x;		
		  tempval1 = deg*x + trackval+deg;
		  if(NoItems == 4)
		  theta = (int16_t)((int32_t)tempval1%250); 
		  else
			theta = (int16_t)((int32_t)tempval1%300); 
		

		  int16_t xoff = 200+(int16_t)(190*cos(theta*0.0174));
		  int16_t yoff = -20+(int16_t)(150*sin(theta*0.0174)); 
                  
                  zinout_f  =  310-yoff;
 		  zinout_f  =  255/zinout_f;
		  int16_t zinout = 5+yoff+(int16_t)(53*zinout_f);

                  if(item_no ==1)
          	  {
                        strcpy_P(buffer, Array_Beverage_name[x]);                                    
                        FTImpl.Cmd_Text(xoff+5,(zinout),16,FT_OPT_CENTERY,buffer);
                  }
                  else if (item_no == 5)
                  {
                       strcpy_P(buffer, Array_Pastry_name[x]);
                       FTImpl.Cmd_Text(xoff+5,(zinout),16,FT_OPT_CENTERY,buffer);
                  }
                  else if (item_no == 10)
                  {
                        strcpy_P(buffer, Array_Frappe_name[x]);
                        FTImpl.Cmd_Text(xoff+5,(zinout),16,FT_OPT_CENTERY,buffer);
                  }
              }
        	FTImpl.Begin(FT_BITMAPS);
        	FTImpl.TagMask(1);
        	FTImpl.ColorA(0);
  

                FTImpl.Tag('T');
                FTImpl.Cmd_Button(10,200,470,70,16,0,"");


                FTImpl.RestoreContext();
           	FTImpl.TagMask(0);
	        if(Read_tag == 'T'||IconCt ==0)
                 {
		       BHFlag =1;
		       alpha = 255;
		  }
		  if(BHFlag == 1)
		  {
		    if((alpha>0)&&(IconCt > 150)&&(Touch== -32768)) 
			alpha--;
	  	    Back_Home(alpha,0);
		  }
               sx = -10;
              /* When menu option is selected, enable point to move to cart */
              if((temp_Menu >0)&&(temp_Menu < 6))
              {





          	   FTImpl.ColorRGB(0,255,0);
          	   FTImpl.Begin(FT_POINTS);		
          	   FTImpl.PointSize(5*16);
			   FTImpl.GetTagXY(sTagxy);
			   sx = sTagxy.x;
          	   int16_t sy =  sTagxy.y;
          	   if(sx != -32768 && sy != -32768)

          	   FTImpl.Vertex2f(sx*16,sy*16);		
                  
              }
            if(item_selected_counts >=16)item_selected_counts = 15;
           /* place points inside cart according to the number of items selected */
            if(item_selected_counts>0)
            {



          	    FTImpl.ColorRGB(0,255,0);
          	    FTImpl.Begin(FT_POINTS);		
          	    FTImpl.PointSize(3*16);		
          	    for(k=1;k<=item_selected_counts;k++)
          	    {
          	          FTImpl.Vertex2f(points_cdsX[k]*16,points_cdsY[k]*16) ;	 
          	    }
             }
          
              /*  PLace Cart and change cart color when the menu item is selected */

             FTImpl.Begin(FT_BITMAPS); 
             if((temp_Menu >0)&&(temp_Menu < 6))

          	   FTImpl.ColorRGB(255,255,0);  
             else

             FTImpl.ColorRGB(255,255,255);  
             

             FTImpl.Vertex2ii(CartX,40,3,0);  
             
             /* Draw points with only alpha and assign Cart tag 'S' to it. This is done to get proper tag effect when cart is touched */
             FTImpl.Begin(FT_POINTS); 
             FTImpl.PointSize(20*16); 
             FTImpl.ColorA(0);  	 
             FTImpl.TagMask(1);
             FTImpl.Tag('S');
             FTImpl.Vertex2f(230*16,50*16);
             FTImpl.TagMask(0);
             FTImpl.DLEnd();
             FTImpl.Finish();       
             
             /* Move the cart from the corner of the screen to the centre only at the beginning. Then enablr CartFlag to 1.*/
             if((CartX> 215)&& (CartFlag==0)) CartX-=5;
             if(CartX == 215) CartFlag = 1;
             
             /* Cancel Background tag to zero. so that when value  being returned is Menu item value */
             if(Read_tag == 90) Read_tag=0;
             if(key_deteted_cts > 15) 
             {
          	   Menu =0;
          	   temp_Menu = 0;
             }

            if((Read_tag == 'B') || (Read_tag == 'H'))  
            {

               FTImpl.PlaySound(255,0x51);
	       Menu = 'H';
            }
             if(Read_tag == 'O')Menu = 'O'; 
             if(Menu!=0) return Menu;
              IconCt++;
            }while(1); // while((Menu ==0)&&(Menu!='B')&&(Menu != 'O'));
            i=0;// clear the item which has been selected but not dropped in the cart
  return Menu;
}



uint8_t Description(uint8_t description,uint8_t item,uint8_t Noitems)
{
	char MenuItemName[100];
        uint8_t Read_tag = 0,BHFlag=0;

			sTagXY sTagxy;
        uint16_t alpha=255,IconCt=0;
        int16_t Touch;

        do
        {

		FTImpl.GetTagXY(sTagxy);
          Touch = sTagxy.y;
          Read_tag = Read_Keys();
          FTImpl.DLStart();        // start
      	  FTImpl.Clear(1,1,1);	
      	  FTImpl.SaveContext();	
		  FTImpl.BitmapHandle(0);
          FTImpl.Cell(description+1);
    	  FTImpl.BitmapSize(FT_BILINEAR,FT_BORDER,FT_BORDER,imw*2,imh*2);		
    	  FTImpl.Cmd_Gradient(0, 0, 0x000000, 0, 180, 0xFF0000);
  	      FTImpl.Begin(FT_BITMAPS);
  	      FTImpl.BitmapTransformA(128) ;
          FTImpl.BitmapTransformE(128) ;
          FTImpl.Vertex2ii(30,80,0,description+1);
          FTImpl.RestoreContext();
          strcpy_P(MenuItemName, Item_Property[description-1].ItemName);        
          FTImpl.Cmd_Text(30,30,25,0,MenuItemName);	        
          strcpy_P(MenuItemName, Item_Property[description-1].ItemPrice);              
          FTImpl.Cmd_Text(370,85,21,0,MenuItemName);	        
          strcpy_P(MenuItemName, Item_Property[description-1].ItemDescription);              
          FTImpl.Cmd_Text(30,190,18,0,MenuItemName);			
          FTImpl.Begin(FT_BITMAPS);
          FTImpl.TagMask(1);
          FTImpl.ColorA(0);
          FTImpl.Tag('T');
          FTImpl.Cmd_Button(10,200,470,70,16,0,"");
          FTImpl.RestoreContext();
          FTImpl.TagMask(0);
          if(Read_tag == 'T'||IconCt ==0)
          {
           BHFlag =1;
           alpha = 255;
          }
          if(BHFlag == 1)
          {
         	if((alpha>0)&&(IconCt > 150)&&(Touch== -32768)) 
        		alpha--;
          	Back_Home(alpha,0);
           }
          FTImpl.DLEnd();
          FTImpl.Finish();      
          IconCt++;
        }	
        while((Read_tag==0)|| (Read_tag =='T')||(Read_tag  == 255));      
       	if(Read_tag == 'H') FTImpl.PlaySound(255,0x51);
        return Read_tag;
}


void load_inflate_image(uint8_t address,char* name)
{
    uint8_t ibuff[512];    
    FT_SDFile imfile;

	if(FtSd.OpenFile(imfile,name)){
		Serial.print("Unable to open: ");
		Serial.println(name);
	}
	else{
		FTImpl.WriteCmd(CMD_INFLATE);
		FTImpl.WriteCmd(address*1024L);
		while (imfile.Offset < imfile.Size)
		{	
			uint16_t n = min(512, imfile.Size - imfile.Offset);
			n = (n + 3) & ~3;   // force 32-bit alignment
			imfile.ReadSector(ibuff);
			FTImpl.WriteCmd(ibuff, n);//alignment is already taken care by this api
		}
	}
}





uint8_t Menu_prp = 0;
void Logo_Intial_setup(PROGMEM struct logoim sptr[],uint8_t num )
{
   char buffer1[10]; 
   for(uint8_t z=0;z<num;z++)
   {
       uint16_t pc = pgm_read_word(&sptr[z].gram_address);
       prog_char *pc1 = sptr[z].name;
                      while (pgm_read_byte(pc1)) {
                      strcpy_P(buffer1, sptr[z].name);
                      pc1 += (strlen_P(pc1) + 1);
                      }
       load_inflate_image(pc,buffer1);
   }     


   FTImpl.DLStart();        // start

   for(uint8_t z=0;z<num;z++)
   {
           FTImpl.BitmapHandle(z);	
           uint16_t pc = pgm_read_word(&sptr[z].gram_address);                    
           FTImpl.BitmapSource(pc*1024L);    
           uint8_t pc2 = pgm_read_byte(&sptr[z].image_format);
           uint16_t pc3 = pgm_read_word(&sptr[z].linestride);                      
           uint16_t pc4 = pgm_read_word(&sptr[z].image_height);
           FTImpl.BitmapLayout(pc2,pc3,pc4);      
           pc2 = pgm_read_byte(&sptr[z].filter);
           pc3 =pgm_read_word(&sptr[z].sizex);                      
           pc4 = pgm_read_word(&sptr[z].sizey);
           FTImpl.BitmapSize(pc2,FT_BORDER,FT_BORDER,pc3,pc4); 
   }
   
   FTImpl.DLEnd();
   FTImpl.Finish();    
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
     char buffer[20];
	 sTagXY sTagxy;
     Info();
     trackval = 0;
     Logo_Intial_setup(Main_Icons,7);

    while(1)
    {
      uint8_t exit = 0; 
  	  uint8_t Menu = Selection_Menu(3,1); 
        
		do{  
        FTImpl.GetTagXY(sTagxy);
        int16_t Touch = sTagxy.x; 
		item_selection = 0;
        FTImpl.PlaySound(255,0x51);
			do
			{
				FTImpl.DLStart();    
           
				/* Based on main menu return value, choose different submenus */
				switch(Menu)
				{
			
				case 1:			
				FTImpl.DLEnd();
				FTImpl.Finish();     
				item_no =1;
				exit = ItemSelection(4,3); 
				break;
		
				case 2:          	
				FTImpl.DLEnd();
				FTImpl.Finish(); 
				item_no =5;
				exit = ItemSelection(5,7);    
				break;
		
				case 3: 
				FTImpl.DLEnd();
				FTImpl.Finish();       
				item_no =10;
				exit = ItemSelection(4,12); 
				break;	

				}  
			}while(exit==0); 
			
		if((exit >0)&& (exit <6) &&(Touch == -32768))
        {		
            description_no = exit+item_no;
            if(description_no < 80)
			{
                FTImpl.PlaySound(255,0x51);
    	        exit= Description(description_no,exit,item_no);
            }
         } 
         if(exit == 'O') 
         {
             FTImpl.PlaySound(255,0x51);
             exit = Order_list();		
         }
         
		}while(exit!='H');
	}
	FTImpl.Exit();  /*  Safely close all opened handles */
	Serial.println("--End Application--");
	}
}

void loop()
{
}


/* Nothing beyond this */














