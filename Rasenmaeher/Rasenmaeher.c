/*
***************************************************************************************
*
* Projekt: Rasenmäher
* Datum: 01.02.2014
* Hardwareversion: 3.0
* Softwareversion: 2.0
* MCU: ATmega16
* Teil: Komplette Software
* fCPU = 1MHz
* Versionsverwaltung: Git(rasenm)
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
#include "Rasenmaeher.h"
#include "AuswertungFernst.h"
//#include "AuswertungFernst_Fkt.c"
//#include "Rasenmaeher_Fun.c"

int main(void)
{
	init_All();
	
    while(1)
    {
        //TODO:: Please write your application code 
    }
}