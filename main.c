/*
	Author: Justin Rhyner, Lukas Rietman, Michael Pluess, Lucas Pollak
	Date:	25.11.2020
	Location: Duebendorf

	Description:
	This code is used to control the hardware of the "Batterietestgeraet". This device meassures the
	voltage of a batterypack with or without load. All configuration data like loadcurrent is
	taken from an SD-card.

	Libary documentation:
	Fatfs:				http://elm-chan.org/fsw/ff/00index_e.html
	Uart.c/uart.h:		http://www.roland-riegel.de
	ili9341 Display:	https://www.avrfreaks.net/projects/ili9341-library-drive-22-tft-displayderived-adafruit-tft-library-ili9340-type-controller
*/
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include "ili9341.h"
#include "ili9341gfx.h"
#include "adc.h"
#include "uart.h"
#include "ff.h"
#include "Stop.h"
#include "Achtung.h"
#include "Volt.h"
#include "Gear.h"

//define textsize of the display
#define TEXTSIZE 2

//Bitmap for RUAG logo
#define logo_width 100
#define logo_height 23
static const unsigned char logo_bits[] PROGMEM = {
   0x55, 0x01, 0x41, 0x01, 0x15, 0x40, 0x15, 0x00, 0x00, 0x00, 0x50, 0x55,
   0x05, 0x22, 0x02, 0x02, 0x02, 0x22, 0x20, 0x22, 0x00, 0x00, 0x00, 0x20,
   0x22, 0x0a, 0x55, 0x05, 0x41, 0x01, 0x15, 0x50, 0x15, 0x00, 0x00, 0x18,
   0x50, 0x55, 0x0d, 0x00, 0x88, 0x80, 0x80, 0x08, 0x08, 0x00, 0x00, 0x00,
   0x08, 0x80, 0x88, 0x0a, 0x05, 0x04, 0x41, 0x41, 0x51, 0x50, 0x00, 0x00,
   0x00, 0x16, 0x50, 0x55, 0x07, 0x22, 0x02, 0x02, 0x02, 0x20, 0x20, 0x00,
   0x00, 0x00, 0x0a, 0x20, 0xa2, 0x0a, 0x45, 0x05, 0x41, 0x41, 0x51, 0x50,
   0x10, 0x00, 0x80, 0x15, 0x50, 0xd5, 0x0d, 0x80, 0x80, 0x80, 0x80, 0x80,
   0x00, 0x08, 0x00, 0x80, 0x0a, 0x80, 0xa8, 0x0a, 0x45, 0x01, 0x45, 0x41,
   0x40, 0x50, 0x10, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x07, 0x02, 0x02, 0x22,
   0x20, 0x20, 0x20, 0x22, 0x00, 0x00, 0x00, 0x00, 0xa0, 0x02, 0x05, 0x05,
   0x55, 0x51, 0x40, 0x41, 0x15, 0x00, 0x18, 0x00, 0x00, 0xd0, 0x01, 0x00,
   0x00, 0x08, 0x00, 0x80, 0x80, 0x08, 0x00, 0x08, 0x00, 0x00, 0xa0, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x70,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00,
   0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x15, 0x00,
   0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xaa,
   0x0a, 0xa0, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60,
   0xff, 0x1f, 0xf0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0xa0, 0xba, 0x1b, 0xa0, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0xc0, 0xfd, 0x1f, 0xd0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0xa0, 0xae, 0x0e, 0xa0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0xe0, 0xff, 0x1f, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0xa0, 0xbb, 0x1b, 0x20, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xff, 0x1f, 0x10, 0x00, 0x00 };

//gray background for display
#define BACK_GRAY (LCD_RGB(32,32,32))

#define LED_ORA_ON      PORTB|= (1<<PB1)
#define LED_ORA_OFF		PORTB &= ~(1<<PB1)
#define LED_ROT_ON		PORTB |= (1<<PB2)
#define LED_ROT_OFF		PORTB &= ~(1<<PB2)
#define LED_GRUN_ON		PORTB |= (1<<PB3)
#define LED_GRUN_OFF	PORTB &= ~(1<<PB3)

#define TRANSISTOR_OFF	PORTC &= ~(1<<PC7)
#define TRANSISTOR_ON	PORTC |= (1<<PC7)

#define PIEZZO_OFF		PORTC &= ~((1<<PC6)| (1<<PC7))
#define PIEZZO_ON		PORTC |= ((1<<PC6) | (1<<PC7))
#define LED_BAT_ON		PORTD |= (1<<PD0)
#define LED_BAT_OFF		PORTD &= ~(1<<PD0)

#define linearOffset 0		//Offset in mA
#define gainOffset 260		//Gainoffset for current measurement

//dimensions of display
extern uint16_t _width ;
extern uint16_t _height;

//pwm variables
unsigned short pwm = 0;
unsigned short pwm_offset=150;

unsigned short tst_voltage=12000; 	// default test voltage in mV
unsigned short tst_cur=50;    		// default test current in mA 
unsigned char piezo=0;     			// PIEZO-speaker

// if >0 show debug infos @serial
unsigned char debug=1;    			

//Date variables
uint16_t KW = 0;
uint16_t YEAR = 0;

char *user_name;


////////////////////////////////////////////////////////////////////////

static FILE lcd_out = FDEV_SETUP_STREAM(ili9341_putchar_printf, NULL, _FDEV_SETUP_WRITE);
static FILE mydata2 = FDEV_SETUP_STREAM(uart_putchar_printf, NULL, _FDEV_SETUP_WRITE);

////////////////////////////////////////////////////////////////////////

void messInit(void )
{
	ADC_init(0x18);				//AD0 und AD1 einschalten
//----PWM Init------------------------------------
	TCCR2A = 0x23;				//PWM initialisierung
	TCCR2B = 0x01;				//Datenblatt: s.131
//------------------------------------------------
}

//sets loadcurrent and reads the batteryvoltage
unsigned short setLast_readVolt(unsigned short sollStrom)
{
  unsigned int volt, cur;
  unsigned int adc1,adc2;
  int a,i;

  pwm=(sollStrom/3) + pwm_offset;
  _delay_ms(100);

  adc1=read_ADC(3);
  if(adc1>0) volt=(adc1*29)+170;
  
  adc2=read_ADC(4);
  if(adc2 >0) cur= (adc2*3200/1000)+13;

  OCR2B=(pwm);
  _delay_ms(200);
  for(a=0;a<15;a++){
    cur=0;
    for(i=0;i<10;i++) {
      adc2=read_ADC(4);
      if(adc2 >0) cur+= ((long)adc2*3200/1000)+13; else cur+=0;
    }
    cur/=10;
    
	if((cur+1) < sollStrom) pwm ++;
	if((cur-1) > sollStrom) pwm --;
	OCR2B=pwm;
	_delay_ms(200);   //stabilize pwm and current meassure
	if(debug > 0 ) printf("%s: adc1: %d adc2: %d volt: %d cur: %d pwm %d porta %x\n",__FUNCTION__,adc1,adc2,volt,cur, pwm,PINA );
  }

  adc1=read_ADC(3);
  if(adc1>0) volt=(adc1*29)+170; else volt =0;

  OCR2B=pwm=0;
  
  return volt;
}

//prints a variabel to the display
void print_at_lcd(int x, int y, int fc, int bc, int fs, const char * fmt, ...)
{
  va_list args;
  char buf[80];
  
  ili9341_setcursor(x, y);									//
  ili9341_settextcolour(fc,bc);							//
  ili9341_settextsize(fs);
  va_start(args, fmt);
  vsnprintf(buf,80,fmt,args);
  va_end(args);
  fprintf(&lcd_out,"%s",buf);
}

// draw sunken box ala windows 
void draw_box(int x,int y , int sx, int sy, int len , int flag)
{
  int lcolor1=(flag==1)?LCD_RGB(0x70,0x70,0x70):LCD_RGB(0xf0,0xf0,0xf0);
  int lcolor2=(flag==1)?LCD_RGB(0xf0,0xf0,0xf0):LCD_RGB(0x70,0x70,0x70);
  int a;
  
  ili9341_fillrect(x+len,y+len,sx-(2*len),sy-(2*len),(flag==1)?LCD_RGB(0x50,0x50,0x50):LCD_RGB(0x80,0x80,0x80));
  for(a=0;a<=len;a++){
   ili9341_drawhline(x+a,y+a,sx-(2*a),lcolor1);
   ili9341_drawhline(x+a,(y+sy)-a,sx-(2*a),lcolor2);
   ili9341_drawvline(x+a,y+a,sy-(2*a),lcolor1);
   ili9341_drawvline((x+sx)-a,y+a,sy-(2*a),lcolor2);
  }
}

//draw button on the display
void draw_button(int x, int y, int sx, int sy, int len ,int flag, char * text)
{
 draw_box(x,y,sx,sy,len,flag);
 print_at_lcd(x+len+15, y+len + (sy/2) - 4, (flag==1)?LCD_RGB(255,255,255):LCD_RGB(0,0,0),(flag==1)?LCD_RGB(0x50,0x50,0x50):LCD_RGB(0x80,0x80,0x80),TEXTSIZE,text);
}

//draw meassagebox on display
void draw_msg(int x, int y, int sx, int sy, int flag, int which, char * text)
{
  int w,h;
  const unsigned char * pics;

  switch (which){
   case 0:
    w=volt_width; h=volt_height; pics=volt_bits;
   break;
   case 1:
    w=Stop_width; h=Stop_height; pics=Stop_bits;
   break;
   case 2:
   default:
    w=Achtung_width; h=Achtung_height; pics=Achtung_bits;
   break;
   case 3:
    w=Gear_width; h=Gear_height; pics=Gear_bits;
   break; 

  }
  draw_box(x,y,sx,sy,3,flag);
  ili9341_drawXBitmap(x+10,y+((sy/2) - (h/2)),pics,w,h, (flag==1)?LCD_RGB(255,255,255):LCD_RGB(0,0,0));
  print_at_lcd(x+w+15, y+((sy/2)- (h/2)),  (flag==1)?LCD_RGB(255,255,255):LCD_RGB(0,0,0),(flag==1)?LCD_RGB(0x50,0x50,0x50):LCD_RGB(0x80,0x80,0x80),TEXTSIZE,text );
}

//see:	https://www.arduino.cc/reference/en/language/functions/math/map/
long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//draw progressbar on display
void draw_progress(int x, int y, int sx, int sy, int len, int prog)
{
 int a,mlen=sx-(2*len)-6;

 draw_box(x,y,sx,sy,len,0);
 for(a=0;a<(sy-(2*len)-6);a++)
 {
  ili9341_drawhline(x+len+3,y+len+3+a,map(prog,0,100,0,mlen),LCD_RGB(0xf0,0,0));
 }
}

//set backgroundcolour
void draw_back(int bc)
{
 ili9341_clear(bc);
 ili9341_drawXBitmap(ILI9341_TFTWIDTH-logo_width-20,20,logo_bits,logo_width,logo_height,LCD_RGB(255,255,255));
}

volatile UINT Timer;	/* Performance timer (100Hz increment) */

ISR(TIMER0_COMPA_vect)
{
	Timer++;			/* Performance counter for this module */
	disk_timerproc();	/* Drive timer procedure of low level disk I/O module */
}


/* 
Read the "read.ini" file, with the configuration for the tester 
return 0 if everything was ok, otherwise -1 for file problems, -2 Format error, -3 missing fields

read.ini file has follow structure :
"variable" = "value"

"variable" is a predefined variable, "value" is an assosiated value with the variable, such as "year" current year, "week" current week number. 
lines starting with "#" are ignored (comment)

Following variable are defined :
year = [integer number]
week = [integer number]
name = [string]
pwm_offset = [integer number]
*/
int read_ini()
{
  FIL file ;
  int stat;
  char line[80];
  char cmd[16];
  char var[40];
  int st;
  int tst=0;

  stat=f_open(&file,"0:read.ini",FA_READ);
  printf("%s: open returned %d",__FUNCTION__,stat);
  if (stat != FR_OK) return -1;
  while(f_gets(line,80,&file) )
  {
   if(line[0] == 0) continue; // probably newline
   if(line[0] == '#') continue;
   st=sscanf(line,"%s = %16[^\n]",cmd,var) ;
   printf("%s: %s %s scanf %d\n\r",__FUNCTION__,cmd,var,st);
   if(st != 2) return -2; // Format Error
   if(strcmp(cmd,"week")==0) { KW=atoi(var); tst|= 1; } // flag for week
   if(strcmp(cmd,"year")==0) { YEAR=atoi(var); tst|=2; } // flag for year
   if(strcmp(cmd,"name")==0) { user_name=strdup(var); tst|= 4; } // flag for name
   if(strcmp(cmd,"pwm_offset")==0) pwm_offset=atoi(var);
   if(strcmp(cmd,"piezo")==0) piezo=atoi(var);
   if(strcmp(cmd,"debug")==0) debug=atoi(var);
   if(strcmp(cmd,"voltage")==0) { tst_voltage=atoi(var); tst|=8; }
   if(strcmp(cmd,"current")==0) { tst_cur=atoi(var); tst|=16; } 
  }
  
  f_close(&file);
  if(tst != 31 ) return -3; // missing fields
  else
    return 0;
}

//write result of meassurement to sd-card
int write_res(char *name, unsigned short cur, unsigned short volt, char *txt )
{
   FIL file ;
   int stat;
   char fname[16];
   char line[80];
   int len;
  
   printf("%s2 open returned %d \n\r",__FUNCTION__,stat);
   stat=f_open(&file, name,FA_OPEN_APPEND | FA_WRITE );
   printf("%s3 open returned %d \n\r",__FUNCTION__,stat);
   if(stat != FR_OK) return -1;
   //f_printf(&file,"%s;%d;%d;%d;%d\n\r",user_name,YEAR,KW,cur,volt);
   sprintf(line,"%s;%d;%d;%d;%d;%s\n",user_name,YEAR,KW,cur,volt,txt);
   f_write(&file,line,strlen(line),&len);
   printf("%s4 open returned %d \n\r",__FUNCTION__,stat);
   f_sync(&file);
   printf("%s5 open returned %d \n\r",__FUNCTION__,stat);
   f_close(&file);
   printf("%s6 open returned %d \n\r",__FUNCTION__,stat);

   return 0;
}


//////////////////////////////////////Hauptprogramm///////////////////////////////////////

int main(void)
{
	char output[20];
	unsigned short volt = 0;
	
    FRESULT fr;
    FATFS fs;
    FIL fil;
    int stat;
  
	DDRD |= (1<<PD5);
	PORTD |= (1<<PD5);			//LED Display
	
	DDRB |= (1<<PB4);
	PORTB |= (1<<PB4);			//SD-Karte; CS
	
	DDRD |= (1<<PD0);
	PORTD &= ~(1<<PD0);			//LED Bat
	
	DDRB |= (1<<PB2);
	PORTB &= ~(1<<PB2);			//LED Rot
	
	DDRB |= (1<<PB3);
	PORTB &= ~(1<<PB3);			//LED Grün
	
	DDRC |= (1<<PC7);
	PORTC &= ~(1<<PC7);			//Transistor
	
	DDRC |= (1<<PC6);
	PORTC &= ~(1<<PC6);			//Piezzo
	
	DDRC &= ~(1<<PC4);
	PORTC |= (1<<PC4);			//ENTER-Taster
	
	DDRC &= ~(1<<PC3);
	PORTC |= (1<<PC3);			//DOWN-Taster
	
	DDRC &= ~(1<<PC5);
	PORTC |= (1<<PC5);			//UP-Taster
	
	DDRA &= ~(1<<PA2);
	PORTA &= ~(1<<PA2);			//Taster im Adapter
	
	DDRA &= ~(1<<PA1);
	PORTA &= ~(1<<PA1);			//Adaptererkennung
	
	DDRD |= (1<<PD6);

	DDRD |= (1<<PD1);
	//^tx
	
	/* Start 100Hz system timer with TC0 */
	OCR0A = F_CPU / 1024 / 100 - 1;
	TCCR0A = _BV(WGM01);
	TCCR0B = 0b101;
	TIMSK0 = _BV(OCIE0A);

	//enable interupt
	sei();

	uart_init();

	stdout = &mydata2;
	printf("Booting..");
	messInit();

	//Display init
	ili9341_init();						//initial driver setup to drive ili9341
	ili9341_clear(BACK_GRAY);			//fill screen with black colour
	//_delay_ms(1000);
	ili9341_setRotation(3);				//rotate screen
	_delay_ms(2);

	

////////////////////////////////////////////////////////////////////////

	TRANSISTOR_OFF;						//Ton ausschalten / nur beim Programmieren notwenig					

	draw_back(BACK_GRAY);

	draw_msg((ILI9341_TFTWIDTH/2) -(250/2),90,250,50,3,3,"Booting..");

	f_mount(&fs, "", 0);
	_delay_ms(500);

//try to read SD
sd_read:
	stat=read_ini();
	if(stat < 0) {

		if(stat == -1) draw_msg((ILI9341_TFTWIDTH/2) -(250/2),90,250,50,1,1,"SD File Missing");
		if(stat == -2) draw_msg((ILI9341_TFTWIDTH/2) -(250/2),90,250,50,1,1,"File Error");
		if(stat == -3) draw_msg((ILI9341_TFTWIDTH/2) -(250/2),90,250,50,1,1,"File Incomplete");

		do{
		 if((PINC & 16) == 0 ) goto sd_read;
		}
		while(1);
	}

//////////////////////////Endlosschleife////////////////////////////////
	
	  
	while(1)  
	{	
	  draw_back(BACK_GRAY);
	  print_at_lcd(40,220,WHITE, BACK_GRAY ,1, "User:%s KW%d Jahr:%d\n",user_name, KW, YEAR);


//test if Messadapter is connected
back:
	  if((PINA & 2 ) != 2)
	  {
	    draw_msg((ILI9341_TFTWIDTH/2) -(250/2),90,150,50,1,1,"Missing Adapter");
	    if((PINC & 16) == 16)
		goto back;
	  }

	  //Wait until battery is inserted
	  if ((PINA & 4) != 4)
	  {
	    draw_msg((ILI9341_TFTWIDTH/2) -(250/2),90,250,50,1,2,"Insert Battery");
	    while((PINA &4 ) != 4);  
      }

	  //Test battery
	  _delay_ms(30); // switch prellung
	  draw_msg((ILI9341_TFTWIDTH/2) -(250/2),90,250,50,1,0,"Test Battery");
	  LED_ORA_ON;
	  volt = setLast_readVolt(tst_cur);
	  write_res("0:Res.csv",tst_cur,volt,volt<tst_voltage?"Failed":"Passed");
      LED_ORA_OFF;	
		
	  //Battery not ok or ok
	  if(volt < tst_voltage)
	  {
		LED_ROT_ON;
		draw_back(RED);
		sprintf(output, "%2d.%03d V FAILED", volt/1000,(volt%1000));
		draw_msg((ILI9341_TFTWIDTH/2) -(250/2),90,250,50,1,1,output);
		for(int x=0;x<30;x++)
		 {
		  _delay_ms(100);
		  if(piezo == 1){
		   if(x&1) PIEZZO_ON;
		   else PIEZZO_OFF;
		  }
		 }
	  }
	  else{
	   LED_GRUN_ON;
	  
	   if(piezo ==1 ) PIEZZO_ON;
	   draw_back(GREEN);
	   sprintf(output, "%2d.%03d V PASSED", volt/1000,(volt%1000));
	   draw_msg((ILI9341_TFTWIDTH/2) -(250/2),90,250,50,1,0,output);
	   _delay_ms(3000);
	  }
		
	  //Turn off LED output
	  LED_ROT_OFF; LED_GRUN_OFF; PIEZZO_OFF;
	  while ((PINA & 4) == 4); //wait until pack is out
	}
}
