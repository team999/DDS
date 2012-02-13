// ----------------------------------------------------------------------------
// Team 9.99 DDS implementation for use with DominoEX
// DDS.h  --  Direct Digital Synthesis
//
// Copyright (C) 2011-2012
//		Andre Geldenhuis (andre.geldenhuis@gmail.com)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser Public License as published by
// the Free Software Foundation, either version 2.1 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser Public License
// along with This program.  If not, see <http://www.gnu.org/licenses/>.
// 
// DDS components based on:
// DDS Sine Generator mit ATMEGS 168
// Timer2 generates the  31250 KHz Clock Interrupt
//
// KHM 2009 /  Martin Nawrath
// Kunsthochschule fuer Medien Koeln
// Academy of Media Arts Cologne

#ifndef DDS_h
#define DDS_h

#include "WProgram.h"
#include <avr/pgmspace.h>

class DDS
{
	public:
		DDS(double startfreq);
		void SetDDSTimers(byte enableCom);
		void SetFreq(double inputfreq);
		volatile unsigned long c4ms; 
};
#endif
