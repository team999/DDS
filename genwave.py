import numpy as np
import matplotlib.pyplot as plt

freespace=24000*4 #the amount of flash sapce available for the lookuptable
elementsize=8 #size of each element in the wave table in bits, probabably 8, 16 or 32 bits
phaccu_len=32



fileout="wavetable.h"
fileoutwidth=70  #how far to write the output table before line breaks
for i in range(8,32):
	requiredspace=(2**i)*elementsize
	if (requiredspace>freespace):
		tablelength=i-1
		requiredspace=(2**tablelength)*elementsize
		break
truncval=phaccu_len-tablelength
print "wave table length is {} bits.  Phase Accumulator will be truncated by {}\nIt will consume {} bytes".format(tablelength,truncval,requiredspace)
#calculate array size based on available space.  do this in a cheap and cheerful way


arrayS=2**tablelength
AoutRes=2**10 #pwm output res
halfAout=int(AoutRes/2.)
x=np.linspace(-np.pi,np.pi,arrayS)
sinwave=halfAout*(np.cos(x+np.pi/2.))
sinwave=sinwave+halfAout
sinwaveint=[]
for i in range(0,len(sinwave)):
	sinwaveint.append(int(sinwave[i]))


plt.plot(sinwaveint)
plt.show()



testwave=[
  127,130,133,136,139,143,146,149,152,155,158,161,164,167,170,173,176,178,181,184,187,190,192,195,198,200,203,205,208,210,212,215,217,219,221,223,225,227,229,231,233,234,236,238,239,240,
  242,243,244,245,247,248,249,249,250,251,252,252,253,253,253,254,254,254,254,254,254,254,253,253,253,252,252,251,250,249,249,248,247,245,244,243,242,240,239,238,236,234,233,231,229,227,225,223,
  221,219,217,215,212,210,208,205,203,200,198,195,192,190,187,184,181,178,176,173,170,167,164,161,158,155,152,149,146,143,139,136,133,130,127,124,121,118,115,111,108,105,102,99,96,93,90,87,84,81,78,
  76,73,70,67,64,62,59,56,54,51,49,46,44,42,39,37,35,33,31,29,27,25,23,21,20,18,16,15,14,12,11,10,9,7,6,5,5,4,3,2,2,1,1,1,0,0,0,0,0,0,0,1,1,1,2,2,3,4,5,5,6,7,9,10,11,12,14,15,16,18,20,21,23,25,27,29,31,
  33,35,37,39,42,44,46,49,51,54,56,59,62,64,67,70,73,76,78,81,84,87,90,93,96,99,102,105,108,111,115,118,121,124
]

#~ 
#~ stringwave=str(sinwaveint)
#~ stringwave=stringwave.replace(" ","")

f=open(fileout,'w')
f.write("#ifndef wavetable_h\n#define wavetable_h\n")
f.write("// table of {} sine values of {} bit resolution\n\n".format(tablelength,elementsize))
f.write("const char truncval = {};\n".format(truncval))
f.write("PROGMEM  prog_uint16_t sine256[]  = {\n")


widthcounter=0
for i in range(0,len(sinwaveint)-1):  #every element but the last
	thisstr=str(sinwaveint[i]) + ","
	
	widthcounter=widthcounter+len(thisstr)
	if(widthcounter>fileoutwidth):
		f.write("\n")
		widthcounter=0
	f.write(thisstr)
f.write(str(sinwaveint[-1])) #write the last element, no comma
f.write("};\n\n")
f.write("#endif")
f.close()

