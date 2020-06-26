#ifndef SIM800L_h
#define SIM800L_h
#define __SIM800L_VERSION__ 1.0
#include "Arduino.h"

#ifndef ESP32
	#include <WiFi.h>
#elif ESP8266
	#include <ESP8266WiFi.h>
#endif


#define TIMEOUT 60 // Wait for 60 Seconds

class SIM800L		
{									
  private:
	int _timeout;
	Stream* _serial;
	String _serialBuffer="";
	uint8_t rstpin=0; 			//GSM HARD RESET PIN
	bool rstDeclair=0; 		// CONFIG BIT
	String _readSerial();
	void _clearSerial();
  public:

  	SIM800L();
    bool begin(Stream &serial);
 	bool begin(Stream &serial,uint8_t pin);
 	// Methods for calling	
 	bool dialNumber(char* number);
 	bool answerCall(); 
 	bool hangoffCall();
 	int8_t callStatus();

 	//Methods for sms 
	bool sendSMS(char* number,char* text);

	
	//Methods for network
	int8_t signalStrength();
	bool checkNetwork();
	String serviceProvider();
	bool GSMTime(uint8_t *_time);
	bool enAutoTimeZone();

	//Methods for power
	bool softReset();
	bool hardReset();
};

#endif 