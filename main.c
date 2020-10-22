#define F_CPU 			16000000UL

#include <avr/io.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <inttypes.h>
#include "ili9341.h"
#include <avr/eeprom.h>
#include "ili9341gfx.h"
#include "ADC.h"
#include "I2C.h"
#include <avr/pgmspace.h>
#include <util/delay.h>

#define LED_ROT_ON		PORTB |= (1<<PB2)
#define LED_ROT_OFF		PORTB &= ~(1<<PB2)
#define LED_GRUN_ON		PORTB |= (1<<PB3)
#define LED_GRUN_OFF	PORTB &= ~(1<<PB3)
#define TRANSISTOR_OFF	PORTC &= ~(1<<PC7)
#define TRANSISTOR_ON	PORTC |= (1<<PC7)
#define PIEZZO_OFF		PORTC &= ~(1<<PC6)
#define PIEZZO_ON		PORTC |= (1<<PC6)
#define LED_BAT_ON		PORTD |= (1<<PD0)
#define LED_BAT_OFF		PORTD &= ~(1<<PD0)

extern uint16_t _width ;
extern uint16_t _height;

uint16_t KA = 0;
uint16_t KW = 0;
uint16_t Jahr = 0;
uint16_t YEAR = 0;
uint8_t	 background = 0;	
uint8_t	 piezo;				// Lautsprecher ein-/ausschalten (PIEZO-Summer)

////////////////////////////////////////////////////////////////////////

static FILE mydata = FDEV_SETUP_STREAM(ili9341_putchar_printf, NULL, _FDEV_SETUP_WRITE);

char string[10]={0};

////////////////////////////////////////////////////////////////////////

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
  printf("%s",buf);
}

// draw sunken box ala windows 
void draw_box(int x,int y , int sx, int sy, int len , int flag)
{
  int lcolor1=(flag==1)?LCD_RGB(0x70,0x70,0x70):LCD_RGB(0xf0,0xf0,0xf0);
  int lcolor2=(flag==1)?LCD_RGB(0xf0,0xf0,0xf0):LCD_RGB(0x70,0x70,0x70);
  int a;
  
  ili9341_fillrect(x+len,y+len,sx-(2*len),sy-(2*len),(flag==1)?LCD_RGB(0x50,0x50,0x50):LCD_RGB(0x80,0x80,0x80));
  for(a=0;a<=len;a++)
    {
      ili9341_drawhline(x+a,y+a,sx-(2*a),lcolor1);
      ili9341_drawhline(x+a,(y+sy)-a,sx-(2*a),lcolor2);
      ili9341_drawvline(x+a,y+a,sy-(2*a),lcolor1);
      ili9341_drawvline((x+sx)-a,y+a,sy-(2*a),lcolor2);
    }
}

void draw_button(int x, int y, int sx, int sy, int len ,int flag, char * text)
{
 draw_box(x,y,sx,sy,len,flag);
 print_at_lcd(x+len+15, y+len + (sy/2) - 4, (flag==1)?LCD_RGB(255,255,255):LCD_RGB(0,0,0),(flag==1)?LCD_RGB(0x50,0x50,0x50):LCD_RGB(0x80,0x80,0x80),1,text);
}

long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void draw_progress(int x, int y, int sx, int sy, int len, int prog)
{
 int a,mlen=sx-(2*len)-6;

 draw_box(x,y,sx,sy,len,0);
 for(a=0;a<(sy-(2*len)-6);a++)
  {
   ili9341_drawhline(x+len+3,y+len+3+a,map(prog,0,100,0,mlen),LCD_RGB(0xf0,0,0)); 
  }
}

//////////////////////////////////////Hauptprogramm///////////////////////////////////////

int main(void)
{
	DDRD |= (1<<PD5);													//
	PORTD |= (1<<PD5);													// LED Display
	
	DDRB |= (1<<PB4);													//
	PORTB &= ~(1<<PB4);													// SD-Karte; CS
	
	DDRD |= (1<<PD0);													//
	PORTD &= ~(1<<PD0);													// LED Bat
	
	DDRB |= (1<<PB2);													//
	PORTB &= ~(1<<PB2);													// LED Rot
	
	DDRB |= (1<<PB3);													//
	PORTB &= ~(1<<PB3);													// LED Grün
	
	DDRC |= (1<<PC7);													//
	PORTC &= ~(1<<PC7);													// Transistor
	
	DDRC |= (1<<PC6);													//
	PORTC &= ~(1<<PC6);													// Piezzo
	
	DDRC &= ~(1<<PC4);													//
	PORTC |= (1<<PC4);													// ENTER-Taster
	
	DDRC &= ~(1<<PC3);													//
	PORTC |= (1<<PC3);													// DOWN-Taster
	
	DDRC &= ~(1<<PC5);													//
	PORTC |= (1<<PC5);													// UP-Taster
	
	DDRA &= ~(1<<PA2);													//
	PORTA &= ~(1<<PA2);													// Taster im Adapter
	
	DDRA &= ~(1<<PA1);													//
	PORTA &= ~(1<<PA1);													// Adaptererkennung
	
	DDRD |= (1<<PD4);													//
	PORTD &= ~(1<<PD4);													// Digitales Poti aktivieren
	
	stdout = &mydata;
	ili9341_init();														//initial driver setup to drive ili9341
	ili9341_clear(BLACK);												//fill screen with black colour
	//_delay_ms(1000);
	ili9341_setRotation(3);												//rotate screen
	_delay_ms(2);

////////////////////////////////////////////////////////////////////////

	TRANSISTOR_OFF;															//Ton ausschalten / nur beim Programmieren notwenig
	piezo = eeprom_read_word((uint16_t *) 4);
	
////////////////////////////////////////////////////////////////////////						
							
	ili9341_settextcolour(YELLOW,LCD_RGB(16,16,16));
	ili9341_settextsize(2.4);

	ili9341_fillRect(0,0,_width,_height,LCD_RGB(0x0,0x0,0x0));
	
	ili9341_setcursor(130,180);
	printf("RUAG Schweiz AG");

////////////////////////////////////////////////////////////////////////
//////////////////////////Endlosschleife//////////////////////////////////////////////
	
	while(1)  
	{	
		int fc[]={CYAN,BLACK,BLACK,BLACK};
		int bg[]={BLACK,GREEN,RED,CYAN};
		KA = eeprom_read_word((uint16_t*)8);
		   
		print_at_lcd(10,220,fc[background],bg[background],1,"KW%d %d\n", KA);
		YEAR = eeprom_read_word((uint16_t*)12);							  
		print_at_lcd(205,220,fc[background],bg[background],2,"Jahr:%d\n",YEAR);


		//draw_box(10,30,150,40,2,0);
		//draw_box(10,80,150,40,2,1);
		draw_button(10, 10 , 150 , 40 , 2 , 0 , "Hallo Lucas ");
		draw_button(10, 60 , 150 , 40 , 2 , 1 , "Was meinst? ");
		for(int a=0;a<100;a++)
			draw_progress(10,110,150,40,2,a);
		for(;;);
	}
}
