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

//Note that this is a limited implementation of the DominoEX spec, it 
//currently has no support for the secondary alphabet, as such it is good 
//practise to continually transmit as this will help the receiver
//maintain sync 
 
// DDS components based on:
// DDS Sine Generator mit ATMEGS 168
// Timer2 generates the  31250 KHz Clock Interrupt
//
// KHM 2009 /  Martin Nawrath
// Kunsthochschule fuer Medien Koeln
// Academy of Media Arts Cologne

#include <DDS.h>

DDS myDDS(1500);

void setup()
{
  myDDS.SetFreq(1500); //get the frequncy from the DominoEX object
    
}
void loop()
{
  //do nothing
 
 }
