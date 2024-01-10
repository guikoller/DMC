// Hochschule Mannheim
// Institut f�r Embedded Systems
// PIC3_Ultraschall_ccp: Entfernungsmessung per Ultraschall + Echtzeituhr
// 13.12.2023 (Poh) sprintf / char LCDtext1+2

#include <stdio.h>
#include <stdlib.h>

#pragma config OSC=HS,WDT=OFF,LVP=OFF,CCP2MUX=OFF  // HS Oszillator, Watchdog Timer disabled, Low Voltage Programming

#define Simulator        // zum Gebrauch mit Hardware auskommentieren
// Define f�r LCD des neuen, gr�nen Demo-Boards:
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

void time_update(void);	

void high_prior_InterruptHandler (void);
void low_prior_InterruptHandler (void);

void time_update(void)
{	
	if(Sekunde==60)				// bei 60 Sekunden
	{
		Sekunde=0;				// Sekunden auf 0 setzen
		Minute++;				// Minuten hochz�hlen
		if(Minute==60)			// bei 60 Minuten
		{
			Minute=0;			// Minuten auf 0 setzen
			Stunde++;			// Stunden hochz�hlen
			if(Stunde==24)		// bei 24 Stunden
				Stunde=0;		// Stunden auf 0 setzen
		}
	}
	Sekunde++;					// Sekunden hochz�hlen
}

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
	Sekunde=0;		// Uhrzeit initialisieren
	Minute=0;
	Stunde=0;

	//lcd initialisieren
	//PORTB und RB1 aus Ausgang
	TRISB = 0x00;
	TRISBbits.TRISB1 = 0;

	//Timer1 16 bit zugriff kein prescaler 0b10000000
	T1CON = 0x80;

	//TIMER3 to konfigurieren, dass 100ms Intervalle entstehen und TIMER1 für den CPP2-Capture verwendet wird
	// Timer3 ON, 16-bit mode, 1:8 prescaler
	T3CON = 0x30; 

	// Set Timer3 period to 12500 cycles (100ms at 1MHz with 1:8 prescaler)
	TMR3H = 0x30; // High byte
	TMR3L = 0xD4; // Low byte

	//CPP2 Modul im Capture-Modus betreiben 0000 0101
	CCP2CON = 0x05;

	//Interrupt configurieren 
	RCONbits.IPEN = 1; // Enable priority levels on interrupts
	INTCONbits.GIEH = 1; // Enable high-priority interrupts
	INTCONbits.GIEL = 1; // Enable low-priority interrupts

	//CPP2-Capture Ereignis: hohe Priorität
	IPR2bits.CCP2IP = 1; // CCP2 interrupt high priority
	//TIMER3 niedrige Priorität
	IPR2bits.TMR3IP = 0; // Timer3 interrupt low priority

	T1CONbits.TMR1ON = 1; // Timer1 starten
	T3CONbits.TMR3ON = 1; // Timer3 starten

	//global interrupt enable
	INTCONbits.GIE = 1; // Enable all unmasked interrupts
	INTCONbits.PEIE = 1; // Enable all unmasked peripheral interrupts

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
// Die Intervalle dienen zugleich als Zeitbasis f�r die Uhr.
#pragma code
#pragma interruptlow low_prior_InterruptHandler
void low_prior_InterruptHandler(void)
{
	// Siehe Flussdiagramm:
	// Startwert f�r 100ms Intervalle in Timer3 laden

	
	//if(Abstand != ... )	// Timer �berlauf?

		sprintf(LCDtext1, (const far rom char*)"Abstand: %3dcm  ", Abstand);  // Abstand anzeigen
	//else
		sprintf(LCDtext1, (const far rom char*)"Abstand: ---    ");  // kein Messwert vorhanden
	
#ifndef Simulator
	lcd_gotoxy(1,1);		// LCD Zeile 1 ausgeben:
	lcd_printf(LCDtext1);	// LCDtext1 Abstand: ...
#endif

	// Z�hlung der Echtzeit-Uhr
	time_update();
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
