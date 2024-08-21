#include "SIM800L.h"

////////////////////////////////////////////////////PRIVATE DEFINITION////////////////////////////////////////////////////

String SIM800L::_readSerial()
{
	_timeout = 0;
	while (!_serial->available() && _timeout < (TIMEOUT * 100))
	{
		_Delay(10);
		yield();
		_timeout++;
	}
	if (_serial->available())
	{
		return _serial->readString();
	}
	return "";
}

void SIM800L::_clearSerial()
{
	if (_serial->available())
	{
		_serial->readString();
	}
}

void SIM800L::_Delay(unsigned long ms)
{
	unsigned long startTime = millis();
	while (millis() <= startTime + ms)
	{
	}
}

/////////////////////////////////////////////////INSTANT/INIT DEFINITION/////////////////////////////////////////////////

SIM800L::SIM800L(void)
{
}

bool SIM800L::begin(Stream &serial) // begin Definition with Serial port assignment
{
	cSFA(_serialBuffer, _charBuffer);
	_serialBuffer.clear();

	_serial = &serial;

	_Delay(1000);
	yield();
	_clearSerial();

	_serial->print(F("AT\r\n"));
	_serialBuffer = _readSerial().c_str();
	if ((_serialBuffer.indexOf("OK")) != -1)
	{
		yield();
		_clearSerial();
		_serial->print(F("ATE0\r\n"));
		_serialBuffer = _readSerial().c_str();
		if ((_serialBuffer.indexOf("OK")) != -1)
		{
			_clearSerial();
			_serial->print(F("AT+CLIP=1\r\n")); // ENABLE CALLER ID
			return enAutoTimeZone();
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return false;
}

bool SIM800L::begin(Stream &serial, uint8_t pin) // begin Definition with Serial port and reset pin assignment
{
	rstpin = pin;
	pinMode(rstpin, OUTPUT);
	digitalWrite(rstpin, LOW);
	rstDeclair = true;
	return begin(serial);
}

////////////////////////////////////////////////////PUBLIC DEFINITION////////////////////////////////////////////////////
/*void SIM800L::tcpCallBack(void (*callback)(const char* _data, const uint16_t len))
{
	this->tcp_callback = callback;
}*/

bool SIM800L::startGPRS()
{
	cSFA(_serialBuffer, _charBuffer);
	_serialBuffer.clear();

	_clearSerial();
	_serial->print(F("AT+CIPSHUT\r\n"));
	_Delay(200);
	_serial->print(F("AT+CIPMUX=1\r\n"));
	_Delay(200);
	_serial->print(F("AT+CIPQSEND=1\r\n"));
	_Delay(200);
	_serial->print(F("AT+CIPRXGET=1\r\n"));
	_Delay(200);
	_serial->print(F("AT+CSTT=\"\"\r\n"));
	_Delay(200);
	_clearSerial();
	_serial->println("AT+CIICR");
	_serialBuffer = _readSerial().c_str();
	if ((_serialBuffer.indexOf("OK")) == -1)
	{
		return false;
	}
	_clearSerial();
	_serial->println("AT+CIFSR;E0");
	_serialBuffer = _readSerial().c_str();
	if ((_serialBuffer.indexOf("ERROR")) != -1)
	{
		return false;
	}

	_clearSerial();
	_serial->println("AT+CDNSCFG=\"8.8.8.8\",\"8.8.4.4\"");
	_serialBuffer = _readSerial().c_str();
	if ((_serialBuffer.indexOf("OK")) == -1)
	{
		return false;
	}

	return true;
}

void SIM800L::tcpConnect(char *host, uint16_t port)
{
	char _tempBuff[50] = {0};
	sprintf(_tempBuff, "AT+CIPSTART=0,\"TCP\",\"%s\",\"%d\"\r\n", host, port);
	_serial->print(_tempBuff);
}

bool SIM800L::tcpStatus()
{
	cSFA(_serialBuffer, _charBuffer);
	_serialBuffer.clear();

	_clearSerial();
	_serial->print(F("AT+CIPSTATUS=0\r\n"));
	_serialBuffer = _readSerial().c_str();
	if ((_serialBuffer.indexOf("+CIPSTATUS:")) != -1)
	{
		if ((_serialBuffer.indexOf("CONNECTED")) != -1)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

int16_t SIM800L::tcpAvailable()
{
	cSFA(_serialBuffer, _charBuffer);
	_serialBuffer.clear();

	_clearSerial();
	_serial->print(F("AT+CIPRXGET=4,0\r\n"));
	_serialBuffer = _readSerial().c_str();
	if ((_serialBuffer.indexOf("+CIPRXGET: 4,0,")) != -1)
	{
		createSafeString(result, 128, "");
		int returnInt;
		_serialBuffer.substring(result, _serialBuffer.indexOf("+CIPRXGET: 4,0,") + 15, _serialBuffer.indexOf("\r", _serialBuffer.indexOf("+CIPRXGET: 4,0,") + 15));
		result.toInt(returnInt);
		return returnInt;
	}

	return -1;
}

void SIM800L::tcpRead(char *buffer, uint16_t length)
{
	cSFA(_serialBuffer, _charBuffer);
	_serialBuffer.clear();

	char _tempBuff[30] = {0};
	sprintf(_tempBuff, "AT+CIPRXGET=2,0,%d\r\n", length);
	_clearSerial();
	_serial->print(_tempBuff);
	_serialBuffer = _readSerial().c_str();
	if ((_serialBuffer.indexOf("+CIPRXGET: 2,0,")) != -1)
	{
		uint16_t dataindex = _serialBuffer.indexOf('\n', _serialBuffer.indexOf(",", _serialBuffer.indexOf("+CIPRXGET: 2,0,"))) + 1;

		memset(_tempBuff, '\0', sizeof(_tempBuff));
		for (uint16_t i = 0; i < length; i++)
		{
			buffer[i] = _serialBuffer[i + dataindex];
		}
	}
}

void SIM800L::tcpSend(char *buffer)
{
	_clearSerial();
	_serial->print("AT+CIPSEND=0\r\n");
	_Delay(500);
	_serial->print(buffer);
	_clearSerial();
	_serial->write(0x1A); // command for send data;
}

/*void SIM800L::loop()
{
	cSFA(_serialBuffer, _charBuffer);
	_serialBuffer.clear();
	if(available())
	{
		_serialBuffer=_readSerial();
		if((_serialBuffer.indexOf("+IPD,"))!=-1)
		{
			uint16_t size = _serialBuffer.substring(_serialBuffer.indexOf("+IPD,")+5,_serialBuffer.indexOf(":")).toInt();
			uint16_t dataindex = _serialBuffer.indexOf(':',_serialBuffer.indexOf("+IPD,"))+1;
			char _tempBuff[size+1]={0};
			for(uint16_t i=0;i<size;i++)
			{
				_tempBuff[i]=_serialBuffer[i+dataindex];
			}

			(*tcp_callback)(_tempBuff, size);
		}
	}
}*/

bool SIM800L::available()
{
	if (_serial->available())
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void SIM800L::incomingCall(SafeString &returnSFstr)
{
	cSFA(_serialBuffer, _charBuffer);
	_serialBuffer.clear();

	/*Example buffer:
	RING
	+CLIP: "+987654321234", 145,"","CallerID",0
	*/
	_serialBuffer = _readSerial().c_str();
	if ((_serialBuffer.indexOf("RING")) != -1)
	{
		int in1 = _serialBuffer.indexOf("+CLIP: \"") + String("+CLIP: \"").length();
		int in2 = _serialBuffer.indexOf("\",", in1);
		_serialBuffer.substring(returnSFstr, in1, in2);
	}
	else
	{
		returnSFstr = "";
	}
}

bool SIM800L::dialNumber(const char *phoneNumber)
{
	cSFA(_serialBuffer, _charBuffer);
	_serialBuffer.clear();

	_clearSerial();
	_serial->print(F("ATD"));
	_serial->print(phoneNumber);
	_serial->print(F(";\r\n"));
	_serialBuffer = _readSerial().c_str();
	if ((_serialBuffer.indexOf("OK")) != -1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool SIM800L::answerCall()
{
	cSFA(_serialBuffer, _charBuffer);
	_serialBuffer.clear();

	_clearSerial();
	_serial->print(F("ATA\r\n"));
	_serialBuffer = _readSerial().c_str();
	if ((_serialBuffer.indexOf("OK")) != -1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool SIM800L::hangoffCall()
{
	cSFA(_serialBuffer, _charBuffer);
	_serialBuffer.clear();

	_clearSerial();
	_serial->print(F("ATH\r\n"));
	_serialBuffer = _readSerial().c_str();
	if ((_serialBuffer.indexOf("OK")) != -1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool SIM800L::forwardCall(const char *phoneNumber)
{
	cSFA(_serialBuffer, _charBuffer);
	_serialBuffer.clear();

	_clearSerial();
	_serial->print(F("AT+CCFC=0,3,"));
	_serial->print(phoneNumber);
	_serial->print(F(",145\r\n"));

	_serialBuffer = _readSerial().c_str();

	if ((_serialBuffer.indexOf("OK")) != -1)
	{
		return true;
	}
	else
	{
		return false;
	}

	return false;
}

bool SIM800L::stopForwading()
{
	cSFA(_serialBuffer, _charBuffer);
	_serialBuffer.clear();

	_clearSerial();
	_serial->print(F("AT+CCFC=0,0\r\n"));
	_serialBuffer = _readSerial().c_str();
	if ((_serialBuffer.indexOf("OK")) != -1)
	{
		return true;
	}
	else
	{
		return false;
	}

	return false;
}

bool SIM800L::startMPTY(const char *originNumber, const char *destinationNumber, unsigned long callHoldTimeout)
{
	cSFA(_serialBuffer, _charBuffer);
	_serialBuffer.clear();

	_clearSerial();
	_serial->print(F("AT+CHLD=2\r\n")); // hold the call
	_serialBuffer = _readSerial().c_str();
	if ((_serialBuffer.indexOf("OK")) != -1)
	{
	}
	else
	{
		return false;
	}

	if (dialNumber(destinationNumber))
	{
	}
	else
	{
		_clearSerial();
		_serial->print(F("AT+CHLD=0\r\n"));
		return false;
	}

	unsigned long startTime = millis();
	while (callStatus(destinationNumber) != 0)
	{
		if (millis() - startTime > callHoldTimeout)
		{
			_clearSerial();
			_serial->print(F("AT+CHLD=0\r\n"));
			return false;
		}
	}

	if (callStatus(originNumber) == 1 && callStatus(destinationNumber) == 0)
	{
		_clearSerial();
		_serial->print(F("AT+CHLD=3\r\n"));
		_serialBuffer = _readSerial().c_str();

		if ((_serialBuffer.indexOf("OK")) != -1)
		{
			return true;
		}
		else
		{
			_clearSerial();
			_serial->print(F("AT+CHLD=0\r\n"));
			return false;
		}
	}
	else
	{
		_clearSerial();
		_serial->print(F("AT+CHLD=0\r\n"));
		return false;
	}

	_clearSerial();
	_serial->print(F("AT+CHLD=0\r\n"));
	return false;
}

int8_t SIM800L::callStatus()
{
	cSFA(_serialBuffer, _charBuffer);
	_serialBuffer.clear();

	_clearSerial();
	_serial->print(F("AT+CLCC\r\n"));
	_serialBuffer = _readSerial().c_str();

	if ((_serialBuffer.indexOf("+CLCC: ")) != -1)
	{
		createSafeString(result, 128, "");
		int returnInt;
		_serialBuffer.substring(result, _serialBuffer.indexOf("+CLCC: ") + 11, _serialBuffer.indexOf("+CLCC: ") + 13);
		result.toInt(returnInt);
		return returnInt;
	}
	else if ((_serialBuffer.indexOf("OK")) != -1)
	{
		return -1;
	}

	return -1;
}
int8_t SIM800L::callStatus(const char *phoneNumber)
{
	cSFA(_serialBuffer, _charBuffer);
	_serialBuffer.clear();

	_clearSerial();
	_serial->print(F("AT+CLCC\r\n"));
	_serialBuffer = _readSerial().c_str();

	// Template: +CLCC: 1,1,4,0,0,"+9XXXXXXXXXXX",145,""

	if ((_serialBuffer.indexOf("+CLCC: ")) != -1)
	{
		if ((_serialBuffer.indexOf(phoneNumber)) != -1)
		{
			createSafeString(result, 128, "");
			int returnInt;

			_serialBuffer.substring(result, _serialBuffer.indexOf(phoneNumber) - 7, _serialBuffer.indexOf(phoneNumber) - 5);
			result.toInt(returnInt);
			return returnInt;
		}
		else
		{
			return -1;
		}
	}
	else if ((_serialBuffer.indexOf("OK")) != -1)
	{
		return -1;
	}

	return -1;
}

bool SIM800L::sendSMS(const char *number, const char *text)
{
	cSFA(_serialBuffer, _charBuffer);
	_serialBuffer.clear();

	_clearSerial();
	_serial->print(F("ATZ\r\n"));
	_serialBuffer = _readSerial().c_str();
	_serial->print(F("AT+CMGF=1\r\n")); // set sms to text mode
	_serialBuffer = _readSerial().c_str();
	_serial->print(F("AT+CSCS=\"IRA\"\r\n"));
	_serialBuffer = _readSerial().c_str();
	_serial->print(F("AT+CMGS=\"")); // command to prepare smss
	_serial->print(number);
	_serial->print(F("\", 145\r"));
	_serialBuffer = _readSerial().c_str();
	_serial->print(text);
	_serial->print(F("\r"));
	_serial->write(0x1A); // command for send sms

	_clearSerial();
	uint32_t tempTime = millis();
	while (millis() - tempTime <= 20000) // Wait for SMS sent response for 1 minute(Maximum response time of AT+CMGS is 1 min)
	{
		if (available())
		{
			_serialBuffer = _readSerial().c_str();
			if (((_serialBuffer.indexOf("+CMGS")) != -1))
			{
				return true;
			}
		}
	}

	return false;
}

void SIM800L::readSMS(uint8_t msgIndex, SafeString &returnSFstr)
{
	cSFA(_serialBuffer, _charBuffer);
	_serialBuffer.clear();

	_clearSerial();
	_serial->print(F("AT+CMGF=1\r")); // set sms to text mode
	_serialBuffer = _readSerial().c_str();
	if ((_serialBuffer.indexOf("ERR")) != -1) // CHECK IF ERROR
	{
		_serial->print(F("AT+CMGR="));
		_serial->print(msgIndex);
		_serial->print(F("\r"));
		_serialBuffer = _readSerial().c_str();
		if (_serialBuffer.indexOf("CMGR:") != -1)
		{
			returnSFstr = _serialBuffer.c_str();
		}
		else
		{
			returnSFstr = "";
		}
	}

	returnSFstr = "";
}

bool SIM800L::sendHEXsms(const char *number, const char *text)
{
	cSFA(_serialBuffer, _charBuffer);
	_serialBuffer.clear();

	_clearSerial();
	_serial->print(F("ATZ\r\n"));
	_serialBuffer = _readSerial().c_str();
	_serial->print(F("AT+CSCS=\"HEX\"\r\n"));
	_serialBuffer = _readSerial().c_str();
	_serial->print(F("AT+CSMP=17,168,0,8\r\n"));
	_serialBuffer = _readSerial().c_str();
	_serial->print(F("AT+CMGF=1\r\n"));
	_serialBuffer = _readSerial().c_str();

	_serial->print(F("AT+CMGS=\""));
	_serial->print(number);
	_serial->print(F("\"\r\n"));
	_serialBuffer = _readSerial().c_str();
	_serial->print(text);
	_serial->print(F("\r"));
	_serialBuffer = _readSerial().c_str();
	_serial->write(0x1A);

	_clearSerial();
	uint32_t tempTime = millis();
	while (millis() - tempTime <= 20000)
	{
		if (available())
		{
			_serialBuffer = _readSerial().c_str();
			if (((_serialBuffer.indexOf("+CMGS")) != -1))
			{
				return true;
			}
		}
	}

	return false;
}

int8_t SIM800L::signalStrength()
{
	cSFA(_serialBuffer, _charBuffer);
	_serialBuffer.clear();

	if (checkNetwork())
	{
		_clearSerial();
		_serial->print(F("AT+CSQ\r\n"));
		_serialBuffer = _readSerial().c_str();
		if ((_serialBuffer.indexOf("+CSQ: ")) != -1)
		{
			createSafeString(result, 128, "");
			int returnInt;
			_serialBuffer.substring(result, _serialBuffer.indexOf("+CSQ: ") + 6, _serialBuffer.indexOf(","));
			result.toInt(returnInt);
			return returnInt;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}
}

bool SIM800L::checkNetwork()
{
	cSFA(_serialBuffer, _charBuffer);
	_serialBuffer.clear();

	_clearSerial();
	_serial->print(F("AT+CREG?\r\n"));
	_serialBuffer = _readSerial().c_str();
	if ((_serialBuffer.indexOf("+CREG: ")) != -1)
	{
		createSafeString(result, 128, "");
		int returnInt;
		_serialBuffer.substring(result, _serialBuffer.indexOf("+CREG: ") + 9, _serialBuffer.indexOf("+CREG: ") + 11);
		result.toInt(returnInt);
		return (bool)returnInt;
	}
	else
	{
		return 0;
	}
}

void SIM800L::serviceProvider(SafeString &returnSFstr)
{
	cSFA(_serialBuffer, _charBuffer);
	_serialBuffer.clear();

	if (checkNetwork())
	{
		_clearSerial();
		_serial->print(F("AT+CSPN?\r\n"));
		_serialBuffer = _readSerial().c_str();
		if ((_serialBuffer.indexOf("+CSPN: ")) != -1)
		{
			uint8_t index1 = _serialBuffer.indexOf("\"");
			uint8_t index2 = _serialBuffer.indexOf("\"", index1 + 1);
			_serialBuffer.substring(returnSFstr, index1 + 1, index2);
		}
		else
		{
			returnSFstr = "No network";
		}
	}
	else
	{
		returnSFstr = "No network";
	}

	returnSFstr = "No network";
}

bool SIM800L::GSMTime(uint8_t *_time)
{
	cSFA(_serialBuffer, _charBuffer);
	_serialBuffer.clear();

	_clearSerial();
	_serial->print(F("AT+CCLK?\r\n"));
	_serialBuffer = _readSerial().c_str();
	if ((_serialBuffer.indexOf("+CCLK: ")) != -1)
	{
		createSafeString(result, 128, "");
		createSafeString(temp, 128, "");
		int tempInt = 0;
		// 3 temporary variables are better than heap frag

		_serialBuffer.substring(result, _serialBuffer.indexOf("\"") + 1, _serialBuffer.lastIndexOf("\"") - 1);

		result.substring(temp, 0, result.indexOf("/"));
		temp.toInt(tempInt);
		_time[0] = tempInt;
		result.substring(temp, result.indexOf("/") + 1, result.lastIndexOf("/"));
		temp.toInt(tempInt);
		_time[1] = tempInt;
		result.substring(temp, result.lastIndexOf("/") + 1, result.indexOf(","));
		temp.toInt(tempInt);
		_time[2] = tempInt;
		result.substring(temp, result.indexOf(",") + 1, result.indexOf(":"));
		temp.toInt(tempInt);
		_time[3] = tempInt;
		result.substring(temp, result.indexOf(":") + 1, result.lastIndexOf(":"));
		temp.toInt(tempInt);
		_time[4] = tempInt;
		result.substring(temp, result.lastIndexOf(":") + 1, result.indexOf("+"));
		temp.toInt(tempInt);
		_time[5] = tempInt;
		return 1;
	}
	else
	{
		memset(_time, 0xFF, sizeof(_time) * sizeof(_time[0]));
		// memset(_time, 0xFF, sizeof(_time));
		return 0;
	}
}

bool SIM800L::enAutoTimeZone()
{
	cSFA(_serialBuffer, _charBuffer);
	_serialBuffer.clear();

	_clearSerial();
	_serial->print(F("AT+CFUN=1\r\n"));
	_Delay(2000);
	_clearSerial();
	_serial->print(F("AT+COPS=2\r\n")); // DE REGISTER
	_Delay(2000);
	_clearSerial();
	_serial->print(F("AT+CLTS=1\r\n")); // AUTOMATIC TIME ZONE UPDATE ENABLE
	_Delay(2000);
	_clearSerial();
	_serial->print(F("AT+COPS=0\r\n")); // REGISTER NETWORK
	_Delay(6000);
	_serialBuffer = _readSerial().c_str();
	if ((_serialBuffer.indexOf("ERR")) != -1) // CHECK IF ERROR
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool SIM800L::softReset()
{
	_clearSerial();
	_serial->print(F("AT+CFUN=1,1\r\n"));
	return 1;
}

bool SIM800L::hardReset()
{
	if (rstDeclair)
	{
		digitalWrite(rstpin, HIGH);
		_Delay(500);
		digitalWrite(rstpin, LOW);

		return 1;
	}
	else
	{
		return 0;
	}
}
