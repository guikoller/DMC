#include <stdio.h>
#include <stdlib.h>

#pragma config OSC=HS,WDT=OFF,LVP=OFF,CCP2MUX=OFF  // HS Oszillator, Watchdog Timer disabled, Low Voltage Programming

// Define für LCD des neuen, grünen Demo-Boards:
//#define NEUE_PLATINE  // Achtung: define vor include! Bei altem braunem Demo-Board auskommentieren!

#include "p18f452.h"
#include "lcd.h"


unsigned char cm_text[]="cm";
unsigned char no_obj_text[]="--              ";
unsigned char zeit_text[]="Zeit: ";
unsigned char colon_text[]=":";
unsigned char abstand_text[]="Abstand: ";

unsigned int abstand=0; 			// Abstand des Objekts
unsigned char vorzaehler=0;
unsigned char Stunde=23;
unsigned char Minute=59;
unsigned char Sekunde=55;


void high_prior_InterruptHandler (void);
void low_prior_InterruptHandler (void);


#pragma code high_prior_InterruptVector = 0x08
void high_prior_InterruptVector(void)
{
	_asm
			goto high_prior_InterruptHandler
	_endasm
}


#pragma code low_prior_InterruptVector = 0x18
void low_prior_InterruptVector(void)
{
	_asm
			goto low_prior_InterruptHandler
	_endasm
}


#pragma code init = 0x30
void init (void)
{
	lcd_init();
	lcd_clear();

	// weiter siehe Flussdiagramm ...
}


// hochpriorisierte ISR:
// Messung der Dauer des Echoimpulses (an RB3/CCP2) durch Timer 1.
// Steigende Flanke an RB3/CCP2: Capture-Wert speichern; Fallende Flanke: Abstand berechnen
#pragma code
#pragma interrupt high_prior_InterruptHandler
void high_prior_InterruptHandler(void)
{
	// weiter siehe Flussdiagramm ...
}


// niedrigpriorisierte ISR:
// 100ms-Intervalle von Timer 3 verwenden, um die Abstandsmessung darzustellen.
// Die Intervalle dienen zugleich als Zeitbasis für die Uhr.
#pragma code
#pragma interruptlow low_prior_InterruptHandler
void low_prior_InterruptHandler(void)
{
	// weiter siehe Flussdiagramm ...
}


void main() {
	init();
	while(1);
}
