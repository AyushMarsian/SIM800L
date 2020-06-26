/*  Copyright to AyushMarsian(Ankit Ghevariya)
 *  Licence GNU GPL V3
 *  
 *  Example for dial a phone from GSM SIM800L module
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

char number[]="xxxxxxxxxx"; //Write your mobile number here

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
  
  gsm.dialNumber(number);
  delay(1000); // Wait for one 1 Sec
}

void loop()
{
  switch(gsm.callStatus())
  {
    case 0:
      Serial.println("Call is going on");
      break;
    case 1:
      Serial.println("Call held");
      break;
    case 2:
      Serial.println("Dialing...");
      break;
    case 3:
      Serial.println("Alerting (Ringing)");
      break;
    case 4:
      Serial.println("Incoming Call");
      break;
    case 5:
      Serial.println("Call Waiting");
      break;
    case 6:
      Serial.println("Disconnected");
      break;
    case -1:
      Serial.println("Ideal condition");
      break;  
  }
  delay(1000); // Wait for one 1 Sec
}
