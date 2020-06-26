/*  Copyright to AyushMarsian(Ankit Ghevariya)
 *  Licence GNU GPL V3
 *  
 *  Example for network service provider
 *  Written by Ankit Ghevariya, public domain
 *  
 *  You have to connect pin as written below
 *  (GSM) --------- (Arduino)
 *   Vcc  --------> 3.8v to 4.2v (Do not give 5v(Damage) or 3.3v(Under voltage))
 *   RST  --------> Digital 9 (Optional, only if you want to perform hardware reset)
 *   TxD  --------> Digital 10
 *   RxD  --------> Digital 11
 *   Gnd  --------> Gnd
 */
 
#include <SIM800L.h>
#include <SoftwareSerial.h>

SoftwareSerial GSMserial(10, 11); // RX, TX
SIM800L gsm;

String spName="";
uint8_t sStrength=0;

void setup() 
{
  Serial.begin(9600);
  GSMserial.begin(9600);
  
  if(gsm.begin(GSMserial)) // Will return 1 if GSM responds and SIM card inserted then
  {
    Serial.println("GSM Initialized");
  }
  else
  {
    Serial.println("GSM not responding");
    while(1); // Infinite loop, code will not execute other stuff
  }
  
  if(gsm.checkNetwork()) // Check network before reading service provider name
  {
  	sStrength = gsm.signalStrength(); // read signal strength
  	spName = gsm.serviceProvider(); // read service provider name

  	Serial.print("Service Provider:");
  	Serial.println(spName);
  	Serial.print("Network signalStrength:");
  	Serial.println(sStrength);
  }
  else
  {
  	Serial.println("No network available");
  }
}

void loop()
{
  
}
