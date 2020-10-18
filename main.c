///////////////////////Einbinden der Biblotheken////////////////////////
#define F_CPU 			16000000UL

#include <avr/io.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <inttypes.h>
#include "ili9341.h"													//Displaybiblitheke
#include <avr/eeprom.h>													//eeprombiblitheke
#include "ili9341gfx.h"													//Displayschriftartenbiblitheke
#include "ADC.h"														//Analog/Digital Wandlerbiblitheke
#include "I2C.h"														//I2C Schnittstellenbiblitheke
#include <avr/pgmspace.h>
#include <util/delay.h>													//Verzögerungsbiblitheke





extern uint16_t _width ;
extern uint16_t _height;
////////////////////////////////////////////////////////////////////////
////////////////////////Definitionen machen/////////////////////////////



#define POINTCOLOUR 	PINK
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
#define TST_UP			!(PINC&(1<<PC5))
#define TST_DOWN		!(PINC&(1<<PC3))
#define TST_ENTER		!(PINC&(1<<PC4))
#define TST_BAT 		(PINA&(1<<PA2))									//Taster im Adapter
#define TST_ADA			(PINA&(1<<PA1))									//Adapptererkennung
#define TST_PASSWORT	(TST_DOWN && TST_UP)							//Tastenkombination für zurücksetzung des Passwortes
#define	zeile_x1		59
#define	zeile_y1		57
#define	zeile_x2		59
#define	zeile_y2		101
#define	zeile_x3		86
#define	zeile_y3  		145
#define	zeile_x4		104
#define	zeile_y4		189
#define	code_x1			112
#define	code_y1			97
#define	code_x2			148
#define	code_y2			97
#define	code_x3			184
#define	code_y3			97
#define	code_x4			112
#define	code_y4			129
#define	code_x5			148
#define	code_y5			129
#define	code_x6			184
#define	code_y6			129
#define	code_x7			112
#define	code_y7			162
#define	code_x8			148
#define	code_y8			162
#define	code_x9			184
#define	code_y9			162
#define	code_xd			112
#define	code_yd			195
#define	code_x0			148
#define	code_y0			195


////////////////////////////////////////////////////////////////////////
///////////////////////////////Variablen////////////////////////////////


//////////////Flankenerkennung/////////////
uint8_t ENTER = 0;
uint8_t UP = 0;
uint8_t DOWN = 0;
//////////////////////////////////////////


/////////////Rechteck zeichnen////////////
uint8_t  x = 0;		// X-Koordinate
uint8_t  y = 0;		// Y-Koordinate
uint16_t w = 0;		// Breite des Rechteckes
uint8_t  h = 0;		// Höhe des Rechteckes
//////////////////////////////////////////


////////////Durchgangszähler ///////////// (wieso soviele????)
uint8_t  Durchgang1 = 0;	//
uint8_t  Durchgang3 = 0;	//
uint8_t  Durchgang4 = 0;	//
uint8_t  Durchgang5 = 0;	// 
//////////////////////////////////////////


/////////////////Passwort/////////////////
uint16_t Passwort = 0;		//
uint16_t Stelle = 1000;		//
uint16_t ZwischenP = 0;		//
uint8_t  Status;			//
uint8_t  Pass = 20;			//
const uint16_t DefaultP = 2019;		// Default Passwort
uint16_t RichtigesP = 0;		// Richtiges Passwort
uint8_t  XPosition = 110;	// Cursor-Position
//////////////////////////////////////////


/////////Tastatursymbole-Position/////////
uint8_t  Delete = 27;	// Character aus ASCII-Tabelle (Delete-Taste)
uint8_t  Enter = 26;	// Character aus ASCII-Tabelle (Enter-Taste)
uint8_t  Plus = 43;		// Character aus ASCII-Tabelle (Plus-Taste)
uint8_t  Minus = 45;	// Character aus ASCII-Tabelle (Minus-Taste)
//////////////////////////////////////////


//////////////////Adapter/////////////////
uint8_t  adapter = 0;	//
uint8_t  warten = 0;	// 
//////////////////////////////////////////


/////////////Spannungskonfig//////////////
uint16_t einer = 0;						//
uint16_t nachkomma = 0;					//
uint16_t Messspannung;					//
uint8_t	 Messspannung_vor_Komma = 1;	//
uint8_t	 Messspannung_nach_Komma = 0;	//
uint16_t Mili_voltage = 1000;			// 
//////////////////////////////////////////


////////////////Stromkonfig///////////////
uint8_t  Messstrom = 10;				//
int8_t	 Positiv_Current_vor_Komma = 0;	//
int8_t	 Positiv_Current_nach_Komma = 0;//
int8_t	 Negativ_Current_vor_Komma = 0;	//
int8_t	 Negativ_Current_nach_Komma = 0;//
int16_t  Positiv_Mili_Current = 0;		//
int16_t  Negativ_Mili_Current = 0;		//
uint16_t strom_o = 0;					//
uint16_t strom_u = 0;					//
//////////////////////////////////////////


/////////////Datumverwaltung//////////////
uint16_t KA = 0;			// 
uint16_t KW = 0;			//
uint16_t Jahr = 0;			// 
uint16_t YEAR = 0;			// 
uint8_t  Zahl_1 = 0;		// Tausender-Zahlenstelle (z.B; 2...)
uint8_t  Zahl_2 = 0;		// Hunterter-Zahlenstelle (z.B; 20..)
uint8_t  Zahl_3 = 0;		// Zehner-Zahlenstelle (z.B; 201.)
uint8_t  Zahl_4 = 0;		// Einer-Zahlenstelle (z.B; 2019)
uint8_t	 background = 0;	// Hintergrundfarbe erkennen, sodass Datum nicht mit derselben Farbe geschrieben wird
//////////////////////////////////////////


/////////////Messungs-Anzeige/////////////
uint8_t  Background_Gruen = 0;	// Gruener Hintergrund schreiben (wenn Batteriepack in Ordnung)
uint8_t  Background_Rot = 1;	// Roter Hintergrund schreiben (wenn Batteriepack nicht in Ordnung)
uint8_t  smileok = 2;			// Lachender Smile ausgeben
uint8_t  smileerror = 1;		// Trauriger Smile ausgeben
uint8_t	 piezo;					// Lautsprecher ein-/ausschalten (PIEZO-Summer)
uint8_t  error_cntr = 0;		// Counter für Blinken der Erroranzeige
//////////////////////////////////////////


///////////////Tasten-Zähler//////////////
uint16_t cntr_pass = 0;		// Wielange wurden Taster gedrückt
//////////////////////////////////////////


///////////Switch-Case-Variablen//////////
uint8_t Zustand;
uint8_t Zustand_Konfig = 1;
uint8_t Zustand_Neu = 1;
uint16_t Zustand_Neu_Name = 65;
uint8_t Zustand_Produktionsdatum = 1;
uint8_t Zustand_Produktionsjahr = 1;
uint8_t Zustand_Kalenderwoche = 1;
uint8_t Zustand_Return_Datum = 1;
uint8_t Zustand_Return = 1;
uint8_t Zustand_Last = 1;
uint8_t stopp = 0;
uint8_t Konfiguration_NR;
//////////////////////////////////////////


uint16_t cntr_menue = 0;
uint8_t  bat = 0;
uint16_t battery = 0;
uint32_t i = 0, ok = 0, error = 0, spannung = 0;
uint8_t  start = 0;
uint8_t  Schreiben = 0;
uint16_t cntr_Name = 115;
uint16_t Dateiname[9];
uint8_t  a = 0;
uint8_t	 loeschen = 0;
uint8_t  abgeschlossen = 0;


uint16_t Ausgabe;
uint8_t  Namen_ausgeben = 0;

uint8_t Name_Nr = 0;

char Testausgabe[2] = {'A'};

char Name_1[10];
char Name_2[10];
char Name_3[10];
char Name_4[10];
char Name_5[10];
char Name_6[10];
char Name_7[10];
char Name_8[10];
char Name_9[10];
char Name_10[10];
char Name_11[10];
char Name_12[10];
char Name_13[10];
char Name_14[10];
char Name_15[10];
char Name_16[10];
char Name_17[10];
char Name_18[10];
char Name_19[10];
char Name_20[10];

char N1_B1;
char N1_B2;
char N1_B3;
char N1_B4;
char N1_B5;
char N1_B6;
char N1_B7;
char N1_B8;
char N1_B9;
char N1_B10;

char N2_B1;
char N2_B2;
char N2_B3;
char N2_B4;
char N2_B5;
char N2_B6;
char N2_B7;
char N2_B8;
char N2_B9;
char N2_B10;

char N3_B1;
char N3_B2;
char N3_B3;
char N3_B4;
char N3_B5;
char N3_B6;
char N3_B7;
char N3_B8;
char N3_B9;
char N3_B10;

char N4_B1;
char N4_B2;
char N4_B3;
char N4_B4;
char N4_B5;
char N4_B6;
char N4_B7;
char N4_B8;
char N4_B9;
char N4_B10;

char N5_B1;
char N5_B2;
char N5_B3;
char N5_B4;
char N5_B5;
char N5_B6;
char N5_B7;
char N5_B8;
char N5_B9;
char N5_B10;

char N6_B1;
char N6_B2;
char N6_B3;
char N6_B4;
char N6_B5;
char N6_B6;
char N6_B7;
char N6_B8;
char N6_B9;
char N6_B10;

char N7_B1;
char N7_B2;
char N7_B3;
char N7_B4;
char N7_B5;
char N7_B6;
char N7_B7;
char N7_B8;
char N7_B9;
char N7_B10;

char N8_B1;
char N8_B2;
char N8_B3;
char N8_B4;
char N8_B5;
char N8_B6;
char N8_B7;
char N8_B8;
char N8_B9;
char N8_B10;

char N9_B1;
char N9_B2;
char N9_B3;
char N9_B4;
char N9_B5;
char N9_B6;
char N9_B7;
char N9_B8;
char N9_B9;
char N9_B10;

char N10_B1;
char N10_B2;
char N10_B3;
char N10_B4;
char N10_B5;
char N10_B6;
char N10_B7;
char N10_B8;
char N10_B9;
char N10_B10;

char N11_B1;
char N11_B2;
char N11_B3;
char N11_B4;
char N11_B5;
char N11_B6;
char N11_B7;
char N11_B8;
char N11_B9;
char N11_B10;

char N12_B1;
char N12_B2;
char N12_B3;
char N12_B4;
char N12_B5;
char N12_B6;
char N12_B7;
char N12_B8;
char N12_B9;
char N12_B10;

char N13_B1;
char N13_B2;
char N13_B3;
char N13_B4;
char N13_B5;
char N13_B6;
char N13_B7;
char N13_B8;
char N13_B9;
char N13_B10;

char N14_B1;
char N14_B2;
char N14_B3;
char N14_B4;
char N14_B5;
char N14_B6;
char N14_B7;
char N14_B8;
char N14_B9;
char N14_B10;

char N15_B1;
char N15_B2;
char N15_B3;
char N15_B4;
char N15_B5;
char N15_B6;
char N15_B7;
char N15_B8;
char N15_B9;
char N15_B10;

char N16_B1;
char N16_B2;
char N16_B3;
char N16_B4;
char N16_B5;
char N16_B6;
char N16_B7;
char N16_B8;
char N16_B9;
char N16_B10;

char N17_B1;
char N17_B2;
char N17_B3;
char N17_B4;
char N17_B5;
char N17_B6;
char N17_B7;
char N17_B8;
char N17_B9;
char N17_B10;

char N18_B1;
char N18_B2;
char N18_B3;
char N18_B4;
char N18_B5;
char N18_B6;
char N18_B7;
char N18_B8;
char N18_B9;
char N18_B10;

char N19_B1;
char N19_B2;
char N19_B3;
char N19_B4;
char N19_B5;
char N19_B6;
char N19_B7;
char N19_B8;
char N19_B9;
char N19_B10;

char N20_B1;
char N20_B2;
char N20_B3;
char N20_B4;
char N20_B5;
char N20_B6;
char N20_B7;
char N20_B8;
char N20_B9;
char N20_B10;


////////////////////////////////////////////////////////////////////////
/////////////////////////////setup//////////////////////////////////////


static FILE mydata = FDEV_SETUP_STREAM(ili9341_putchar_printf, NULL, _FDEV_SETUP_WRITE);

char string[10]={0};


////////////////////////////////////////////////////////////////////////
/////////////////////////////Prototypen/////////////////////////////////


void rechteck(void);
void rechteck_clear(void);
void codefeld(void);
void Zifferneingabe(void);
void Eingabecurser(void);
void auswahl_anzeige(void);
void gut(void);
void schlecht(void);
uint8_t auswahl(void);
void Blinken(void);
void Konfig_Auswahl(void);
void ENTER_Flankenerkennung(void);
void UP_Flankenerkennung(void);
void DOWN_Flankenerkennung(void);
void Jahreseingabe(void);
void Kalendereingabe(void);
void Namen_speichern(void);


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
  int lcolor1=(flag==1)?LCD_RGB(0x10,0x10,0x10):LCD_RGB(0x18,0x18,0x18);
  int lcolor2=(flag==1)?LCD_RGB(0x18,0x18,0x18):LCD_RGB(0x10,0x10,0x10);
  int a;
  
  ili9341_fillrect(x+len,y+len,sx-(2*len),sy-(2*len),LCD_RGB(0x8,0x8,0x8));
  for(a=0;a<=len;a++)
    {
      ili9341_drawhline(x+a,y+a,sx-(2*a),lcolor1);
      ili9341_drawhline(x+a,(y+sy)-a,sx-(2*a),lcolor2);
      ili9341_drawvline(x+a,y+a,sy-(2*a),lcolor1);
      ili9341_drawvline((x+sx)-a,y+a,sy-(2*a),lcolor2);
     
    }
  
}

void ENTER_Flankenerkennung(void)
{
	static char old_level = 0;
	char new_level;

	new_level = (TST_ENTER != 0);	
	if(!old_level && new_level)
	{
		ENTER = 1;
	}
	old_level = new_level;
}

void UP_Flankenerkennung(void)
{
	static char old_level = 0;
	char new_level;

	new_level = (TST_UP != 0);	
	if(!old_level && new_level)
	{
		UP = 1;
	}
	old_level = new_level;
}

void DOWN_Flankenerkennung(void)
{
	static char old_level = 0;
	char new_level;

	new_level = (TST_DOWN != 0);	
	if(!old_level && new_level)
	{
		DOWN = 1;
	}
	old_level = new_level;
}


//////////////////////////////////////Hauptprogramm///////////////////////////////////////


int main(void)
{
/////////////////////////////Initialisierung////////////////////////////	


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
	
	
	stdout = & mydata;
	ili9341_init();														//initial driver setup to drive ili9341
	ili9341_clear(BLACK);												//fill screen with black colour
	_delay_ms(1000);
	ili9341_setRotation(3);												//rotate screen
	_delay_ms(2);


////////////////////////////////////////////////////////////////////////
////////////////////////////Default-Setting/////////////////////////////


TRANSISTOR_OFF;															//Ton ausschalten / nur beim Programmieren notwenig
piezo = eeprom_read_word((uint16_t *) 4);


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
	
	
	uint32_t resultat = 0;												//Speicher für ADC Resultat
	uint16_t spannung_o = 6100, spannung_u = 5900;						//obere und untere Spannungsgrenze definieren
	
	enum {Pruefmodus, Warten, Pin, Auswahl_Menue, Pin_aendern, Lautstaerke, Speicher, Konfig, Neu, Bearbeiten, Last, Spannung, Datum, Produktionsdatum, Kalenderwoche, Produktionsjahr, Return, Neu_Name, Mit_Last, Konfig_waehlen, Return_Datum} state = Warten;
	
	Status = 1;
	Zustand = 1;
	
	
////////////////////////////////////////////////////////////////////////
////////////////////////////////Startup/////////////////////////////////
							
							
	ili9341_settextcolour(YELLOW,LCD_RGB(16,16,16));
	ili9341_settextsize(2.4);

	ili9341_fillRect(0,0,_width,_height,LCD_RGB(0x3f,0x3f,0x3f));
	
	ili9341_setcursor(130,160);
	printf("created by:");
	ili9341_setcursor(130,180);
	printf("RUAG Schweiz AG");
	_delay_ms(2000);
	//for(uint32_t m = 0; m <= 2000; m++);


////////////////////////////////////////////////////////////////////////
//////////////////////////Endlosschleife//////////////////////////////////////////////
	
	while(1)  
	{
		ENTER_Flankenerkennung();
		UP_Flankenerkennung();
		DOWN_Flankenerkennung();
		
	
		
/////////////////Hintergrundfarbe mit Datum abstimmen///////////////////
		// This do all the stuff as the uncommented stuff below just with few lines ;)
		{
		  int fc[]={CYAN,BLACK,BLACK,BLACK};
		  int bg[]={BLACK,GREEN,RED,CYAN};
		    KA = eeprom_read_word((uint16_t*)8);
		   
       		    print_at_lcd(10,220,fc[background],bg[background],2,"KW%d %d\n", KA);
		    YEAR = eeprom_read_word((uint16_t*)12);							  
		    print_at_lcd(205,220,fc[background],bg[background],2,"Jahr:%d\n",YEAR);

		}

		draw_box(10,30,150,30,3,0);
		for(;;);

	}

}

