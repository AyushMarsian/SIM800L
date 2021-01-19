# Library SIM800L GSM Module for Arduino UNO, ESP8266 and ESP32

This Library is made for easy interfacing of GSM SIM800l module with various board in Arduino IDE.

## Download and Install.
[Download](https://github.com/AyushMarsian/SIM800L/archive/master.zip), then rename the uncompressed folder to **SIM800L**. 

Go to arduino IDE Sketch >> Import Library >> add library... Select the folder SIM800L

OR 

Place the SIM800L library folder into your \Documents\Arduino\libraries folder. 
You may need to create the libraries subfolder if its your first library. Restart the IDE.

## Encoragement

Encourage us by donating a penny

[![Donate](https://img.shields.io/badge/Donate-Paypal-blue.svg)](https://www.paypal.com/paypalme/Ayushmarsian)

## Connections & pinouts
Arduino UNO   |   SIM800L   |    Notes  
-------------|-------------|------------
(3.8v)~(4.4v)!| Vcc | Power supply input, Use circuit diagram shown below
10 RX_PIN | TX |  If you are using SoftwareSerial
11 TX_PIN | RX |  If you are using SoftwareSerial
RESET_PIN | RST|  You have to define reset pin and then pass argument during calling of begin, Also make reset circuit as shown in image below
GND | GND |

## Methods and functions

Name   |Return |  Notes
-------|-------|-----------------------------------------------|
begin(SerialPort)        |true or false|Initialize the library, Insert SIM card compulsorily 
begin(SerialPort,Rst_pin)|true or false|Initialize the library, Insert SIM card compulsorily. If you define and pass reset pin of your board you can reset GSM by external reset pin of the SIM800l
dialNumber(number)       |true or false| It returns true if ATD passed successfully
answerCall()             |true or false|
hangoffCall()            |true or false|
callStatus()             |int8_t|Return the call status. -1=Ideal, 0=Call in progress, 2=Dialing, 3=Ringing
sendSMS(number,text)     |true or false|parameters must be Strings
sendSMS(msgIndex)        |String|
signalStrength()         |int8_t|Return signal strength, if -1 no network
checkNetwork()           |true or false|
serviceProvider()        |String|Return the name of service provider
GSMTime(time)            |true or false|Will fill up the array you passed. Where time[0]=year,time[1]=Month,time[2]=date,time[3]=hour,time[4]=minute,time[5]=seconds
enAutoTimeZone()         |true or false|You have to enable Auto time zone before you read the time.
softReset()              |true or false|Reset the module by AT commands
hardReset()              |true or false|Reset the module by external reset pin. You must have to pass reset pin during begin
startGPRS()              |true or false|Start internet of GSM
tcpConnect(hostname,port)|void|This will try to connect with given TCP server parameter
tcpStatus()				 |true or false|Return TCP connection status
tcpAvailable()           |uint16_t|Return available data count in TCP buffer
tcpRead(buffer,length)	 |void|Fill the buffer which you passed upto given length
tcpSend(buffer)			 |void|Send the buffer to the TCP server


You can regulate the voltage, use +5v which can give higher current.

![alt text](https://github.com/AyushMarsian/SIM800L/blob/master/circuits/Power_Circuit.JPG "Power_Circuit")



For the **Reset circuit** you need to use NPN transistor, follow the below circuit.

![alt text](https://github.com/AyushMarsian/SIM800L/blob/master/circuits/Reset_Circuit.JPG "Reset_Circuit")

## Copyright and license

- This library is modified from original ([Sim800l](https://github.com/cristiansteib/Sim800l))
- Copyright (c) 2020 Ankit Ghevariya (https://github.com/ayushmarsian)
- Licensed under GPL-3.0 ([text](LICENSE))
