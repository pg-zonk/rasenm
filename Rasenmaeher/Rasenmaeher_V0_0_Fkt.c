/*
***************************************************************************************
*
* Projekt: Rasenm�her
* Datum: 21.08.2013
* Hardwareversion: 3.0
* Softwareversion: 1.0
* MCU: ATmega16
* Teil: Funktionen des Hauptteils der SW. Funktionen f�r die Fernsteuerung sind enthalten
*		in "AuswertungFernst_Fkt.c" und "Auswertungfernst.h"
* fCPU = 1MHz
*
* Kurzbeschrieb
* ---------------
*
* Softwareinhalt:		Alle Fahrzeugspezifischen Funktionen und Prozeduren
*
***************************************************************************************
*/

#include "Rasenmaeher_V0_0.h"

//============================================================================================
//Erste Initialisierung

void init_All(void)
{

//I/Os initialisieren																  

//Port B
DDRB |= (1<<Qx_MLP_GenOK) | (1<<Qx_GenEin);											//Ausg�nge
DDRB &= ~((1<<IxEntpan) | (1<<IxLadekon) | (1<<IxGenStart) | (1<<IxBenzMotAus));	//Eing�nge

//Port C
DDRC |= (1<<Qx_Fahrr_Seilw) | (1<<Qx_Fahrr_links) | (1<<Qx_Fahrr_rechts);			//Ausg�nge
//--> Ausg�nge f�r die Funkkanalvorwahl werden in "AuswertungFernst_Fkt.c" initialisiert

//Port C hat keine Eingangspins

//Port D
DDRD |= (1<<Qx_MLP_Stoer) | (1<<Qx_BenzMotEin) | (1<<Qpwm_FML) | (1<<Qpwm_FMR);		//Ausg�nge
DDRD &= ~(1<<Qx_BenzMotEin);														//Eing�nge

//Timer 2 initialisieren f�r Zeitverz�gerung (Allgemeiner Timer)

TCCR2 |= (1<<CS22) | (1<<CS21) | (1<<CS20);		//Prescaler = 1024 // fTimer2 = ca. 4Hz
TIMSK |= (1<<TOIE2);							//Interrupt erlauben bei Overflow

//Timer 1 initialisieren f�r Fahrmotoransteuerung
//Fast PWM 8Bit non-inverting mode
TCCR1A |= (1<<COM1A1) | (1<<COM1B1) | (1<<WGM10);
TCCR1A &= ~((1<<WGM11));
TCCR1B |= (1<<WGM12);
TCCR1B &= ~(1<<WGM13);

//Prescaler = 8 --> fPWM = 490Hz
TCCR1B |= (1<<CS11);
TCCR1B &= ~((1<<CS12) | (1<<CS10));

//Fernsteuerungsauswertung initialisieren
RemoteInit();									

}

//============================================================================================
//Motorstartfunktion

void engineStart(void)
{
	stopFahrmot();								//Fahrmotoren abschalten
	PORTD |= (1<<Qx_BenzMotEin);				//Z�ndung freigeben
	
	while(!(PINB & (1<<IxGenStart)))			//Warten bis Motor l�uft
	{	
		watchNotAus();							//Schauen ob Notaus anliegt
		
		if(TimeCheck(halfsnd) >= 0b00000001)	//Generatorlampe blinken lassen mit ca. 1Hz
		{
			TimeReset(halfsnd);					//Halbsekundenverz�gerung auf Null setzen
			PORTB ^= (1<<Qx_MLP_GenOK);
		}
	}
	
	PORTB |= (1<<Qx_GenEin);					//Fremderregung am Generator einschalten
	PORTB |= (1<<Qx_MLP_GenOK);					//Generatorlampe dauernd einschalten
	TimeReset(fullsnd);							//Sekundenverz�gerung Null setzen
	
	while(!(TimeCheck(fullsnd) >= 0b00000010))	//1 Sekunde warten
	{
		watchNotAus();							//W�hrenddessen NotAus kontrollieren
	}
	
	//Ab hier ge�ndert dass st�ndig erregung... 11.01.2014
	//Normalisieren: Auskommentiertes rausnehmen, n�chste if bedingnung wegnehmen und lampe immer l�schen lassen!!! 
	/*while(!(PINB & (1<<IxLadekon)))				//Wenn Generator bis dahin nicht angesprungen weiter warten
	{
		watchNotAus();							//W�hrenddessen NotAus kontrollieren
	}
	
	*/PORTB &= ~(1<<Qx_GenEin);					//Fremderregung am Generator ausschalten
	if(!(PINB & (1<<IxLadekon)))
	{
		PORTB &= ~(1<<Qx_MLP_GenOK);				//Generatorlampe dauernd ausschalten
	}
	
}

//============================================================================================
//Sicherheits�berwachung

void watchNotAus(void)		//Notaus wenn Taste gedr�ckt oder Fernsteueung ausgefallen
{
	if((PIND & (1<<IxNotAus)) || ((RemoteSignalTest(RemoteStatus) & (1<<Fail))) || ((RemoteValue[5] < 140) && (RemoteValue[2] > 240)))
	{
		PORTB &= ~(1<<Qx_GenEin);					//Fremderregung am Generator ausschalten
		PORTB &= ~(1<<Qx_MLP_GenOK);				//Generatorlampe ausschalten
		OCR1AL = 0x00;								//Traktion ausschalten links
		OCR1BL = 0x00;								//Traktion ausschalten rechts
		
		while((PIND & (1<<IxNotAus)) || ((RemoteSignalTest(RemoteStatus) & (1<<Fail))) || ((RemoteValue[5] < 140) && (RemoteValue[2] > 235)))	//Wenn und Solange Notaus bet�tigt oder Fernst aus
		{
			PORTD |= (1<<Qx_MLP_Stoer);				//St�rlampe einschalten
			PORTD &= ~(1<<Qx_BenzMotEin);			//Benzinmotor ausschalten
			startRemoteIn();
		}
	
		while(!(PINB & (1<<IxEntpan)))				//Wenn NotAus normalisiert warten bis Enpannen gedr�ckt
		{
			if(TimeCheck(halfsnd) >= 0b00000001)	//St�rlampe blinken lassen mit ca. 1Hz
			{
				TimeReset(halfsnd);
				PORTD ^= (1<<Qx_MLP_Stoer);
			}
		}
	
		PORTD &= ~(1<<Qx_MLP_Stoer);				//St�rlampe ausschalten falls noch brennt
		
		if(operatingstates & (1<<warmstart))		//Wenn der Motor schonmal gelaufen
		{
			engineStart();							//Motorstart einleiten
		}											//Beim Kaltstart wird die Prozedur von Main-Loop aufgerufen
	}
}

//============================================================================================
//Fahrmotoren ausser Betrieb nehmen

void stopFahrmot(void)
{
	OCR1AL = 0x00;			//Mindestwert als PWM
	OCR1BL = 0x00;
}

//============================================================================================
//Aus Fernsteuersignal Stromrichteransteuerung berechnen und ausgeben

void updateFahrmot(void)
{
	uint8_t outvalFML = 0x00;		//PWM-Wert f�r Fahrmot links (links = OC1A)
	uint8_t outvalFMR = 0x00;		//PWM-Wert f�r Fahrmot links (rechts = OC1B)
	
	//========================================================================================
	//Fahrmotor links
	
	if((RemoteValue[3] > 178) & (RemoteValue[3] < 198))	//Nullstellung
	{
		outvalFML = 0x00;		//Mittelstellung: Motor anhalten
	}
	else if((RemoteValue[3] > 237) & (RemoteValue[3] < 255))				//Fahren vorw�rts Maximum (ex 0x241)
	{
		outvalFML = 0xFE;						//Maximale Fahrstufe (PWM muss immer da sein)
		PORTC |= (1<<Qx_Fahrr_links);			//Fahrrichtung vorw�rts: Pin = High
	}
	else if((RemoteValue[3] < 139) & (RemoteValue[3] > 47))				//Fahrrichtung retour Maximum
	{
		outvalFML = 0xFE;						//Maximale Fahrstufe (PWM muss immer da sein)
		PORTC &= ~(1<<Qx_Fahrr_links);			//Fahrrichtung retour: Pin = Low
	}
	else if((RemoteValue[3] >= 198) & (RemoteValue[3] <= 237))				//Fahren vorw�rts
	{
		outvalFML = 6*(RemoteValue[3]-0xc6);	//PWM-Wert = 6*(Kanalwert - 198)
		PORTC |= (1<<Qx_Fahrr_links);			//Fahrrichtung vorw�rts: Pin = High
	}
	else if((RemoteValue[3] <= 178) & (RemoteValue[3] >= 139))				//Fahren retour
	{
		outvalFML = 6*(0xb2-RemoteValue[3]);	//PWM-Wert = 6*(178 - Kanalwert)
		PORTC &= ~(1<<Qx_Fahrr_links);			//Fahrrichtung retour: Pin = Low
	}
	else
	{
		
	}
	
	//========================================================================================
	//Fahrmotor rechts
	
	if((RemoteValue[4] > 178) & (RemoteValue[4] < 198))	//Nullstellung
	{
		outvalFMR = 0x00;		//Mittelstellung: Motor anhalten
	}
	else if((RemoteValue[4] > 237) & (RemoteValue[4] < 255))				//Fahren vorw�rts Maximum (ex 0x241)
	{
		outvalFMR = 0xFE;						//Maximale Fahrstufe (PWM muss immer da sein)
		PORTC |= (1<<Qx_Fahrr_rechts);			//Fahrrichtung vorw�rts: Pin = High
	}
	else if((RemoteValue[4] < 139) & (RemoteValue[4] > 47))				//Fahrrichtung retour Maximum
	{
		outvalFMR = 0xFE;						//Maximale Fahrstufe (PWM muss immer da sein)
		PORTC &= ~(1<<Qx_Fahrr_rechts);			//Fahrrichtung retour: Pin = Low
	}
	else if((RemoteValue[4] >= 198) & (RemoteValue[4] <= 237))				//Fahren vorw�rts
	{
		outvalFMR = 6*(RemoteValue[4]-0xc6);	//PWM-Wert = 6*(Kanalwert - 198)
		PORTC |= (1<<Qx_Fahrr_rechts);			//Fahrrichtung vorw�rts: Pin = High
	}
	else if((RemoteValue[4] <= 178) & (RemoteValue[4] >= 139))				//Fahren retour
	{
		outvalFMR = 6*(0xb2-RemoteValue[4]);	//PWM-Wert = 6*(178 - Kanalwert)
		PORTC &= ~(1<<Qx_Fahrr_rechts);			//Fahrrichtung retour: Pin = Low
	}
	else
	{
		
	}
	
	//========================================================================================
	//PWM-Werte updaten
	
	OCR1AL = outvalFML;
	OCR1BL = outvalFMR;
	
}

//============================================================================================
//Lampentest

void lamptest(void)
{
	TimeReset(doublesnd);							//Zweisekundenverz�gerung Null setzen
	
	while(!(TimeCheck(doublesnd) >= 0b00000100))
	{
		PORTB |= (1<<Qx_MLP_GenOK);					//Generatorlampe einschalten
		PORTD |= (1<<Qx_MLP_Stoer);					//St�rungslampe einschalten
	}
	
	PORTB &= ~(1<<Qx_MLP_GenOK);					//Generatorlampe ausschalten
	PORTD &= ~(1<<Qx_MLP_Stoer);					//St�rungslampe ausschalten
}

//============================================================================================
//Benzinmotor ausser Betrieb nehmen

void stopBenzMot(void)
{
	PORTD &= ~(1<<Qx_BenzMotEin);					//Z�ndung auf Masse legen
}

//////////////////////////////////////////////////////////////////////////////////////////////
//Zeitverz�gerungsfunktionen

//============================================================================================
//Zeit �berpr�fen
uint8_t TimeCheck(uint8_t slot)		//slot = Arrayplatz
{
	uint8_t result = 0x00;			//Ergebnisvariable --> Jedes bit steht f�r eine Zeit�berschr
	uint8_t maxval = 0x02;			//maximalwert: Wird berechnet aus space: 
	uint8_t i;
	
	for(i=1;i<slot+1;i++)			//Berechnung des Maximalwertes
	{
		maxval = maxval*0x02;		//Schleife implementiert die Fkt: val=2^(space+1)
	}

	
	if(Timearray[slot]>=maxval)		//Wenn Zeitz�hlung �berschreitet jeweilige Marke
	{
		result |= (1<<slot);		//Entsprechendes Bit wird gesetzt
		Timearray[slot] = 0x00;		//Reset des Arrayplatzes, Zeitz�hlung beginnt von vorn
	}
	
	return result;					//0 wird zur�ckgegeben wenn keine Zeit�berschreitung detektiert
}

//============================================================================================
//Zeit auf null setzen
void TimeReset(uint8_t slot)
{
	uint8_t i;							//Laufindex
	
	if(slot == 0xFF)
	{
		for(i=0;i<Timearray_len;i++)
		{
			Timearray[i] = 0x00;		//den ganzen Arrayinhalt l�schen wenn slot = 0xFF
		}
	}
	else
	{
		Timearray[slot] = 0x00;			//gew�nschte Zeitz�hlung auf null setzen
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////
//Interrupthandler

//============================================================================================
//Overflow-Interrupt Handler Timer 2
ISR(TIMER2_OVF_vect)
{
	uint8_t i;
	
	cli();										//Interrupts global deaktivieren
	for(i=0; i<=Timearray_len; i++)
	{
		Timearray[i] ++;						//inkrementieren aller Zeitz�hler
	}
	sei();										//Interrupts global aktivieren
}
