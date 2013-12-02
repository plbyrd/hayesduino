/***********************************************
HAYESDUINO PROJECT - COPYRIGHT 2013, PAYTON BYRD

Project homepage: http://hayesduino.codeplex.com
License: http://hayesduino.codeplex.com/license
***********************************************/
#ifndef __LOGGER_H
#define __LOGGER_H

#include <Stream.h>
#include <HardwareSerial.h>

class Logger : public Stream
{
public:
	virtual int available(void);
    virtual int peek(void);
    virtual int read(void);
    virtual void flush(void);
    virtual size_t write(uint8_t);
	inline size_t write(unsigned long n) { if(Serial) return Serial.write((uint8_t)n); return 0; }
    inline size_t write(long n) { if(Serial) return Serial.write((uint8_t)n); return 0;  }
    inline size_t write(unsigned int n) { if(Serial) return Serial.write((uint8_t)n); return 0;  }
    inline size_t write(int n) { if(Serial) return Serial.write((uint8_t)n); return 0;  }
};

extern Logger lggr;

#endif