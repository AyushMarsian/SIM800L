/*  Copyright to AyushMarsian(Ankit Ghevariya)
 *  Licence GNU GPL V3
 *  
 *  Example for read network time from GSM SIM800L module
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

uint8_t time[6]={0}; //An array for store time
char printBuffer[50]={0};


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
  
  if(gsm.GSMTime(time))
  {
  	Serial.println("Time read succesfully");

  	sprintf(printBuffer,"Time is :%d/%d/%d %d:%d:%d",time[2],time[1],time[0],time[3],time[4],time[5]);
  }
  else
  {
  	Serial.println("SMS sending fail");
  }
}

void loop()
{
  
}
