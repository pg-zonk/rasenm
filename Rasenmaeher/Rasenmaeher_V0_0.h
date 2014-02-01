/*
***************************************************************************************
*
* Projekt: Rasenm�her
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
* Softwareinhalt:		Zusammenf�hrung aller Funktionen
*
***************************************************************************************
*/
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

//=====================================================================================
// Definitionen f�r die Zeitverz�gerung
#define		halfsnd			0			//0.5 Sekunden
#define		fullsnd			1			//1 Sekunde
#define		doublesnd		2			//2 Sekunden

#define		Timearray_len	5			//L�nge des Timearray
#define		TimeTick		7			//MSB der Arraypl�tze wird gesetzt wenn Platz inkrementiert

//=====================================================================================
// Definitionen f�r Betriebszustandsregister (operatingstates)
#define		warmstart		0			//Wird auf 1 gesetzt sobald Benzinmotor schonmal in Betrieb war

//=====================================================================================
// Definitionen f�r Ansteuerung der Fahrmotoren
#define		Value_FML		1
#define		Value_FMR		2

///////////////////////////////////////////////////////////////////////////////////////////////
//Globale Variabeln

uint8_t Timearray[Timearray_len];		//Array f�r die verschiedenen Zeitverz�gerungen (Auslesen mit Polling)
uint8_t operatingstates;				//Flags der Betriebszust�nde

///////////////////////////////////////////////////////////////////////////////////////////////
//Funktionskonstruktoren

//=====================================================================================
// Funktionen f�r Zeitverz�gerungen
uint8_t TimeCheck(uint8_t slot);
void TimeReset(uint8_t slot);

//=====================================================================================
// Allgemeine Funktionen
void init_All(void);
void watchNotAus(void);
void engineStart(void);
void stopFahrmot(void);
void startFahrmot(void);
void updateFahrmot(void);
void lamptest(void);
