// Hochschule Mannheim
// Institut für Embedded Systems
// PIC3_Ultraschall_ccp: Entfernungsmessung per Ultraschall + Echtzeituhr
// 13.12.2023 (Poh) sprintf / char LCDtext1+2

#include <stdio.h>
#include <stdlib.h>

#pragma config OSC=HS,WDT=OFF,LVP=OFF,CCP2MUX=OFF  // HS Oszillator, Watchdog Timer disabled, Low Voltage Programming

#define Simulator        // zum Gebrauch mit Hardware auskommentieren
// Define für LCD des neuen, grünen Demo-Boards:
//#define NEUE_PLATINE  // Achtung: define vor include! Bei altem braunem Demo-Board auskommentieren!

#include "p18f452.h"
#include "lcd.h"

//   LCD-Strings: "1234567890123456"   // 16 Zeichen/Zeile
char LCDtext1[20]=" Interrupt      ";  // LCD Zeile 1: Abstand Ultraschallsensor
char LCDtext2[20]=" Ultraschall    ";  // LCD Zeile 2: Echtzeituhr

unsigned int  Abstand=0; 		// Abstand des Objekts

unsigned char Vorzaehler=0;		// Uhrenvariablen:
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
#ifndef Simulator	// LCD-Initialisierung mit Portzuweisung RA<3:1> und RD<3:0>
	lcd_init();
	lcd_clear();
#endif

	// weiter siehe Flussdiagramm ...
}


// hochpriorisierte ISR:
// Messung der Dauer des Echoimpulses (an RB3) durch CCP2 Modul
// Steigende Flanke an RB3/CCP2: Capture-Wert speichern
// Fallende Flanke: Differenz mit gespeichertem Wert bilden
#pragma code
#pragma interrupt high_prior_InterruptHandler
void high_prior_InterruptHandler(void)
{
	// Siehe Flussdiagramm:
	
}


// niedrigpriorisierte ISR:
// 100ms-Intervalle von Timer 3 verwenden, um die Abstandsmessung darzustellen.
// Die Intervalle dienen zugleich als Zeitbasis für die Uhr.
#pragma code
#pragma interruptlow low_prior_InterruptHandler
void low_prior_InterruptHandler(void)
{
	// Siehe Flussdiagramm:
	// Startwert für 100ms Intervalle in Timer3 laden
	
	//if(Abstand != ... )	// Timer Überlauf?
		sprintf(LCDtext1, (const far rom char*)"Abstand: %3dcm  ", Abstand);  // Abstand anzeigen
	//else
		sprintf(LCDtext1, (const far rom char*)"Abstand: ---    ");  // kein Messwert vorhanden
	
#ifndef Simulator
	lcd_gotoxy(1,1);		// LCD Zeile 1 ausgeben:
	lcd_printf(LCDtext1);	// LCDtext1 Abstand: ...
#endif

	// Zählung der Echtzeit-Uhr

		// Jede Sekunde die Uhrzeit anzeigen
		sprintf(LCDtext2, (const far rom char*)"Zeit: %02d:%02d:%02d  ", Stunde, Minute, Sekunde);

#ifndef Simulator
		// LCD Zeile 2 ausgeben:
		// LCDtext2 Zeit: ...
#endif

	// weiter siehe Flussdiagramm ...


}
void main() {
	init();
	while(1);
}
