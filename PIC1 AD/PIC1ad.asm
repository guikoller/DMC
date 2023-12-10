; Dateiname:
; Beschreibung:
;
;
; Entwickler:
; Version: 1.0    Datum:

	list p=18f452         ; Select Processor for Assembler
	#include <p18f452.inc> ; Registers / ASM Include File in Search Path  C:\Program Files (x86)\Microchip\MPASM Suite

	config OSC=HS, WDT=OFF, LVP=OFF ; Configuration Bits - defined in include file
; HS Oszillator, Watchdog Timer disabled, Low Voltage Programming

BANK0 EQU 0x000
BANK1 EQU 0x100
BANK2 EQU 0x200
BANK3 EQU 0x300
BANK4 EQU 0x400
BANK5 EQU 0x500

; ***** Variables *****
DelayCount EQU 0x21 ; Declare DelayCount as a variable
; Bank0
; Bank1
; Bank2
; Bank4
; Bank5


; ***** Vector Table *****
	ORG 0x00
	GOTO Init

	ORG 0x08
IntVectHigh
; No Interrupts Enabled

	ORG 0x18
IntVectLow
; No Interrupts Enabled

; ***** Main Program *****
	ORG 0x30
; Initialization Code
Init
	BSF TRISA, 4
	
	MOVLW	0xF0		; RB3..RB0 als Ausgang
	CLRF	PORTB		; clear output latch
	MOVWF	TRISB		; setze Pins RB<3:0> als Ausgang

	BSF PORTC,RC2  ; set output latch for Lautsprecher
	BCF TRISC,TRISC2 ; RC2 as Ausgang
	
	MOVLW 0x81
	MOVWF ADCON0 ; Configure A/D inputs

	MOVLW 0x0E
	MOVWF ADCON1

	CLRF DelayCount  ; Initialize RC2 state variable

MainLoop

Taster
	BTFSC PORTA, 4
	BRA Taster

	BSF ADCON0, GO
loop
	BTFSC ADCON0, GO
	BRA loop

	SWAPF ADRESH, 0

	MOVWF PORTB

	BTG PORTC, RC2 

Delay
    MOVF ADRESH, WREG   ; Use the ADC value for delay
    MOVWF DelayCount    ; Store the ADC value in DelayCount
	INCF DelayCount 

DelayLoop
    DECFSZ DelayCount, F
    BRA DelayLoop

	BRA MainLoop

END