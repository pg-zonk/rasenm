/*
***************************************************************************************
*
* Projekt: Rasenmäher
* Datum: 01.02.2014
* Hardwareversion: 3.0
* Softwareversion: 2.0
* MCU: ATmega16
* Teil: Funktionen des Hauptteils der SW. Funktionen für die Fernsteuerung sind enthalten
*		in "AuswertungFernst_Fkt.c" und "Auswertungfernst.h"
* fCPU = 1MHz
* Versionsverwaltung: Git(rasenm)
*
* Kurzbeschrieb
* ---------------
*
* Softwareinhalt:		Alle Fahrzeugspezifischen Funktionen und Prozeduren
*
***************************************************************************************
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "Rasenmaeher.h"

//============================================================================================
//Erste Initialisierung

void init_All(void)
{

	//I/Os initialisieren

	//Port B
	DDRB |= (1<<Qx_MLP_GenOK) | (1<<Qx_GenEin);											//Ausgänge
	DDRB &= ~((1<<IxEntpan) | (1<<IxLadekon) | (1<<IxGenStart) | (1<<IxBenzMotAus));	//Eingänge

	//Port C
	DDRC |= (1<<Qx_Fahrr_Seilw) | (1<<Qx_Fahrr_links) | (1<<Qx_Fahrr_rechts);			//Ausgänge
	//--> Ausgänge für die Funkkanalvorwahl werden in "AuswertungFernst_Fkt.c" initialisiert

	//Port C hat keine Eingangspins

	//Port D
	DDRD |= (1<<Qx_MLP_Stoer) | (1<<Qx_BenzMotEin) | (1<<Qpwm_FML) | (1<<Qpwm_FMR);		//Ausgänge
	DDRD &= ~(1<<Qx_BenzMotEin);														//Eingänge

	//Timer 2 initialisieren für Zeitverzögerung (Allgemeiner Timer)

	TCCR2 |= (1<<CS22) | (1<<CS21) | (1<<CS20);		//Prescaler = 1024 // fTimer2 = ca. 4Hz
	TIMSK |= (1<<TOIE2);							//Interrupt erlauben bei Overflow

	//Timer 1 initialisieren für Fahrmotoransteuerung
	//Fast PWM 8Bit inverting mode
	TCCR1A |= (1<<COM1A1) | (1<<COM1B1) | (1<<WGM10) | (1<<COM1A0) | (1<<COM1B0);
	TCCR1A &= ~((1<<WGM11));
	TCCR1B |= (1<<WGM12);
	TCCR1B &= ~(1<<WGM13);

	//Prescaler = 8 --> fPWM = 490Hz
	TCCR1B |= (1<<CS11);
	TCCR1B &= ~((1<<CS12) | (1<<CS10));

	//Fernsteuerungsauswertung initialisieren
	RemoteInit();

}