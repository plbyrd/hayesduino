/***********************************************
HAYESDUINO PROJECT - COPYRIGHT 2013, PAYTON BYRD

Project homepage: http://hayesduino.codeplex.com
License: http://hayesduino.codeplex.com/license
***********************************************/
#ifndef ethernetserver_h
#define ethernetserver_h

#include "Server.h"

class EthernetClient;

class EthernetServer : 
public Server {
private:
  uint16_t _port;
  void accept();
public:
  EthernetServer(uint16_t);
  EthernetClient available();
  EthernetClient connected();
  EthernetClient connected(uint8_t sock);
  virtual void begin();
  virtual size_t write(uint8_t);
  virtual size_t write(const uint8_t *buf, size_t size);
  using Print::write;
};

#endif
