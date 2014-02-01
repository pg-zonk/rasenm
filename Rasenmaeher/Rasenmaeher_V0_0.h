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
* Softwareinhalt:		Zusammenführung aller Funktionen
*
***************************************************************************************
*/
///////////////////////////////////////////////////////////////////////////////////////////////
// I/O Definitionen

//=====================================================================================
// I/O Definitionen für Port B
// Eingänge
#define		IxEntpan		0
#define		IxLadekon		1
#define		IxGenStart		4
#define		IxBenzMotAus	5

// Ausgänge
#define		Qx_MLP_GenOK	3
#define		Qx_GenEin		7

//=====================================================================================
// I/O Definitionen für Port D
// Eingänge
#define		IxNotAus		3

// Ausgänge
#define		Qx_MLP_Stoer	1
#define		Qx_BenzMotEin	6
#define		Qpwm_FML		5
#define		Qpwm_FMR		4

//=====================================================================================
// I/O Definitionen für Port C
// Eingänge

// Ausgänge
#define		Qx_Fahrr_Seilw	2
#define		Qx_Fahrr_links	3
#define		Qx_Fahrr_rechts	4
#define		Qx_FunkCh_0		5
#define		Qx_FunkCh_1		6
#define		Qx_FunkCh_2		7

//=====================================================================================
// Definitionen für die Zeitverzögerung
#define		halfsnd			0			//0.5 Sekunden
#define		fullsnd			1			//1 Sekunde
#define		doublesnd		2			//2 Sekunden

#define		Timearray_len	5			//Länge des Timearray
#define		TimeTick		7			//MSB der Arrayplätze wird gesetzt wenn Platz inkrementiert

//=====================================================================================
// Definitionen für Betriebszustandsregister (operatingstates)
#define		warmstart		0			//Wird auf 1 gesetzt sobald Benzinmotor schonmal in Betrieb war

//=====================================================================================
// Definitionen für Ansteuerung der Fahrmotoren
#define		Value_FML		1
#define		Value_FMR		2

///////////////////////////////////////////////////////////////////////////////////////////////
//Globale Variabeln

uint8_t Timearray[Timearray_len];		//Array für die verschiedenen Zeitverzögerungen (Auslesen mit Polling)
uint8_t operatingstates;				//Flags der Betriebszustände

///////////////////////////////////////////////////////////////////////////////////////////////
//Funktionskonstruktoren

//=====================================================================================
// Funktionen für Zeitverzögerungen
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
