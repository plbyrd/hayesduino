/***********************************************
HAYESDUINO PROJECT - COPYRIGHT 2013, PAYTON BYRD

Project homepage: http://hayesduino.codeplex.com
License: http://hayesduino.codeplex.com/license
***********************************************/
#include "HardwareSerial.h"

#ifdef UBRR1H
#define __MEGA__
#define POWER_LED 29
#else
#define __UNO__
#endif

#ifndef _MODEMBASE_h
#define _MODEMBASE_h
#include <Stream.h>
#include "EthernetClient.h"

#include "Arduino.h"
#include "DEBUG.h"
#include "Global.h"

#define STATUS_LED		 8

#ifdef __UNO__
#define DTE_RTS  2  // Orange
#define DCE_CTS  2
#define DTE_DTR  7  // White/Green
#define DCE_DCD	 7
#define DCE_RI   4  // White/Brown
#define DTE_DCD  5  // Brown
#define DCE_DTR  5
#define DTE_CTS  6  // White/Orange
#define DCE_RTS  6
#define DTE_DSR  3  // White/Blue
#define TxD      0  // Blue
#define RxD      1 // Green
#else
// MEGA
#define DTE_RTS  22  // Orange
#define DCE_CTS  22
#define DTE_DTR  27  // White/Green
#define DCE_DCD  27
#define DCE_RI   24  // White/Brown
#define DTE_DCD  25  // Brown
#define DCE_DTR  25
#define DTE_CTS  26  // White/Orange
#define DCE_RTS  26
#define DTE_DSR  23  // White/Blue
#define TxD      19  // Blue
#define RxD      18 // Green

//#define DTE_RTS  2  // Orange
//#define DCE_CTS  2
//#define DTE_DTR  7  // White/Green
//#define DCE_DCD	 7
//#define DCE_RI   4  // White/Brown
//#define DTE_DCD  5  // Brown
//#define DCE_DTR  5
//#define DTE_CTS  6  // White/Orange
//#define DCE_RTS  6
//#define DTE_DSR  3  // White/Blue
//#define TxD      19  // Blue
//#define RxD      18 // Green
#endif

class ModemBase : public Stream
{
 private:
	int _escapeCount;
	char _lastCommandBuffer[81];
	char _commandBuffer[81];

	HardwareSerial* _serial;

	uint32_t _baudRate;
	
	bool _isCommandMode;
	bool _isConnected;
	bool _isRinging;
	bool _isDcdInverted;

	bool _echoOn;
	bool _verboseResponses;
	bool _quietMode;

	bool _S0_autoAnswer;			// Default false
	uint8_t _S1_ringCounter;		// Default 0
	uint8_t _S2_escapeCharacter;	// Default 43
	uint8_t _S3_crCharacter;		// Default 13
	uint8_t _S4_lfCharacter;		// Default 10
	uint8_t _S5_bsCharacter;		// Default 8
	uint8_t _S6_waitBlindDial;		// Default 2 seconds
	uint8_t _S7_waitForCarrier;		// Default 50 seconds
	uint8_t _S8_pauseForComma;		// Default 2 seconds
	uint8_t _S9_cdResponseTime;		// Default 6 (0.6 seconds)
	uint8_t _S10_delayHangup;		// Default 14 (1.4 seconds)
	uint8_t _S11_dtmf;				// Default 95 miliseconds
	uint8_t _S12_escGuardTime;		// Default 50 (1.0 second)
	uint8_t _S18_testTimer;			// Default 0 seconds
	uint8_t _S25_delayDTR;			// Default 5
	uint8_t _S26_delayRTS2CTS;		// Default 5 miliseconds
	uint8_t _S30_inactivityTimer;	// Default 0 (disable)
	uint8_t _S37_lineSpeed;			// Default 0 (auto)
	uint8_t _S38_delayForced;		// Default 20 seconds
	/*
S37 Command options:
0 Attempt auto mode connection
1 Attempt to connect at 300 bit/s
2 Attempt to connect at 300 bit/s
3 Attempt to connect at 300 bit/s
5 Attempt to connect at 1200 bit/s
6 Attempt to connect at 2400 bit/s
7 Attempt to connect in V.23 75/1200 mode.
8 Attempt to connect at 9600 bit/s
9 Attempt to connect at 12000 bit/s
10 Attempt to connect at 14400 bit/s
	*/

	void resetCommandBuffer(bool);

	void (*onDisconnect)(EthernetClient *client);
	void (*onDialout)(char*, ModemBase*);

	void setLineSpeed(void);
	void loadDefaults(void);
	void saveDefaults(void);
	void writeAddressBook(uint16_t, char *);
	char* getAddressBook(uint16_t);
#ifndef __UNO__
	bool processCommandBufferExtended(EthernetClient *client);
#endif
	void printOK(void);
	void printResponse(const char* code, const char* msg);
	void printResponse(const char* code, const __FlashStringHelper * msg);
	int getString(EthernetClient *client, char *buffer, int maxLength);

 public:
	 ModemBase();

	void begin(
			HardwareSerial *serial, 
			void (*onDisconnectHandler)(EthernetClient *client),
			void (*onDialoutHandler)(char*, ModemBase*));

  	 virtual int available(void);
     virtual int peek(void);
     virtual int read(void);
     virtual void flush(void);
     virtual size_t write(uint8_t);

	 void factoryReset(void);
	 uint32_t getBaudRate(void);
	 void setDefaultBaud(uint32_t baudRate);
	 void setDcdInverted(bool isInverted);
	 bool getDcdInverted(void);
	 bool getIsConnected(void);
	 bool getIsRinging(void);
	 void setIsRinging(bool);
	 bool getIsCommandMode(void);
	 int toggleCarrier(boolean isHigh);
	 void disconnect(EthernetClient *client);
	 void connect(EthernetClient *client);
	 void connectOut(void);

	 void processCommandBuffer(EthernetClient *client);
	 void processData(EthernetClient *client);
	 
	 void resetToDefaults(void);

	 using Print::write;
};

#endif

