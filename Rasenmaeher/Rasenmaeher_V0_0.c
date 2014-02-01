/*
***************************************************************************************
*
* Projekt: Rasenmäher
* Datum: 21.08.2013
* Hardwareversion: 3.0
* Softwareversion: 1.0 
* MCU: ATmega16
* Teil: Komplette Software
* fCPU = 1MHz
*
* Kurzbeschrieb
* ---------------
*
* Softwareinhalt:		Hauptprogramm
*
***************************************************************************************
*/


#include <avr/io.h>
#include <avr/interrupt.h>
#include "Rasenmaeher_V0_0.h"
#include "AuswertungFernst.h"
//#include "AuswertungFernst_Fkt.c"
#include "Rasenmaeher_V0_0_Fkt.c"



int main(void)
{
	init_All();						//Grundinitialisierung
	sei();							//Interrupts erlauben
	PORTB |= (1<<Qx_BenzMotEin);	//Benzinmotor Startfreigabe
	
	while(1)
    {
		startRemoteIn();		//Fernsteuerwert einlesen
		updateFahrmot();		//Fahrmotorwerte aktualisieren	
    }
}