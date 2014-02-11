/***********************************************
HAYESDUINO PROJECT - COPYRIGHT 2013, PAYTON BYRD

Project homepage: http://hayesduino.codeplex.com
License: http://hayesduino.codeplex.com/license
***********************************************/
#include "ModemBase.h"
#include "Dns.h"
#include "EthernetClient.h"
#include "Ethernet.h"
#include "HardwareSerial.h"

#define DEBUG 1
#if DEBUG == 1
#include "Logger.h"
#endif
#include "EEPROM.h"

#ifdef UBRR1H
#define __MEGA__
#define POWER_LED 29
#else
#define __UNO__
#endif


int ModemBase::getString(EthernetClient *client, char *buffer, int maxLength)
{
	char counter = 0;
	char c;

	while(client->available() && counter++ < maxLength)
	{
		c = client->read();
		_serial->print(c);
		buffer[counter] = c;
		if(c == '\n')
		{
			buffer[counter+1] = '\0';
			break;
		}
	}
	
	return counter;
}


int ModemBase::available(void) { if(_serial) return _serial->available(); 
#if DEBUG == 1
lggr.println(F("No serial!")); 
#endif
return 0; }
int ModemBase::peek(void) { if(_serial) return _serial->peek(); 
#if DEBUG == 1
lggr.println(F("No serial!")); 
#endif
return 0; }
int ModemBase::read(void) { if(_serial) return _serial->read(); 
#if DEBUG == 1
lggr.println(F("No serial!")); 
#endif
return 0; }
void ModemBase::flush(void) { if(_serial) _serial->flush(); 
#if DEBUG == 1
lggr.println(F("No serial!"));
#endif
}
size_t ModemBase::write(uint8_t var) { if(_serial) return _serial->write(var); 
#if DEBUG == 1
lggr.println(F("No serial!")); 
#endif
return 0; }

void ModemBase::resetCommandBuffer(bool forceReset)
{
	memset(_commandBuffer, 0, strlen(_commandBuffer));
}

ModemBase::ModemBase()
{
	pinMode(DCE_RI , OUTPUT);
	pinMode(DCE_RTS, OUTPUT);
	pinMode(DCE_DCD, INPUT);
	pinMode(DCE_CTS, INPUT);
	pinMode(DCE_DTR, OUTPUT);

	digitalWrite(DCE_RI, LOW);
	digitalWrite(DCE_RTS, LOW);
	digitalWrite(DCE_DTR, toggleCarrier(false));
}

void ModemBase::factoryReset(void)
{
	EEPROM.write(E_MODE_ADDRESS, (uint8_t)true);
	EEPROM.write(V_MODE_ADDRESS, (uint8_t)true);
	EEPROM.write(Q_MODE_ADDRESS, (uint8_t)false);

	EEPROM.write(S1_ADDRESS, 0);
	EEPROM.write(S2_ADDRESS, 43);
	EEPROM.write(S3_ADDRESS, 13);
	EEPROM.write(S4_ADDRESS, 10);
	EEPROM.write(S5_ADDRESS, 8);
	EEPROM.write(S6_ADDRESS, 2);
	EEPROM.write(S7_ADDRESS, 50);
	EEPROM.write(S8_ADDRESS, 2);
	EEPROM.write(S9_ADDRESS, 6);
	EEPROM.write(S10_ADDRESS, 14);
	EEPROM.write(S11_ADDRESS, 95);
	EEPROM.write(S12_ADDRESS, 50);
	EEPROM.write(S18_ADDRESS, 0);
	EEPROM.write(S25_ADDRESS, 5);
	EEPROM.write(S26_ADDRESS, 5);
	EEPROM.write(S30_ADDRESS, 0);
	EEPROM.write(S37_ADDRESS, 6);
	EEPROM.write(S38_ADDRESS, 20);
	EEPROM.write(S90_ADDRESS, 0);

	EEPROM.write(MODEM_INITIALIZED_ADDRESS, MODEM_INITIALIZED_STATE);

	writeAddressBook(ADDRESS_BOOK_START + (ADDRESS_BOOK_LENGTH * 0), 
		(char*)F("commodoreserver.com:1541"));

	//if(Serial) Serial.println(F("Initialized modem to factory defaults."));
	if(Serial) println(F("INITIALIZED MODEM SETTINGS TO FACTORY DEFAULTS."));
}

void ModemBase::saveDefaults(void)
{
	EEPROM.write(E_MODE_ADDRESS, (uint8_t)_echoOn);
	EEPROM.write(V_MODE_ADDRESS, (uint8_t)_verboseResponses);
	EEPROM.write(Q_MODE_ADDRESS, (uint8_t)_quietMode);

	EEPROM.write(S1_ADDRESS, _S1_ringCounter);
	EEPROM.write(S2_ADDRESS, _S2_escapeCharacter);
	EEPROM.write(S3_ADDRESS, _S3_crCharacter);
	EEPROM.write(S4_ADDRESS, _S4_lfCharacter);
	EEPROM.write(S5_ADDRESS, _S5_bsCharacter);
	EEPROM.write(S6_ADDRESS, _S6_waitBlindDial);
	EEPROM.write(S7_ADDRESS, _S7_waitForCarrier);
	EEPROM.write(S8_ADDRESS, _S8_pauseForComma);
	EEPROM.write(S9_ADDRESS, _S9_cdResponseTime);
	EEPROM.write(S10_ADDRESS, _S10_delayHangup);
	EEPROM.write(S11_ADDRESS, _S11_dtmf);
	EEPROM.write(S12_ADDRESS, _S12_escGuardTime);
	EEPROM.write(S18_ADDRESS, _S18_testTimer);
	EEPROM.write(S25_ADDRESS, _S25_delayDTR);
	EEPROM.write(S26_ADDRESS, _S26_delayRTS2CTS);
	EEPROM.write(S30_ADDRESS, _S30_inactivityTimer);
	EEPROM.write(S37_ADDRESS, _S37_lineSpeed);
	EEPROM.write(S38_ADDRESS, _S38_delayForced);
	EEPROM.write(S90_ADDRESS, (uint8_t)_isDcdInverted);

	EEPROM.write(MODEM_INITIALIZED_ADDRESS, MODEM_INITIALIZED_STATE);
}

void ModemBase::loadDefaults(void)
{
	if(EEPROM.read(MODEM_INITIALIZED_ADDRESS) == MODEM_INITIALIZED_STATE)
	{
		_echoOn = (bool)EEPROM.read(E_MODE_ADDRESS);
		_verboseResponses = (bool)EEPROM.read(V_MODE_ADDRESS);
		_quietMode = (bool)EEPROM.read(Q_MODE_ADDRESS);

		_S0_autoAnswer = false;
		_S1_ringCounter = EEPROM.read(S1_ADDRESS);
		_S2_escapeCharacter = EEPROM.read(S2_ADDRESS);
		_S3_crCharacter = EEPROM.read(S3_ADDRESS);
		_S4_lfCharacter = EEPROM.read(S4_ADDRESS);
		_S5_bsCharacter = EEPROM.read(S5_ADDRESS);
		_S6_waitBlindDial = EEPROM.read(S6_ADDRESS);
		_S7_waitForCarrier = EEPROM.read(S7_ADDRESS);
		_S8_pauseForComma = EEPROM.read(S8_ADDRESS);
		_S9_cdResponseTime = EEPROM.read(S9_ADDRESS);
		_S10_delayHangup = EEPROM.read(S10_ADDRESS);
		_S11_dtmf = EEPROM.read(S11_ADDRESS);
		_S12_escGuardTime = EEPROM.read(S12_ADDRESS);
		_S18_testTimer = EEPROM.read(S18_ADDRESS);
		_S25_delayDTR = EEPROM.read(S25_ADDRESS);
		_S26_delayRTS2CTS = EEPROM.read(S26_ADDRESS);
		_S30_inactivityTimer = EEPROM.read(S30_ADDRESS);
		_S37_lineSpeed = EEPROM.read(S37_ADDRESS);
		_S38_delayForced = EEPROM.read(S38_ADDRESS);

		_isDcdInverted = (bool)EEPROM.read(S90_ADDRESS);
	}
	else
	{
		resetToDefaults();
	}
}

void ModemBase::begin(
	HardwareSerial *serial, 
	void (*onDisconnectHandler)(EthernetClient *client),
	void (*onDialoutHandler)(char*, ModemBase*))
{
#if DEBUG == 1
	lggr.println(F("Beginning ModemBase..."));
#endif

	onDisconnect = onDisconnectHandler;
	onDialout = onDialoutHandler;

	_escapeCount = 0;

	if(serial)
	{
		_serial = serial;
	}
	else
	{
#if DEBUG == 1
		lggr.println(F("serial is not initialized. panicking."));
#endif
		while(true);
	}

	_isCommandMode = true;
	_isConnected = false;
	_isRinging = false;

	loadDefaults();

	setLineSpeed();

	digitalWrite(DCE_DTR, toggleCarrier(false));
	digitalWrite(DCE_RTS, HIGH);
	digitalWrite(DCE_RI, LOW);

	resetCommandBuffer(true);

#ifdef __UNO__
	println(F("COMET BBS READY."));
	//print(F("S90=")); println((int)_isDcdInverted);
#else
	println(F("HAYESDUINO EXTENDED SET READY."));
#endif
#if DEBUG == 1
		lggr.println(F("Modem initialized."));
#endif
}

uint32_t ModemBase::getBaudRate(void)
{
	return _baudRate;
}

void ModemBase::setDefaultBaud(uint32_t baudRate)
{
	_baudRate = baudRate;
	_serial->begin(baudRate);
#if DEBUG == 1
	lggr.print(F("SET DEFAULT BAUD RATE: ")); lggr.println(baudRate);
#endif
}

void ModemBase::setDcdInverted(bool isInverted)
{
	_isDcdInverted = isInverted;
}

bool ModemBase::getDcdInverted(void)
{
	return _isDcdInverted;
}

bool ModemBase::getIsConnected(void)
{
	return _isConnected;
}

bool ModemBase::getIsRinging(void)
{
	return _isRinging;
}

void ModemBase::setIsRinging(bool value)
{
	_isRinging = value;
}

bool ModemBase::getIsCommandMode(void)
{
	return _isCommandMode;
}

int ModemBase::toggleCarrier(boolean isHigh)
{

	int result = _isDcdInverted ? (isHigh ? LOW : HIGH) : (isHigh ? HIGH : LOW);
	digitalWrite(STATUS_LED, result);
	return result;
}

void ModemBase::disconnect(EthernetClient *client)
{
	//println(F("Disconnecting..."));

	_isCommandMode = true;
	_isConnected = false;

	// TODO - According to http://totse2.net/totse/en/technology/telecommunications/trm.html
	//		  The BBS should detect <CR><LF>NO CARRIER<CR><LF> as a dropped carrier sequences.
	//		  DMBBS does not honor this and so I haven't sucessfully tested it, thus it's commented out.

	//delay(100);
	//print('\r'); 
	//print((char)_S4_lfCharacter);
	//print(F("NO CARRIER"));
	//print('\r'); 
	//print((char)_S4_lfCharacter);

	digitalWrite(DCE_RTS, LOW);

	delay(1000);

	if(onDisconnect != NULL)
	{
		onDisconnect(client);
	}

	digitalWrite(DCE_DTR, toggleCarrier(false));

	//delay(5000);
}

void ModemBase::writeAddressBook(uint16_t address, char * host)
{
	for(int i=0; i < 90; ++i)
	{
		EEPROM.write(address + i, host[i]);
	}
}

char * ModemBase::getAddressBook(uint16_t address)
{
	static char result[91];
	for(int i=0; i < 90; ++i)
	{
		result[i] = EEPROM.read(address + i);
	}

	return result;
}
#ifndef __UNO__
bool ModemBase::processCommandBufferExtended(EthernetClient *client)
{
	bool result = false, showOK = false;
	char msg[41];

	if(strcmp(_commandBuffer, "ATS0?") == 0)
	{
		sprintf(msg, "%u", _S0_autoAnswer);
		_serial->println(msg);
		result = true;
	}
	else if(strcmp(_commandBuffer, ("ATS999?")) == 0)
	{
		_serial->println(F("HAYESDUINO EXTENDED SET"));
	}
	else
	if(strcmp(_commandBuffer, "ATS1?") == 0)
	{
		sprintf(msg, "%u", _S1_ringCounter);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS2?") == 0)
	{
		sprintf(msg, "%u", _S2_escapeCharacter);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS3?") == 0)
	{
		sprintf(msg, "%u", _S3_crCharacter);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS4?") == 0)
	{
		sprintf(msg, "%u", _S4_lfCharacter);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS5?") == 0)
	{
		sprintf(msg, "%u", _S5_bsCharacter);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS6?") == 0)
	{
		sprintf(msg, "%u", _S6_waitBlindDial);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS7?") == 0)
	{
		sprintf(msg, "%u", _S7_waitForCarrier);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS8?") == 0)
	{
		sprintf(msg, "%u", _S8_pauseForComma);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS9?") == 0)
	{
		sprintf(msg, "%u", _S9_cdResponseTime);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS10?") == 0)
	{
		sprintf(msg, "%u", _S10_delayHangup);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS11?") == 0)
	{
		sprintf(msg, "%u", _S11_dtmf);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS12?") == 0)
	{
		sprintf(msg, "%u", _S12_escGuardTime);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS18?") == 0)
	{
		sprintf(msg, "%u", _S18_testTimer);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS25?") == 0)
	{
		sprintf(msg, "%u", _S25_delayDTR);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS26?") == 0)
	{
		sprintf(msg, "%u", _S26_delayRTS2CTS);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS30?") == 0)
	{
		sprintf(msg, "%u", _S30_inactivityTimer);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS37?") == 0)
	{
		sprintf(msg, "%u", _S37_lineSpeed);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS38?") == 0)
	{
		sprintf(msg, "%u", _S38_delayForced);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS90?") == 0)
	{
		sprintf(msg, "%u", _isDcdInverted);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATE?") == 0)
	{
		sprintf(msg, "%u", _echoOn);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATV?") == 0)
	{
		sprintf(msg, "%u", _verboseResponses);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATQ?") == 0)
	{
		sprintf(msg, "%u", _quietMode);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS300?") == 0)
	{
		char byte = EEPROM.read(0);
		sprintf(msg, "%u", byte);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS301?") == 0)
	{
		char byte = EEPROM.read(1);
		sprintf(msg, "%u", byte);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS302?") == 0)
	{
		char byte = EEPROM.read(2);
		sprintf(msg, "%u", byte);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS303?") == 0)
	{
		char byte = EEPROM.read(3);
		sprintf(msg, "%u", byte);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS304?") == 0)
	{
		char byte = EEPROM.read(4);
		sprintf(msg, "%u", byte);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS305?") == 0)
	{
		char byte = EEPROM.read(5);
		sprintf(msg, "%u", byte);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS3060?") == 0)
	{
		char byte = EEPROM.read(6);
		sprintf(msg, "%u", byte);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS307?") == 0)
	{
		char byte = EEPROM.read(7);
		sprintf(msg, "%u", byte);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS308?") == 0)
	{
		char byte = EEPROM.read(8);
		sprintf(msg, "%u", byte);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS309?") == 0)
	{
		char byte = EEPROM.read(9);
		sprintf(msg, "%u", byte);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS310?") == 0)
	{
		char byte = EEPROM.read(10);
		sprintf(msg, "%u", byte);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS311?") == 0)
	{
		char byte = EEPROM.read(11);
		sprintf(msg, "%u", byte);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS312?") == 0)
	{
		char byte = EEPROM.read(12);
		sprintf(msg, "%u", byte);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS313?") == 0)
	{
		char byte = EEPROM.read(13);
		sprintf(msg, "%u", byte);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS314?") == 0)
	{
		char byte = EEPROM.read(14);
		sprintf(msg, "%u", byte);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS315?") == 0)
	{
		char byte = EEPROM.read(15);
		sprintf(msg, "%u", byte);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS316?") == 0)
	{
		char byte = EEPROM.read(16);
		sprintf(msg, "%u", byte);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS317?") == 0)
	{
		char byte = EEPROM.read(17);
		sprintf(msg, "%u", byte);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS318?") == 0)
	{
		char byte = EEPROM.read(18);
		sprintf(msg, "%u", byte);
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS101?") == 0)
	{
		sprintf(msg, "%s", getAddressBook(ADDRESS_BOOK_1));
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS102?") == 0)
	{
		sprintf(msg, "%s", getAddressBook(ADDRESS_BOOK_2));
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS103?") == 0)
	{
		sprintf(msg, "%s", getAddressBook(ADDRESS_BOOK_3));
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS104?") == 0)
	{
		sprintf(msg, "%s", getAddressBook(ADDRESS_BOOK_4));
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS105?") == 0)
	{
		sprintf(msg, "%s", getAddressBook(ADDRESS_BOOK_5));
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS106?") == 0)
	{
		sprintf(msg, "%s", getAddressBook(ADDRESS_BOOK_6));
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS107?") == 0)
	{
		sprintf(msg, "%s", getAddressBook(ADDRESS_BOOK_7));
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS108?") == 0)
	{
		sprintf(msg, "%s", getAddressBook(ADDRESS_BOOK_8));
		_serial->println(msg);
		result = true;
	}
	else
	if(strcmp(_commandBuffer, "ATS109?") == 0)
	{
		sprintf(msg, "%s", getAddressBook(ADDRESS_BOOK_9));
		_serial->println(msg);
		result = true;
	}

	if(showOK)
	{
		delay(500);
		printOK();
	}

	return result;
}
#endif

void ModemBase::processCommandBuffer(EthernetClient *client)
{
	for(int i=0; i < strlen(_commandBuffer); ++i)
	{
		_commandBuffer[i] = toupper(_commandBuffer[i]);
	}

	if(strcmp(_commandBuffer, ("AT/")) == 0)
	{
		strcpy(_commandBuffer, _lastCommandBuffer);
	}

	if(strcmp(_commandBuffer, ("ATZ")) == 0)
	{
		loadDefaults();
		printOK();
	}
	else if(strncmp(_commandBuffer, ("ATT "), 4) == 0)
	{
		
		EthernetClient *newClient = new EthernetClient();

		IPAddress remote_addr;
		String result;
		int counter = 0;
		char buffer[81];
		char host[81];
		strncpy(host, _commandBuffer + 5, 80);
		host[strlen(host) - 1] = '\0';

		DNSClient dns;

		dns.begin(Ethernet.dnsServerIP());
		dns.getHostByName(_commandBuffer + 4, remote_addr);
		//remote_addr.printTo(*_serial);
		if(newClient->connect(remote_addr, 13))
		{
			delay(100);
			if(newClient->available())
			{
				while(counter++ < 5)
				{
					getString(newClient, buffer, 80);
					if(strlen(buffer) > 0) break;
				}

				if(counter < 5)
				{
					_serial->println(result);
				}
				else
				{
					_serial->println(F("INVALID RESPONSE."));
				}
			}
			else
			{
				_serial->println(F("no data"));
			}

			newClient->stop();
			delete newClient;
		}
		else
		{
			_serial->println(F("COULD NOT CONNECT."));
		}
	}
	else if(strcmp(_commandBuffer, ("AT&W")) == 0)
	{
		saveDefaults();
#if DEBUG == 1
		lggr.println("Saved to defaults.");
#endif
		printOK();
	}
	else if(strcmp(_commandBuffer, ("AT&F")) == 0)
	{
		if(strcmp(_lastCommandBuffer, ("AT&F")) == 0)
		{
			resetToDefaults();
			loadDefaults();
			printOK();
		}
		else
		{
			_serial->println(F("send command again to verify."));
		}
	}
	else if(strcmp(_commandBuffer, ("ATA")) == 0)
	{
		_isConnected = true;
		_isCommandMode = false;
		_isRinging = false;

		if(_baudRate == 38400)
		{
			printResponse("28", F("CONNECT 38400"));
		}
		else if(_baudRate == 19200)
		{
			printResponse("14", F("CONNECT 19200"));
		}
		else if(_baudRate == 14400)
		{
			printResponse("13", F("CONNECT 14400"));
		}
		else if(_baudRate == 9600)
		{
			printResponse("12", F("CONNECT 9600"));
		}
		else if(_baudRate == 4800)
		{
			printResponse("11", F("CONNECT 4800"));
		}
		else if(_baudRate == 2400)
		{
			printResponse("10", F("CONNECT 2400"));
		}
		else if(_baudRate == 1200)
		{
			printResponse("5", F("CONNECT 1200"));
		}
		else
		{
			if(!_verboseResponses)    
				_serial->println('1');
			else
			{
				_serial->print(F("CONNECT "));
				_serial->println(_baudRate);
			}
		}

		digitalWrite(DCE_RTS, LOW);
	}
	else if(strcmp(_commandBuffer, ("ATD")) == 0)
	{
		////_isConnected = true;
	}
	else if(
		strncmp(_commandBuffer, ("ATDT "), 5) == 0 ||
		strncmp(_commandBuffer, ("ATDP "), 5) == 0 ||
		strncmp(_commandBuffer, ("ATD "), 4) == 0
		)
	{
		if(onDialout != NULL)
		{
			onDialout(strstr(_commandBuffer, " ") + 1, this);
		}
		else
		{
			println(F("onDialout is null"));
		}
	}
	else if(strncmp(_commandBuffer, ("ATD"), 3) == 0)
	{
		if(onDialout != NULL)
		{
			switch(_commandBuffer[3])
			{
			case '0': onDialout(getAddressBook(ADDRESS_BOOK_0), this); break;
			case '1': onDialout(getAddressBook(ADDRESS_BOOK_1), this); break;
			case '2': onDialout(getAddressBook(ADDRESS_BOOK_2), this); break;
			case '3': onDialout(getAddressBook(ADDRESS_BOOK_3), this); break;
			case '4': onDialout(getAddressBook(ADDRESS_BOOK_4), this); break;
			case '5': onDialout(getAddressBook(ADDRESS_BOOK_5), this); break;
			case '6': onDialout(getAddressBook(ADDRESS_BOOK_6), this); break;
			case '7': onDialout(getAddressBook(ADDRESS_BOOK_7), this); break;
			case '8': onDialout(getAddressBook(ADDRESS_BOOK_8), this); break;
			case '9': onDialout(getAddressBook(ADDRESS_BOOK_9), this); break;
			}
		}
	}
	else if((strcmp(_commandBuffer, ("ATH0")) == 0
		|| strcmp(_commandBuffer, ("ATH")) == 0))
	{
#if DEBUG == 1
		lggr.println(F("Hanging up...."));
#endif
		disconnect(client);
	}
	else if(strcmp(_commandBuffer, ("ATO")) == 0 && 
		_isConnected)
	{
		_isCommandMode = false;
	}
	else if(strncmp(_commandBuffer, ("AT"), 2) == 0)
	{
		if(strstr(_commandBuffer, ("E0")) != NULL)
		{
			_echoOn = false;
#if DEBUG == 1
			lggr.println("E0");
#endif
		}

		if(strstr(_commandBuffer, ("E1")) != NULL)
		{
			_echoOn = true;
#if DEBUG == 1
			lggr.println("E1");
#endif
		}

		if(strstr(_commandBuffer, ("M0")) != NULL)
		{
#if DEBUG == 1
			lggr.println("M0 - UNUSED");
#endif
		}



		if(strstr(_commandBuffer, ("Q0")) != NULL)
		{
			_verboseResponses = false;
			_quietMode = false;
#if DEBUG == 1
			lggr.println("Q0");
#endif
		}

		if(strstr(_commandBuffer, ("Q1")) != NULL)
		{
			_quietMode = true;
#if DEBUG == 1
			lggr.println("Q1");
#endif
		}

		if(strstr(_commandBuffer, ("V0")) != NULL)
		{
			_verboseResponses = false;
#if DEBUG == 1
			lggr.println("V0");
#endif
		}

		if(strstr(_commandBuffer, ("V1")) != NULL)
		{
			_verboseResponses = true;
#if DEBUG == 1
			lggr.println("V1");
#endif
		}

		if(strstr(_commandBuffer, ("X0")) != NULL)
		{
#if DEBUG == 1
			lggr.println("X0 - UNUSED");
#endif
		}

		if(strstr(_commandBuffer, ("X1")) != NULL)
		{
#if DEBUG == 1
			lggr.println("X1 - UNUSED");
#endif
		}

		if(strstr(_commandBuffer, ("X2")) != NULL)
		{
#if DEBUG == 1
			lggr.println("X2 - UNUSED");
#endif
		}

		char *currentS;
		char temp[100];

		int offset = 0;
		if((currentS = strstr(_commandBuffer, ("S0="))) != NULL)
		{
			offset = 3;
			while(currentS[offset] != '\0'
				&& isDigit(currentS[offset]))
			{
				offset++;
			}

			memset(temp, 0, 100);
			strncpy(temp, currentS + 3, offset - 3);
			_S0_autoAnswer = atoi(temp);
#if DEBUG == 1
			lggr.print(F("S0=")); lggr.println(temp);
#endif
		}

		if((currentS = strstr(_commandBuffer, ("S1="))) != NULL)
		{
			offset =3;
			while(currentS[offset] != '\0'
				&& isDigit(currentS[offset]))
			{
				offset++;
			}

			memset(temp, 0, 100);
			strncpy(temp, currentS + 3, offset - 3);
			_S1_ringCounter = atoi(temp);
#if DEBUG == 1
			lggr.print(F("S1=")); lggr.println(temp);
#endif
		}

		if((currentS = strstr(_commandBuffer, ("S2="))) != NULL)
		{
			offset =3;
			while(currentS[offset] != '\0'
				&& isDigit(currentS[offset]))
			{
				offset++;
			}

			memset(temp, 0, 100);
			strncpy(temp, currentS + 3, offset - 3);
			_S2_escapeCharacter = atoi(temp);
#if DEBUG == 1
			lggr.print(F("S2=")); lggr.println(temp);
#endif
		}

		if((currentS = strstr(_commandBuffer, ("S3="))) != NULL)
		{
			offset =3;
			while(currentS[offset] != '\0'
				&& isDigit(currentS[offset]))
			{
				offset++;
			}

			memset(temp, 0, 100);
			strncpy(temp, currentS + 3, offset - 3);
			_S3_crCharacter = atoi(temp);
#if DEBUG == 1
			lggr.print(F("S3=")); lggr.println(temp);
#endif
		}

		if((currentS = strstr(_commandBuffer, ("S4="))) != NULL)
		{
			offset =3;
			while(currentS[offset] != '\0'
				&& isDigit(currentS[offset]))
			{
				offset++;
			}

			memset(temp, 0, 100);
			strncpy(temp, currentS + 3, offset - 3);
			_S4_lfCharacter = atoi(temp);
#if DEBUG == 1
			lggr.print(F("S4=")); lggr.println(temp);
#endif
		}

		if((currentS = strstr(_commandBuffer, ("S5="))) != NULL)
		{
			offset =3;
			while(currentS[offset] != '\0'
				&& isDigit(currentS[offset]))
			{
				offset++;
			}

			memset(temp, 0, 100);
			strncpy(temp, currentS + 3, offset - 3);
			_S5_bsCharacter = atoi(temp);
#if DEBUG == 1
			lggr.print(F("S5=")); lggr.println(temp);
#endif
		}

		if((currentS = strstr(_commandBuffer, ("S6="))) != NULL)
		{
			offset =3;
			while(currentS[offset] != '\0'
				&& isDigit(currentS[offset]))
			{
				offset++;
			}

			memset(temp, 0, 100);
			strncpy(temp, currentS + 3, offset - 3);
			_S6_waitBlindDial = atoi(temp);
#if DEBUG == 1
			lggr.print(F("S6=")); lggr.println(temp);
#endif
		}

		if((currentS = strstr(_commandBuffer, ("S7="))) != NULL)
		{
			offset =3;
			while(currentS[offset] != '\0'
				&& isDigit(currentS[offset]))
			{
				offset++;
			}

			memset(temp, 0, 100);
			strncpy(temp, currentS + 3, offset - 3);
			_S7_waitForCarrier = atoi(temp);
#if DEBUG == 1
			lggr.print(("S7=")); lggr.println(temp);
#endif
		}

		if((currentS = strstr(_commandBuffer, ("S8="))) != NULL)
		{
			offset =3;
			while(currentS[offset] != '\0'
				&& isDigit(currentS[offset]))
			{
				offset++;
			}

			memset(temp, 0, 100);
			strncpy(temp, currentS + 3, offset - 3);
			_S8_pauseForComma = atoi(temp);
#if DEBUG == 1
			lggr.print(F("S8=")); lggr.println(temp);
#endif
		}

		if((currentS = strstr(_commandBuffer, ("S9="))) != NULL)
		{
			offset =3;
			while(currentS[offset] != '\0'
				&& isDigit(currentS[offset]))
			{
				offset++;
			}

			memset(temp, 0, 100);
			strncpy(temp, currentS + 3, offset - 3);
			_S9_cdResponseTime = atoi(temp);
#if DEBUG == 1
			lggr.print(F("S9=")); lggr.println(temp);
#endif
		}

		if((currentS = strstr(_commandBuffer, ("S10="))) != NULL)
		{
			offset =4;
			while(currentS[offset] != '\0'
				&& isDigit(currentS[offset]))
			{
				offset++;
			}

			memset(temp, 0, 100);
			strncpy(temp, currentS + 4, offset - 4);
			_S10_delayHangup = atoi(temp);
#if DEBUG == 1
			lggr.print(F("S10=")); lggr.println(temp);
#endif
		}

		if((currentS = strstr(_commandBuffer, ("S11="))) != NULL)
		{
			offset =4;
			while(currentS[offset] != '\0'
				&& isDigit(currentS[offset]))
			{
				offset++;
			}

			memset(temp, 0, 100);
			strncpy(temp, currentS + 4, offset - 4);
			_S11_dtmf = atoi(temp);
#if DEBUG == 1
			lggr.print(F("S11=")); lggr.println(temp);
#endif
		}

		if((currentS = strstr(_commandBuffer, ("S12="))) != NULL)
		{
			offset =4;
			while(currentS[offset] != '\0'
				&& isDigit(currentS[offset]))
			{
				offset++;
			}

			memset(temp, 0, 100);
			strncpy(temp, currentS + 4, offset - 4);
			_S12_escGuardTime = atoi(temp);
#if DEBUG == 1
			lggr.print(F("S12=")); lggr.println(temp);
#endif
		}

		if((currentS = strstr(_commandBuffer, ("S18="))) != NULL)
		{
			offset =4;
			while(currentS[offset] != '\0'
				&& isDigit(currentS[offset]))
			{
				offset++;
			}

			memset(temp, 0, 100);
			strncpy(temp, currentS + 4, offset - 4);
			_S18_testTimer = atoi(temp);
#if DEBUG == 1
			lggr.print(F("S18=")); lggr.println(temp);
#endif
		}

		if((currentS = strstr(_commandBuffer, ("S25="))) != NULL)
		{
			offset =4;
			while(currentS[offset] != '\0'
				&& isDigit(currentS[offset]))
			{
				offset++;
			}

			memset(temp, 0, 100);
			strncpy(temp, currentS + 4, offset - 4);
			_S25_delayDTR = atoi(temp);
#if DEBUG == 1
			lggr.print(F("S25=")); lggr.println(temp);
#endif
		}

		if((currentS = strstr(_commandBuffer, ("S26="))) != NULL)
		{
			offset =4;
			while(currentS[offset] != '\0'
				&& isDigit(currentS[offset]))
			{
				offset++;
			}

			memset(temp, 0, 100);
			strncpy(temp, currentS + 4, offset - 4);
			_S26_delayRTS2CTS = atoi(temp);
#if DEBUG == 1
			lggr.print(F("S26=")); lggr.println(temp);
#endif
		}

		if((currentS = strstr(_commandBuffer, ("S30="))) != NULL)
		{
			offset =4;
			while(currentS[offset] != '\0'
				&& isDigit(currentS[offset]))
			{
				offset++;
			}

			memset(temp, 0, 100);
			strncpy(temp, currentS + 4, offset - 4);
			_S30_inactivityTimer = atoi(temp);
#if DEBUG == 1
			lggr.print(F("S30=")); lggr.println(temp);
#endif
		}

		if((currentS = strstr(_commandBuffer, ("S37="))) != NULL)
		{
			offset =4;
			while(currentS[offset] != '\0'
				&& isDigit(currentS[offset]))
			{
				offset++;
			}

			memset(temp, 0, 100);
			strncpy(temp, currentS + 4, offset - 4);
			_S37_lineSpeed = atoi(temp);
#if DEBUG == 1
			lggr.print(F("S37=")); lggr.println(temp);
#endif

			EEPROM.write(S37_ADDRESS, _S37_lineSpeed);

			setLineSpeed();

#if DEBUG == 1
			lggr.print(F("Set Baud Rate: ")); lggr.println(_baudRate);
#endif
		}

		if((currentS = strstr(_commandBuffer, ("S38="))) != NULL)
		{
			offset =4;
			while(currentS[offset] != '\0'
				&& isDigit(currentS[offset]))
			{
				offset++;
			}

			memset(temp, 0, 100);
			strncpy(temp, currentS + 4, offset - 4);
			_S38_delayForced = atoi(temp);
#if DEBUG == 1
			lggr.print(F("S38=")); lggr.println(temp);
#endif
		}

		if((currentS = strstr(_commandBuffer, ("S90="))) != NULL)
		{
			offset =4;
			while(currentS[offset] != '\0'
				&& isDigit(currentS[offset]))
			{
				offset++;
			}

			memset(temp, 0, 100);
			strncpy(temp, currentS + 4, offset - 4);
			_isDcdInverted = (bool)atoi(temp);
#if DEBUG == 1
			lggr.print(F("S90=")); lggr.println(temp);
#endif
			print(F("SAVED S90=")); println(temp);
			EEPROM.write(S90_ADDRESS, atoi(temp));
		}

		if((currentS = strstr(_commandBuffer, ("S200="))) != NULL)
		{
			offset =5;
			while(currentS[offset] != '\0'
				&& isDigit(currentS[offset]))
			{
				offset++;
			}

			memset(temp, 0, 100);
			strncpy(temp, currentS + 5, offset - 5);
			_baudRate = atoi(temp);
			setDefaultBaud(_baudRate);
#if DEBUG == 1
			lggr.print(F("S90=")); lggr.println(temp);
#endif
//			print(F("SAVED S200=")); println(temp);
//			EEPROM.write(S90_ADDRESS, atoi(temp));
		}

		for(int i=MAC_1 + 300; i<=USE_DHCP + 300; ++i)
		{
			char reg[6];
			sprintf(reg, "S%u=", i);

			if((currentS = strstr(_commandBuffer, reg)) != NULL)
			{
				int value = atoi(currentS + 5);
				EEPROM.write(i - 300, value);
				print(F("SAVED S")); print(i); print("="); println(value);
			}
		}

		for(int i=1; i<10; ++i)
		{
			char reg[6];
			sprintf(reg, ("S10%u="), i);
			if((currentS = strstr(_commandBuffer, reg)) != NULL)
			{
				int offset = 5;
				while(currentS[offset] != '\0')
				{
					offset++;
				}

				memset(temp, 0, 100);
				strncpy(temp, currentS + 5, offset - 5);
#if DEBUG == 1
				lggr.print(reg); lggr.println(temp);
#endif
				print(F("SAVED ")); print(reg); print('='); println(temp);
				writeAddressBook(ADDRESS_BOOK_START + (ADDRESS_BOOK_LENGTH * i), temp);
			}
		}
#ifndef __UNO__
		if(!processCommandBufferExtended(client))
		{
#endif
			printOK();
#ifndef __UNO__
		}
#endif
	}
	else
	{
		printResponse("4", F("ERROR"));

		//lggr.println(F("Sent ERROR/4"));
	}

	strcpy(_lastCommandBuffer, _commandBuffer);
	resetCommandBuffer(false);

	//println(F("after resetCommandBuffer(false)")); delay(1000);
}

void ModemBase::connect(EthernetClient *client)
{
	_isConnected = _isRinging = true;

	if(_S0_autoAnswer != 0) 
	{
		strcpy(_commandBuffer, (const char*)F("ATA\n"));
		processCommandBuffer(client);
	}
	else
	{
		printResponse("2", F("RING"));

		digitalWrite(DTE_DCD, toggleCarrier(true));

		digitalWrite(DCE_RI, HIGH);
		delay(250);
		digitalWrite(DCE_RI, LOW);
	}
}

void ModemBase::connectOut()
{
	if(_verboseResponses)
	{
		_serial->print(F("CONNECT "));
	}

	if(_baudRate == 38400)
	{
		printResponse("28", F("38400"));
	}
	else if(_baudRate == 19200)
	{
		printResponse("15", F("19200"));
	}
	else if(_baudRate == 14400)
	{
		printResponse("13", F("14400"));
	}
	else if(_baudRate == 9600)
	{
		printResponse("12", F("9600"));
	}
	else if(_baudRate == 4800)
	{
		printResponse("11", F("4800"));
	}
	else if(_baudRate == 2400)
	{
		printResponse("10", F("2400"));
	}
	else if(_baudRate == 1200)
	{
		printResponse("5", F("1200"));
	}
	else
	{
		if(!_verboseResponses)    
			_serial->println('1');
		else
		{
			_serial->println(_baudRate);
		}
	}


	digitalWrite(DCE_DTR, toggleCarrier(true));

	_isConnected = true;
	_isCommandMode = false;
	_isRinging = false;
}

void ModemBase::processData(EthernetClient *cl)
{
	while(_serial->available())
	{
		//digitalWrite(DCE_RTS, HIGH);
		if(_isCommandMode)
		{
			char inbound = _serial->read();
			if(_echoOn && inbound != _S2_escapeCharacter)
			{
				_serial->write(inbound);
			}
			if(inbound == _S2_escapeCharacter) _escapeCount++;
			else _escapeCount = 0;
			if(_escapeCount == 3)
			{
				_escapeCount = 0;

				printOK();
			}
			if(inbound == _S5_bsCharacter)
			{
				if(strlen(_commandBuffer) > 0)
				{
					_commandBuffer[strlen(_commandBuffer) - 1] = '\0';
				}
			}
			else if(inbound != '\r' && inbound != '\n' && inbound != _S2_escapeCharacter)
			{
				_commandBuffer[strlen(_commandBuffer)] = inbound;
			}
			else if(_commandBuffer[0] == 'A' && _commandBuffer[1] == 'T')
			{
#if DEBUG == 1
				lggr.print(F("Processing command "));
				lggr.println(_commandBuffer);
#endif
				processCommandBuffer(cl);
			}
			else
			{
				memset(_commandBuffer, 0, sizeof(_commandBuffer));
			}
		}
		else
		{
			if(_isConnected)
			{
				char inbound = _serial->read();
				if(_echoOn) _serial->write(inbound);
				if(inbound == _S2_escapeCharacter) _escapeCount++;
				else _escapeCount = 0;
				if(_escapeCount == 3)
				{
					_escapeCount = 0;
					_isCommandMode = true;

					if(!_verboseResponses)    
						_serial->println(F("0"));
					else
						_serial->println(F("OK"));
				}

				if(!_isCommandMode)
				{
					int result = cl->write(inbound);
					if(result != 1) 
					{
						println();
						print("write error: ");
						println(cl->getWriteError());
					}
				}
			}
		}
	}
	digitalWrite(DCE_RTS, LOW);
}

void ModemBase::setLineSpeed(void)
{
	switch(_S37_lineSpeed)
	{
	case 0: setDefaultBaud(300); break;
	case 1: setDefaultBaud(300); break;
	case 2: setDefaultBaud(300); break;
	case 3: setDefaultBaud(300); break;
	case 5: setDefaultBaud(1200); break;
	case 6: setDefaultBaud(2400); break;
	case 8: setDefaultBaud(9600); break;
	case 10: setDefaultBaud(14400); break;
	case 11: setDefaultBaud(28800); break;
	case 12: setDefaultBaud(57600); break;
	case 13: setDefaultBaud(115200); break;
	default: setDefaultBaud(2400); break;
	}
}

void ModemBase::resetToDefaults(void)
{
	factoryReset();
}

void ModemBase::printOK(void)
{
	printResponse("0", F("OK"));
}

void ModemBase::printResponse(const char* code, const char* msg)
{
	if(!_verboseResponses)
		_serial->println(code);
	else
		_serial->println(msg);
}

void ModemBase::printResponse(const char* code, const __FlashStringHelper * msg)
{
	if(!_verboseResponses)
		_serial->println(code);
	else
		_serial->println(msg);
}