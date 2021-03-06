<meta name='keywords' content='G4FEV, Soyuz Capsule Digital Clock 744H,'>

# SOYUZ CAPSULE DIGITAL CLOCK

## David Whitty G4FEV

<i>Last edit 5th Feb 2021.</i>

This software is given freely for non-commercial use with no guarantee given or implied.


<a href="https://github.com/G4FEV/-Soyuz-Capsule-Digital-Clock/archive/main.zip"><font size="2"> DOWNLOAD SOYUZ CLOCK PROJECT as zip</font></a>
         
## What is this?

An Arduino is used to decode NMEA data from a GPS Module and display the Time and Date.
The display uses a 6 digit 7 segment display for Time and a 4 digit display for Date and Stopwatch.
The clock is in the form of a Soyuz 744H digital clock installed in the Soyuz capsule.
An internet search for "Soyuz 744H digital clock" will produce a host of interesting information
and videos on these rare and interesting icons from the early days of Soviet Space exploration.

<P ALIGN="CENTER"><img src="Images/fronta1.jpg" width=650>
 
 
 ## Hardware Requirements.
Arduino nano or Arduino of your choice.
<br>A suitable GPS module and GPS antenna.
Two MAX7219 Display Drivers (MAX7219 can only drive 8 displays, we require 10 for this project).
Ten 0.36" Red 7 Segment Displays. 
Various things from the junk box. i.e 7805 DC regulator, Vero/Matrix board, 3 Toggle Switches, 2 Push-Button Switches.

The case uses two Eddystone (Hammond) Die Cast Box's type 396 (145x95x49) which are fairly close but not exactly 1:1 scale with the original.
The box's are bolted back-to-back to achieve the correct depth. The join is filled with car body filler (Bondo).

An optional 52x38x31mm box fixed to the rear cover to replicate the "X1" connector housing. (Which also provides an ideal location for a GPS flat ceramic antenna).

<P ALIGN="CENTER"<img src="Images/back.jpg" width=400>
<P ALIGN="LEFT">
An optional relay may be added on pin D3 which is used to add a ticking sound when D3 is pulsed (The clacker).
The maximum current an Arduino pin can switch is 40mA so we use a transistor to switch the relay. Alternatively a small Piezo sounder may provide a similar "ticking" effect.
 <br>
 <P ALIGN="CENTER">
 <img src="Images/schemclocksm.jpg" width=280>
<img src="Images/relaycctx.jpg" width=280>
<img src="Images/MAX7219.jpg" width=280> 
<P ALIGN="LEFT">
 * Suggestion: Use the Hello-World example sketch, included with the IDE, to ensure the display is working first.
 <br>


## SOFTWARE.
<B>Does it compile?</B>

You will require SoftwareSerial.h Library (normally provided within the Arduino IDE).
<i>We use the SoftwareSerial library for the coms from the GPS. 
Pin 8 is Rxd which leaves the normal Arduino coms port for debugging information via the IDE Serial Monitor.
As we never send anything to the GPS Tx is set to Null (255) which saves a pin for future use.</i>

TinyGPS++.h Library. 
It MUST be Mikal Harts TinyGPSPlus. (See Notes at foot of page)<br>  http://arduiniana.org/libraries/tinygpsplus

Timelib.h  Paul Stoffregen's Time Library. https://github.com/PaulStoffregen/Time used to set your local time offset from UTC.

LedControl.h Library Normally provided with Arduino IDE.

Built & tested using Arduino IDE Version 1.8.13

<B>Beware!</B> There are several versions of  SoftwareSerial andTinyGPS libraries which may not be compatible without minor changes to the declarations.



<B>Software Settings.</B>
Various settings may require editing to suit your requirements;-

GPS Baud Rate. Default baud rate for GPS is set at 9600.

Set your local time offset from UTC.
GMT/UTC offset for daylight saving time is set by the variable GMTOffest = ? which may be a positive or negative number. (See Notes at foot of page)

7 Segment Display Brightness.  Set by the following lines in Setup Void. (Default is 5)

       lc.setIntensity(0,5);// 0=Top display. Second digit=brightness 0 to 15
       
       lc.setIntensity(1,5);// 1=Lower display. Second digit=brightness 0 to 15

Clack Void (The Clack void at the bottom of the sketch) set delay to suit your chosen relay for the ticking sound.

 
## Build Notes

<P ALIGN="CENTER"><img src="Images/translation.jpg" width=600>
 
<P ALIGN="CENTER">Translation of the front panel markings.
 
  ----------------------------------------------------------------------

<P ALIGN="LEFT"><img src="Images/guards.jpg" width=250>
             <img src="Images/swguard4a.jpg" width=300>
             <img src="Images/pattern1a.jpg" width=290>
 
The switch guards are fabricated from 4mm Aluminium using a jewellers  saw and files. A template cut from thin brass is used as a cutting pattern. (Took me about week with very sore fingers so persevere as these are a distinctive feature and worth having.

 ----------------------------------------------------------------------

<P ALIGN="CENTER"><img src="Images/back.jpg" width=400>
<P ALIGN="CENTER"><img src="Images/primer1.jpg" width=400>
 
The mounting flange was cut from Paxolin board or a similar plastic and bonded to the case with 2 pack epoxy glue.
 
  ----------------------------------------------------------------------
 
 <P ALIGN="CENTER"><img src="Images/Soyuz_Labels.jpg" width=600>
 <br>
<P ALIGN="LEFT">This is a suggested template for the front panel Decals. Print this file onto A4 Waterslide Decal Transfer paper or edit using your favourite jpg editor.
 
## Useful Informatiom.
You will find more pictures of the construction of this replica here: http://g4fev.atspace.com/soyuz_clock.htm <br>
This code was written in the UK before I even thought it may be used elsewhere in the world hence you will see variables named GMT & BST. These are Greenwich Mean Time (UTC) and British Summer Time (Equivalent to your Summer Daylight Saving Time). Both may be set near the top of the sketch
 
Most GPS modules will output a reliable time from first switch on before the GPS achieves Sat Lock. I found TinyGPS library did not do this so converted to TinyGPSPlus library. Additionally I added Battery Backup (Two AAA of AA Cells) to the Neo-6  GPS module to speed up Sat Lock. Suggestion: The GPS is powered permanently regardless of switch position so the GPS is constantly locked.
 

That's all folks. 

