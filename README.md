Some MSP430 launchpads such as the [MSP430FR4133 LaunchPad](http://www.ti.com/tool/msp-exp430fr4133)
include the so-called "EnergyTrace™ technology". It consists of a 
software-controlled DC-DC converter that can measure the energy it is 
delivering. Looks like TI has patents on that:
[US20130154594](http://www.google.com/patents/US20130154594),
[US20140253096](http://www.google.com/patents/US20140253096)

The "official" way to make use of this feature is TI's "Code 
Composer Studio" IDE. Since I don't use CCS, installing CCS just for 
measuring current consumption seems a bit silly.
Fortunately there's a better way: TI provides an open-source library 
for communicating with some of their MSP430 programmers. As well as 
debugging control and programming the library also gives access to the 
EnergyTrace feature. So I wrote a small program based on an example 
that reads the EnergyTrace measurements.

#Output Format
Data is written to stdout in 4 columns:

 1. Time in seconds
 2. Current in amps
 3. Voltage in volts
 4. Energy in Joules

Debug information gets prefixed with a `#`, so it gets ignored by 
gnuplot and the like. For some reason, differentiating and low-pass 
filtering the energy measurements leads to more accurate readings than 
the current measurement itself.

#Dependencies
You'll need MSP430 debug stack (libmsp430.so) and the usual 
things like make and gcc. Unfortunately, building the MSP430 debug 
stack is a bit difficult at this time since it's missing some 
`#include` and triggers a
[compiler bug](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=71092).
Using Arch Linux? You're lucky, I've created a PKGBUILD and patches for 
easy installation: [aur-mspds](https://github.com/carrotIndustries/aur-mspds)
At the time of writing, the AUR package mspds is broken.

#How do I build and run?
```
$ make
$ ./et <measurement duration in seconds> > et.log
```
Use you favourite tool for visualizing and processing the recorded data.

#Conclusion
EnergyTrace comes in really handy for measuring the power consumption of 
MCUs as it saves you from fiddling with current shunts, differential 
amplifiers and oscilloscopes.
Event when you're just using EnergyTrace, the MSP430FR4133 LaunchPad is 
pretty good value at $14.

This little program has proven to be useful during the development of 
[pluto](https://github.com/carrotIndustries/pluto)
