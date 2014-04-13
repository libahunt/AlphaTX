/*******************************************************************************
AlphaTX.cpp - An Arduino library for RFSolutions FM transmitter module.
		Datasheet: http://www.rfsolutions.co.uk/acatalog/DS-ALPHATX-3.pdf
		Si4021 (used inside AlphaTX module) - is more helpful:
		http://www.silabs.com/Support%20Documents/TechnicalDocs/Si4021.pdf

By Anna Jõgi a.k.a Libahunt Sept 25 2013. Version 0.1.

*******************************************************************************/

#include "Arduino.h"
#include "AlphaTX.h"


AlphaTX::AlphaTX(byte txnsel, byte txsdo, byte txsck){

  _txnsel = txnsel;
  _txsdo = txsdo;
	_txsck = txsck;
	//irq has to be PB0, pin 8 or on Mega board pin 53
	//fsk has to be PD7, pin 7 or on Mega board pin 38

  pinMode(_txnsel, OUTPUT);
	pinMode(_txsdo, OUTPUT);
  pinMode(_txsck, OUTPUT);

	DDRB = DDRB & B11111110; //set nirq as input
	DDRD = DDRD | B10000000; //set fsk as output
	
	/*DDRB &= ~(1<<PINB0); //set nirq as input
	DDRD = (1<<PD7); //set fsk as output*/

}
 
/*** TX PUBLIC FUNCTIONS ******************************************************/

void AlphaTX::initTXDefault() {
//initializes AlphaTX module with default settings
 
  sendCmd(B10001000, B10000000);
  sendCmd(B10100110, B00100000);
  sendCmd(B11001000, B00100011);
  sendCmd(B11000010, B00100000);
  sendCmd(B11000000, B00111001);

}

void AlphaTX::sendCmd(byte command1, byte command2) {
//writes a single command to AlphaTX module - see datasheet and example sketch

  byte bitmask;
  digitalWrite(_txnsel, 0);
  delayMicroseconds(2);
  for (bitmask = B10000000; bitmask > 0; bitmask = bitmask/2) {
    if (command1 & bitmask) {
      cmdOut(1);
    }
    else {
      cmdOut(0);
    }
  }
  for (bitmask = B10000000; bitmask > 0; bitmask = bitmask/2) {
    if (command2 & bitmask) {
      cmdOut(1);
    }
    else {
      cmdOut(0);
    }
  }
  digitalWrite(_txnsel, 1);
  delayMicroseconds(2);
}

void AlphaTX::sendDataPacket(byte label, byte value) {
//label can only contain 4 bits e.g have value from 0 to 15, DEC (0 to f, HEX)
//value is a full byte of data to be sent


	//make sure label is not out of range
	if (label > 15) {
		label = 0;
	}

	//calculate chekcsum to be sent with data
	byte valuePart1 = value >> 4;
	byte valuePart2 = value & B00001111;
	byte checksum = valuePart1 ^ valuePart2;
	checksum = checksum ^ label;

	//add checksum into label byte	
	byte checkAndLabel = checksum << 4;
	checkAndLabel = checkAndLabel + label;

	//AlphaTX module settings
  sendCmd(B11000000, B00110111); //power management settings, low battery detection on, amplifier off
  delayMicroseconds(5);
  sendCmd(B11000000, B00111111); //power management settings, low battery detection on, amplifier on
  
  delayMicroseconds(5);
  
  //send preamble 3 bytes to allow receiver clock recovery lock
  sendFsk(B10101010);
	sendFsk(B10101010);
	sendFsk(B10101010);
  
  /*send synchro bytes 2DD4h*/
  sendFsk(B00101101);
  sendFsk(B11010100);

	//send payload bytes
  sendFsk(checkAndLabel);
	sendFsk(value); 
  
  sendFsk(B10101010); //preamble marks end transmission
  
  sendCmd(B11000000, B00110011); //power management settings, low battery detect off, amplifier off
  
}

void AlphaTX::sendFsk(byte data) {

  byte bitmask;
  for (bitmask = B10000000; bitmask > 0; bitmask = bitmask/2) {
    waitIrqHigh();
    waitIrqLow();
    if (data & bitmask) {
      PORTD |= (1<<PIND7);//digitalWrite FSK pin high very fast
    }
    else {
      PORTD &= ~(1<<PIND7);//digitalWrite FSK pin low very fast
    }
  }

}


/*** TX PRIVATE FUNCTIONS *****************************************************/

void AlphaTX::cmdOut(boolean sendbit) {
  digitalWrite(_txsdo, sendbit);
  digitalWrite(_txsck, 1);
  delayMicroseconds(2);
  digitalWrite(_txsck, 0);
  delayMicroseconds(2);
}

void AlphaTX::waitIrqHigh() {
  boolean val;
  do {
    val = PINB & B00000001;//digitalRead nIRQ pin very fast
  } while ( val != 1 );
}

void AlphaTX::waitIrqLow() {
  boolean val;
  do {
    val = PINB & B00000001;//digitalRead nIRQ pin very fast
  } while ( val != 0 );
}



