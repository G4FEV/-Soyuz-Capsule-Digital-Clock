<meta name='keywords' content='G4FEV, SDRUno, SDRplay'>

# SOYUZ CAPSULE DIGITAL CLOCK

## David Whitty G4FEV

<i>Last edit 18th Jan 2021.</i>

This software is given freely for non-commercial use with no guarantee given or implied.


<a href="https://github.com/G4FEV/Soyuz-Capsule-Digital-Clock/archive/master.zip"><font size="2"> DOWNLOAD SOYUZ CLOCK PROJECT as zip</font></a>

## What is this?

An Arduino is used to decode NMEA data from a GPS Module and display the time and date.
The display uses a 6 digit 7 segment display forTtime and a 4 digit display for Date and Stopwatch.
The clock is in the form of a Soyuz 744H digital clock installed in the Soyuz capsule.
An internet search for "Soyuz 744H digital clock" will produce a host of interesting information
and videos on these rare and interesting icons from the early days of Space exploration.

<P ALIGN="CENTER"><img src="Images/fronta1.jpg" width=650>
 
 
 ## Hardware Requirements.
Arduino nano or Arduino of your choice. A suitable GPS module and GPS antenna.
Two MAX7219 Display Drivers (MAX 7219 can only drive 8 displays, we require 10 for this project).
Ten 0.36" Red 7 Segment Displays. Various things from the junk box. i.e 7805 DC regulator, Vero/Matrix board, 3 Toggle Switches, 2 Push-Botton Switches.

For the case I used two Eddystone (Hammond) Die cast Boxs type 396 (145x95x49) which are fairly close but not exactly 1:1 scale with the original.
The boxs are bolted back-to-back to achieve the correct depth and the join filled with car body filler (Bondo).

An optional 52x38x31mm box fixed to the rear cover to replicate the "X1" connector housing. (Which also provides an ideal location for a GPS flat ceramic antenna)

<P ALIGN="CENTER"<img src="Images/back.jpg" width=400>
<P ALIGN="LEFT">
An optional relay may be added on pin D3 which is used to simply add a ticking sound when D3 is pulsed (The clacker)
The maximum current an Arduino pin can switch is 40mA so we use a transistor to switch the relay.
 <br>
<img src="Images/relaycctx.jpg" width=300>
<img src="Images/schemclocksm.jpg" width=300>
<img src="Images/MAX7219.jpg" width=300> 
<P ALIGN="LEFT">
 <br>


## SOFTWARE.
<U><B>Does it compile?</B></U>

You will require SoftWareSerial.h Library (normally provided within the Arduino IDE).
<i>We use the SoftwareSerial library for the coms from the GPS. 
Pin 8 is Rxd and 9 Txd which leaves the normal Arduino coms port for debugging information via the IDE Serial Monitor.
As we don't send anything to the GPS pin 9 is left unconnected.</i>

TinyGPS++.h Library

LedControl.h Library Normally provided with Arduino IDE.

Built & tested using Arduino IDE Version 1.8.13

<B>Beware!</B> There are several versions of  SoftwareSerial andTinyGPS libraries which may not be compatible without minor changes to the declarations.



<U><B>Software Settings.</B></U>
Various settings may require editing to suit your requirements;-

GPS Baud Rate. Default baud rate for GPS is set at 9600.

GMT offset time Default is 1 Hour.XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

7 Segment Display Brightness.  Set by the following lines in Setup Void.

       lc.setIntensity(0,2);// 0=Top display. Second digit=brightnes 0 to 15
       
       lc.setIntensity(1,2);// 1=Lower display. Second digit=brightnes 0 to 15

Clack Void (See the clack void at the end of the sketch) set delay to suit your chosen relay.

 
 ## Notes.

<P ALIGN="CENTER"><img src="Images/translation.jpg" width=600>



* Get the MAX7219 and its display woring first using the Hello Worls example sketch.




<P ALIGN="LEFT"><img src="Images/guards.jpg" width=600>
     







<P ALIGN="CENTER"><img src="Images/swguard4a.jpg" width=600>




* <b>Multi VRX setups.</b> 




That's all folks. 73 de Dave G4FEV.

<a href="https://github.com/G4FEV/Soyuz-Capsule-Digital-Clock/archive/master.zip"><font size="2"> DOWNLOAD SOYUZ CLOCK PROJECT as zip</font></a>
