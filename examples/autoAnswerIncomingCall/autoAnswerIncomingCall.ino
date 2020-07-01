/*  Copyright to AyushMarsian(Ankit Ghevariya)
 *  Licence GNU GPL V3
 *  
 *  Example for wait for incoming call and auto answer
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
}

void loop()
{
  if(gsm.callStatus()==4)
  {
    Serial.println("Incoming Call");
    gsm.answerCall();
  }
  else
  {
    Serial.println("Waiting for incoming Call");
  }
  delay(1000); // Wait for one 1 Sec
}
