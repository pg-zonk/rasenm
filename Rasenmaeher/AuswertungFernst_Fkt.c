//Projekt: Rasenm�her
//Hardwareversion: 3.0
//Softwareversion: -- (Entwurfsversion)
//MCU: ATmega16
//Teil: Funktionsblock
//fCPU = 1MHz

//Kurzbeschrieb
//---------------

//Softwareinhalt:		Auswertung des Servosignals, vom Fernsteuerempf. FS-R6A kommend.


#include <avr/io.h>
#include <avr/interrupt.h>
#include "AuswertungFernst.h"

///////////////////////////////////////////////////////////////////////////////////////////////
//Initialisierung

void RemoteInit(void)
{
	//IOs initialisieren
	DDRC |= (1<<PC7) | (1<<PC6) | (1<<PC5);		//PortC = Ausgang (PC5 - PC7 Kanalvorwahl auf Multiplexer)
}

///////////////////////////////////////////////////////////////////////////////////////////////
//Einlesen der Fernsteuerungswerte starten

void startRemoteIn(void)
{
	if(!(RemoteStatus & (1<<Busy)))				//Beginne zu Messen, sofern keine Messung im Gange
	{
		//uint16_t i;
		
		RemoteStatus |= (1<<Busy);				//Messflag setzen
		RemoteChannel ++;						//N�chsten Kanal w�hlen
		
		if(RemoteChannel > 0x05)				//�berlauf der Kanalvorwahl
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
		//Timer 0 f�r Eingangskontrolle konfigurieren
		TCCR0 &= ~((1<<CS02) | (1<<CS01) | (1<<CS00));
		TCCR0 |= (1<<CS02);												//Prescaler = 64 (CS01 und CS00 gesetzt)
		TCNT0 = 0x00;													//Vorladen des Timers auf 125 (0x82) ---> Nicht n�tig, da schnell genug
		TIMSK |= (1<<TOIE0);											//Interrupt bei Timer�berlauf
		sei();
	}
	
}

///////////////////////////////////////////////////////////////////////////////////////////////
//St�rung melden
uint8_t RemoteChannelFail(uint8_t RemoteChannel, uint8_t RemoteStatus)
{
	switch (RemoteChannel)					//St�rungsflags setzen	(St�rung wenn Flag gesetzt)
	{
		case 0x00: RemoteStatus |= (1<<CH1_ok); break;
		case 0x01: RemoteStatus |= (1<<CH2_ok); break;
		case 0x02: RemoteStatus |= (1<<CH3_ok); break;
		case 0x03: RemoteStatus |= (1<<CH4_ok); break;
		case 0x04: RemoteStatus |= (1<<CH5_ok); break;
		case 0x05: RemoteStatus |= (1<<CH6_ok); break;
		default: break;
	}
	
	return RemoteStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//St�rung aufheben
uint8_t RemoteChannelUnfail(uint8_t RemoteChannel, uint8_t RemoteStatus)
{
	switch (RemoteChannel)					//St�rungsflags l�schen	(St�rung wenn Flag gesetzt)
	{
		case 0x00: RemoteStatus &= ~(1<<CH1_ok); break;
		case 0x01: RemoteStatus &= ~(1<<CH2_ok); break;
		case 0x02: RemoteStatus &= ~(1<<CH3_ok); break;
		case 0x03: RemoteStatus &= ~(1<<CH4_ok); break;
		case 0x04: RemoteStatus &= ~(1<<CH5_ok); break;
		case 0x05: RemoteStatus &= ~(1<<CH6_ok); break;
		default: break;
	}
	
	return RemoteStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//Testen ob Fernsteuerung aktiv (schauen ob minestens 4 Kan�le eingeschaltet)
//--> Anm: Gibt die Eingangsvariable wieder zur�ck mit MSB als Fail-Bit (Bit7)
uint8_t RemoteSignalTest(uint8_t RemoteStat)
{
	uint8_t Channels;						//Maskierte Kanalstatusvariable
	uint8_t NumberCHnonworking = 0x00;		//Anzahl der nichtfunktionierenden Kan�le
	uint8_t i;
	uint8_t result = RemoteStat;
	
	Channels = RemoteStat & 0b01111110;		//maskieren (nur CH#_ok Signale interessant)
	for(i=1;i<7;i++)
	{
		if(Channels & (1<<i))				//alle Kan�le durchgehen
		{
			NumberCHnonworking ++;			//wenn Kanal inaktiv wird inkrementiert
		}
	}
	
	if(NumberCHnonworking > 0x01)			//wenn mehr als 2 Kan�le versagen, Melden
	{
		result |= (1<<Fail);
	}
	else
	{
		result &= ~(1<<Fail);			//wenn alles okay, freigeben
	}
	
	return result;
}


///////////////////////////////////////////////////////////////////////////////////////////////
//ISR f�r die Zeitmessung des Servosignals

ISR(INT0_vect)
{
	uint8_t i;										//Arbeitsvariable
	
	
	i = MCUCR & 0b00000011;							//Bitmaske von MCUCR erzeugen [00000011] (Die beiden letzten Bits f�r INT0)
	
	if(i == 0b00000011)								//Wenn Interrupt bei steigender Flanke an INT0...
	{
		cli();										//Interrupts global deaktivieren
		
		if(RemoteSkip == 0x01)						//Erster Eingangsimpuls �berspringen
		{
			TIMSK &= ~(1<<TOIE0);						//Kein Interrupt bei Timeroverflow des Timer 0
			TCCR0 &= ~((1<<CS02) | (1<<CS01) | (1<<CS00));
			TCCR0 |= (1<<CS01);							//Prescaler des Timer0 = 8
			TCNT0 = 0x00;								//Timer0 auf null setzen
			MCUCR &= ~(1<<ISC00);						//Interrupt wenn fallende Flanke an INT0
			RemoteSkip = 0x00;
		}
		else
		{
			TCNT0 = 0x00;								//Timer0 wieder auf 0 setzen
			RemoteSkip = 0x01;							//Beim n�chsten Impuls mit Auswertung beginnen
		}
		
		
		sei();										//Interrupts global aktivieren
	}
	else if(i == 0b00000010)						//Wenn Interrupt bei fallender Flanke an INT0...
	{
		cli();
		RemoteValue[RemoteChannel] = TCNT0;			//Gemessener Wert abspeichern im entsprechenden Arrayplatz
		RemoteStatus &= ~(1<<Busy);					//Messflag l�schen
		GICR &= ~(1<<INT0);							//Interrupt an INT0 verbieten
		RemoteStatus = RemoteChannelUnfail(RemoteChannel, RemoteStatus); //St�rungen aufheben
		sei();
		
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////
//ISR f�r den Timer0. �berlauf bedeutet dass der Kanal nicht verf�gbar ist und wird vermerkt.
//Gelegentliche �berl�ufe k�nnen passieren und werden durch die Funktion RemoteSignalTest() abgefangen

ISR(TIMER0_OVF_vect)
{
	static uint8_t ovf_Cntr;										//Z�hlvariable f�r die Timer�berl�ufe
	
	cli();
	
	if(ovf_Cntr > 0x0F)
	{
		RemoteStatus &= ~(1<<Busy);									//Messflag l�schen, da kein Signal an diesem Eingang
		GICR &= ~(1<<INT0);											//Interrupts an INT0 deaktivieren
		TIMSK &= ~(1<<TOIE0);										//Interrupt bei Timer�berlauf verbieten
		//RemoteValue[RemoteChannel] = 0xFF;						//Kanal abschalten
		RemoteStatus = RemoteChannelFail(RemoteChannel,RemoteStatus);		//St�rung am Kanal
		ovf_Cntr = 0x00;
	}
	ovf_Cntr ++;
	
	sei();
}