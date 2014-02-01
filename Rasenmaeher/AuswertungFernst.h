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

///////////////////////////////////////////////////////////////////////////////////////////////
//Bitdefinition von Register RemoteStatus

#define Busy 		0			//Messung im Gange
#define CH1_ok		1			//Diagnose Kanal 1
#define CH2_ok		2			//Diagnose Kanal 2
#define CH3_ok		3			//Diagnose Kanal 3
#define CH4_ok		4			//Diagnose Kanal 4
#define CH5_ok		5			//Diagnose Kanal 5
#define CH6_ok		6			//Diagnose Kanal 6
#define Fail		7			//Verlangt Stop (mehr als 2 Kanäle versagen)

///////////////////////////////////////////////////////////////////////////////////////////////
//Globale Variabeln
uint8_t RemoteStatus;		//Flags für den Fernsteuereingang
uint8_t RemoteChannel;		//Register für Kanalvorwahl
uint8_t RemoteValue[6];		//Array für die Eingangswerte
uint8_t RemoteSkip;			//Zählvariable zum überspringen der ersten PWM-Flanke

///////////////////////////////////////////////////////////////////////////////////////////////
//Konstruktoren

void RemoteInit(void);
void startRemoteIn(void);
uint8_t RemoteChannelFail(uint8_t RemoteChannel, uint8_t RemoteStatus);
uint8_t RemoteChannelUnfail(uint8_t RemoteChannel, uint8_t RemoteStatus);
uint8_t RemoteSignalTest(uint8_t RemoteStat);


