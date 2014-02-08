#ifndef _VSARDUINO_H_
#define _VSARDUINO_H_
//Board = Arduino Mega 2560 or Mega ADK
#define __AVR_ATmega2560__
#define ARDUINO 152
#define ARDUINO_MAIN
#define __AVR__
#define __avr__
#define F_CPU 16000000L
#define __cplusplus
#define __inline__
#define __asm__(x)
#define __extension__
#define __ATTR_PURE__
#define __ATTR_CONST__
#define __inline__
#define __asm__ 
#define __volatile__

#define __builtin_va_list
#define __builtin_va_start
#define __builtin_va_end
#define __DOXYGEN__
#define __attribute__(x)
#define NOINLINE __attribute__((noinline))
#define prog_void
#define PGM_VOID_P int
            
typedef unsigned char byte;
extern "C" void __cxa_pure_virtual() {;}

void disconnectClient(EthernetClient *client);
void dialout(char * host, ModemBase *modm);
//
//
void resetToModemDefaults();

#include "s:\arduino-1.5.2\hardware\arduino\avr\cores\arduino\arduino.h"
#include "s:\arduino-1.5.2\hardware\arduino\avr\variants\mega\pins_arduino.h" 
#include "S:\Hayesduino\Hayesduino.ino"
#include "S:\Hayesduino\DEBUG.h"
#include "S:\Hayesduino\Global.h"
#include "S:\Hayesduino\Logger.cpp"
#include "S:\Hayesduino\Logger.h"
#include "S:\Hayesduino\ModemBase.cpp"
#include "S:\Hayesduino\ModemBase.h"
#endif
