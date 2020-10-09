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
							
							
	ili9341_settextcolour(YELLOW,BLACK);
	ili9341_settextsize(2.4);
	
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
		
		/*eeprom_read_block((void*)&Testausgabe, (const void*)12, 2);
		ili9341_setcursor(70, 220);								
		ili9341_settextcolour(GREEN,BLACK);							
		ili9341_settextsize(2);										
		printf("%s\n", Testausgabe);*/		
		
/////////////////Hintergrundfarbe mit Datum abstimmen///////////////////

		  int fc[]={CYAN,BLACK,BLACK,BLACK};
		  int bg[]={BLACK,GREEN,RED,CYAN};
		    KA = eeprom_read_word((uint16_t*)8);
       		print_at_lcd(10,220,fc[background],bg[background],2,"KW%d\n",KA);
		    YEAR = eeprom_read_word((uint16_t*)12);							  
		    print_at_lcd(205,220,fc[background],bg[background],2,"Jahr:%d\n",YEAR);


		
////////////////////////////////////////////////////////////////////////
////////////////////////Abfrage ob Pin eingeben/////////////////////////


		if((UP) && (DOWN) && (ENTER))									// Alle Taster betätigt?
		{
			UP = 0;
			DOWN = 0;
			ENTER = 0;
			if(Durchgang1 == 0)											// Programmdurchgang == 0?
			{
				ili9341_clear(BLACK);									// Display mit schwarzer Farbe überschreiben
		
				state = Pin; 											// Eingabefeld aufrufen
				Durchgang1 = 0;											// Programmdurchgang zurücksetzen
			}
		}

		
////////////////////////////////////////////////////////////////////////
/////////////////////////PIN-Eingabe-Abfrage////////////////////////////

		
			//Abfrage ob PIN eingeben
		/*if((UP) && (DOWN) && (ENTER))									// Alle Taster betätigt?
		{
			UP = 0;
			DOWN = 0;
			ENTER = 0;
			if(Durchgang1 == 0)
			{
				cntr_menue++;											//counter hochzählen um verzögerung zu messen
			}
			Durchgang1 = 0;
		}else{															//tastenkombination nicht mehr gedrückt
			cntr_menue = 0;												//counter zurücksetzen
		}*/
		
		/*if(cntr_menue == 5)												//3sek verzögert?
		{
			ili9341_clear(BLACK);
			state = Pin; 												//statuswechsel zu Pin
		}*/

						
////////////////////////////////////////////////////////////////////////
///////////////////////////////switch/////////////////////////////////////						
						
						
		switch(state)
		{
			
//////////////////////////Bereit für Messung////////////////////////////	

		
			case Warten:
				LED_GRUN_OFF;
				LED_ROT_OFF;	
				
				
				if(!TST_ADA)											//Adapter nicht angeschlossen?
				{
					if(!adapter)									  	//damit nur 1 mal geschrieben wird
					{
						ili9341_clear(BLACK);
						ili9341_settextsize(3);
						ili9341_setcursor(55, 100);
						ili9341_settextcolour(YELLOW, BLACK);
						printf("Kein Adapter");
						ili9341_settextsize(3);
						ili9341_setcursor(90, 130);
						ili9341_settextcolour(YELLOW, BLACK);
						printf("erkannt!");
						adapter = 1;
						warten = 0;
					}	
				}else{
					
					adapter = 0;
					spannung = resultat * 111 / 12;							//berechnung der effektiven spannung (Spannungsteiler: 100kOhm und 12kOhm)
					resultat = read_adc(REFS0);								//Interne Referenzspannung für ADC
					resultat = resultat * 1270;								
					resultat = resultat / 500;
					
					if(warten == 0)										//Damit nur 1 mal geschrieben wird
					{
						background = 3;
						TRANSISTOR_OFF;									//Ton ist aus
						ili9341_clear(CYAN);
						/*
						ili9341_settextsize(3);
						ili9341_setcursor(10, 110);
						ili9341_settextcolour(BLACK, CYAN);
						printf("Batterie einlegen");					//Batterie verlangen anzeigen
						*/
						print_at_lcd(10,110,BLACK,CYAN,3,"Batterie einlegen");
						warten = 1;
					}
					
					if(TST_BAT)											//sobad Batterie erkennt wurde
					{
						background = 3;
						ili9341_settextsize(3);
						ili9341_setcursor(10, 110);
						ili9341_settextcolour(CYAN, CYAN);
						printf("Batterie einlegen");					//Anzeige mit Cyan berschreiben, um zu loeschen
						warten = 0;
						state = Pruefmodus;								//Statuswechsel zu Pruefmodus
						error = 0;
						ok = 0;
						if(piezo == 1)
						{
							TRANSISTOR_ON;								//wenn man Ton will, auf ON
						}
						if(piezo == 0)
						{
							TRANSISTOR_OFF;								//wenn man keinen Ton will, auf OFF
						}
						error_cntr = 0;									//Counter für Blinken der Erroranzeige
					}
					
					if((UP) && (DOWN) && (ENTER))									// Alle Taster betätigt?
					{
						UP = 0;
						DOWN = 0;
						ENTER = 0;
						if(Durchgang1 == 0)											// Programmdurchgang == 0?
						{
							ili9341_clear(BLACK);									// Display mit schwarzer Farbe überschreiben
					
							state = Pin; 											// Eingabefeld aufrufen
							Durchgang1 = 0;											// Programmdurchgang zurücksetzen
						}
					}
					
				}
			break;
			
			
////////////////////////////////////////////////////////////////////////
//////////////////////////Batterie testen///////////////////////////////			
			
			
			case Pruefmodus:
				
				if(!TST_BAT)											// wenn Batterie entfernt wurde
				{
					state = Warten;										// Statuswechsel zu Warten
				}
				
					spannung = resultat * 111 / 12;						// berechnung der effektiven spannung (Spannungsteiler: 100kOhm und 12kOhm)
					
					
					resultat = read_adc(REFS0);							// Interne Referenzspannung für ADC
					resultat = resultat * 1270;								
					resultat = resultat / 500;							// umrechnung des ADCwertes in mV
					
					if(TST_BAT && bat == 0)								// Batterie in Adapter?
					{
						Blinken();										// Blinken der Status LED und Verzögern
						
						battery = spannung;								// Batteriespannung abspeichern
						bat = 1;										// verhindern einer mehrfachen speicherung
					}
					
					if(!TST_BAT)										// Batterie nicht mehr in adapter
					{
						bat = 0;										// Hilfsvar zurücksetzen 
						LED_BAT_OFF;									// Status LED ausschalten
					}
					
						if((spannung >= spannung_u) && (spannung <= spannung_o) && (ok == 0))		//Spannung inerhalb Toleranz und 1. Durchgang
						{
							error = 0;																
							ok = 1;										// startvar damit Text nur einaml geschriebn wird
							background = 1;
							
							LED_GRUN_ON;								// Gruenes LED anschalten
							LED_ROT_OFF;								// Rotes LED anschalten
							Background_Gruen = 1;
							Background_Rot = 0;
							gut();										// Ton ausgeben
							
							ili9341_clear(GREEN);						// Display gruen faerben
							

//////////////////////////////////////////////////////////////////////////////
							ili9341_settextcolour(BLACK,GREEN);			//
							ili9341_settextsize(8);						//
							ili9341_setcursor(140, 170);				//
							printf("%c", smileok);						//
																		//
							ili9341_settextcolour(BLACK, BLACK);		//
							ili9341_settextsize(4);						//
							ili9341_setcursor(148, 182);				//
							printf(".");								//
																		//
							ili9341_settextcolour(BLACK, BLACK);		//
							ili9341_settextsize(4);						//
							ili9341_setcursor(144, 178);				//
							printf(".");								//
																		//
							ili9341_settextcolour(BLACK, BLACK);		//
							ili9341_settextsize(4);						//
							ili9341_setcursor(152, 178);				//
							printf(".");								//
																		//
							ili9341_settextcolour(GREEN, GREEN);		//	
							ili9341_settextsize(4);						//
							ili9341_setcursor(144, 188);				//
							printf(".");								//
																		//
							ili9341_settextcolour(GREEN, GREEN);		//
							ili9341_settextsize(4);						//
							ili9341_setcursor(152, 188);				//
							printf(".");								// Froehliches Smile ausgeben
//////////////////////////////////////////////////////////////////////////////

							
							ili9341_settextcolour(BLACK,GREEN);

							ili9341_settextsize(3);
							ili9341_setcursor(70, 80);
							printf("Voltage ok");
							
							ili9341_settextsize(4);
							ili9341_setcursor(85, 120);
							printf("%d.", einer);						// anzeigen der vorkommastellen
							printf("%03dV     ", nachkomma);			// anzeigen der nachkommastellen
							
						}else if(ok == 1)								// wenn Text bereits geschrieben ist
						{
							ili9341_settextsize(4);
							ili9341_setcursor(85, 120);
							printf("%d.", einer);						// anzeigen der vorkommastellen
							printf("%03dV     ", nachkomma);			// anzeigen der nachkommastellen
							gut();										// Ton ausgeben
						}
							
						if(((spannung < spannung_u) || (spannung > spannung_o)) && (error == 0))	// Spannung error
						{
							schlecht();									// Ton ausgeben
							error = 1;
							ok = 0;										// startvar damit Text nur einaml geschriebn wird
							background = 2;
							ili9341_clear(RED);
							
							LED_GRUN_OFF;
							LED_ROT_ON;
							Background_Gruen = 0;
							Background_Rot = 1;
							
							schlecht();
						
							
//////////////////////////////////////////////////////////////////////////////
							ili9341_settextcolour(BLACK,RED);			//
							ili9341_settextsize(8);						//
							ili9341_setcursor(140, 172);				//
							printf("%c", smileerror);					//
																		//
							ili9341_settextcolour(BLACK, BLACK);		//
							ili9341_settextsize(4);						//
							ili9341_setcursor(148, 194);				//
							printf(".");								//
																		//
							ili9341_settextcolour(BLACK, BLACK);		//
							ili9341_settextsize(4);						//
							ili9341_setcursor(144, 197);				//
							printf(".");								//
																		//
							ili9341_settextcolour(BLACK, BLACK);		//
							ili9341_settextsize(4);						//
							ili9341_setcursor(152, 197);				//
							printf(".");								//
																		//
							ili9341_settextcolour(RED, RED);			//
							ili9341_settextsize(3);						//
							ili9341_setcursor(146, 192);				//
							printf(".");								//
																		//
							ili9341_settextcolour(RED, RED);			//
							ili9341_settextsize(3);						//
							ili9341_setcursor(156, 192);				//
							printf(".");								// Trauriges Smile ausgeben
//////////////////////////////////////////////////////////////////////////////

							
							ili9341_settextcolour(BLACK,RED);

							ili9341_settextsize(3);
							ili9341_setcursor(50, 80);
							printf("Voltage error");
							
							ili9341_setcursor(85, 120);
							ili9341_settextsize(4);
							printf("%d.", einer);						// anzeigen der vorkommastellen
							printf("%03dV     ", nachkomma);			// anzeigen der nachkommastellen
							
						}else if(error == 1)							// wenn Text bereits geschrieben ist
						{
							error_cntr++;
							schlecht();									// Ton ausgeben
							ili9341_settextcolour(BLACK,RED);
							ili9341_setcursor(85, 120);
							ili9341_settextsize(4);
							printf("%d.", einer);						// anzeigen der vorkommastellen
							printf("%03dV     ", nachkomma);			// anzeigen der nachkommastellen
							
							
//////////////////////////////////////////////////////////////////////////////					
							if(error_cntr & 0x01)					  	//
							{										  	//
								ili9341_settextcolour(RED, RED);	  	//
																		//
								ili9341_settextsize(3);				 	//
								ili9341_setcursor(50, 80);			  	//
								printf("Voltage error");			  	//
							}else{									  	//
								error_cntr = 0;						  	//
								ili9341_settextcolour(BLACK, RED);	  	//
																		//
								ili9341_settextsize(3);				  	//
								ili9341_setcursor(50, 80);			  	//
								printf("Voltage error");			  	//
							}										  	// Blinken der Erroranzeige
//////////////////////////////////////////////////////////////////////////////

						}
						
			break;
			
			
////////////////////////////////////////////////////////////////////////		
/////////////////////////korrekter PIN eingebn//////////////////////////			


			case Pin:
			
			background = 0;
			Eingabecurser();											// die Bedienung der Pin-Anzeige

			if(Pass < 10 && ENTER && XPosition <= 200)					// ist der Curser auf einer Zahl, wird die Taste Enter gedrückt und wurden vier Zahlen eingegeben?
			{
				ENTER = 0;
				if (Stelle > 1 && Durchgang1 > 1)						// ist die Einerziffer des Passworts grösser als 1 und ist das Programm im zweiten Durchgang?
				{
					Stelle = Stelle / 10;								// Von der Tausenderziffer, wird immer durch 10 gerechnet um die Hunderter- und die Zehnerziffer zu definieren					
				}
										
					ZwischenP = Pass * Stelle;							// Die Ausgewählte Zahl wird mit der Zahlenstelle Multipliziert --> 4 * 1000 = 4000 --> 3 * 100 = 300...
					Passwort = Passwort + ZwischenP;					// Das Zwischnpasswort wird zum Endpasswort hinzugerechnet --> 4000 + 300 = 4300
					Durchgang1 = 2;										// Damit beim zweiten Durchgang die Stelle dividiert wird, wird die Durchgangs-Variabel auf 2 gesetzt
					
					ili9341_setcursor(XPosition,28);					
					ili9341_settextcolour(ORANGE,BLACK);				
					ili9341_settextsize(2);
					printf("*");
					
					XPosition = XPosition + 30;							// XPosition bestimmt auf welcher X Position die Sternchen gesetzt werden müssen
					
				if(Passwort == RichtigesP && Stelle == 1)				// Ist das eingegeben Passwort dasselbe, wie das geänderte Passwort und alle vier Zahlen eingegeben
				{	
					w = 201;											//
					h = 32;												//
					Passwort = 0;										//
					ZwischenP = 0;										// Variabeln zurückgesetzt
					XPosition = 110;									//
					Stelle = 1000;										//
					Durchgang1 = 1;										//
				
					ili9341_clear(BLACK);
					Status = 1;
					auswahl_anzeige();									// Anzeige für das Auswahl-Menu
					state = Auswahl_Menue;
					
				}else if(Passwort == DefaultP && Stelle == 1)
				{
					w = 201;											//
					h = 32;												//
					Passwort = 0;										//
					ZwischenP = 0;										// Variabeln zurückgesetzt
					XPosition = 110;									//
					Stelle = 1000;										//
					Durchgang1 = 1;										//
				
					ili9341_clear(BLACK);
					Status = 1;
					auswahl_anzeige();									// Anzeige für das Auswahl-Menu
					state = Auswahl_Menue;
				}else if(Passwort != DefaultP && XPosition > 200)	    // Ist das Passwort falsch und alle vier Zahlen eingegeben?
				{
					Passwort = 0;										//
					ZwischenP = 0;										//
					XPosition = 110;									// Variablen zurückgesetzt
					Stelle = 1000;										//
					Durchgang1 = 1;										//
				
					ili9341_setcursor(110,28);								
					ili9341_settextcolour(BLACK,BLACK);
					ili9341_settextsize(2);
					printf("*");
					
					ili9341_setcursor(140,28);								
					ili9341_settextcolour(BLACK,BLACK);
					ili9341_settextsize(2);
					printf("*");
					
					ili9341_setcursor(170,28);								
					ili9341_settextcolour(BLACK,BLACK);
					ili9341_settextsize(2);
					printf("*");
				
					ili9341_setcursor(200,28);								
					ili9341_settextcolour(BLACK,BLACK);
					ili9341_settextsize(2);
					printf("*");
				
				}			
			}
			
			if(Pass == 10 && ENTER && XPosition > 110)					// befindet sich der Curser auf einer Zahl, wird die Enter-Taste gedrückt und wurde mehr als eine Zahl bereits eingegeben?
			{					
				ENTER = 0;
				Stelle = Stelle * 10; 
				
				Passwort = Passwort / Stelle;							// Das Passwort wird um die Stelle * 10 dividiert damit die letzte Ziffer wegfällt 
				Passwort = Passwort * Stelle;
				
				XPosition = XPosition - 30;
				
				ili9341_setcursor(XPosition,28);								
				ili9341_settextcolour(BLACK,BLACK);
				ili9341_settextsize(2);
				printf("*");
				
			}
		break;
			
			case Auswahl_Menue:
				
				switch(Zustand)											// die Bedienung des Auswahl-Menus
				{
					case 1:
					x = 59;
					y = 50;
					w = 201;
					h = 30;
					
					if(start == 0)
					{
						rechteck();
						start = 1;
					}
					
					if(UP)
					{
						UP = 0;
						rechteck_clear();
						Zustand = 4;
						start = 0;
						
					}else if(DOWN)
					{
						DOWN = 0;
						rechteck_clear();
						Zustand = 2;
						start = 0;
					}
					
					if(ENTER)
					{
						ENTER = 0;
						Zustand = 1;
						start = 0;
						state = Pin_aendern;
						ili9341_clear(BLACK);
						rechteck_clear();
						Zifferneingabe();
						
						x = code_x1;
						y = code_y1;
						w = 23;
						h = 30;
					}
					
					break;
					
					case 2:
					x = 132;
					y = 94;
					w = 57; 
					h = 30;
					
					if(start == 0)
					{
						rechteck();
						start = 1;
					}
					
					if(UP)
					{
						UP = 0;
						rechteck_clear();
						Zustand = 1;
						start = 0;
						
					}else if(DOWN)
					{
						DOWN = 0;
						rechteck_clear();
						Zustand = 3;
						start = 0;
					}
					
					if(ENTER)
					{
						ENTER = 0;
						Zustand = 1;
						start = 0;
						state = Lautstaerke;
						ili9341_clear(BLACK);
						if(piezo == 1)
						{
							ili9341_setcursor(10, 110);					
							ili9341_settextcolour(PINK,BLACK);				 
							ili9341_settextsize(3);
							printf("Ton eingeschaltet");
						}
						
						if(piezo == 0)
						{
							ili9341_setcursor(10, 110);						
							ili9341_settextcolour(PINK,BLACK);				 
							ili9341_settextsize(3);
							printf("Ton ausgeschaltet");
						}
					}
					
					break;
					
					case 3:
					x = 86;
					y = 138;
					w = 147;
					h = 30;
					
					if(start == 0)
					{
						rechteck();
						start = 1;
					}
				
					if(UP)
					{
						UP = 0;
						rechteck_clear();
						Zustand = 2;
						start = 0;
						
					}else if(DOWN)
					{
						DOWN = 0;
						rechteck_clear();
						Zustand = 4;
						start = 0;
					}
					
					if(ENTER)
					{
						ENTER = 0;
						state = Speicher;
						Zustand = 1;
						start = 0;
						ili9341_clear(BLACK);
					}
					
					break;
					
					case 4:
					x = 104;
					y = 183;
					w = 111;
					h = 30;
					
					if(start == 0)
					{
						rechteck();
						start = 1;
					}
					
					if(UP)
					{
						UP = 0;
						rechteck_clear();
						Zustand = 3;
						start = 0;
						
					}else if(DOWN)
					{
						DOWN = 0;
						rechteck_clear();
						Zustand = 1;
						start = 0;
					}
					
					if(ENTER)
					{
						ENTER = 0;
						Zustand = 1;
						start = 0;
						state = Konfig;
						ili9341_clear(BLACK);
					}
					
					break;
				}
				
				break;
			
			
			case Pin_aendern:
				
				if(start == 0)
				{
					start = 1;
					RichtigesP = 0;
				}
				
					Eingabecurser();
															//die Bedienung der Pin-Anzeige
				
				if(Pass < 10 && ENTER && XPosition <= 200)				//ist der Curser auf einer Zahl, wird die Taste Enter gedrückt und wurden vier Zahlen eingegeben?
				{
					ENTER = 0;
					if (Stelle > 1 && Durchgang1 > 1)					//ist die Einerziffer des Passworts grösser als 1 und ist das Programm im zweiten Durchgang?
					{
						Stelle = Stelle / 10;					
					}
										
					ZwischenP = Pass * Stelle;
					RichtigesP = ZwischenP + RichtigesP;				//Das Zwischnpasswort wird zum zurückgesetzten Passwort hinzugerechnet
					Durchgang1 = 2;										//Damit beim zweiten Durchgang die Stelle dividiert wird, wird die Durchgangs-Variabel auf 2 gesetzt
					
					ili9341_setcursor(XPosition,28);								
					ili9341_settextcolour(ORANGE,BLACK);
					ili9341_settextsize(2);
					printf("*");
					
					XPosition = XPosition + 30;
				}
					
				if(Pass == 10 && ENTER && XPosition > 110)			//befindet sich der Curser auf einer Zahl, wird die Enter-Taste gedrückt und wurde mehr als eine Zahl bereits eingegeben?
				{					
					ENTER = 0;
					Stelle = Stelle * 10; 
				
					RichtigesP = RichtigesP / Stelle;					//Das zurückgesetzte Passwort wird um die Stelle * 10 dividiert damit die letzte Ziffer wegfällt
					RichtigesP = RichtigesP * Stelle;
				
					XPosition = XPosition - 30;
				
					ili9341_setcursor(XPosition,28);								
					ili9341_settextcolour(BLACK,BLACK);
					ili9341_settextsize(2);
					printf("*");
				
				}

				if(Stelle == 1)											//das ganze Passwort eingegeben wurde
				{
					UP = 0;
					DOWN = 0;
					ENTER = 0;
					Status = 1;											//
					Passwort = 0;										//					
					ZwischenP = 0;										//
					XPosition = 110;									// Variabeln zurückgesetzt
					Stelle = 1000;										//
					Durchgang1 = 0;										//
					error = 0;											//
					start = 0;											//
					
					ili9341_setcursor(45,110);		
					ili9341_clear(BLACK);						
					ili9341_settextcolour(YELLOW,BLACK);
					ili9341_settextsize(3);
					printf("Pin Geaendert!");
					for(uint32_t i = 0; i <=240000; i++);
					ili9341_clear(BLACK);
					state = Pin;
				}
				
			break;
			
			
			case Lautstaerke:
				//Anzeige
				if(start == 0)
				{
					// Ton anzeigen
					start = 1;
					ili9341_setcursor(120, 10);						
					ili9341_settextcolour(YELLOW,BLACK);				 
					ili9341_settextsize(4);
					printf("Ton");
					
					// Einschalten Anzeige
					ili9341_setcursor(40, 160);						
					ili9341_settextcolour(CYAN,BLACK);				 
					ili9341_settextsize(3);
					printf("OFF");
					
					// Ausschalten Anzeige
					ili9341_setcursor(240, 160);						
					ili9341_settextcolour(CYAN,BLACK);				 
					ili9341_settextsize(3);
					printf("ON");
					
					// Bestätigungsanzeige
					ili9341_setcursor(140, 160);						
					ili9341_settextcolour(CYAN,BLACK);				 
					ili9341_settextsize(3);
					printf("OK");
				}
				
				gut();
				
				if(DOWN)	
				{
					DOWN = 0;
					piezo = 1;											// Ton einschalten
					ili9341_setcursor(10, 110);						
					ili9341_settextcolour(PINK,BLACK);				 
					ili9341_settextsize(3);
					printf("Ton eingeschaltet");
				}
				
				if(UP)	
				{
					UP = 0;
					piezo = 0;											// Ton ausschalten
					ili9341_setcursor(10, 110);						
					ili9341_settextcolour(PINK,BLACK);				 
					ili9341_settextsize(3);
					printf("Ton ausgeschaltet");
				}
				
				if(ENTER)
				{
					ENTER = 0;
					background = 3;
					state = Warten;
					ili9341_clear(CYAN);
					ili9341_settextsize(3);
					ili9341_setcursor(10, 110);
					ili9341_settextcolour(BLACK, CYAN);
					printf("Batterie einlegen");					//Batterie verlangen anzeigen
					eeprom_write_word((uint16_t *) 4, piezo);
				}
			break;
			
			
			case Konfig:
				if(Schreiben == 0)
				{		
					Schreiben = 1;
					// Titel "Konfig"
					ili9341_setcursor(87, 10);								
					ili9341_settextcolour(YELLOW,BLACK);
					ili9341_settextsize(4);
					printf("Konfig");
							
							
					// "Neue Konfig erstellen"
					ili9341_setcursor(134, 64);						
					ili9341_settextcolour(WHITE,BLACK);				 
					ili9341_settextsize(3);
					printf("Neu");	
					
					
					// "Alte Kofig verwenden"	
					ili9341_setcursor(71, 108);						
					ili9341_settextcolour(WHITE,BLACK);				 
					ili9341_settextsize(3);
					printf("Bearbeiten");
					
					
					// "Alte Kofig verwenden"	
					ili9341_setcursor(35, 152);						
					ili9341_settextcolour(WHITE,BLACK);				 
					ili9341_settextsize(3);
					printf("Konfig waehlen");
				}
				
				
				
				switch(Zustand_Konfig)
				{
					case 1:
						x = 124;
						y = 59;
						w = 71;
						rechteck();
						if(UP)
						{
							UP = 0;
							rechteck_clear();
							Zustand_Konfig = 3;
							
						}else if(DOWN)
						{
							DOWN = 0;
							rechteck_clear();
							Zustand_Konfig = 2;
						}
					
						if(ENTER)
						{
							ENTER = 0;
							ili9341_clear(BLACK);
							Schreiben = 0;
							Zustand_Konfig = 1;
							stopp = 0;
							Name_Nr++;
							state = Neu_Name;
							ili9341_drawRect((cntr_Name - 3), 16, 21, 28, GREEN);
						}
					break;
					
					
					case 2:
						x = 59;
						y = 103;
						w = 201;
						rechteck();
						if(UP)
						{
							UP = 0;	
							rechteck_clear();
							Zustand_Konfig = 1;
							
						}else if(DOWN)
						{
							DOWN = 0;
							rechteck_clear();
							Zustand_Konfig = 3;
						}
					
						if(ENTER)
						{
							ENTER = 0;
							ili9341_clear(BLACK);
							Schreiben = 0;
							Zustand_Konfig = 1;
							state = Bearbeiten;
						}
					break;
					
					
					case 3:
						x = 24;
						y = 147;
						w = 270;
						rechteck();
						if(UP)
						{
							UP = 0;
							rechteck_clear();
							Zustand_Konfig = 2;
							
						}else if(DOWN)
						{
							DOWN = 0;
							rechteck_clear();
							Zustand_Konfig = 1;
						}
					
						if(ENTER)
						{
							ENTER = 0;
							ili9341_clear(BLACK);
							Schreiben = 0;
							Zustand_Konfig = 1;
							state = Konfig_waehlen;
						}
					break;
				}		
			break;
			
			
			case Neu_Name:
			if(Schreiben == 0)
			{
				ili9341_setcursor(20, 20);								
				ili9341_settextcolour(CYAN,BLACK);
				ili9341_settextsize(3);
				printf("Name:");
				
				ili9341_fillRect(110, 44, 210, 3, GREEN);
				
				
				ili9341_setcursor(54, 60);								
				ili9341_settextcolour(ORANGE,BLACK);
				ili9341_settextsize(3);
				printf("A");
				
				ili9341_setcursor(94, 60);								
				ili9341_settextcolour(ORANGE,BLACK);
				ili9341_settextsize(3);
				printf("B");
				
				ili9341_setcursor(134, 60);								
				ili9341_settextcolour(ORANGE,BLACK);
				ili9341_settextsize(3);
				printf("C");
				
				ili9341_setcursor(174, 60);								
				ili9341_settextcolour(ORANGE,BLACK);
				ili9341_settextsize(3);
				printf("D");
				
				ili9341_setcursor(214, 60);								
				ili9341_settextcolour(ORANGE,BLACK);
				ili9341_settextsize(3);
				printf("E");
				
				ili9341_setcursor(254, 60);								
				ili9341_settextcolour(ORANGE,BLACK);
				ili9341_settextsize(3);
				printf("F");
				
				
				ili9341_setcursor(74, 90);								
				ili9341_settextcolour(ORANGE,BLACK);
				ili9341_settextsize(3);
				printf("G");
				
				ili9341_setcursor(114, 90);								
				ili9341_settextcolour(ORANGE,BLACK);
				ili9341_settextsize(3);
				printf("H");
				
				ili9341_setcursor(154, 90);								
				ili9341_settextcolour(ORANGE,BLACK);
				ili9341_settextsize(3);
				printf("I");
				
				ili9341_setcursor(194, 90);								
				ili9341_settextcolour(ORANGE,BLACK);
				ili9341_settextsize(3);
				printf("J");
				
				ili9341_setcursor(234, 90);								
				ili9341_settextcolour(ORANGE,BLACK);
				ili9341_settextsize(3);
				printf("K");
				
				
				ili9341_setcursor(54, 120);								
				ili9341_settextcolour(ORANGE,BLACK);
				ili9341_settextsize(3);
				printf("L");
				
				ili9341_setcursor(94, 120);								
				ili9341_settextcolour(ORANGE,BLACK);
				ili9341_settextsize(3);
				printf("M");
				
				ili9341_setcursor(134, 120);								
				ili9341_settextcolour(ORANGE,BLACK);
				ili9341_settextsize(3);
				printf("N");
				
				ili9341_setcursor(174, 120);								
				ili9341_settextcolour(ORANGE,BLACK);
				ili9341_settextsize(3);
				printf("O");
				
				ili9341_setcursor(214, 120);								
				ili9341_settextcolour(ORANGE,BLACK);
				ili9341_settextsize(3);
				printf("P");
				
				ili9341_setcursor(254, 120);								
				ili9341_settextcolour(ORANGE,BLACK);
				ili9341_settextsize(3);
				printf("Q");
				
				
				ili9341_setcursor(74, 150);								
				ili9341_settextcolour(ORANGE,BLACK);
				ili9341_settextsize(3);
				printf("R");
				
				ili9341_setcursor(114, 150);								
				ili9341_settextcolour(ORANGE,BLACK);
				ili9341_settextsize(3);
				printf("S");
				
				ili9341_setcursor(154, 150);								
				ili9341_settextcolour(ORANGE,BLACK);
				ili9341_settextsize(3);
				printf("T");
				
				ili9341_setcursor(194, 150);								
				ili9341_settextcolour(ORANGE,BLACK);
				ili9341_settextsize(3);
				printf("U");
				
				ili9341_setcursor(234, 150);								
				ili9341_settextcolour(ORANGE,BLACK);
				ili9341_settextsize(3);
				printf("V");
				
				
				ili9341_setcursor(54, 180);								
				ili9341_settextcolour(ORANGE,BLACK);
				ili9341_settextsize(3);
				printf("W");
				
				ili9341_setcursor(94, 180);								
				ili9341_settextcolour(ORANGE,BLACK);
				ili9341_settextsize(3);
				printf("X");
				
				ili9341_setcursor(134, 180);								
				ili9341_settextcolour(ORANGE,BLACK);
				ili9341_settextsize(3);
				printf("Y");
				
				ili9341_setcursor(174, 180);								
				ili9341_settextcolour(ORANGE,BLACK);
				ili9341_settextsize(3);
				printf("Z");
				
				ili9341_setcursor(214, 180);								
				ili9341_settextcolour(RED,BLACK);
				ili9341_settextsize(3);
				printf("%c", Delete);
				 
				
				ili9341_setcursor(254, 180);								
				ili9341_settextcolour(GREEN,BLACK);
				ili9341_settextsize(3);
				printf("OK");
				
				Schreiben = 1;
			}


				switch(Zustand_Neu_Name)
				{
					case 65:
						x = 49;
						y = 56;
						w = 25;
						h = 28;
						
						if(start == 0)
						{
							rechteck();
							start = 1;
						}
						
						if(UP)
						{
							UP = 0;
							rechteck_clear();
							Zustand_Neu_Name = 92;
							start = 0;
							
						}else if(DOWN)
						{
							DOWN = 0;
							rechteck_clear();
							Zustand_Neu_Name = 66;
							start = 0;
						}
						
						if(ENTER)
						{
							ENTER = 0;
							ili9341_drawRect((cntr_Name - 3), 16, 21, 28, BLACK);
							ili9341_setcursor(cntr_Name, 20);								
							ili9341_settextcolour(PINK,BLACK);
							ili9341_settextsize(3);
							printf("A");
							cntr_Name = cntr_Name + 20;
							if(cntr_Name <= 305)
							{
								ili9341_drawRect((cntr_Name - 3), 16, 21, 28, GREEN);
							}
							start = 0;
							stopp = 0;
							Namen_speichern();
						}
						
					break;
					
					
					case 66:
						x = 89;
						y = 56;
						w = 25;
						h = 28;
						
						if(start == 0)
						{
							rechteck();
							start = 2;
						}
						
						if(UP)
						{
							UP = 0;
							rechteck_clear();
							Zustand_Neu_Name = 65;
							start = 0;
							
						}else if(DOWN)
						{
							DOWN = 0;
							rechteck_clear();
							Zustand_Neu_Name = 67;
							start = 0;
						}
						
						if(ENTER)
						{
							ENTER = 0;
							ili9341_drawRect((cntr_Name - 3), 16, 21, 28, BLACK);
							ili9341_setcursor(cntr_Name, 20);								
							ili9341_settextcolour(PINK,BLACK);
							ili9341_settextsize(3);
							printf("B");
							cntr_Name = cntr_Name + 20;
							if(cntr_Name <= 305)
							{
								ili9341_drawRect((cntr_Name - 3), 16, 21, 28, GREEN);
							}
							start = 0;
							stopp = 0;
							Namen_speichern();
						}
						
					break;
					
					
					case 67:
						x = 129;
						y = 56;
						w = 25;
						h = 28;
						
						if(start == 0)
						{
							rechteck();
							start = 1;
						}
						
						if(UP)
						{
							UP = 0;
							rechteck_clear();
							Zustand_Neu_Name = 66;
							start = 0;
							
						}else if(DOWN)
						{
							DOWN = 0;
							rechteck_clear();
							Zustand_Neu_Name = 68;
							start = 0;
						}
						
						if(ENTER)
						{
							ENTER = 0;
							ili9341_drawRect((cntr_Name - 3), 16, 21, 28, BLACK);
							ili9341_setcursor(cntr_Name, 20);								
							ili9341_settextcolour(PINK,BLACK);
							ili9341_settextsize(3);
							printf("C");
							cntr_Name = cntr_Name + 20;
							if(cntr_Name <= 305)
							{
								ili9341_drawRect((cntr_Name - 3), 16, 21, 28, GREEN);
							}
							start = 0;
							stopp = 0;
							Namen_speichern();
						}
						
					break;
					
					
					case 68:
						x = 169;
						y = 56;
						w = 25;
						h = 28;
						
						if(start == 0)
						{
							rechteck();
							start = 1;
						}
						
						if(UP)
						{
							UP = 0;
							rechteck_clear();
							Zustand_Neu_Name = 67;
							start = 0;
							
						}else if(DOWN)
						{
							DOWN = 0;
							rechteck_clear();
							Zustand_Neu_Name = 69;
							start = 0;
						}
						
						if(ENTER)
						{
							ENTER = 0;
							ili9341_drawRect((cntr_Name - 3), 16, 21, 28, BLACK);
							ili9341_setcursor(cntr_Name, 20);								
							ili9341_settextcolour(PINK,BLACK);
							ili9341_settextsize(3);
							printf("D");
							cntr_Name = cntr_Name + 20;
							if(cntr_Name <= 305)
							{
								ili9341_drawRect((cntr_Name - 3), 16, 21, 28, GREEN);
							}
							start = 0;
							stopp = 0;
							Namen_speichern();
						}
						
					break;
					
					
					case 69:
						x = 209;
						y = 56;
						w = 25;
						h = 28;
						
						if(start == 0)
						{
							rechteck();
							start = 1;
						}
						
						if(UP)
						{
							UP = 0;
							rechteck_clear();
							Zustand_Neu_Name = 68;
							start = 0;
							
						}else if(DOWN)
						{
							DOWN = 0;
							rechteck_clear();
							Zustand_Neu_Name = 70;
							start = 0;
						}
						
						if(ENTER)
						{
							ENTER = 0;
							ili9341_drawRect((cntr_Name - 3), 16, 21, 28, BLACK);
							ili9341_setcursor(cntr_Name, 20);								
							ili9341_settextcolour(PINK,BLACK);
							ili9341_settextsize(3);
							printf("E");
							cntr_Name = cntr_Name + 20;
							if(cntr_Name <= 305)
							{
								ili9341_drawRect((cntr_Name - 3), 16, 21, 28, GREEN);
							}
							start = 0;
							stopp = 0;
							Namen_speichern();
						}
						
					break;
					
					
					case 70:
						x = 249;
						y = 56;
						w = 25;
						h = 28;
						
						if(start == 0)
						{
							rechteck();
							start = 1;
						}
						
						if(UP)
						{
							UP = 0;
							rechteck_clear();
							Zustand_Neu_Name = 69;
							start = 0;
							
						}else if(DOWN)
						{
							DOWN = 0;
							rechteck_clear();
							Zustand_Neu_Name = 71;
							start = 0;
						}
						
						if(ENTER)
						{
							ENTER = 0;
							ili9341_drawRect((cntr_Name - 3), 16, 21, 28, BLACK);
							ili9341_setcursor(cntr_Name, 20);								
							ili9341_settextcolour(PINK,BLACK);
							ili9341_settextsize(3);
							printf("F");
							cntr_Name = cntr_Name + 20;
							if(cntr_Name <= 305)
							{
								ili9341_drawRect((cntr_Name - 3), 16, 21, 28, GREEN);
							}
							start = 0;
							stopp = 0;
							Namen_speichern();
						}
						
					break;
					
					
					case 71:
						x = 69;
						y = 86;
						w = 25;
						h = 28;
						
						if(start == 0)
						{
							rechteck();
							start = 1;
						}
						
						if(UP)
						{
							UP = 0;
							rechteck_clear();
							Zustand_Neu_Name = 70;
							start = 0;
							
						}else if(DOWN)
						{
							DOWN = 0;
							rechteck_clear();
							Zustand_Neu_Name = 72;
							start = 0;
						}
						
						if(ENTER)
						{
							ENTER = 0;
							ili9341_drawRect((cntr_Name - 3), 16, 21, 28, BLACK);
							ili9341_setcursor(cntr_Name, 20);								
							ili9341_settextcolour(PINK,BLACK);
							ili9341_settextsize(3);
							printf("G");
							cntr_Name = cntr_Name + 20;
							if(cntr_Name <= 305)
							{
								ili9341_drawRect((cntr_Name - 3), 16, 21, 28, GREEN);
							}
							start = 0;
							stopp = 0;
							Namen_speichern();
						}
						
					break;
					
					
					case 72:
						x = 109;
						y = 86;
						w = 25;
						h = 28;
						
						if(start == 0)
						{
							rechteck();
							start = 1;
						}
						
						if(UP)
						{
							UP = 0;
							rechteck_clear();
							Zustand_Neu_Name = 71;
							start = 0;
							
						}else if(DOWN)
						{
							DOWN = 0;
							rechteck_clear();
							Zustand_Neu_Name = 73;
							start = 0;
						}
						
						if(ENTER)
						{
							ENTER = 0;
							ili9341_drawRect((cntr_Name - 3), 16, 21, 28, BLACK);
							ili9341_setcursor(cntr_Name, 20);								
							ili9341_settextcolour(PINK,BLACK);
							ili9341_settextsize(3);
							printf("H");
							cntr_Name = cntr_Name + 20;
							if(cntr_Name <= 305)
							{
								ili9341_drawRect((cntr_Name - 3), 16, 21, 28, GREEN);
							}
							start = 0;
							stopp = 0;
							Namen_speichern();
						}
						
					break;
					
					
					case 73:
						x = 149;
						y = 86;
						w = 25;
						h = 28;
						
						if(start == 0)
						{
							rechteck();
							start = 1;
						}
						
						if(UP)
						{
							UP = 0;
							rechteck_clear();
							Zustand_Neu_Name = 72;
							start = 0;
							
						}else if(DOWN)
						{
							DOWN = 0;
							rechteck_clear();
							Zustand_Neu_Name = 74;
							start = 0;
						}
						
						if(ENTER)
						{
							ENTER = 0;
							ili9341_drawRect((cntr_Name - 3), 16, 21, 28, BLACK);
							ili9341_setcursor(cntr_Name, 20);								
							ili9341_settextcolour(PINK,BLACK);
							ili9341_settextsize(3);
							printf("I");
							cntr_Name = cntr_Name + 20;
							if(cntr_Name <= 305)
							{
								ili9341_drawRect((cntr_Name - 3), 16, 21, 28, GREEN);
							}
							start = 0;
							stopp = 0;
							Namen_speichern();
						}
						
					break;


					case 74:
						x = 189;
						y = 86;
						w = 25;
						h = 28;
						
						if(start == 0)
						{
							rechteck();
							start = 1;
						}
						
						if(UP)
						{
							UP = 0;
							rechteck_clear();
							Zustand_Neu_Name = 73;
							start = 0;
							
						}else if(DOWN)
						{
							DOWN = 0;
							rechteck_clear();
							Zustand_Neu_Name = 75;
							start = 0;
						}
						
						if(ENTER)
						{
							ENTER = 0;
							ili9341_drawRect((cntr_Name - 3), 16, 21, 28, BLACK);
							ili9341_setcursor(cntr_Name, 20);								
							ili9341_settextcolour(PINK,BLACK);
							ili9341_settextsize(3);
							printf("J");
							cntr_Name = cntr_Name + 20;
							if(cntr_Name <= 305)
							{
								ili9341_drawRect((cntr_Name - 3), 16, 21, 28, GREEN);
							}
							start = 0;
							stopp = 0;
							Namen_speichern();
						}
						
					break;
					
					
					case 75:
						x = 229;
						y = 86;
						w = 25;
						h = 28;
						
						if(start == 0)
						{
							rechteck();
							start = 1;
						}
						
						if(UP)
						{
							UP = 0;
							rechteck_clear();
							Zustand_Neu_Name = 74;
							start = 0;
							
						}else if(DOWN)
						{
							DOWN = 0;
							rechteck_clear();
							Zustand_Neu_Name = 76;
							start = 0;
						}
						
						if(ENTER)
						{
							ENTER = 0;
							ili9341_drawRect((cntr_Name - 3), 16, 21, 28, BLACK);
							ili9341_setcursor(cntr_Name, 20);								
							ili9341_settextcolour(PINK,BLACK);
							ili9341_settextsize(3);
							printf("K");
							cntr_Name = cntr_Name + 20;
							if(cntr_Name <= 305)
							{
								ili9341_drawRect((cntr_Name - 3), 16, 21, 28, GREEN);
							}
							start = 0;
							stopp = 0;
							Namen_speichern();
						}
						
					break;
					
					
					case 76:
						x = 49;
						y = 116;
						w = 25;
						h = 28;
						
						if(start == 0)
						{
							rechteck();
							start = 1;
						}
						
						if(UP)
						{
							UP = 0;
							rechteck_clear();
							Zustand_Neu_Name = 75;
							start = 0;
							
						}else if(DOWN)
						{
							DOWN = 0;
							rechteck_clear();
							Zustand_Neu_Name = 77;
							start = 0;
						}
						
						if(ENTER)
						{
							ENTER = 0;
							ili9341_drawRect((cntr_Name - 3), 16, 21, 28, BLACK);
							ili9341_setcursor(cntr_Name, 20);								
							ili9341_settextcolour(PINK,BLACK);
							ili9341_settextsize(3);
							printf("L");
							cntr_Name = cntr_Name + 20;
							if(cntr_Name <= 305)
							{
								ili9341_drawRect((cntr_Name - 3), 16, 21, 28, GREEN);
							}
							start = 0;
							stopp = 0;
							Namen_speichern();
						}
						
					break;
					
					
					case 77:
						x = 89;
						y = 116;
						w = 25;
						h = 28;
						
						if(start == 0)
						{
							rechteck();
							start = 1;
						}
						
						if(UP)
						{
							UP = 0;
							rechteck_clear();
							Zustand_Neu_Name = 76;
							start = 0;
							
						}else if(DOWN)
						{
							DOWN = 0;
							rechteck_clear();
							Zustand_Neu_Name = 78;
							start = 0;
						}
						
						if(ENTER)
						{
							ENTER = 0;
							ili9341_drawRect((cntr_Name - 3), 16, 21, 28, BLACK);
							ili9341_setcursor(cntr_Name, 20);								
							ili9341_settextcolour(PINK,BLACK);
							ili9341_settextsize(3);
							printf("M");
							cntr_Name = cntr_Name + 20;
							if(cntr_Name <= 305)
							{
								ili9341_drawRect((cntr_Name - 3), 16, 21, 28, GREEN);
							}
							start = 0;
							stopp = 0;
							Namen_speichern();
						}
						
					break;
					
					
					case 78:
						x = 129;
						y = 116;
						w = 25;
						h = 28;
						
						if(start == 0)
						{
							rechteck();
							start = 1;
						}
						
						if(UP)
						{
							UP = 0;
							rechteck_clear();
							Zustand_Neu_Name = 77;
							start = 0;
							
						}else if(DOWN)
						{
							DOWN = 0;
							rechteck_clear();
							Zustand_Neu_Name = 79;
							start = 0;
						}
						
						if(ENTER)
						{
							ENTER = 0;
							ili9341_drawRect((cntr_Name - 3), 16, 21, 28, BLACK);
							ili9341_setcursor(cntr_Name, 20);								
							ili9341_settextcolour(PINK,BLACK);
							ili9341_settextsize(3);
							printf("N");
							cntr_Name = cntr_Name + 20;
							if(cntr_Name <= 305)
							{
								ili9341_drawRect((cntr_Name - 3), 16, 21, 28, GREEN);
							}
							start = 0;
							stopp = 0;
							Namen_speichern();
						}
						
					break;
					
					
					case 79:
						x = 169;
						y = 116;
						w = 25;
						h = 28;
						
						if(start == 0)
						{
							rechteck();
							start = 1;
						}
						
						if(UP)
						{
							UP = 0;
							rechteck_clear();
							Zustand_Neu_Name = 78;
							start = 0;
							
						}else if(DOWN)
						{
							DOWN = 0;
							rechteck_clear();
							Zustand_Neu_Name = 80;
							start = 0;
						}
						
						if(ENTER)
						{
							ENTER = 0;
							ili9341_drawRect((cntr_Name - 3), 16, 21, 28, BLACK);
							ili9341_setcursor(cntr_Name, 20);								
							ili9341_settextcolour(PINK,BLACK);
							ili9341_settextsize(3);
							printf("O");
							cntr_Name = cntr_Name + 20;
							if(cntr_Name <= 305)
							{
								ili9341_drawRect((cntr_Name - 3), 16, 21, 28, GREEN);
							}
							start = 0;
							stopp = 0;
							Namen_speichern();
						}
						
					break;
					
					
					case 80:
						x = 209;
						y = 116;
						w = 25;
						h = 28;
						
						if(start == 0)
						{
							rechteck();
							start = 1;
						}
						
						if(UP)
						{
							UP = 0;
							rechteck_clear();
							Zustand_Neu_Name = 79;
							start = 0;
							
						}else if(DOWN)
						{
							DOWN = 0;
							rechteck_clear();
							Zustand_Neu_Name = 81;
							start = 0;
						}
						
						if(ENTER)
						{
							ENTER = 0;
							ili9341_drawRect((cntr_Name - 3), 16, 21, 28, BLACK);
							ili9341_setcursor(cntr_Name, 20);								
							ili9341_settextcolour(PINK,BLACK);
							ili9341_settextsize(3);
							printf("P");
							cntr_Name = cntr_Name + 20;
							if(cntr_Name <= 305)
							{
								ili9341_drawRect((cntr_Name - 3), 16, 21, 28, GREEN);
							}
							start = 0;
							stopp = 0;
							Namen_speichern();
						}
						
					break;
					
					
					case 81:
						x = 249;
						y = 116;
						w = 25;
						h = 28;
						
						if(start == 0)
						{
							rechteck();
							start = 1;
						}
						
						if(UP)
						{
							UP = 0;
							rechteck_clear();
							Zustand_Neu_Name = 80;
							start = 0;
							
						}else if(DOWN)
						{
							DOWN = 0;
							rechteck_clear();
							Zustand_Neu_Name = 82;
							start = 0;
						}
						
						if(ENTER)
						{
							ENTER = 0;
							ili9341_drawRect((cntr_Name - 3), 16, 21, 28, BLACK);
							ili9341_setcursor(cntr_Name, 20);								
							ili9341_settextcolour(PINK,BLACK);
							ili9341_settextsize(3);
							printf("Q");
							cntr_Name = cntr_Name + 20;
							if(cntr_Name <= 305)
							{
								ili9341_drawRect((cntr_Name - 3), 16, 21, 28, GREEN);
							}
							start = 0;
							stopp = 0;
							Namen_speichern();
						}
						
					break;
					
					
					case 82:
						x = 69;
						y = 146;
						w = 25;
						h = 28;
						
						if(start == 0)
						{
							rechteck();
							start = 2;
						}
						
						if(UP)
						{
							UP = 0;
							rechteck_clear();
							Zustand_Neu_Name = 81;
							start = 0;
							
						}else if(DOWN)
						{
							DOWN = 0;
							rechteck_clear();
							Zustand_Neu_Name = 83;
							start = 0;
						}
						
						if(ENTER)
						{
							ENTER = 0;
							ili9341_drawRect((cntr_Name - 3), 16, 21, 28, BLACK);
							ili9341_setcursor(cntr_Name, 20);								
							ili9341_settextcolour(PINK,BLACK);
							ili9341_settextsize(3);
							printf("R");
							cntr_Name = cntr_Name + 20;
							if(cntr_Name <= 305)
							{
								ili9341_drawRect((cntr_Name - 3), 16, 21, 28, GREEN);
							}
							start = 0;
							stopp = 0;
							Namen_speichern();
						}
						
					break;
					
					
					case 83:
						x = 109;
						y = 146;
						w = 25;
						h = 28;
						
						if(start == 0)
						{
							rechteck();
							start = 1;
						}
						
						if(UP)
						{
							UP = 0;
							rechteck_clear();
							Zustand_Neu_Name = 82;
							start = 0;
							
						}else if(DOWN)
						{
							DOWN = 0;
							rechteck_clear();
							Zustand_Neu_Name = 84;
							start = 0;
						}
						
						if(ENTER)
						{
							ENTER = 0;
							ili9341_drawRect((cntr_Name - 3), 16, 21, 28, BLACK);
							ili9341_setcursor(cntr_Name, 20);								
							ili9341_settextcolour(PINK,BLACK);
							ili9341_settextsize(3);
							printf("S");
							cntr_Name = cntr_Name + 20;
							if(cntr_Name <= 305)
							{
								ili9341_drawRect((cntr_Name - 3), 16, 21, 28, GREEN);
							}
							start = 0;
							stopp = 0;
							Namen_speichern();
						}
						
					break;
					
					
					case 84:
						x = 149;
						y = 146;
						w = 25;
						h = 28;
						
						if(start == 0)
						{
							rechteck();
							start = 1;
						}
						
						if(UP)
						{
							UP = 0;
							rechteck_clear();
							Zustand_Neu_Name = 83;
							start = 0;
							
						}else if(DOWN)
						{
							DOWN = 0;
							rechteck_clear();
							Zustand_Neu_Name = 85;
							start = 0;
						}
						
						if(ENTER)
						{
							ENTER = 0;
							ili9341_drawRect((cntr_Name - 3), 16, 21, 28, BLACK);
							ili9341_setcursor(cntr_Name, 20);								
							ili9341_settextcolour(PINK,BLACK);
							ili9341_settextsize(3);
							printf("T");
							cntr_Name = cntr_Name + 20;
							if(cntr_Name <= 305)
							{
								ili9341_drawRect((cntr_Name - 3), 16, 21, 28, GREEN);
							}
							start = 0;
							stopp = 0;
							Namen_speichern();
						}
						
					break;
					
					
					case 85:
						x = 189;
						y = 146;
						w = 25;
						h = 28;
						
						if(start == 0)
						{
							rechteck();
							start = 1;
						}
						
						if(UP)
						{
							UP = 0;
							rechteck_clear();
							Zustand_Neu_Name = 84;
							start = 0;
							
						}else if(DOWN)
						{
							DOWN = 0;
							rechteck_clear();
							Zustand_Neu_Name = 86;
							start = 0;
						}
						
						if(ENTER)
						{
							ENTER = 0;
							ili9341_drawRect((cntr_Name - 3), 16, 21, 28, BLACK);
							ili9341_setcursor(cntr_Name, 20);								
							ili9341_settextcolour(PINK,BLACK);
							ili9341_settextsize(3);
							printf("U");
							cntr_Name = cntr_Name + 20;
							if(cntr_Name <= 305)
							{
								ili9341_drawRect((cntr_Name - 3), 16, 21, 28, GREEN);
							}
							start = 0;
							stopp = 0;
							Namen_speichern();
						}
						
					break;
					
					
					case 86:
						x = 229;
						y = 146;
						w = 25;
						h = 28;
						
						if(start == 0)
						{
							rechteck();
							start = 1;
						}
						
						if(UP)
						{
							UP = 0;
							rechteck_clear();
							Zustand_Neu_Name = 85;
							start = 0;
							
						}else if(DOWN)
						{
							DOWN = 0;
							rechteck_clear();
							Zustand_Neu_Name = 87;
							start = 0;
						}
						
						if(ENTER)
						{
							ENTER = 0;
							ili9341_drawRect((cntr_Name - 3), 16, 21, 28, BLACK);
							ili9341_setcursor(cntr_Name, 20);								
							ili9341_settextcolour(PINK,BLACK);
							ili9341_settextsize(3);
							printf("V");
							cntr_Name = cntr_Name + 20;
							if(cntr_Name <= 305)
							{
								ili9341_drawRect((cntr_Name - 3), 16, 21, 28, GREEN);
							}
							start = 0;
							stopp = 0;
							Namen_speichern();
						}
						
					break;
					
					
					case 87:
						x = 49;
						y = 176;
						w = 25;
						h = 28;
						
						if(start == 0)
						{
							rechteck();
							start = 1;
						}
						
						if(UP)
						{
							UP = 0;
							rechteck_clear();
							Zustand_Neu_Name = 86;
							start = 0;
							
						}else if(DOWN)
						{
							DOWN = 0;
							rechteck_clear();
							Zustand_Neu_Name = 88;
							start = 0;
						}
						
						if(ENTER)
						{
							ENTER = 0;
							ili9341_drawRect((cntr_Name - 3), 16, 21, 28, BLACK);
							ili9341_setcursor(cntr_Name, 20);								
							ili9341_settextcolour(PINK,BLACK);
							ili9341_settextsize(3);
							printf("W");
							cntr_Name = cntr_Name + 20;
							if(cntr_Name <= 305)
							{
								ili9341_drawRect((cntr_Name - 3), 16, 21, 28, GREEN);
							}
							start = 0;
							stopp = 0;
							Namen_speichern();
						}
						
					break;
					
					
					case 88:
						x = 89;
						y = 176;
						w = 25;
						h = 28;
						
						if(start == 0)
						{
							rechteck();
							start = 1;
						}
						
						if(UP)
						{
							UP = 0;
							rechteck_clear();
							Zustand_Neu_Name = 87;
							start = 0;
							
						}else if(DOWN)
						{
							DOWN = 0;
							rechteck_clear();
							Zustand_Neu_Name = 89;
							start = 0;
						}
						
						if(ENTER)
						{
							ENTER = 0;
							ili9341_drawRect((cntr_Name - 3), 16, 21, 28, BLACK);
							ili9341_setcursor(cntr_Name, 20);								
							ili9341_settextcolour(PINK,BLACK);
							ili9341_settextsize(3);
							printf("X");
							cntr_Name = cntr_Name + 20;
							if(cntr_Name <= 305)
							{
								ili9341_drawRect((cntr_Name - 3), 16, 21, 28, GREEN);
							}
							start = 0;
							stopp = 0;
							Namen_speichern();
						}
						
					break;
					
					
					case 89:
						x = 129;
						y = 176;
						w = 25;
						h = 28;
						
						if(start == 0)
						{
							rechteck();
							start = 1;
						}
						
						if(UP)
						{
							UP = 0;
							rechteck_clear();
							Zustand_Neu_Name = 88;
							start = 0;
							
						}else if(DOWN)
						{
							DOWN = 0;
							rechteck_clear();
							Zustand_Neu_Name = 90;
							start = 0;
						}
						
						if(ENTER)
						{
							ENTER = 0;
							ili9341_drawRect((cntr_Name - 3), 16, 21, 28, BLACK);
							ili9341_setcursor(cntr_Name, 20);								
							ili9341_settextcolour(PINK,BLACK);
							ili9341_settextsize(3);
							printf("Y");
							cntr_Name = cntr_Name + 20;
							if(cntr_Name <= 305)
							{
								ili9341_drawRect((cntr_Name - 3), 16, 21, 28, GREEN);
							}
							start = 0;
							stopp = 0;
							Namen_speichern();
						}
						
					break;
					
					
					case 90:
						x = 169;
						y = 176;
						w = 25;
						h = 28;
						
						if(start == 0)
						{
							rechteck();
							start = 1;
						}
						
						if(UP)
						{
							UP = 0;
							rechteck_clear();
							Zustand_Neu_Name = 89;
							start = 0;
							
						}else if(DOWN)
						{
							DOWN = 0;
							rechteck_clear();
							Zustand_Neu_Name = 91;
							start = 0;
						}
						
						if(ENTER)
						{
							ENTER = 0;
							ili9341_drawRect((cntr_Name - 3), 16, 21, 28, BLACK);
							ili9341_setcursor(cntr_Name, 20);								
							ili9341_settextcolour(PINK,BLACK);
							ili9341_settextsize(3);
							printf("Z");
							cntr_Name = cntr_Name + 20;
							if(cntr_Name <= 305)
							{
								ili9341_drawRect((cntr_Name - 3), 16, 21, 28, GREEN);
							}
							start = 0;
							stopp = 0;
							Namen_speichern();
						}
						
					break;
					
					
					case 91:
						x = 209;
						y = 176;
						w = 25;
						h = 28;
						
						if(start == 0)
						{
							rechteck();
							start = 1;
						}
						
						if(UP)
						{
							UP = 0;
							rechteck_clear();
							Zustand_Neu_Name = 90;
							start = 0;
							
						}else if(DOWN)
						{
							DOWN = 0;
							rechteck_clear();
							Zustand_Neu_Name = 92;
							start = 0;
						}
						
						if(ENTER)
						{
							ENTER = 0;
							if(cntr_Name > 115)
							{
								a = a - 1;
								Dateiname[a] = '\0';
								ili9341_drawRect((cntr_Name - 3), 16, 21, 28, BLACK);
								cntr_Name = cntr_Name - 20;
								ili9341_fillrect(cntr_Name, 20, 15, 21, BLACK);		// Buchstaben löschen (scwarzes Viereck einfügen)
								ili9341_drawRect((cntr_Name - 3), 16, 21, 28, GREEN);
								start = 0;
								stopp = 0;
							}
						}
						
					break;
					
					
					case 92:
						x = 249;
						y = 176;
						w = 43;
						h = 28;
						
						if(start == 0)
						{
							rechteck();
							start = 1;
						}
						
						if(UP)
						{
							UP = 0;
							rechteck_clear();
							Zustand_Neu_Name = 91;
							start = 0;
							
						}else if(DOWN)
						{
							DOWN = 0;
							rechteck_clear();
							Zustand_Neu_Name = 65;
							start = 0;
						}
						
						if(ENTER)
						{	
							ENTER = 0;
							start = 0;
							stopp = 1;
							Schreiben = 0;
							Namen_speichern();
							state = Neu;
							ili9341_clear(BLACK);
							ili9341_setcursor(cntr_Name, 20);								
						}
						
					break;
				}
			
			break;

			case Neu:
				if(Schreiben == 0)
				{
					Schreiben = 1;
					// Anzeige "Neue Konfig" *Titel*
					ili9341_setcursor(30, 10);								
					ili9341_settextcolour(YELLOW,BLACK);
					ili9341_settextsize(4);
					printf("Neue Konfig");
					
					ili9341_fillRect(3, 42, 315, 3, YELLOW);
							
							
					// Anzeige "Spannung"	
					ili9341_setcursor(89, 64);						
					ili9341_settextcolour(WHITE,BLACK);				 
					ili9341_settextsize(3);
					printf("Spannung");
							
							
					// Anzeige "Last"
					ili9341_setcursor(125, 108);							
					ili9341_settextcolour(WHITE,BLACK);				 
					ili9341_settextsize(3);
					printf("Last");
					
					// Anzeige "Datum"
					ili9341_setcursor(115, 152);						
					ili9341_settextcolour(WHITE,BLACK);				 
					ili9341_settextsize(3);
					printf("Datum");
				}		
					
					
				switch(Zustand_Neu)
				{
					case 1:
						x = 82;
						y = 59;
						w = 155;
						h = 32;
						rechteck();
						if(UP)
						{
							UP = 0;
							rechteck_clear();
							Zustand_Neu = 3;
							
						}else if(DOWN)
						{
							DOWN = 0;
							rechteck_clear();
							Zustand_Neu = 2;
						}
					
						if(ENTER)
						{
							ENTER = 0;
							ili9341_clear(BLACK);
							Schreiben = 0;
							Zustand_Neu = 1;
							state = Spannung;
						}
					break;
					
					
					case 2:
						x = 119;
						y = 103;
						w = 81;
						h = 32;
						rechteck();
						if(UP)
						{
							UP = 0;
							rechteck_clear();
							Zustand_Neu = 1;
							
						}else if(DOWN)
						{
							DOWN = 0;
							rechteck_clear();
							Zustand_Neu = 3;
						}
					
						if(ENTER)
						{
							ENTER = 0;
							ili9341_clear(BLACK);
							Schreiben = 0;
							Zustand_Neu = 1;
							state = Last;
						}
					break;
					
					
				case 3:
						x = 108;
						y = 147;
						w = 101;
						h = 32;
						rechteck();
						if(UP)
						{
							UP = 0;
							rechteck_clear();
							Zustand_Neu = 2;
							
						}else if(DOWN)
						{
							DOWN = 0;
							rechteck_clear();
							Zustand_Neu = 1;
						}
					
						if(ENTER)
						{
							ENTER = 0;
							ili9341_clear(BLACK);
							Schreiben = 0;
							Zustand_Neu = 1;
							state = Produktionsdatum;
						}
					break;
				}	
			break;
			
			case Bearbeiten:
				ili9341_setcursor(40, 152);						
				ili9341_settextcolour(WHITE,BLACK);				 
				ili9341_settextsize(3);
				printf("Alte Konfig");
			break;
			
			
			case Konfig_waehlen:
				if(Schreiben == 0)
				{
					Schreiben = 1;
					ili9341_setcursor(25, 10);								
					ili9341_settextcolour(WHITE,BLACK);
					ili9341_settextsize(4);
					printf("Konfig waehlen");
					
					
				}
			break;
			
			
			case Last:
			if(Schreiben == 0)
			{
				Schreiben = 1;
				ili9341_setcursor(115, 10);								
				ili9341_settextcolour(YELLOW,BLACK);
				ili9341_settextsize(4);
				printf("Last");
				
				ili9341_fillRect(3, 42, 315, 3, YELLOW);
				
				ili9341_setcursor(90, 90);								
				ili9341_settextcolour(WHITE,BLACK);
				ili9341_settextsize(3);
				printf("Mit Last");
				
				ili9341_setcursor(82, 150);								
				ili9341_settextcolour(WHITE,BLACK);
				ili9341_settextsize(3);
				printf("Ohne Last");
			}
			
			switch(Zustand_Last)
			{
				case 1:
					x = 85;
					y = 85;
					w = 151;
					h = 32;
					
					if(start == 0)
					{
						rechteck();
						start = 1;
					}
					
					if(UP)
					{
						UP = 0;
						rechteck_clear();
						Zustand_Last = 2;
						start = 0;
						
					}else if(DOWN)
					{
						DOWN = 0;
						rechteck_clear();
						Zustand_Last = 2;
						start = 0;
					}
					
					if(ENTER)
					{
						ENTER = 0;
						state = Mit_Last;
						ili9341_clear(BLACK);
						Schreiben = 0;
						Zustand_Last = 1;
						start = 0;
					}
					
				break;
				
				
				case 2:
					x = 77;
					y = 145;
					w = 169;
					h = 32;
					
					if(start == 0)
					{
						rechteck();
						start = 2;
					}
					
					if(UP)
					{
						UP = 0;
						rechteck_clear();
						Zustand_Last = 1;
						start = 0;
						
					}else if(DOWN)
					{
						DOWN = 0;
						rechteck_clear();
						Zustand_Last = 1;
						start = 0;
					}
					
					if(ENTER)
					{
						ENTER = 0;
						state = Neu;
						ili9341_clear(BLACK);
						Schreiben = 0;
						Zustand_Last = 1;
						start = 0;
					}
					
				break;
			}
			break;


			case Datum:
				ili9341_setcursor(60, 152);						
				ili9341_settextcolour(WHITE,BLACK);				 
				ili9341_settextsize(3);
				printf("Datum");
			break;
			
			case Produktionsdatum:
				if(Schreiben == 0)
				{
					Schreiben = 1;
					ili9341_setcursor(24, 20);						
					ili9341_settextcolour(YELLOW,BLACK);				 
					ili9341_settextsize(2);
					printf("Was wollen Sie aendern?");
					
					ili9341_setcursor(120, 80);						
					ili9341_settextcolour(WHITE,BLACK);				 
					ili9341_settextsize(3);
					printf("Jahr");
					
					ili9341_setcursor(45, 130);						
					ili9341_settextcolour(WHITE,BLACK);				 
					ili9341_settextsize(3);
					printf("Kalenderwoche");
				}
				
					switch(Zustand_Produktionsdatum)
					{
						case 1:
							x = 115;
							y = 75;
							w = 80;
							h = 32;
							
							if(start == 0)
							{
								rechteck();
								start = 1;
							}
							
							if(UP)
							{
								UP = 0;
								rechteck_clear();
								Zustand_Produktionsdatum = 2;
								start = 0;
								
							}else if(DOWN)
							{
								DOWN = 0;
								rechteck_clear();
								Zustand_Produktionsdatum = 2;
								start = 0;
							}
							
							if(ENTER)
							{
								ENTER = 0;
								Zustand_Produktionsdatum = 1;
								state = Produktionsjahr;
								ili9341_clear(BLACK);
								Schreiben = 0;
								start = 0;
							}
					
						break;
						
						
						case 2:
							x = 40;
							y = 125;
							w = 240;
							h = 32;
							
							if(start == 0)
							{
								rechteck();
								start = 1;
							}
							
							if(UP)
							{
								UP = 0;
								rechteck_clear();
								Zustand_Produktionsdatum = 1;
								start = 0;
								
							}else if(DOWN)
							{
								DOWN = 0;
								rechteck_clear();
								Zustand_Produktionsdatum = 1;
								start = 0;
							}
							
							if(ENTER)
							{
								ENTER = 0;
								Zustand_Produktionsdatum = 1;
								state = Kalenderwoche;
								ili9341_clear(BLACK);
								Schreiben = 0;
								start = 0;
							}
					
						break;
					}
			break;
			
			
		case Kalenderwoche:
		if(!abgeschlossen)
		{
			if((Schreiben == 0) && (!abgeschlossen))
				{
					ili9341_setcursor(25, 20);								
					ili9341_settextcolour(GREEN,BLACK);
					ili9341_settextsize(4);
					printf("KW");
								
					codefeld();
							
								
					// "1"
					ili9341_setcursor(116, 101);								
					ili9341_settextcolour(ORANGE,BLACK);
					ili9341_settextsize(3);
					printf("1");
					
								
					// "2"
					ili9341_setcursor(152, 101);								
					ili9341_settextcolour(ORANGE,BLACK);
					ili9341_settextsize(3);
					printf("2");
					
					
					// "3"
					ili9341_setcursor(188, 101);								
					ili9341_settextcolour(ORANGE,BLACK);
					ili9341_settextsize(3);
					printf("3");
					
					
					// "4"
					ili9341_setcursor(116, 134);								
					ili9341_settextcolour(ORANGE,BLACK);
					ili9341_settextsize(3);
					printf("4");
					
					
					// "5"
					ili9341_setcursor(152, 134);								
					ili9341_settextcolour(ORANGE,BLACK);
					ili9341_settextsize(3);
					printf("5");
					
					
					// "6"
					ili9341_setcursor(188, 134);								
					ili9341_settextcolour(ORANGE,BLACK);
					ili9341_settextsize(3);
					printf("6");
					
					
					// "7"
					ili9341_setcursor(116, 167);								
					ili9341_settextcolour(ORANGE,BLACK);
					ili9341_settextsize(3);
					printf("7");
					
					
					// "8"
					ili9341_setcursor(152, 167);								
					ili9341_settextcolour(ORANGE,BLACK);
					ili9341_settextsize(3);
					printf("8");
					
					
					// "9"
					ili9341_setcursor(188, 167);								
					ili9341_settextcolour(ORANGE,BLACK);
					ili9341_settextsize(3);
					printf("9");
					
					
					// "Delete"
					ili9341_setcursor(116, 200);								
					ili9341_settextcolour(ORANGE,BLACK);
					ili9341_settextsize(3);
					printf("%c", Delete);
					
					
					// "0"
					ili9341_setcursor(152, 200);								
					ili9341_settextcolour(ORANGE,BLACK);
					ili9341_settextsize(3);
					printf("0");
					
					Schreiben = 1;
				}
				
			
			switch(Zustand_Kalenderwoche)
			{
				case 1:
					x = code_x1;
					y = code_y1;
					w = 23;
					h = 30;
					
					if(start == 0)
					{
						rechteck();
						start = 1;
					}
					
					if(UP)
					{
						UP = 0;
						rechteck_clear();
						Zustand_Kalenderwoche = 0;
						start = 0;
					}
					
					if(DOWN)
					{
						DOWN = 0;
						rechteck_clear();
						Zustand_Kalenderwoche = 2;
						start = 0;
					}
					
					if(ENTER)
					{
						ENTER = 0;
						Status = 1;
						Kalendereingabe();
					}
				break;
				
				
				case 2:
					x = code_x2;
					y = code_y2;
					w = 23;
					h = 30;
					
					if(start == 0)
					{
						rechteck();
						start = 1;
					}
					
					if(UP)
					{
						UP = 0;
						rechteck_clear();
						Zustand_Kalenderwoche = 1;
						start = 0;
					}
					
					if(DOWN)
					{
						DOWN = 0;
						rechteck_clear();
						Zustand_Kalenderwoche = 3;
						start = 0;
					}
					
					if(ENTER)
					{
						ENTER = 0;
						Status = 2;
						Kalendereingabe();
					}
				break;
				
				
				case 3:
					x = code_x3;
					y = code_y3;
					w = 23;
					h = 30;
					
					if(start == 0)
					{
						rechteck();
						start = 1;
					}
					
					if(UP)
					{
						UP = 0;
						rechteck_clear();
						Zustand_Kalenderwoche = 2;
						start = 0;
					}
					
					if(DOWN)
					{
						DOWN = 0;
						rechteck_clear();
						Zustand_Kalenderwoche = 4;
						start = 0;
					}
					
					if(ENTER)
					{
						ENTER = 0;
						Status = 3;
						Kalendereingabe();
					}
				break;
				
				
				case 4:
					x = code_x4;
					y = code_y4;
					w = 23;
					h = 30;
					
					if(start == 0)
					{
						rechteck();
						start = 1;
					}
					
					if(UP)
					{
						UP = 0;
						rechteck_clear();
						Zustand_Kalenderwoche = 3;
						start = 0;
					}
					
					if(DOWN)
					{
						DOWN = 0;
						rechteck_clear();
						Zustand_Kalenderwoche = 5;
						start = 0;
					}
					
					if(ENTER)
					{
						ENTER = 0;
						Status = 4;
						Kalendereingabe();
					}
				break;
				
				
				case 5:
					x = code_x5;
					y = code_y5;
					w = 23;
					h = 30;
					
					if(start == 0)
					{
						rechteck();
						start = 1;
					}
					
					if(UP)
					{
						UP = 0;
						rechteck_clear();
						Zustand_Kalenderwoche = 4;
						start = 0;
					}
					
					if(DOWN)
					{
						DOWN = 0;
						rechteck_clear();
						Zustand_Kalenderwoche = 6;
						start = 0;
					}
					
					if(ENTER)
					{
						ENTER = 0;
						Status = 5;
						Kalendereingabe();
					}
				break;
				
				
				case 6:
					x = code_x6;
					y = code_y6;
					w = 23;
					h = 30;
					
					if(start == 0)
					{
						rechteck();
						start = 1;
					}
					
					if(UP)
					{
						UP = 0;
						rechteck_clear();
						Zustand_Kalenderwoche = 5;
						start = 0;
					}
					
					if(DOWN)
					{
						DOWN = 0;
						rechteck_clear();
						Zustand_Kalenderwoche = 7;
						start = 0;
					}
					
					if(ENTER)
					{
						ENTER = 0;
						Status = 6;
						Kalendereingabe();
					}
				break;
				
				
				case 7:
					x = code_x7;
					y = code_y7;
					w = 23;
					h = 30;
					
					if(start == 0)
					{
						rechteck();
						start = 1;
					}
					
					if(UP)
					{
						UP = 0;
						rechteck_clear();
						Zustand_Kalenderwoche = 6;
						start = 0;
					}
					
					if(DOWN)
					{
						DOWN = 0;
						rechteck_clear();
						Zustand_Kalenderwoche = 8;
						start = 0;
					}
					
					if(ENTER)
					{
						ENTER = 0;
						Status = 7;
						Kalendereingabe();
					}
				break;
				
				
				case 8:
					x = code_x8;
					y = code_y8;
					w = 23;
					h = 30;
					
					if(start == 0)
					{
						rechteck();
						start = 1;
					}
					
					if(UP)
					{
						UP = 0;
						rechteck_clear();
						Zustand_Kalenderwoche = 7;
						start = 0;
					}
					
					if(DOWN)
					{
						DOWN = 0;
						rechteck_clear();
						Zustand_Kalenderwoche = 9;
						start = 0;
					}
					
					if(ENTER)
					{
						ENTER = 0;
						Status = 8;
						Kalendereingabe();
					}
				break;
				
				
				case 9:
					x = code_x9;
					y = code_y9;
					w = 23;
					h = 30;
					
					if(start == 0)
					{
						rechteck();
						start = 1;
					}
					
					if(UP)
					{
						UP = 0;
						rechteck_clear();
						Zustand_Kalenderwoche = 8;
						start = 0;
					}
					
					if(DOWN)
					{
						DOWN = 0;
						rechteck_clear();
						Zustand_Kalenderwoche = 10;
						start = 0;
					}
					
					if(ENTER)
					{
						ENTER = 0;
						Status = 9;
						Kalendereingabe();
					}
				break;
				
				
				case 10:
					x = code_xd;
					y = code_yd;
					w = 23;
					h = 30;
					
					if(start == 0)
					{
						rechteck();
						start = 1;
					}
					
					if(UP)
					{
						UP = 0;
						rechteck_clear();
						Zustand_Kalenderwoche = 9;
						start = 0;
					}
					
					if(DOWN)
					{
						DOWN = 0;
						rechteck_clear();
						Zustand_Kalenderwoche = 0;
						start = 0;
					}
					
					if((ENTER) && (XPosition >= 110))
					{
						ENTER = 0;
						loeschen = 1;
						ili9341_fillRect(XPosition, 28, 20, 20, BLACK);
						XPosition = XPosition - 30;
						Durchgang3 = Durchgang3 - 1;
					}
				break;
				
				
				case 0:
					x = code_x0;
					y = code_y0;
					w = 23;
					h = 30;
					
					if(start == 0)
					{
						rechteck();
						start = 1;
					}
					
					if(UP)
					{
						UP = 0;
						rechteck_clear();
						Zustand_Kalenderwoche = 10;
						start = 0;
					}
					
					if(DOWN)
					{
						DOWN = 0;
						rechteck_clear();
						Zustand_Kalenderwoche = 1;
						start = 0;
					}
					
					if(ENTER)
					{
						ENTER = 0;
						Status = 0;
						Kalendereingabe();
					}
				break;
			}
		}else if(abgeschlossen)
		{
			abgeschlossen = 0;
			ili9341_clear(BLACK);
			state = Return_Datum;
		}
		break;
			
			
		case Produktionsjahr:
		if(!abgeschlossen)
		{
			if((Schreiben == 0) && (!abgeschlossen))
				{
					ili9341_setcursor(1, 20);								
					ili9341_settextcolour(GREEN,BLACK);
					ili9341_settextsize(4);
					printf("JAHR");
								
					codefeld();
							
								
					// "1"
					ili9341_setcursor(116, 101);								
					ili9341_settextcolour(ORANGE,BLACK);
					ili9341_settextsize(3);
					printf("1");
					
								
					// "2"
					ili9341_setcursor(152, 101);								
					ili9341_settextcolour(ORANGE,BLACK);
					ili9341_settextsize(3);
					printf("2");
					
					
					// "3"
					ili9341_setcursor(188, 101);								
					ili9341_settextcolour(ORANGE,BLACK);
					ili9341_settextsize(3);
					printf("3");
					
					
					// "4"
					ili9341_setcursor(116, 134);								
					ili9341_settextcolour(ORANGE,BLACK);
					ili9341_settextsize(3);
					printf("4");
					
					
					// "5"
					ili9341_setcursor(152, 134);								
					ili9341_settextcolour(ORANGE,BLACK);
					ili9341_settextsize(3);
					printf("5");
					
					
					// "6"
					ili9341_setcursor(188, 134);								
					ili9341_settextcolour(ORANGE,BLACK);
					ili9341_settextsize(3);
					printf("6");
					
					
					// "7"
					ili9341_setcursor(116, 167);								
					ili9341_settextcolour(ORANGE,BLACK);
					ili9341_settextsize(3);
					printf("7");
					
					
					// "8"
					ili9341_setcursor(152, 167);								
					ili9341_settextcolour(ORANGE,BLACK);
					ili9341_settextsize(3);
					printf("8");
					
					
					// "9"
					ili9341_setcursor(188, 167);								
					ili9341_settextcolour(ORANGE,BLACK);
					ili9341_settextsize(3);
					printf("9");
					
					
					// "Delete"
					ili9341_setcursor(116, 200);								
					ili9341_settextcolour(ORANGE,BLACK);
					ili9341_settextsize(3);
					printf("%c", Delete);
					
					
					// "0"
					ili9341_setcursor(152, 200);								
					ili9341_settextcolour(ORANGE,BLACK);
					ili9341_settextsize(3);
					printf("0");
					
					Schreiben = 1;
				}
				
				
			switch(Zustand_Produktionsjahr)
			{
				case 1:
					x = code_x1;
					y = code_y1;
					w = 23;
					h = 30;
					
					if(start == 0)
					{
						rechteck();
						start = 1;
					}
					
					if(UP)
					{
						rechteck_clear();
						UP = 0;
						start = 0;
						Zustand_Produktionsjahr = 0;
					}
					
					if(DOWN)
					{
						rechteck_clear();
						DOWN = 0;
						start = 0;
						Zustand_Produktionsjahr = 2;
					}
					
					if(ENTER)
					{
						ENTER = 0;
						Status = 1;
						Jahreseingabe();
					}
				break;
				
				
				case 2:
					x = code_x2;
					y = code_y2;
					w = 23;
					h = 30;
					
					if(start == 0)
					{
						rechteck();
						start = 1;
					}
					
					if(UP)
					{
						rechteck_clear();
						UP = 0;
						start = 0;
						Zustand_Produktionsjahr = 1;
					}
					
					if(DOWN)
					{
						rechteck_clear();
						DOWN = 0;
						start = 0;
						Zustand_Produktionsjahr = 3;
					}
					
					if(ENTER)
					{
						ENTER = 0;
						Status = 2;
						Jahreseingabe();
					}
				break;
				
				
				case 3:
					x = code_x3;
					y = code_y3;
					w = 23;
					h = 30;
					
					if(start == 0)
					{
						rechteck();
						start = 1;
					}
					
					if(UP)
					{
						rechteck_clear();
						UP = 0;
						start = 0;
						Zustand_Produktionsjahr = 2;
					}
					
					if(DOWN)
					{
						rechteck_clear();
						DOWN = 0;
						start = 0;
						Zustand_Produktionsjahr = 4;
					}
					
					if(ENTER)
					{
						ENTER = 0;
						Status = 3;
						Jahreseingabe();
					}
				break;
				
				
				case 4:
					x = code_x4;
					y = code_y4;
					w = 23;
					h = 30;
					
					if(start == 0)
					{
						rechteck();
						start = 1;
					}
					
					if(UP)
					{
						rechteck_clear();
						UP = 0;
						start = 0;
						Zustand_Produktionsjahr = 3;
					}
					
					if(DOWN)
					{
						rechteck_clear();
						DOWN = 0;
						start = 0;
						Zustand_Produktionsjahr = 5;
					}
					
					if(ENTER)
					{
						ENTER = 0;
						Status = 4;
						Jahreseingabe();
					}
				break;
				
				
				case 5:
					x = code_x5;
					y = code_y5;
					w = 23;
					h = 30;
					
					if(start == 0)
					{
						rechteck();
						start = 1;
					}
					
					if(UP)
					{
						rechteck_clear();
						UP = 0;
						start = 0;
						Zustand_Produktionsjahr = 4;
					}
					
					if(DOWN)
					{
						rechteck_clear();
						DOWN = 0;
						start = 0;
						Zustand_Produktionsjahr = 6;
					}
					
					if(ENTER)
					{
						ENTER = 0;
						Status = 5;
						Jahreseingabe();
					}
				break;
				
				
				case 6:
					x = code_x6;
					y = code_y6;
					w = 23;
					h = 30;
					
					if(start == 0)
					{
						rechteck();
						start = 1;
					}
					
					if(UP)
					{
						rechteck_clear();
						UP = 0;
						start = 0;
						Zustand_Produktionsjahr = 5;
					}
					
					if(DOWN)
					{
						rechteck_clear();
						DOWN = 0;
						start = 0;
						Zustand_Produktionsjahr = 7;
					}
					
					if(ENTER)
					{
						ENTER = 0;
						Status = 6;
						Jahreseingabe();
					}
				break;
				
				
				case 7:
					x = code_x7;
					y = code_y7;
					w = 23;
					h = 30;
					
					if(start == 0)
					{
						rechteck();
						start = 1;
					}
					
					if(UP)
					{
						rechteck_clear();
						UP = 0;
						start = 0;
						Zustand_Produktionsjahr = 6;
					}
					
					if(DOWN)
					{
						rechteck_clear();
						DOWN = 0;
						start = 0;
						Zustand_Produktionsjahr = 8;
					}
					
					if(ENTER)
					{
						ENTER = 0;
						Status = 7;
						Jahreseingabe();
					}
				break;
				
				
				case 8:
					x = code_x8;
					y = code_y8;
					w = 23;
					h = 30;
					
					if(start == 0)
					{
						rechteck();
						start = 1;
					}
					
					if(UP)
					{
						rechteck_clear();
						UP = 0;
						start = 0;
						Zustand_Produktionsjahr = 7;
					}
					
					if(DOWN)
					{
						rechteck_clear();
						DOWN = 0;
						start = 0;
						Zustand_Produktionsjahr = 9;
					}
					
					if(ENTER)
					{
						ENTER = 0;
						Status = 8;
						Jahreseingabe();
					}
				break;
				
				
				case 9:
					x = code_x9;
					y = code_y9;
					w = 23;
					h = 30;
					
					if(start == 0)
					{
						rechteck();
						start = 1;
					}
					
					if(UP)
					{
						rechteck_clear();
						UP = 0;
						start = 0;
						Zustand_Produktionsjahr = 8;
					}
					
					if(DOWN)
					{
						rechteck_clear();
						DOWN = 0;
						start = 0;
						Zustand_Produktionsjahr = 10;
					}
					
					if(ENTER)
					{
						ENTER = 0;
						Status = 9;
						Jahreseingabe();
					}
				break;
				
				
				case 10:
					x = code_xd;
					y = code_yd;
					w = 23;
					h = 30;
					
					if(start == 0)
					{
						rechteck();
						start = 1;
					}
					
					if(UP)
					{
						rechteck_clear();
						UP = 0;
						start = 0;
						Zustand_Produktionsjahr = 9;
					}
					
					if(DOWN)
					{
						rechteck_clear();
						DOWN = 0;
						start = 0;
						Zustand_Produktionsjahr = 0;
					}
					
					if((ENTER) && (XPosition >= 110))
					{
						ENTER = 0;
						loeschen = 1;
						ili9341_fillRect(XPosition, 28, 20, 20, BLACK);
						XPosition = XPosition - 30;
						Durchgang3 = Durchgang3 - 1;
					}
				break;
				
				
				case 0:
					x = code_x0;
					y = code_y0;
					w = 23;
					h = 30;
					
					if(start == 0)
					{
						rechteck();
						start = 1;
					}
					
					if(UP)
					{
						rechteck_clear();
						UP = 0;
						start = 0;
						Zustand_Produktionsjahr = 10;
					}
					
					if(DOWN)
					{
						rechteck_clear();
						DOWN = 0;
						start = 0;
						Zustand_Produktionsjahr = 1;
					}
					
					if(ENTER)
					{
						ENTER = 0;
						Status = 0;
						Jahreseingabe();
					}
				break;
			}
		}else if(abgeschlossen)
		{
			abgeschlossen = 0;
			ili9341_clear(BLACK);
			state = Return_Datum;
		}
		break;
			
			
			case Return_Datum:
				if(Schreiben == 0)
				{
					start = 0;
					Schreiben = 1;
					ili9341_setcursor(18, 50);								
					ili9341_settextcolour(WHITE,BLACK);
					ili9341_settextsize(2);
					printf("Konfig weiter bearbeiten");
					
					ili9341_setcursor(22, 115);								
					ili9341_settextcolour(WHITE,BLACK);
					ili9341_settextsize(2);
					printf("Datum weiter bearbeiten");	
					
					ili9341_setcursor(60, 180);								
					ili9341_settextcolour(WHITE,BLACK);
					ili9341_settextsize(2);
					printf("Konfig speichern");
				}
				
				switch(Zustand_Return_Datum)							//die Bedienung des Auswahl-Menus
				{
					case 1:
						x = 12;
						y = 45;
						w = 297;
						h = 25;
						
						if(start == 0)
						{
							rechteck();
							start = 1;
						}
						
						if(UP)
						{
							UP = 0;
							rechteck_clear();
							Zustand_Return_Datum = 3;
							start = 0;
							
						}
						
						if(DOWN)
						{
							DOWN = 0;
							rechteck_clear();
							Zustand_Return_Datum = 2;
							start = 0;
						}
						
						if(ENTER)
						{
							ENTER = 0;
							ili9341_clear(BLACK);
							start = 0;
							Schreiben = 0;
							Zustand_Return_Datum = 1;
							state = Neu;
						}
					break;
					
					
					case 2:
						x = 16;
						y = 110;
						w = 286;
						h = 25;
						
						if(start == 0)
						{
							rechteck();
							start = 1;
						}
						
						if(UP)
						{
							UP = 0;
							rechteck_clear();
							Zustand_Return_Datum = 1;
							start = 0;
						}
						
						if(DOWN)
						{
							DOWN = 0;
							rechteck_clear();
							Zustand_Return_Datum = 3;
							start = 0;
						}
						
						if(ENTER)
						{
							ENTER = 0;
							ili9341_clear(BLACK);
							Schreiben = 0;
							Zustand_Return_Datum = 1;
							start = 0;
							state = Produktionsdatum;
						}
					break;
					
					
					case 3:
						x = 54;
						y = 175;
						w = 202;
						h = 25;
						
						if(start == 0)
						{
							rechteck();
							start = 1;
						}
						
						if(UP)
						{
							UP = 0;
							rechteck_clear();
							Zustand_Return_Datum = 2;
							start = 0;
							
						}
						
						if(DOWN)
						{
							DOWN = 0;
							rechteck_clear();
							Zustand_Return_Datum = 1;
							start = 0;
						}
						
						if(ENTER)
						{
							ENTER = 0;
							ili9341_clear(BLACK);
							Schreiben = 0;
							Zustand_Return_Datum = 1;
							start = 0;
							Konfiguration_NR = Konfiguration_NR + 1;
							adapter = 0;
							state = Warten;
						}
					break;
				}
			break;
			
			
			case Return:
				if(Schreiben == 0)
				{
					Schreiben = 1;
					ili9341_setcursor(18, 75);								
					ili9341_settextcolour(WHITE,BLACK);
					ili9341_settextsize(2);
					printf("Konfig weiter bearbeiten");
					
					ili9341_setcursor(65, 140);								
					ili9341_settextcolour(WHITE,BLACK);
					ili9341_settextsize(2);
					printf("Konfig speichern");
				}
				
				switch(Zustand_Return)											//die Bedienung des Auswahl-Menus
				{
					case 1:
						x = 10;
						y = 70;
						w = 300;
						h = 25;
						
						if(start == 0)
						{
							rechteck();
							start = 2;
						}
						
						if(UP)
						{
							UP = 0;
							rechteck_clear();
							Zustand_Return = 2;
							start = 0;
							
						}
						
						if(DOWN)
						{
							DOWN = 0;
							rechteck_clear();
							Zustand_Return = 2;
							start = 0;
						}
						
						if(ENTER)
						{
							ENTER = 0;
							start = 0;
							Schreiben = 0;
							Zustand_Return = 1;
							ili9341_clear(BLACK);
							state = Neu;
						}
					break;
					
					
					case 2:
						x = 57;
						y = 135;
						w = 205;
						h = 25;
						
						if(start == 0)
						{
							rechteck();
							start = 1;
						}
						
						if(UP)
						{
							UP = 0;
							rechteck_clear();
							Zustand_Return = 1;
							start = 0;
							
						}
						
						if(DOWN)
						{
							DOWN = 0;
							rechteck_clear();
							Zustand_Return = 1;
							start = 0;
						}
						
						if(ENTER)
						{
							ENTER = 0;
							ili9341_clear(BLACK);
							Schreiben = 0;
							Zustand_Return = 1;
							start = 0;
							Konfiguration_NR = Konfiguration_NR + 1;
							state = Pruefmodus;
						}
					break;
				}
			break;
			
			
			case Mit_Last:
				if(Schreiben == 0)
				{
					Schreiben = 1;
					ili9341_setcursor(120, 10);								
					ili9341_settextcolour(YELLOW,BLACK);
					ili9341_settextsize(4);
					printf("Last");
					
					ili9341_fillRect(3, 42, 315, 3, YELLOW);
					
					x = 120;
					y = 90;
					w = 80;
					rechteck();
					
					ili9341_setcursor(60,94);								
					ili9341_settextcolour(ORANGE,BLACK);
					ili9341_settextsize(4);
					printf("%c",Minus);
					
					ili9341_setcursor(240,94);								
					ili9341_settextcolour(ORANGE,BLACK);
					ili9341_settextsize(4);
					printf("%c",Plus);
					
					ili9341_setcursor(143, 160);								
					ili9341_settextcolour(ORANGE,BLACK);
					ili9341_settextsize(3);
					printf("OK");
				}
			
			
				ili9341_setcursor(130,100);								
				ili9341_settextcolour(ORANGE,BLACK);
				ili9341_settextsize(2);
				printf("%dmA",Messstrom);
				
				if((Messstrom <= 110) && (UP) && (!stopp))
				{
					UP = 0;
					Messstrom = Messstrom + 10;
				}else if(Messstrom >= 120)
				{
					stopp = 1;
					Messstrom = 120;
				}
				
				if((Messstrom >= 10) && (DOWN) && (!stopp))
				{
					DOWN = 0;
					Messstrom = Messstrom - 10;
				}else if(Messstrom <= 10)
				{
					stopp = 1;
					Messstrom = 10;
				}
				
				if((Messstrom == 10) && (UP))
				{
					UP = 0;
					Messstrom = Messstrom + 10;
					stopp = 0;
				}
				
				if((Messstrom == 120) && (DOWN))
				{
					DOWN = 0;
					Messstrom = Messstrom - 10;
					stopp = 0;
				}
				
				if((Messstrom == 90) && (DOWN))
				{
					DOWN = 0;
					ili9341_fillRect(177, 100, 12, 18, BLACK);
				}
				
				if(ENTER)
				{	
					ENTER = 0;
					ili9341_clear(BLACK);				
					state = Return;
					Schreiben = 0;
				}
			break;
			
			
			case Spannung:
				if(Schreiben == 0)
				{
					Schreiben = 1;
					ili9341_setcursor(20, 10);								
					ili9341_settextcolour(YELLOW,BLACK);
					ili9341_settextsize(4);
					printf("Messspannung");
					
					ili9341_fillRect(3, 42, 315, 3, YELLOW);
					
					x = 120;
					y = 90;
					w = 80;
					rechteck();
					
					ili9341_setcursor(60,94);								
					ili9341_settextcolour(ORANGE,BLACK);
					ili9341_settextsize(4);
					printf("%c",Minus);
					
					ili9341_setcursor(240,94);								
					ili9341_settextcolour(ORANGE,BLACK);
					ili9341_settextsize(4);
					printf("%c",Plus);
					
					ili9341_setcursor(143, 160);								
					ili9341_settextcolour(ORANGE,BLACK);
					ili9341_settextsize(3);
					printf("OK");
				}
			
			
				ili9341_setcursor(130,100);								
				ili9341_settextcolour(ORANGE,BLACK);
				ili9341_settextsize(2);
				printf("%d.%dV",Messspannung_vor_Komma, Messspannung_nach_Komma);
			
				
				if((UP) && (Messspannung_nach_Komma <= 8) && (Messspannung_vor_Komma <= 29) && (!stopp))
				{
					UP = 0;
					Messspannung_nach_Komma++;
				}else if((UP) && (Messspannung_nach_Komma == 9))
				{
					stopp = 1;
					Messspannung_nach_Komma = 0;
					Messspannung_vor_Komma++;
					UP = 0;
					stopp = 0;
				}
				
				if((DOWN) && (Messspannung_nach_Komma >= 1) && (Messspannung_vor_Komma >= 1) && (!stopp))
				{
					DOWN = 0;
					Messspannung_nach_Komma--;
				}else if((DOWN) && (Messspannung_nach_Komma == 0))
				{
					stopp = 1;
					Messspannung_nach_Komma = 9;
					Messspannung_vor_Komma--;
					ili9341_fillRect(177, 100, 12, 18, BLACK);
					DOWN = 0;
					stopp = 0;
				}
				
				if((Messspannung_vor_Komma == 0) && (Messspannung_nach_Komma == 9))
				{
					stopp = 1;
					Messspannung_vor_Komma = 1;
					Messspannung_nach_Komma = 0;
				}
				
				if((Messspannung_vor_Komma == 1) && (Messspannung_nach_Komma == 0) && (UP))
				{
					UP = 0;
					stopp = 0;
				}
				
				if((Messspannung_vor_Komma == 30) && (Messspannung_nach_Komma == 1) && (UP))
				{
					stopp = 1;
					Messspannung_vor_Komma = 30;
					Messspannung_nach_Komma = 0;
				}
				
				if((Messspannung_vor_Komma == 30) && (Messspannung_nach_Komma == 0) && (DOWN))
				{
					DOWN = 0;
					stopp = 0;
				}
				
				if(ENTER)
				{
					ENTER = 0;
					spannung_u = Mili_voltage;	
					ili9341_clear(BLACK);				
					state = Return;
					Schreiben = 0;
				}
			break;
			
			
			case Speicher:
				ili9341_setcursor(60, 64);						
				ili9341_settextcolour(YELLOW,BLACK);				 
				ili9341_settextsize(3);
				printf("Speicher");
			break;
		} // end of switch
	} // end of while
} // end of main


void rechteck(void)
{
	ili9341_drawRect(x, y, w, h, GREEN); 	
	ili9341_drawRect(x-1, y-1, w+2, h+2, GREEN); 
	ili9341_drawRect(x-2, y-2, w+4, h+4, GREEN);
	ili9341_drawRect(x-3, y-3, w+6, h+6, GREEN);
}

void rechteck_clear(void)
{
	ili9341_drawRect(x, y, w, h, BLACK); 	
	ili9341_drawRect(x-1, y-1, w+2, h+2, BLACK); 
	ili9341_drawRect(x-2, y-2, w+4, h+4, BLACK);
	ili9341_drawRect(x-3, y-3, w+6, h+6, BLACK);
}

void codefeld(void)
{
	x = 100;
	y = 20;
	w = 120;
	h = 28;
	 	
	ili9341_drawRect(x-1, y-1, w+2, h+2, GREEN); 
	ili9341_drawRect(x-2, y-2, w+4, h+4, GREEN);
	ili9341_drawRect(x-3, y-3, w+6, h+6, GREEN);
}

void Zifferneingabe(void)	//Die Ansicht der Pin-Eingabe
{


  int x1[]={116,152,188,116,152,188,116,152,188};
  int y1[]={101,101,101,134,134,134,167,167,167};
  int a;

  /*  
		ili9341_setcursor(20, 20);								
		ili9341_settextcolour(GREEN,BLACK);
		ili9341_settextsize(4);
		printf("PIN");
  */
  print_at_lcd(20,20,GREEN,BLACK,4,"PIN");
		codefeld();
				
		for(a=0;a<9;a++){
		  print_at_lcd(x1[a],y1[a],ORANGE,BLACK,3,"%c",'1'+a);
		}
		/*
		// "1"
		ili9341_setcursor(116, 101);								
		ili9341_settextcolour(ORANGE,BLACK);
		ili9341_settextsize(3);
		printf("1");
		
					
		// "2"
		ili9341_setcursor(152, 101);								
		ili9341_settextcolour(ORANGE,BLACK);
		ili9341_settextsize(3);
		printf("2");
		
		
		// "3"
		ili9341_setcursor(188, 101);								
		ili9341_settextcolour(ORANGE,BLACK);
		ili9341_settextsize(3);
		printf("3");
		
		
		// "4"
		ili9341_setcursor(116, 134);								
		ili9341_settextcolour(ORANGE,BLACK);
		ili9341_settextsize(3);
		printf("4");
		
		
		// "5"
		ili9341_setcursor(152, 134);								
		ili9341_settextcolour(ORANGE,BLACK);
		ili9341_settextsize(3);
		printf("5");
		
		
		// "6"
		ili9341_setcursor(188, 134);								
		ili9341_settextcolour(ORANGE,BLACK);
		ili9341_settextsize(3);
		printf("6");
		
		
		// "7"
		ili9341_setcursor(116, 167);								
		ili9341_settextcolour(ORANGE,BLACK);
		ili9341_settextsize(3);
		printf("7");
		
		
		// "8"
		ili9341_setcursor(152, 167);								
		ili9341_settextcolour(ORANGE,BLACK);
		ili9341_settextsize(3);
		printf("8");
		
		
		// "9"
		ili9341_setcursor(188, 167);								
		ili9341_settextcolour(ORANGE,BLACK);
		ili9341_settextsize(3);
		printf("9");
		*/
		
		// "Delete"
		ili9341_setcursor(116, 200);								
		ili9341_settextcolour(ORANGE,BLACK);
		ili9341_settextsize(3);
		printf("%c", Delete);
		
		
		// "0"
		ili9341_setcursor(152, 200);								
		ili9341_settextcolour(ORANGE,BLACK);
		ili9341_settextsize(3);
		printf("0");

}

void Eingabecurser(void)
{
	if(Durchgang1 == 0)
			{
				Zifferneingabe();
				
				// auswaehlen
					// Start_Koordinaten fuer Rechteck definieren
					x = 112;
					y = 97;
					w = 23;
					h = 30;
					
					Durchgang1 = 1;
			}
			
			switch(Status)
				{
					case 1: 
					x = code_x1;
					y = code_y1;
					Pass = 1;
					rechteck();
					
					if(UP)
					{
						UP = 0;
						rechteck_clear();
						Status = 0;
						
					}else if(DOWN)
					{
						DOWN = 0;
						rechteck_clear();
						Status = 2;
					}
				break;
				
					case 2:
					x = code_x2;
					y = code_y2;
					Pass = 2;
					rechteck();
					
					if(UP)
					{
						UP = 0;
						rechteck_clear();
						Status = 1;
						
					}else if(DOWN)
					{
						DOWN = 0;
						rechteck_clear();
						Status = 3;
					}
				break;
					
					case 3:
					x = code_x3;
					y = code_y3;
					Pass = 3;
					rechteck();
					
					if(UP)
					{
						UP = 0;
						rechteck_clear();
						Status = 2;
						
					}else if(DOWN)
					{
						DOWN = 0;
						rechteck_clear();
						Status = 4;
					}
				break;
				
					case 4:
					x = code_x4;
					y = code_y4;
					Pass = 4;
					rechteck();
					
					if(UP)
					{
						UP = 0;
						rechteck_clear();
						Status = 3;
						
					}else if(DOWN)
					{
						DOWN = 0;
						rechteck_clear();
						Status = 5;
					}
				break;
				
				case 5:
				x = code_x5;
				y = code_y5;
				Pass = 5;
				rechteck();
				
					if(UP)
					{
						UP = 0;
						rechteck_clear();
						Status = 4;
						
					}else if(DOWN)
					{
						DOWN = 0;
						rechteck_clear();
						Status = 6;
					}
				break;
				
				case 6:
				x = code_x6;
				y = code_y6;
				Pass = 6;
				rechteck();
				
					if(UP)
					{
						UP = 0;
						rechteck_clear();
						Status = 5;
						
					}else if(DOWN)
					{
						DOWN = 0;
						rechteck_clear();
						Status = 7;
					}
				break;
				
				case 7:
				x = code_x7;
				y = code_y7;
				Pass = 7;
				rechteck();
				
					if(UP)
					{
						UP = 0;
						rechteck_clear();
						Status = 6;
						
					}else if(DOWN)
					{
						DOWN = 0;
						rechteck_clear();
						Status = 8;
					}
				break;
				
				case 8:
				x = code_x8;
				y = code_y8;
				Pass = 8;
				rechteck();
				
					if(UP)
					{
						UP = 0;
						rechteck_clear();
						Status = 7;
						
					}else if(DOWN)
					{
						DOWN = 0;
						rechteck_clear();
						Status = 9;
					}
				break;
				
				case 9:
				x = code_x9;
				y = code_y9;
				Pass = 9;
				rechteck();
				
					if(UP)
					{
						UP = 0;
						rechteck_clear();
						Status = 8;
						
					}else if(DOWN)
					{
						DOWN = 0;
						rechteck_clear();
						Status = 10;
					}
				break;
				
				case 10:
				x = code_xd;
				y = code_yd;
				Pass = 10;
				rechteck();
				
					if(UP)
					{
						UP = 0;
						rechteck_clear();
						Status = 9;
						
					}else if(DOWN)
					{
						DOWN = 0;
						rechteck_clear();
						Status = 0;
					}
				break;
				
				case 0:
				x = code_x0;
				y = code_y0;
				Pass = 0;
				rechteck();
				
					if(UP)
					{
						UP = 0;
						rechteck_clear();
						Status = 10;
						
					}else if(DOWN)
					{
						DOWN = 0;
						rechteck_clear();
						Status = 1;
					}
				break;
				
				}
}

void auswahl_anzeige(void)
{
	// Titel "Auswahl Menue"
	ili9341_setcursor(6, 10);								
	ili9341_settextcolour(YELLOW,BLACK);
	ili9341_settextsize(4);
	printf("Auswahl Menue");
					
	ili9341_fillRect(3, 42, 315, 3, YELLOW);
							
							
	// "Pin aendern"	
	ili9341_setcursor(62, 55);						
	ili9341_settextcolour(WHITE,BLACK);				 
	ili9341_settextsize(3);
	printf("Pin aendern");
							
							
	// "Ton"
	ili9341_setcursor(135, 99);						
	ili9341_settextcolour(WHITE,BLACK);				 
	ili9341_settextsize(3);
	printf("Ton");
							
							
	// "Speicher"		
	ili9341_setcursor(89, 143);						
	ili9341_settextcolour(WHITE,BLACK);				 
	ili9341_settextsize(3);
	printf("Speicher");
							
							
	// "Konfig"	
	ili9341_setcursor(107, 187);						
	ili9341_settextcolour(WHITE,BLACK);				 
	ili9341_settextsize(3);
	printf("Konfig");
}

void gut(void)
{
	PIEZZO_ON;
}

void schlecht(void)
{
	TRANSISTOR_ON;
	PIEZZO_ON;
	_delay_ms(25);
	PIEZZO_OFF;
	_delay_ms(20);
	PIEZZO_ON;
	_delay_ms(25);
	PIEZZO_OFF;
	_delay_ms(20);
	TRANSISTOR_OFF;
}

uint8_t auswahl(void)													//auswählen der angezeigten zeile
{
	if(ENTER && (x == zeile_x1))
	{
		ENTER = 0;
		ili9341_clear(BLACK);
		return 1;														//zeilenzahl wird zurückgegeben
	}
	if(ENTER && (x == zeile_x2))
	{
		ENTER = 0;
		ili9341_clear(BLACK);
		return 2;														//zeilenzahl wird zurückgegeben
	}
	if(ENTER && (x == zeile_x3))
	{
		ENTER = 0;
		ili9341_clear(BLACK);
		return 3;														//zeilenzahl wird zurückgegeben
	}
	if(ENTER && (x == zeile_x4))
	{
		ENTER = 0;
		ili9341_clear(BLACK);
		return 4;														//zeilenzahl wird zurückgegeben
	}
	
	return 0;
}

void Blinken(void)
{
					background = 3;
					ili9341_settextsize(3);
					ili9341_setcursor(12, 110);
					ili9341_settextcolour(BLACK, CYAN);
					printf(" Verarbeitung.   ");
					LED_BAT_ON;
					PIEZZO_ON;
					//~ _delay_ms(10);
					ili9341_setcursor(30, 110);
					printf("Verarbeitung..  ");
					LED_BAT_OFF;
					PIEZZO_OFF; 
					//~ _delay_ms(10);
					ili9341_setcursor(30, 110);
					printf("Verarbeitung... ");
					LED_BAT_ON;
					PIEZZO_ON;
					//~ _delay_ms(10);
					ili9341_setcursor(30, 110);
					printf("Verarbeitung.   ");
					LED_BAT_OFF;
					PIEZZO_OFF; 
					//~ _delay_ms(10);
					ili9341_setcursor(30, 110);
					printf("Verarbeitung..  ");
					LED_BAT_ON;
					PIEZZO_ON; 
					//~ _delay_ms(10);
					ili9341_setcursor(30, 110);
					printf("Verarbeitung... ");
					LED_BAT_OFF;
					PIEZZO_OFF; 
					//~ _delay_ms(10);
					ili9341_setcursor(30, 110);
					printf("Verarbeitung.   ");
					LED_BAT_ON;
					PIEZZO_ON; 
					//~ _delay_ms(10);
					ili9341_setcursor(30, 110);
					printf("Verarbeitung..  ");
					LED_BAT_OFF;
					PIEZZO_OFF; 
					//~ _delay_ms(10);
					ili9341_setcursor(30, 110);
					printf("Verarbeitung... ");
					LED_BAT_ON;
					PIEZZO_ON; 
					//~ _delay_ms(10);
					ili9341_setcursor(30, 110);
					printf("Verarbeitung.   ");
					LED_BAT_OFF;
					PIEZZO_OFF; 
					//~ _delay_ms(10);
					ili9341_setcursor(30, 110);
					printf("Verarbeitung..  ");
					LED_BAT_ON;
					PIEZZO_ON; 
					//~ _delay_ms(10);
					ili9341_setcursor(30, 110);
					printf("Verarbeitung... ");
					LED_BAT_OFF;
					PIEZZO_OFF; 
					//~ _delay_ms(10);
					ili9341_setcursor(30, 110);
					printf("Verarbeitung.   ");
					LED_BAT_ON;

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

void Jahreseingabe(void)
{
	if(Durchgang3 == 0)
	{
		XPosition = 110;
		Zahl_1 = Status;
		ili9341_setcursor(XPosition,28);								
		ili9341_settextcolour(ORANGE,BLACK);
		ili9341_settextsize(2);
		printf("%d\n",Status);
		Durchgang3++;
	}
	else if(Durchgang3 == 1)
	{
		XPosition = 140;
		Zahl_2 = Status;
		ili9341_setcursor(XPosition,28);								
		ili9341_settextcolour(ORANGE,BLACK);
		ili9341_settextsize(2);
		printf("%d\n",Status);
		Durchgang3++;
	}
	else if(Durchgang3 == 2)
	{
		XPosition = 170;
		Zahl_3 = Status;
		ili9341_setcursor(XPosition,28);								
		ili9341_settextcolour(ORANGE,BLACK);
		ili9341_settextsize(2);
		printf("%d\n",Status);
		Durchgang3++;
	}
	else if(Durchgang3 == 3)
	{
		XPosition = 200;
		Zahl_4 = Status;
		ili9341_setcursor(XPosition,28);								
		ili9341_settextcolour(ORANGE,BLACK);
		ili9341_settextsize(2);
		printf("%d\n",Status);
		Durchgang3++;
	}
	else if(Durchgang3 == 4)
	{
		Jahr = ((Zahl_1 * 1000) + (Zahl_2 * 100) + (Zahl_3 * 10) + (Zahl_4));
		eeprom_write_word((uint16_t*)12, Jahr);
		Durchgang3 = 0;
		XPosition = 110;
		ili9341_clear(BLACK);
		Zahl_1 = 0;
		Zahl_2 = 0;
		Zahl_3 = 0;
		Zahl_4 = 0;
		Schreiben = 0;
		start = 0;
		Zustand_Produktionsjahr = 1;
		abgeschlossen = 1;
	}
}

void Kalendereingabe(void)
{
	if(Durchgang3 == 0)
	{
		XPosition = 110;
		Zahl_1 = Status;
		ili9341_setcursor(XPosition,28);								
		ili9341_settextcolour(ORANGE,BLACK);
		ili9341_settextsize(2);
		printf("%d\n",Status);
		Durchgang3++;
	}
	else if(Durchgang3 == 1)
	{
		XPosition = 140;
		Zahl_2 = Status;
		ili9341_setcursor(XPosition,28);								
		ili9341_settextcolour(ORANGE,BLACK);
		ili9341_settextsize(2);
		printf("%d\n",Status);
		Durchgang3++;
	}
	
	else if(Durchgang3 == 2)
	{
		KA = ((Zahl_1 * 10) + (Zahl_2));
		eeprom_write_word((uint16_t*)8, KA);
		Durchgang3 = 0;
		XPosition = 110;
		ili9341_clear(BLACK);
		Zahl_1 = 0;
		Zahl_2 = 0;
		Schreiben = 0;
		start = 0;
		Zustand_Kalenderwoche = 1;
		abgeschlossen = 1;
	}
}		

void Namen_speichern(void)
{	
	switch(Name_Nr)
	{
		case 1:
			if((Durchgang4 == 0) && (!stopp))
			{
				N1_B1 = Zustand_Neu_Name;
				Durchgang4++;
				stopp = 1;
			}
			
			if((Durchgang4 == 1) && (!stopp))
			{
				N1_B2 = Zustand_Neu_Name;
				Durchgang4++;
				stopp = 1;
			}
			
			if((Durchgang4 == 2) && (!stopp))
			{
				N1_B3 = Zustand_Neu_Name;
				Durchgang4++;
				stopp = 1;
			}
			
			if((Durchgang4 == 3) && (!stopp))
			{
				N1_B4 = Zustand_Neu_Name;
				Durchgang4++;
				stopp = 1;
			}
			
			if((Durchgang4 == 4) && (!stopp))
			{
				N1_B5 = Zustand_Neu_Name;
				Durchgang4++;
				stopp = 1;
			}
			
			if((Durchgang4 == 5) && (!stopp))
			{
				N1_B6 = Zustand_Neu_Name;
				Durchgang4++;
				stopp = 1;
			}
			
			if((Durchgang4 == 6) && (!stopp))
			{
				N1_B7 = Zustand_Neu_Name;
				Durchgang4++;
				stopp = 1;
			}
			
			if((Durchgang4 == 7) && (!stopp))
			{
				N1_B8 = Zustand_Neu_Name;
				Durchgang4++;
				stopp = 1;
			}
			
			if((Durchgang4 == 8) && (!stopp))
			{
				N1_B9 = Zustand_Neu_Name;
				Durchgang4++;
				stopp = 1;
			}
			
			if((Durchgang4 == 9) && (!stopp))
			{
				N1_B10 = Zustand_Neu_Name;
				Durchgang4++;
				stopp = 1;
			}
			
			if((Durchgang4 == 10) && (!stopp))
			{
				stopp = 1;
			}
			
			if((stopp) && (Zustand_Neu_Name == 92))
			{
				stopp = 0;
				Zustand_Neu_Name = 65;
				snprintf(Name_1,sizeof Name_1,"%c%c%c%c%c%c%c%c%c%c", N1_B1, N1_B2, N1_B3, N1_B4, N1_B5, N1_B6, N1_B7, N1_B8, N1_B9, N1_B10);
				//eeprom_write_block((const void*)12, (void*)&Testausgabe, 2/*Name_1, 10*/);
				//eeprom_write_word((uint16_t *) 4, Name_1);
			}
		break;
		
		case 2:
			
		break;
		
		case 3:
			
		break;
		
		case 4:
			
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
			
		break;
		
		case 10:
			
		break;
		
		case 11:
			
		break;
		
		case 12:
			
		break;
		
		case 13:
			
		break;
		
		case 14:
			
		break;
		
		case 15:
			
		break;
		
		case 16:
			
		break;
		
		case 17:
			
		break;
		
		case 18:
			
		break;
		
		case 19:
			
		break;
		
		case 20:
			
		break;
	}
}

