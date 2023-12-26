// Hochschule Mannheim / Institut f�r Mikrocomputertechnik und Embedded Systems
//
// Versuch: PIC2  DA-Wandler durch PWM    Dateiname: PIC2_PWM.c
//
// Eine am Analogeingang RA0/AN0 vorgegebene Spannung wird digitalisiert,
// der Wert AnalogIn=xxxx am LCD angezeigt
// und �ber eine Pulsweitenmodulation am Ausgang RC2/CCP1 ausgegeben.
// Das dort angeschlossene RC-Glied macht daraus wieder eine Analogspannung,
// die am Eingang RE2/AN7 eingelesen und als Istwert AnalogOut=yyyy angezeigt wird.
//
// 08.12.2011 (Poh) Kommentare f�r LCD (Prototypen in lcd.h)
// 24.05.2011 (Poh) Configuration Bit Settings, Anpassungen f�r NEUE_PLATINE, Includes im Projektverzeichnis
// 07.06.2020 (Poh) #ifdef Umschaltung zwischen Simulation und Hardware mit LCD
// 30.06.2020 (Poh) umstrukturiert / AD-Wandlungsstart 1x
//
// Name/Gruppe:
//

#pragma config OSC = HS, WDT = OFF, LVP = OFF // HS Oszillator, Watchdog Timer disabled, Low Voltage Programming

#define Simulator // zum Gebrauch mit Hardware auskommentieren
// Define f�r LCD des neuen, gr�nen Demo-Boards:
// #define NEUE_PLATINE  // Achtung: define vor include! Bei altem braunem Demo-Board auskommentieren!
#include "p18f452.h"
#include "lcd.h" // Enth�lt alle Prototypen f�r das LCD

void init();

unsigned int x = 0;							   // Analogwert AN0 (Vorgabe durch Poti)
unsigned int y = 0;							   // Analogwert AN7 (Istwert des PWM-Mittelwerts am RC-Glied)
unsigned char LCDtext1[20] = "AnalogIn ="; // Analogwert AN0 (Poti)  16 Zeichen pro Zeile
unsigned char LCDtext2[20] = "AnalogOut="; // Analogwert AN7 (Istwert des PWM-Mittelwerts am RC-Glied)
unsigned char leer[] = "                ";

void init()
{
	// IO Ports

	TRISAbits.TRISA0=1;		//RA0 als Eingang für Poti
	TRISEbits.TRISE2=1;		
	TRISCbits.TRISC2=0;		// RC2 als Ausgang für PWM

	// TRISC = 0xFB; // 0b11111011;
	// TRISE = 0x7;  // 0b111;

	// // RC2 als Ausgang für PWM
	// TRISC2 = 0;

#ifndef Simulator // LCD-Initialisierung mit Portzuweisung RA<3:1> und RD<3:0>
	lcd_init();	  // Alle LCD-Funktionen werden f�r die Simulation herausgenommen,
	lcd_clear();  // da man sonst hier stecken bleibt.
#endif

	// CCP1 als PWM Modul konfigurieren
	CCP1CON = 0b00001100;

	// Timer 2 Einstellungen 1:4
	T2CON = 0b00000110;
	TMR2 = 0xFF // PWM Frequenz einstellen

	// A/D-Umsetzer Einstellungen
	ADCON0 = 0x81;
	// 0000 1110
	ADCON1 = 0x0E;
}

void main()
{
	init();
	while (1)
	{
		// A/D-Umsetzung durchf�hren
		ADCON0bits.GO = 1; // Start A/D conversion
		while (ADCON0bits.GO); // Wait for conversion to finish

		// A/D-Converter: Werteverarbeitung Kanal 0 oder 7
		// Analogkanal 0 wurde eingelesen (Poti Sollwert)
		if (!ADCON0bits.CHS2 && !ADCON0bits.CHS1 && !ADCON0bits.CHS0)
		{
			// Berechnung von x
			x = ADRES >> 6;

			// Duty Cycle für PWM  einstellen
			CCPR1L = ADRESH;


			CCP1CON = CCP1CON | (ADRESL >> 2); //CCp1con hat die LSB von 10bit PWM signal, dann ADRESL als maske verwenden!
			T2CONbits.TMR2ON=1;

			// Kanal 7 auswählen
			ADCON0 = 0b10111001;		//Fosc/32 und AN7 und ADON = 1
		}
		// Analogkanal 7 wurde eingelesen (RC-Ausgang Istwert)
		else if (ADCON0bits.CHS2 && ADCON0bits.CHS1 && ADCON0bits.CHS0)
		{
			// Berechnung von y
			y = ADRES >> 6;

			sprintf(LCDtext1, (const far rom char*)"AIn :%#5X %4dd", x,x);
			sprintf(LCDtext2, (const far rom char*)"AOut :%#5X %4dd", y,y);

			// Kanal 0 auswählen
			ADCON0 = 0b10000001;
		}

#ifndef Simulator
		// Hardware: Ausgabe an LCD
		// Port Konfiguration
		ADCON1 = 0b00001110; // RA3:RA1 wieder digital I/O für LCD, nur AN0-Eingang analog

		lcd_gotoxy(1, 1);
		lcd_printf(LCDtext1);
		lcd_gotoxy(2, 1);
		lcd_printf(LCDtext2);

		// Port Konfiguration AAAA AAAA
		ADCON1 = 0b00000000;

#else // Simulation: PWM-Periode abwarten + Haltepunkt bei bestimmtem Analogwert erm�glichen
		while (!PIR1bits.TMR2IF)
			;				 // Eine Periode der PWM (Timer 2) abwarten
		PIR1bits.TMR2IF = 0; // bis der n�chste Analogwert gelesen wird.

		if (y == 0x1FA)
		{		   // Letzter aus "Stimulus ADRESL pic2pwm.txt" zu lesender AD-Wert
			Nop(); // <-- hier einen Haltepunkt zum Anhalten nach einem Datenzyklus setzten!
		}		   // Ohne Haltepunkt wird die Injaktionsdatei zyklisch wiederholt gelesen.
#endif
	}
}
