/*  Copyright to AyushMarsian(Ankit Ghevariya)
 *  Licence GNU GPL V3
 *  
 *  Example for read SMS from perticular location of memory
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

String message="";

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
  
  message=gsm.readSMS(1); // Read a SMS from location 1 and store content into message variable.

  if(message.length()==0)
  {
  	Serial.println("Message is Empty");
  }
  else
  {
  	Serial.print("Message :");
  	Serial.println(message);
  }
}

void loop()
{
  
}
