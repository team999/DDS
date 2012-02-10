// ----------------------------------------------------------------------------
// Team 9.99 DDS implementation for use with DominoEX
// DDS.cpp  --   Direct Digital Synthesis
//
// Copyright (C) 2011-2012
//		Andre Geldenhuis (andre.geldenhuis@gmail.com)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with This program.  If not, see <http://www.gnu.org/licenses/>.
// 
// DDS components based on:
// DDS Sine Generator mit ATMEGS 168
// Timer2 generates the  31250 KHz Clock Interrupt
//
// KHM 2009 /  Martin Nawrath
// Kunsthochschule fuer Medien Koeln
// Academy of Media Arts Cologne

#include "WProgram.h"
#include "DDS.h"

// table of 256 sine values / one sine period / stored in flash memory
PROGMEM  prog_uchar sine256[]  = {
  127,130,133,136,139,143,146,149,152,155,158,161,164,167,170,173,176,178,181,184,187,190,192,195,198,200,203,205,208,210,212,215,217,219,221,223,225,227,229,231,233,234,236,238,239,240,
  242,243,244,245,247,248,249,249,250,251,252,252,253,253,253,254,254,254,254,254,254,254,253,253,253,252,252,251,250,249,249,248,247,245,244,243,242,240,239,238,236,234,233,231,229,227,225,223,
  221,219,217,215,212,210,208,205,203,200,198,195,192,190,187,184,181,178,176,173,170,167,164,161,158,155,152,149,146,143,139,136,133,130,127,124,121,118,115,111,108,105,102,99,96,93,90,87,84,81,78,
  76,73,70,67,64,62,59,56,54,51,49,46,44,42,39,37,35,33,31,29,27,25,23,21,20,18,16,15,14,12,11,10,9,7,6,5,5,4,3,2,2,1,1,1,0,0,0,0,0,0,0,1,1,1,2,2,3,4,5,5,6,7,9,10,11,12,14,15,16,18,20,21,23,25,27,29,31,
  33,35,37,39,42,44,46,49,51,54,56,59,62,64,67,70,73,76,78,81,84,87,90,93,96,99,102,105,108,111,115,118,121,124

};

//start and stop timers for DDS alorithm
#define disableT(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define enableT(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

//double dfreq;
// const double refclk=31372.549;  // =16MHz / 510
const double refclk=31376.6;      // measured

// variables used inside interrupt service declared as volatile
volatile byte icnt;              // var inside interrupt
volatile byte icnt1;             // var inside interrupt
volatile unsigned long c4ms;              // counter incremented all 4ms
volatile unsigned long phaccu;   // phase accumulator
volatile unsigned long tword_m;  // dds tuning word m

DDS::DDS(double startfreq=1000)
{
	
  // Timer2 Clock Prescaler to : 1
  enableT (TCCR2B, CS20);
  disableT (TCCR2B, CS21);
  disableT (TCCR2B, CS22);

  // Timer2 PWM Mode set to Phase Correct PWM
  disableT (TCCR2A, COM2A0);  // clear Compare Match
  enableT (TCCR2A, COM2A1);

  enableT (TCCR2A, WGM20);  // Mode 1  / Phase Correct PWM
  disableT (TCCR2A, WGM21);
  disableT (TCCR2B, WGM22);
  
  //set timers and interupt 
  SetDDSTimers(1);
  SetFreq(startfreq);
  
  //activate outputs on D11
  pinMode(11, OUTPUT); 
  
	
}

void DDS::SetFreq(double inputfreq)
{
	tword_m=pow(2,32)*inputfreq/refclk;  // calulate DDS new tuning word 
	
}


void DDS::SetDDSTimers(byte enableCom){
  if(enableCom==1){
    disableT (TIMSK0,TOIE0);   // disable Timer0 !!! delay() is now not available
    enableT (TIMSK2,TOIE2);   // enable Timer2 Interrupt 
  }
  else{
    disableT(TIMSK2,TOIE2); //disable Timer2 Interupt, stops DDS.
    enableT (TIMSK0,TOIE0); //renable Timer0, delay now worls again
  }
}


//******************************************************************
// Timer2 Interrupt Service at 31372,550 KHz = 32uSec
// this is the timebase REFCLOCK for the DDS generator
// FOUT = (M (REFCLK)) / (2 exp 32)
// runtime : 8 microseconds ( inclusive push and pop)
ISR(TIMER2_OVF_vect) {

  //sbi(PORTD,7);          // Test / set PORTD,7 high to observe timing with a oscope

  phaccu=phaccu+tword_m; // soft DDS, phase accu with 32 bits
  icnt=phaccu >> 24;     // use upper 8 bits for phase accu as frequency information
                         // read value fron ROM sine table and send to PWM DAC
  OCR2A=pgm_read_byte_near(sine256 + icnt);    

  if(icnt1++ >= 125) {  // increment variable c4ms all 4 milliseconds
    c4ms+=4;
    icnt1=0;
   }  
 
 //cbi(PORTD,7);            // reset PORTD,7
}
