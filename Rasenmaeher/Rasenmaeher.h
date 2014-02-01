/*
***************************************************************************************
*
* Projekt: Rasenm�her
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
* Inhalt:		Hardwarebeschrieb, Funktionsprototypen
*
***************************************************************************************
*/


#ifndef RASENMAEHER_H_
#define RASENMAEHER_H_

///////////////////////////////////////////////////////////////////////////////////////////////
// I/O Definitionen

//=====================================================================================
// I/O Definitionen f�r Port B
// Eing�nge
#define		IxEntpan		0
#define		IxLadekon		1
#define		IxGenStart		4
#define		IxBenzMotAus	5

// Ausg�nge
#define		Qx_MLP_GenOK	3
#define		Qx_GenEin		7

//=====================================================================================
// I/O Definitionen f�r Port D
// Eing�nge
#define		IxNotAus		3

// Ausg�nge
#define		Qx_MLP_Stoer	1
#define		Qx_BenzMotEin	6
#define		Qpwm_FML		5
#define		Qpwm_FMR		4

//=====================================================================================
// I/O Definitionen f�r Port C
// Eing�nge

// Ausg�nge
#define		Qx_Fahrr_Seilw	2
#define		Qx_Fahrr_links	3
#define		Qx_Fahrr_rechts	4
#define		Qx_FunkCh_0		5
#define		Qx_FunkCh_1		6
#define		Qx_FunkCh_2		7

///////////////////////////////////////////////////////////////////////////////////////////////
//Funktionskonstruktoren

//=====================================================================================
// Allgemeine Funktionen
void initAll(void);

#endif /* RASENMAEHER_H_ */