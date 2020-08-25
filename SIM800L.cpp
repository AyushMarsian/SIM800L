#include "SIM800L.h"

////////////////////////////////////////////////////PRIVATE DEFINITION////////////////////////////////////////////////////

String SIM800L::_readSerial()
{
	_timeout=0;
	while(!_serial->available() && _timeout<(TIMEOUT*100)) 
	{
		delay(10);
		yield();
		_timeout++;
	}
	if(_serial->available())
	{
		return _serial->readString();
	}

	return "";
}

void SIM800L::_clearSerial()
{
	if(_serial->available())
	{
		_serial->readString();
	}
}

/////////////////////////////////////////////////INSTANT/INIT DEFINITION/////////////////////////////////////////////////

SIM800L::SIM800L(void)
{
  _serialBuffer.reserve(1500); //reserve memory to prevent fragmention
}

bool SIM800L::begin(Stream &serial) // begin Definition with Serial port assignment
{
	_serial = &serial;
	delay(1000);
	yield();
	_clearSerial();

	_serial->print(F("AT\r\n"));
	_serialBuffer=_readSerial();
	if ( (_serialBuffer.indexOf("OK") )!=-1 )
	{
		yield();
		_clearSerial();
		_serial->print(F("ATE0\r\n"));
		_serialBuffer=_readSerial();
		if ((_serialBuffer.indexOf("OK"))!=-1 )
		{	
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
}

bool SIM800L::begin(Stream &serial,uint8_t pin) // begin Definition with Serial port and reset pin assignment
{
	rstpin=pin;
	pinMode(rstpin,OUTPUT);
	digitalWrite(rstpin,LOW);
	rstDeclair=true;
	begin(serial);
}


////////////////////////////////////////////////////PUBLIC DEFINITION////////////////////////////////////////////////////
/*void SIM800L::tcpCallBack(void (*callback)(const char* _data, const uint16_t len))
{
    this->tcp_callback = callback;
}*/

bool SIM800L::startGPRS()
{
	_clearSerial();
	_serial->print(F("AT+CIPSHUT\r\n"));
	delay(200);
	_serial->print(F("AT+CIPMUX=1\r\n"));
	delay(200);
	_serial->print(F("AT+CIPQSEND=1\r\n"));
	delay(200);
	_serial->print(F("AT+CIPRXGET=1\r\n"));
	delay(200);
	_serial->print(F("AT+CSTT=\"\"\r\n"));
	delay(200);
	_clearSerial();
	_serial->println("AT+CIICR");
	_serialBuffer=_readSerial();
	if((_serialBuffer.indexOf("OK"))==-1 )
	{
		return false;  
	}
	_clearSerial();
	_serial->println("AT+CIFSR;E0");
	_serialBuffer=_readSerial();
	if((_serialBuffer.indexOf("ERROR")) != -1 )
	{
		return false;  
	}

	_clearSerial();
	_serial->println("AT+CDNSCFG=\"8.8.8.8\",\"8.8.4.4\"");
	_serialBuffer=_readSerial();
	if((_serialBuffer.indexOf("OK")) == -1 )
	{
		return false;  
	}
	
	return true;
}

void SIM800L::tcpConnect(char* host,uint16_t port)
{
	uint32_t tempTime=0;
	char _tempBuff[50]={0};
	sprintf(_tempBuff,"AT+CIPSTART=0,\"TCP\",\"%s\",\"%d\"\r\n",host,port);
	_serial->print(_tempBuff);
}

bool SIM800L::tcpStatus()
{
	_clearSerial();
	_serial->print(F("AT+CIPSTATUS=0\r\n"));
	_serialBuffer=_readSerial();
	if((_serialBuffer.indexOf("+CIPSTATUS:") )!=-1 )
	{
		if((_serialBuffer.indexOf("CONNECTED") )!=-1 )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

int16_t SIM800L::tcpAvailable()
{
	_clearSerial();
	_serial->print(F("AT+CIPRXGET=4,0\r\n"));
	_serialBuffer=_readSerial();
	if((_serialBuffer.indexOf("+CIPRXGET: 4,0,") )!= -1)
	{
		return _serialBuffer.substring(_serialBuffer.indexOf("+CIPRXGET: 4,0,")+15,_serialBuffer.indexOf("\r",_serialBuffer.indexOf("+CIPRXGET: 4,0,")+15)).toInt();
	}

	return -1;
}

void SIM800L::tcpRead(char* buffer,uint16_t length)
{
	char _tempBuff[30]={0};
	sprintf(_tempBuff,"AT+CIPRXGET=2,0,%d\r\n",length);
	_clearSerial();
	_serial->print(_tempBuff);
	_serialBuffer=_readSerial();
	if((_serialBuffer.indexOf("+CIPRXGET: 2,0,"))!=-1 )
	{
		uint16_t dataindex = _serialBuffer.indexOf('\n',_serialBuffer.indexOf(",",_serialBuffer.indexOf("+CIPRXGET: 2,0,")))+1;

		memset(_tempBuff,'\0',sizeof(_tempBuff));
		for(uint16_t i=0;i<length;i++)
		{
			buffer[i]=_serialBuffer[i+dataindex];
		}
	}
}

void SIM800L::tcpSend(char* buffer)
{
	_clearSerial();
	_serial->print("AT+CIPSEND=0\r\n");
	delay(500);
	_serial->print(buffer);
	_clearSerial();
	_serial->write (0x1A); // command for send data;
}

/*void SIM800L::loop()
{
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
	if(_serial->available())
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

bool SIM800L::incomingCall()
{
	_serialBuffer=_readSerial();
	if ( (_serialBuffer.indexOf("RING") )!=-1 )
	{
		return true;
	}
	else 
	{
		return false;
	}
}

bool SIM800L::dialNumber(char* number)
{
	_clearSerial();
	_serial->print (F("ATD"));
	_serial->print (number);
	_serial->print (F(";\r\n"));
	_serialBuffer=_readSerial();
	if ( (_serialBuffer.indexOf("OK") )!=-1 )
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
	_clearSerial();
	_serial->print(F("ATA\r\n"));
	_serialBuffer=_readSerial();
	if ( (_serialBuffer.indexOf("OK") )!=-1 )
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
	_clearSerial();
	_serial->print(F("ATH\r\n"));
	_serialBuffer=_readSerial();
	if ( (_serialBuffer.indexOf("OK") )!=-1 )
	{
		return true;  
	}
	else 
	{
		return false;
	}
}

int8_t SIM800L::callStatus()
{
	_clearSerial();
	_serial->print (F("AT+CLCC\r\n"));
	_serialBuffer=_readSerial();
	if((_serialBuffer.indexOf("+CLCC: ") )!=-1)
	{
		return _serialBuffer.substring(_serialBuffer.indexOf("+CLCC: ")+11,_serialBuffer.indexOf("+CLCC: ")+13).toInt();
	}  
	else if( (_serialBuffer.indexOf("OK") )!=-1 )
	{
		return -1;  
	}
}

bool SIM800L::sendSMS(char* number,char* text)
{
	uint32_t tempTime=0;
	_serial->print (F("AT+CMGF=1\r")); //set sms to text mode  
	_serialBuffer=_readSerial();
	_serial->print (F("AT+CMGS=\""));  // command to prepare sms
	_serial->print (number);           
	_serial->print (F("\"\r"));       
	_serialBuffer=_readSerial(); 
	_serial->print (text);
	_serial->print ("\r"); 
	
	delay(100);
	_clearSerial();
	_serial->write (0x1A); // command for send sms

	tempTime=millis();
	while(millis()-tempTime <= 20000) // Wait for SMS sent response for 1 minute(Maximum response time of AT+CMGS is 1 min)
	{
		if(available())
		{
			_serialBuffer=_readSerial();
			if (((_serialBuffer.indexOf("+CMGS") ) != -1 ) )
			{
				return true;
			}
		}
	}

	return false;
}

String SIM800L::readSMS(uint8_t msgIndex)
{
	_serial->print (F("AT+CMGF=1\r")); //set sms to text mode  
	_serialBuffer=_readSerial(); 
	if ((_serialBuffer.indexOf("ERR"))!=-1) // CHECK IF ERROR
	{
		_serial->print (F("AT+CMGR="));
		_serial->print (msgIndex);
		_serial->print ("\r");
		_serialBuffer=_readSerial();
		if (_serialBuffer.indexOf("CMGR:")!=-1)
		{
			return _serialBuffer;
		}
		else
		{
			return ""; 
		}   
	}

	return "";
}

int8_t SIM800L::signalStrength()
{
	if(checkNetwork())
	{
		_clearSerial();
		_serial->print (F("AT+CSQ\r\n"));
		_serialBuffer=_readSerial();
		if((_serialBuffer.indexOf("+CSQ: ") )!=-1)
		{
			return _serialBuffer.substring(_serialBuffer.indexOf("+CSQ: ")+6,_serialBuffer.indexOf(",")).toInt();
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
	_clearSerial();
	_serial->print (F("AT+CREG?\r\n"));
	_serialBuffer=_readSerial();
	if((_serialBuffer.indexOf("+CREG: ") )!=-1)
	{
		return _serialBuffer.substring(_serialBuffer.indexOf("+CREG: ")+9,_serialBuffer.indexOf("+CREG: ")+11).toInt();
	}  
	else
	{
		return 0;  
	}
}

String SIM800L::serviceProvider()
{
	if(checkNetwork())
	{
		_clearSerial();
		_serial->print (F("AT+CSPN?\r\n"));
		_serialBuffer=_readSerial();
		if((_serialBuffer.indexOf("+CSPN: ") )!=-1)
		{
			uint8_t index1=_serialBuffer.indexOf("\"");
			uint8_t index2=_serialBuffer.indexOf("\"",index1+1);
			return _serialBuffer.substring(index1+1,index2);
		} 
		else
		{
			return "No network";
		}
	}
	else
	{
		return "No network";
	}	
}

bool SIM800L::GSMTime(uint8_t *_time)
{
	_clearSerial();
	_serial->print (F("AT+CCLK?\r\n"));
	_serialBuffer=_readSerial();
	if((_serialBuffer.indexOf("+CCLK: ") )!=-1)
	{
		_serialBuffer = _serialBuffer.substring(_serialBuffer.indexOf("\"")+1,_serialBuffer.lastIndexOf("\"")-1);  

		_time[0]=_serialBuffer.substring(0,_serialBuffer.indexOf("/")).toInt();
		_time[1]=_serialBuffer.substring(_serialBuffer.indexOf("/")+1,_serialBuffer.lastIndexOf("/")).toInt();
		_time[2]=_serialBuffer.substring(_serialBuffer.lastIndexOf("/")+1,_serialBuffer.indexOf(",")).toInt();
		_time[3]=_serialBuffer.substring(_serialBuffer.indexOf(",")+1,_serialBuffer.indexOf(":")).toInt();
		_time[4]=_serialBuffer.substring(_serialBuffer.indexOf(":")+1,_serialBuffer.lastIndexOf(":")).toInt();
		_time[5]=_serialBuffer.substring(_serialBuffer.lastIndexOf(":")+1,_serialBuffer.indexOf("+")).toInt();
		return 1;
	}  
	else
	{
		memset(_time,0xFF,sizeof(_time));
		return 0;  
	}
}

bool SIM800L::enAutoTimeZone()
{
	_clearSerial();
	_serial->print(F("AT+CFUN=1\r\n"));
	delay(2000);
	_clearSerial();
	_serial->print(F("AT+COPS=2\r\n"));//DE REGISTER
	delay(2000);
	_clearSerial();
	_serial->print(F("AT+CLTS=1\r\n"));//AUTOMATIC TIME ZONE UPDATE ENABLE
	delay(2000);
	_clearSerial();
	_serial->print(F("AT+COPS=0\r\n"));//REGISTER NETWORK
	delay(6000);
	_serialBuffer=_readSerial();
	if ((_serialBuffer.indexOf("ERR"))!=-1) // CHECK IF ERROR
	{
		softReset(); // SOFT-RESET GSM IF ERROR 
		delay(5000);
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
	_serial->print (F("AT+CFUN=1,1\r\n"));
	return 1;
}

bool SIM800L::hardReset()
{
	if(rstDeclair)
	{
		digitalWrite(rstpin,HIGH);
		delay(500);
		digitalWrite(rstpin,LOW);

		return 1;
	}
	else
	{
		return 0;
	}
}