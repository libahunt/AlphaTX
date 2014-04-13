/*******************************************************************************
AlphaTX.h - An Arduino library for RFSolutions FM transmitter module.
		Datasheet: http://www.rfsolutions.co.uk/acatalog/DS-ALPHATX-3.pdf
		Si4021 (used inside AlphaTX module) - is more helpful:
		http://www.silabs.com/Support%20Documents/TechnicalDocs/Si4021.pdf

By Anna Jõgi a.k.a Libahunt Sept 25 2013. Version 0.1.

*******************************************************************************/

#ifndef AlphaTX_h
#define AlphaTX_h

#include "Arduino.h"

class AlphaTX {
 
	public:
		AlphaTX(byte txnsel, byte txsdo, byte txsck);
		void initTXDefault();
		void sendDataPacket(byte label, byte value);
		void sendCmd(byte command1, byte command2);
	 
	private:
		byte _txnsel;
		byte _txsdo;
		byte _txsck;
		byte _txnirq; //has to be PB0, pin 8 or on Mega board pin 53
		byte _txfsk;	//has to be PD7, pin 7 or on Mega board pin 38
		
		void sendFsk(byte data);
		void cmdOut(boolean sendbit);
		void waitIrqHigh();
		void waitIrqLow();
 
};

 
#endif
