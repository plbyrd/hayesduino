#include "Arduino.h"
#include "Logger.h"

#ifdef UBRR1H
#define __MEGA__
#define POWER_LED 29
#else
#define __UNO__
#endif


int Logger::available(void) { if(Serial) return Serial.available(); return 0; }
int Logger::peek(void) { if(Serial) return Serial.peek(); return 0; }
int Logger::read(void) { if(Serial) return Serial.read(); return 0; }
void Logger::flush(void) { if(Serial) Serial.flush(); }
size_t Logger::write(uint8_t var) 
{ 
#ifdef POWER_LED
	digitalWrite(POWER_LED, LOW);
#endif

	if(Serial) 
	{
#ifdef POWER_LED
		//delay(50);
		digitalWrite(POWER_LED, HIGH);
#endif
		return Serial.write(var); 
	}

#ifdef POWER_LED
	//delay(50);
	digitalWrite(POWER_LED, HIGH);
#endif
	return 0; 
}

Logger lggr;