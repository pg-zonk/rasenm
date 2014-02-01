//Projekt: Rasenmäher
//Hardwareversion: 3.0
//Softwareversion: -- (Entwurfsversion)
//MCU: ATmega16
//Teil: Funktionsblock
//fCPU = 1MHz

//Kurzbeschrieb
//---------------

//Softwareinhalt:		Auswertung des Servosignals, vom Fernsteuerempf. FS-R6A kommend.
//						--> Die hier entwickelten Funktionen sollen später in der
//							Fahrzeugsoftware implementiert werden. Es geht im wesentlichen
//							darum die Auswertmethode mit einem Multiplexer zu testen

#include <avr/io.h>
#include <avr/interrupt.h>
#include "AuswertungFernst.h"

///////////////////////////////////////////////////////////////////////////////////////////////
//Initialisierung

void init(void)
{
	//IOs initialisieren
	DDRA = 0xFF;				//PortA = Ausgang (Verbinden mit LEDs)
	DDRB = 0x00;				//PortB = Eingang (Für Anschluss der Wahlschalter)
	DDRC = 0xFF;				//PortC = Ausgang (PC5 - PC7 Kanalvorwahl auf Multiplexer)
	DDRD = 0x00;				//PortD = Eingang (Es wird nur der INT0 Pin gebraucht)
	
	
}

void startRemoteIn(void)
{
	if(!(RemoteStatus & (1<<Busy)))				//Beginne zu Messen, sofern keine Messung im Gange
	{
		uint16_t i;
		
		RemoteStatus |= (1<<Busy);				//Messflag setzen
		RemoteChannel ++;						//Nächsten Kanal wählen
		
		if(RemoteChannel > 0x05)				//Überlauf der Kanalvorwahl
		{
			RemoteChannel = 0x00;
		}
		
		switch (RemoteChannel)					//Vorwahl ausgeben		
		{																		//Kanalnummerierung als Grey-Code
			case 0x00: PORTC &= ~((1<<PC7) | (1<<PC6) | (1<<PC5)); break;		//CH1 = 000
			case 0x01: PORTC |= (1<<PC5); break;								//CH2 = 001
			case 0x02: PORTC |= (1<<PC6); break;								//CH3 = 011
			case 0x03: PORTC &= ~(1<<PC5); break;								//CH4 = 010
			case 0x04: PORTC |= (1<<PC7); break;								//CH5 = 110
			case 0x05: PORTC |= (1<<PC5); break;								//CH6 = 111
		}
		
		
			
		MCUCR |= (1<<ISC01) | (1<<ISC00);								//Interrupt bei steigender Flanke an INT0
		GICR |= (1<<INT0);												//Interrupts an INT0-Pin zulassen
		//Timer 0 für Eingangskontrolle konfigurieren
		TCCR0 &= ~((1<<CS02) | (1<<CS01) | (1<<CS00));
		TCCR0 |= (1<<CS02);												//Prescaler = 64 (CS01 und CS00 gesetzt)
		TCNT0 = 0x00;													//Vorladen des Timers auf 125 (0x82) ---> Nicht nötig, da schnell genug
		TIMSK |= (1<<TOIE0);											//Interrupt bei Timerüberlauf
		sei();
	}
	
}

///////////////////////////////////////////////////////////////////////////////////////////////
//ISR für die Zeitmessung des Servosignals

ISR(INT0_vect)
{
	uint8_t i;										//Arbeitsvariable
	
	
	i = MCUCR & 0b00000011;							//Bitmaske von MCUCR erzeugen [00000011] (Die beiden letzten Bits für INT0)
	
	if(i == 0b00000011)								//Wenn Interrupt bei steigender Flanke an INT0...
	{
		cli();										//Interrupts global deaktivieren
		TIMSK &= ~(1<<TOIE0);						//Kein Interrupt bei Timeroverflow des Timer 0
		TCCR0 &= ~((1<<CS02) | (1<<CS01) | (1<<CS00));
		TCCR0 |= (1<<CS01);							//Prescaler des Timer0 = 8
		TCNT0 = 0x00;								//Timer0 auf null setzen
		MCUCR &= ~(1<<ISC00);						//Interrupt wenn fallende Flanke an INT0
		sei();										//Interrupts global aktivieren
	}
	else if(i == 0b00000010)						//Wenn Interrupt bei fallender Flanke an INT0...
	{
		cli();
		RemoteValue[RemoteChannel] = TCNT0;			//Gemessener Wert abspeichern im entsprechenden Arrayplatz
		RemoteStatus &= ~(1<<Busy);					//Messflag löschen
		GICR &= ~(1<<INT0);							//Interrupt an INT0 verbieten
		sei();
		
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//ISR für den Timer0

ISR(TIMER0_OVF_vect)
{
	cli();
	RemoteStatus &= ~(1<<Busy);						//Messflag löschen, da kein Signal an diesem Eingang
	GICR &= ~(1<<INT0);								//Interrupts an INT0 deaktivieren
	TIMSK &= ~(1<<TOIE0);							//Interrupt bei Timerüberlauf verbieten
	RemoteValue[RemoteChannel] = 0xFF;				//Kanal abschalten
	sei();
}