/******************************************************************************
Example sketch for AlphaTX library.

Use AlphaRX module with it's library's example sketch to receive
and show received data in serial monitor.

AlphaRX library uses FIFO mode that can only receive two bytes of data at a time.
To also include checksum in the data packet, the first byte can only have 
four bits of information in it - that is decimal value 0-15 or hexadecimal 0-f.
Lets call this part of data a "label". The other part is a full byte 
and called "value" in this sketch.

Known issue with AlphaRX sketch - sometimes datapackets containing 0 0 value
are erroneously read out. When using the library do not send 0 0. And discard
the packet if this value is received.
*******************************************************************************/


#include <AlphaTX.h>


/*Because digitalRead and digitaWrite could not be used 
and faster direct pin manipulation code was needed instead
FSK and nIRQ are tied to specific pins 7 and 8 on
small Arduinos (AtMega168/328), and 38 and 53 on Mega boards.

// Arduino       Arduino             Alpha module
   pin number    Mega pin nr         pin number and name
   
   7             38                  13 FSK
   8             53                  14 nIRQ

*/


// Define the rest of the pins
// Arduino pin number                Alpha module pin number and name

byte txsdo = 4;  //                  1 SDI    
byte txsck = 5;  //                  2 SCK  
byte txnsel = 6; //                  3 NSEL 





const int led = 13; //just and indicator led


//some random test data
byte testLabels[5] = {0x01, 0x02, 0x03, 0x04, 0x05};//labels can only have value that uses LSB 4 bits e.g form 0 to 15 DEC or 0 to F HEX
byte testValues[5] = {0x11, 0x22, 0x33, 0x44, 0x55};


//Create an instance of the AlphaTX class
AlphaTX alphaTX = AlphaTX(txnsel, txsdo, txsck);



void setup() {
  
  pinMode(led, OUTPUT);
  //pinModes for AlphaTX module were automatically declared when creating an object
  
}



void loop() {
  
  
/*** Initializing AlphaTX module ****************************************************************
Modify following commands or add needed ones according to Alpha module datasheet instructions 
***                                        Command name in datasheet                        ***/
  alphaTX.sendCmd(B10001100, B10000000); //Configuration setting
  alphaTX.sendCmd(B10100110, B00100000); //Frequency setting
  alphaTX.sendCmd(B11001000, B00100011); //Data rate
  alphaTX.sendCmd(B11000010, B00100000); //Low battery detction and Tx bit synchronization 
  alphaTX.sendCmd(B11000000, B00111001); //Power management
/*end of initializing commands*/

/*Alternatively if you don't care about the settings 
you can use a default TX module initialization instead of
previous set of commands, like so:*/
  //alphaTX.initTXDefault();

  int i;
  for (i=0; i< sizeof(testValues)/sizeof(byte); i++) {
    
    digitalWrite(led, HIGH);//Led blinks for indication when data is being sent.
    
    alphaTX.sendDataPacket(testLabels[i], testValues[i]);
    
    delay(300);
    digitalWrite(led, LOW);
    
    delay(3000);
  }
  
}


