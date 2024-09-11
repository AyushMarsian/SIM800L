#ifndef SIM800L_h
#define SIM800L_h
#define __SIM800L_VERSION__ 1.0
#include "Arduino.h"
#include "SafeString.h"
#include "SafeStringReader.h"

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
	Stream *_serial;
	char _charBuffer[1024] = {0};
	uint8_t rstpin = 0;	 // GSM HARD RESET PIN
	bool rstDeclair = 0; // CONFIG BIT
	// void (*tcp_callback)(const char* _data, const uint16_t len) = NULL;
	// char* apn,user,password;

	String _readSerial();
	void _clearSerial();
	void _Delay(unsigned long ms);

public:
	SIM800L();
	bool begin(Stream &serial);
	bool begin(Stream &serial, uint8_t pin);
	// Methods for calling
	bool dialNumber(const char *phoneNumber);
	bool answerCall();
	bool hangoffCall();
	bool forwardCall(const char *phoneNumber);
	bool stopForwading();
	bool startMPTY(const char *originNumber, const char *destinationNumber, unsigned long callHoldTimeout);
	int8_t callStatus();
	int8_t callStatus(const char *phoneNumber);
	void incomingCall(SafeString &returnSFstr);
	bool available();

	// Methods for sms
	bool sendSMS(const char *number, const char *text);
	void readSMS(uint8_t msgIndex, SafeString &returnSFstr);
	bool sendHEXsms(const char *number, const char *text);

	// Methods for network
	int8_t signalStrength();
	bool checkNetwork();
	void serviceProvider(SafeString &returnSFstr);
	bool GSMTime(uint8_t *_time);
	bool enAutoTimeZone();

	// Methods for power
	bool softReset();
	bool hardReset();

	// Methonds for TCP
	// void loop();
	// void tcpCallBack(void (*callback)(const char* _data, const uint16_t len));
	bool startGPRS();
	void tcpConnect(char *host, uint16_t port);
	bool tcpStatus();
	int16_t tcpAvailable();
	void tcpRead(char *buffer, uint16_t length);
	void tcpSend(char *buffer);
};

#endif