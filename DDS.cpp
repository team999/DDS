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

//Note to improve performance make sure to picka  frequency which is not a multiple of the clock frequency.

#include "WProgram.h"
#include "DDS.h"
#include "wavetable.h"


//start and stop timers for DDS alorithm
#define disableT(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define enableT(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))



//double dfreq;
// const double refclk=31372.549;  // =16MHz / 510
//scope measurement 31.39717425
const double refclk=31397.2;      // measured

volatile int tempword;
// variables used inside interrupt service declared as volatile
volatile int icnt;              // var inside interrupt
volatile byte icnt1;             // var inside interrupt
extern volatile unsigned long c4ms=0;              // counter incremented all 4ms
volatile unsigned long phaccu;   // phase accumulator
volatile unsigned long tword_m;  // dds tuning word m 


unsigned int ddsrand;
volatile unsigned char randloc;

volatile char ditherbit;

DDS::DDS()
{
	

  
  //set timers and interupt 
  //SetDDSTimers(1);
  //SetFreq(startfreq);
  
  //activate outputs on D11
  pinMode(10, OUTPUT); 
  pinMode(7,OUTPUT);
  ddsrand=random(MAXRANDOM);
  randloc=0;
	
}

void DDS::SetFreq(double inputfreq)
{
	tword_m=pow(2,32)*inputfreq/refclk;  // calulate DDS new tuning word 
	
}

void DDS::ReRand(){
	ddsrand=random(MAXRANDOM);
	
}

void DDS::initTimers(){
   //~ // Timer2 Clock Prescaler to : 1
  //~ enableT (TCCR2B, CS20);
  //~ disableT (TCCR2B, CS21);
  //~ disableT (TCCR2B, CS22);
//~ 
  //~ // Timer2 PWM Mode set to Phase Correct PWM
  //~ disableT (TCCR2A, COM2A0);  // clear Compare Match
  //~ enableT (TCCR2A, COM2A1);
//~ 
  //~ enableT (TCCR2A, WGM20);  // Mode 1  / Phase Correct PWM
  //~ disableT (TCCR2A, WGM21);
  //~ disableT (TCCR2B, WGM22);
  
    //Timer1 Clock Prescaler to : 1
  TCCR1B &= ~(1 << CS12);  //clear cs12
  TCCR1B  &=  ~(1 << CS11);  //clear cs11
  TCCR1B |= (1 << CS10);   //set cs10
  
  //set compare match to non-inverted HIGH at bottom Low on MATCH
  disableT(TCCR1A, COM1B0);
  enableT(TCCR1A,COM1B1);
  
	//Timer1 PWM mode Phase Correct PWM 10bit
	TCCR1B &= ~(1 << WGM13);    // Timer B clear bit 3
	TCCR1B &= ~ (1 << WGM12);    // set bit 2
	
	TCCR1A &= ~(1 << WGM11);    //  Timer A set bit 1
	TCCR1A |=(1 << WGM10);    //  set bit 0
}

void DDS::SetDDSTimers(byte enableCom){
  if(enableCom==1){
    disableT (TIMSK0,TOIE0);   // disable Timer0 !!! delay() is now not available
    enableT (TIMSK1,TOIE1);   // enable Timer2 Interrupt 
  }
  else{
    disableT(TIMSK1,TOIE1); //disable Timer2 Interupt, stops DDS.
    enableT (TIMSK0,TOIE0); //renable Timer0, delay now worls again
    //test=5;
    //Serial.println(test,DEC);
  }
}


//******************************************************************
// Timer2 Interrupt Service at 31372,550 KHz = 32uSec
// this is the timebase REFCLOCK for the DDS generator
// FOUT = (M (REFCLK)) / (2 exp 32)
// runtime : 8 microseconds ( inclusive push and pop)
ISR(TIMER1_OVF_vect) {

  //enableT(PORTD,7);          // Test / set PORTD,7 high to observe timing with a oscope

  phaccu=phaccu+tword_m; // soft DDS, phase accu with 32 bits
  icnt=phaccu >> truncval;     // use upper 8 bits for phase accu as frequency information
                         // read value fron ROM sine table and send to PWM DAC
                         
  ditherbit=bitRead(ddsrand,randloc++);
  if(randloc>15){
	randloc=0;  
  }
  //OCR2A=(pgm_read_byte_near(sine256 + icnt))^ditherbit;    
  //OCR1B=(pgm_read_word_near(sine256 + icnt));    
  tempword=(pgm_read_word_near(sine256 + icnt));    
  OCR1BH=tempword >> 8;
  OCR1BL=tempword;
  
  //icnt1=icnt1+1;
  
  if(icnt1++ >= 125) {  // increment variable c4ms all 4 milliseconds
    c4ms+=4;
    icnt1=0;
   }  
 
// disableT(PORTD,7);            // reset PORTD,7
}
