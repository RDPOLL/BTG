//-------------------------------Header---------------------------------
//Autor:			Justin Rhyner
//Datum:			18.04.2019
//Ort:				8600, Duebendorf
//Dateiname:		I2C.c
//Funktion:			Dies iste eine Bibliothek zur ansteuerung von
//					Bauteilen welche ueber I2C angesprochen werden.
//----------------------------------------------------------------------


#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>

void TWIStart(void)														//Funktin: Starten von I2C
{
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);								//Bits im Kontrollregister setzen
    while ((TWCR & (1<<TWINT)) == 0);
}

void TWIStop(void)														//Funktin: Stoppen von I2C
{
    TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN);								//Bits im Kontrollregister setzen
}
	
void TWIWrite(uint8_t u8data)											//Funktin: Daten über I2C senden
{ 
    TWDR = u8data;
    TWCR = (1<<TWINT)|(1<<TWEN);										//Bits im Kontrollregister setzen
    while ((TWCR & (1<<TWINT)) == 0);
}
	
uint8_t TWIGetStatus(void)												//Funktin: Status von I2C auslesen
{
	uint8_t status = 0;													//Siegelvariabel für I2C Status
    status = TWSR & 0xF8;
    return status;														//Status wird zurückgegeben
}
	

uint8_t TWIReadACK(void)												//Funktin: Daten über I2C auslesen
{
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);								//Bits im Kontrollregister setzen
    while ((TWCR & (1<<TWINT)) == 0);
    return TWDR;														//Rückgabewert TWDR (TWI Data Register)
}


uint8_t TWIReadNACK(void)												//Funktin: Daten über I2C auslesen
{
    TWCR = (1<<TWINT)|(1<<TWEN);										//Bits im Kontrollregister setzen
    while ((TWCR & (1<<TWINT)) == 0);
    return TWDR;														//Rückgabewert TWDR (TWI Data Register)
}

