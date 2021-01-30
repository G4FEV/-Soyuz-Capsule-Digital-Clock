/*
                        2020 Winter Lockdown Project
                        David Whitty  G4FEV
                             Version 1.2 
This is my version of a replica Soyuz Capsule 744H Digital Clock.
This is never going to be an EXACT toolroom copy but as close as I can get 
using hand tools in the workshop (Garden Shed).
The project works well having GPS accuracy and helped keep me sane during
the 2020 year of confusion.
This software given freely for personal, non=commercial use.

This code was written in the UK before I even thought it may be
used elsewhere in the world hence you will see variables named
GMT & BST. These are Grenwich Mean Time (UTC) and British Summer
Time (Equivalent to your Summer Daylight Saving Time). 
Your UTC and Daylght Saving Time offsets may be set near the top
of the sketch may Both may be set near the top of the sketch.
  
            github page for this project is here.
            https://github.com/G4FEV/-Soyuz-Capsule-Digital-Clock


/////////////////List of interface connections.//////////////////////
GPS NMEA Data In = D8  (9600 but you can change this below.)          
           
       The pins below all use a 10k pull-down resistor to ground.
                      --------------------------------
Clacker        D3  This is simply an old relay, driven by a suitable transistor
               fixed to the case to make a ticking sound.
Clackpin       D4  A Switch to 5v Turns on the ticking sound
BSTpin         D5  A Switch to 5v sets time + 1 Hour for BST / GMT *
Stopwatch/Date D6  Push button to 5v cycles through options for lower display.
Stopwatch      D7  Push button to 5v start/stop/reset stopwatch             
                      --------------------------------
*
BST = British Summer Time known as Daylight Saving Time in other countries.
GMT =  Grenwich Mean Time otherwise known as UTC.
Your UTC offset may be selected below.
Your Daylight Saving time offset may be selected below (int GMTOffset).                      
*/

#include <TinyGPS++.h>  //Take Care. There are several version of TinyGPS
                       // You MUST have TinyGPSPlus
                   // http://arduiniana.org/libraries/tinygpsplus
// TinyGPS and SoftwareSerial libraries are the work of Mikal Hart
                   
TinyGPSPlus gps;      // The TinyGPS++ object

#include <TimeLib.h> //Paul Stoffregen's Time Library.
                    // https://github.com/PaulStoffregen/Time
                    
#include "LedControl.h"   //include the library                        
// Now we need a LedControl for the 7 sement displays.
LedControl lc=LedControl(12,11,10,2);
   //                     |  |  | | 
   //                     |  |  | |---This final 2 is because we have 2 MAX7219s  *
   //                     |  |  |-----Pin 10 connected to LOAD  (12)
   //                     |  |--------Pin 11 connected to the CLK (13)
   //                     |-----------Pin 12 connected to the DataIn (1)
   //                                                             
   //                                 We have two MAX7219  0 is for the clock *
   //                                 MAX7219  1 is for the lower display.

//Software Serial allows us to use pins 8&9 for the GPS leaving the standard
//Serial port for debug messages to the Arduino serial monitor.
#include <SoftwareSerial.h>
SoftwareSerial mySerial(8,255); 
//We never TX anything to the GPS so set to 255 = Null pin.  

/////////////////////////////////////////////////////////////
// Offset hours from gps time (UTC) / (GMT)
   int offset = 0;   // United Kingdom Time (UK)
// int offset = 1;   // Central European Time (EU)
// int offset = -5;  // Eastern Standard Time (USA)
// int offset = -4;  // Eastern Daylight Time (USA)
// int offset = -8;  // Pacific Standard Time (USA)
// int offset = -7;  // Pacific Daylight Time (USA)
// int offset = 10;  // Melbourne Daylight Time (AUS)
// int offset = 5;  // Greece Daylight Time (GR)

int GMTOffest = 1;  //GMT to British Summer time offset or your Daylight Saving Time offset)
////////////////////////////////////////////////////////////

time_t prevDisplay = 0; //For Time display

unsigned long Year;
unsigned long Month;
unsigned long Day;
unsigned long Hour;  
unsigned long Minute;
unsigned long Second;

const int BSTpin = 5;  // BST GMT Pin D5 (Brit Summer Time)
int BSTState = 0;     //State of BST GMT Switch

int BSTLO=0; //Used as a flip flop when the GMT/BST switch toggles do we
             // only adjust the time once

const int Clackpin = 4; //Switch to turn the clacker on/off D4
int ClackState = 0;    //State
  
    int del = 80;          //Delay used for the Display Boot-up Dance.
        int del2 = 30;    //Delay2 used for the Display Boot-up Dance.
          int del3 = 20; //Delay3 used for the Display Boot-up Dance.

     
 int Clacker = 3;//The clacker is a relay on pin D3 which makes a ticking sound
                // when the clack routine is called.

int Timeclack=0; //Set

int Clacks = 10; //Number of ticks at first bootup
int Cycles= 0;  //The number of cycles. set in setup below

//////////////////FUNCTIONS FOR LOWER LED DISPLAY//////////////////////////////
const int TimeDatepin= 6; //Time date selection button on D6
int TimeDateState= 0;

// variables to hold the new and old switch states of timeDate push button
boolean oldSwitchState = LOW;
boolean newSwitchState = LOW;
byte DisplayState = 1; //Lower display = Date, OFF or Stopwatch

/////////////////////////FUNCTIONS FOR STOPWATCH///////////////////////////////////////
const int StopWatchpin= 7; //Stopwatch selection button on D7
int StopWatchState= 0;
// variables to hold the new and old switch states of Stopwatch push button
boolean oldStopState = LOW;
boolean newStopState = LOW;
byte Display2State = 1; //Lower display = Start, Stop or Reset

//////////////////////////////////////////////////////////////////
 
unsigned long StopTime=0;
int Stop1=0;     //Various Flags for setting & Running the Stopwatch
int StopFlag1=0;
int StopMin=0;

int StartUp = 1; //Used for first pass of the display when
                 //building up the digits individually the first
              
int Flag1=0;  //used for checking updates of each second

int TimeStore=0; //For stopwatch

int FirstPass=0; //Allow a few loops so the time settles before displaying.
 
/////////////// END OF DECLARATIONS ///////////////////////////
void setup() {
   /* and clear the display */
    lc.shutdown(0,false);
    lc.shutdown(1,false);
 /* Set the brightness   max value==15 */
    lc.setIntensity(0,5);// 0=Top display. Second digit=brightnes 0 to 15
     lc.setIntensity(1,5);// 1=Lower display. Second digit=brightnes 0 to 15
    
 lc.clearDisplay(0);
   lc.clearDisplay(1);

 pinMode(Clacker, OUTPUT);
 pinMode(BSTpin, INPUT);

  pinMode(Clackpin, INPUT);

  pinMode (TimeDatepin, INPUT);
  pinMode (StopWatchpin, INPUT); 

Cycles = Clacks; //For the number of ticks at first bootup
    
    Serial.begin(9600);  //For Debug Terminal
       mySerial.begin(9600);  //  For GPS input

///////////// MAKE THE DISPLAY DANCE ON POWER-UP //////////////////
////////////top row///////

lc.setRow(0,0,B01000000);  
lc.setRow(1,0,B01000000);
lc.setRow(0,1,B01000000);    
lc.setRow(1,1,B01000000);
lc.setRow(0,2,B01000000);
lc.setRow(1,2,B01000000);
lc.setRow(0,3,B01000000);   
lc.setRow(1,3,B01000000);
lc.setRow(0,4,B01000000); 
clack(); 
delay(500);
////////////////////
/////////////////////////middle row///////
lc.setRow(0,0,B00000001);  
lc.setRow(1,0,B00000001); 
lc.setRow(0,1,B00000001);    
lc.setRow(1,1,B00000001);
lc.setRow(0,2,B00000001);
lc.setRow(1,2,B00000001);
lc.setRow(0,3,B00000001);   
lc.setRow(1,3,B00000001);
lc.setRow(0,4,B00000001); 
clack(); 
delay(150);
/////////////////////////lower row///////
lc.setRow(0,0,B00001000);  
lc.setRow(1,0,B00001000); 
lc.setRow(0,1,B00001000);    
lc.setRow(1,1,B00001000); 
lc.setRow(0,2,B00001000);
lc.setRow(1,2,B00001000);
lc.setRow(0,3,B00001000);
lc.setRow(1,3,B00001000);
lc.setRow(0,4,B00001000); 
clack(); 
delay(150);
/////////////////////////middle row///////
lc.setRow(0,0,B00000001);  
lc.setRow(1,0,B00000001); 
lc.setRow(0,1,B00000001);    
lc.setRow(1,1,B00000001);
lc.setRow(0,2,B00000001);
lc.setRow(1,2,B00000001);
lc.setRow(0,3,B00000001);   
lc.setRow(1,3,B00000001);
lc.setRow(0,4,B00000001); 
clack(); 
delay(150);
/////////////////////////top row///////
lc.setRow(0,0,B01000000);  
lc.setRow(1,0,B01000000);
lc.setRow(0,1,B01000000);    
lc.setRow(1,1,B01000000);
lc.setRow(0,2,B01000000);
lc.setRow(1,2,B01000000);
lc.setRow(0,3,B01000000);   
lc.setRow(1,3,B01000000);
lc.setRow(0,4,B01000000); 
clack(); 
delay(150);
/////////////////////////middle row///////
lc.setRow(0,0,B00000001);  
lc.setRow(1,0,B00000001); 
lc.setRow(0,1,B00000001);    
lc.setRow(1,1,B00000001);
lc.setRow(0,2,B00000001);
lc.setRow(1,2,B00000001);
lc.setRow(0,3,B00000001);   
lc.setRow(1,3,B00000001);
lc.setRow(0,4,B00000001);
clack();  
delay(150);
/////////////////////////lower row///////
lc.setRow(0,0,B00001000);  
lc.setRow(1,0,B00001000); 
lc.setRow(0,1,B00001000);    
lc.setRow(1,1,B00001000); 
lc.setRow(0,2,B00001000);
lc.setRow(1,2,B00001000);
lc.setRow(0,3,B00001000);
lc.setRow(1,3,B00001000);
lc.setRow(0,4,B00001000); 
clack(); 
delay(150);
////////////////wipe//////////
lc.setRow(0,0,B00000000);  
delay(del);
lc.setRow(0,1,B00000000);    
delay(del);
lc.setRow(0,2,B00000000); 
delay(del);
lc.setRow(0,3,B00000000);   
delay(del);
lc.setRow(0,4,B00000000); 
lc.setRow(1,0,B00000000);  
delay(del);
lc.setRow(1,1,B00000000);    
delay(del);
lc.setRow(1,2,B00000000); 
delay(del);
lc.setRow(1,3,B00000000);   
delay(del);
clack(); 
 delay(150);
////////////////SOYUZ////////////////////

lc.setChar(0,0,'5',false);//S O Y U Z
clack(); 
delay(del);
lc.setRow(0,1,B00011101);    //o
clack(); 
delay(del);
 lc.setRow(0,2,B00111011); //y
 clack(); 
delay(del);
 lc.setRow(0,3,B00011100);   //u
 clack(); 
delay(del);
  lc.setChar(0,4,'2',false); //z
  clack(); 
  delay(1500);
  ///////////////// now wipe it
lc.setRow(0,0,B00000000); 
clack();  
delay(del2);
lc.setRow(0,1,B00000000); 
clack();    
delay(del2);
lc.setRow(0,2,B00000000); 
clack(); 
delay(del2);
lc.setRow(0,3,B00000000); 
clack();   
delay(del2);
lc.setRow(0,4,B00000000); 
clack();   
delay(150);

/////////////////CLOCK//////////////////////////
lc.clearDisplay(0);

lc.setRow(0,0,B00110011);    //y       C L O C K
clack(); 
delay(del);
lc.setRow(0,1,B01111101);    //a
clack(); 
delay(del);
lc.setRow(0,2,B00001101);    //c
clack(); 
delay(del);
lc.setRow(0,3,B00011111);    //b
clack(); 
delay(del);
lc.setRow(0,4,B00000110);    //l
clack(); 
delay(1500);
////////////////wipe//////////
lc.setRow(0,0,B00000000); 
clack();  
delay(del2);
lc.setRow(0,1,B00000000);
clack();     
delay(del2);
lc.setRow(0,2,B00000000); 
clack(); 
delay(del2);
lc.setRow(0,3,B00000000);  
clack();  
delay(del2);
lc.setRow(0,4,B00000000); 
clack(); 
delay(150);
/////////////////START//////////////////////////
 lc.clearDisplay(0);

lc.setRow(0,0,B00110111);    //H   S T A R T
clack(); 
delay(del);
lc.setRow(0,1,B01111101);    //a
clack(); 
delay(del);
lc.setRow(0,2,B00110011);    //Y
clack(); 
delay(del);
lc.setRow(0,3,B01111101);    //a
clack(); 
delay(del);
lc.setRow(0,4,B00010101);    //n
clack(); 
delay(del);
lc.setRow(0,5,B00011101);    //o
clack(); 
delay(1800);
//////////////////////////////88888888//////////////
lc.setChar(0,0,'8',false);//8 
clack(); 
delay(del2);
lc.setChar(0,1,'8',false);//8
clack();  
delay(del2);
lc.setChar(0,2,'8',false);//8 
clack(); 
delay(del2);
lc.setChar(0,3,'8',false);//8
clack();  
delay(del2);
lc.setChar(0,4,'8',false);//8 
clack(); 
delay(del2);
lc.setChar(0,5,'8',false);//8
clack();  
delay(del2);
lc.setRow(0,0,B00000000);  
lc.setChar(1,0,'8',false);//8
clack();  
delay(del3);
lc.setRow(0,1,B00000000);  
lc.setChar(1,1,'8',false);//8
clack();  
delay(del3);
lc.setRow(0,2,B00000000);  
lc.setChar(1,2,'8',false);//8
clack();  
delay(del3);
lc.setRow(0,3,B00000000);  
lc.setChar(1,3,'8',false);//8 
clack(); 
delay(del3);
lc.setRow(0,4,B00000000); 
clack();  
delay(del3);
lc.setRow(0,5,B00000000);
clack();   
delay(del3);
lc.setRow(1,0,B00000000);
clack();   
delay(del3);
lc.setRow(1,1,B00000000);
clack();   
delay(del3);
lc.setRow(1,2,B00000000);
clack(); 
delay(del3);
lc.setRow(1,3,B00000000);
clack();   
 ////////////////////////////////////////////
////////END OF DISPLAY DANCE ///

}

/////////////////////////////START OF LOOP///////////////////////////
void loop() {

 while (mySerial.available() > 0)  //   Grab the NMEA data
    if (gps.encode(mySerial.read()))
  
 Year=gps.date.year(); 
  Month=gps.date.month();
   Day=gps.date.day();        
     Hour=gps.time.hour(); //Copy to variables for the time adjustment
       Minute=gps.time.minute(); // 
        Second=gps.time.second(); 
              
       setTime(Hour, Minute, Second, Day, Month,Year); //Set Time Library
       adjustTime(offset * SECS_PER_HOUR); //update according to offset 

if( StartUp == 1){        
      StartUp=2; //Used when we send the digits individually
                //the first time we write to the display         
     }

 if (now() != prevDisplay) { //update the display only if the time has changed
      prevDisplay = now();          // i.e. once per Second
   displayInfo();  // GO TO DISPLAY VOID
    }
/////////////////////////////////////////////////
//THIS SECTION READS THE STATE OF THE CENTRE PUSHBUTTON (DisplayState)
//CYCLES THROUGH 3 STATES ALLOWING THE LOWER DISPLAY TO SHOW
//                 BLANK   or  DATE   or  STOPWATCH.
 newSwitchState = digitalRead(TimeDatepin);//Read the state of theBST/GMT pin
 if ( newSwitchState != oldSwitchState )
 {
    // has the button switch been closed?
    if ( newSwitchState == HIGH )
    {
      // increase the value of state
      DisplayState++;
      if (DisplayState > 3) {
        DisplayState = 1;                  
      }
}
    oldSwitchState = newSwitchState;  //  UPDATE THE STATE IF MIDDLE BUTTON IS PUSHED
  }

///////////////////////////End of Time/Date switch state///////////

}
///////////////////////////   END OF LOOP    /////////////////////

 //////////////////////////   DISPLAY VOID   /////////////////////
void displayInfo()
{
 BSTState = digitalRead(BSTpin); //Read state of BST/GMT pin

 ClackState = digitalRead(Clackpin); //Read the state of the Clacker pin

/////////This next section checks for updates of each second and only
//////   runs once per second
////// used for counting seconds and clacker (ticking effect) once per second
 
if (Flag1== 0){   //Once we have a valid second we don't need
                 //it again until the nextsecond

TimeStore = (second()); //whats this do ?????????????????
Flag1=1;       
}
/////////////PULSE  EACH SECOND FOR CLACKER & STOPWATCH////////////
 if (FirstPass==2){ 
    if  (Timeclack < Clacks){   // set the limit on the number of clacks-
Timeclack = Timeclack +1;      //   on startup or it gets on your nerves

if (ClackState == LOW){ //Only if clack switch is off otherwise we get two clacks per second
 clack(); //Tick Tick Tick
}
}}
 //////////////CLACKER///////////////
 /////////Turn on the clacker if the clack switch is high. /////
if (ClackState == HIGH){
  
clack(); //go to clack void  
   
   } 
 
/////////////////////////////////////
////////////////Are we GMT or BST////READ SWITCH ON D4 PIN.
if (BSTState == LOW){

if (BSTLO==0){
  
offset = offset + GMTOffest;

 // Serial.print (" Add GMT offset for Summer Time");
 BSTLO=1;
 } 
}
if (BSTState == 1){
  if (BSTLO==1){
offset = offset - GMTOffest;
//Serial.print (" Winter time = GMT");
 BSTLO=0;
}    }
//////////////END OF GMT - BST STATE////////////

if (FirstPass==2){ //Allow a few loops so time settles before we send to display

//////////////SEND TIME TO UPPER DISPLAY - ON MAX7219 ZERO ////////////

if( month()>0){ // Only display valid data at first boot (Not zeros)

/////First we will blank the leading digit if hour less than 10
if (hour()>=1 && hour() < 10){ //But not if hour = 00
  lc.setRow(0,0,B00000000);
  }
else{
       if( StartUp == 1){
          delay(del); 
            clack();
                }
  
lc.setChar(0,0,(hour()/10),false);
}
       if( StartUp == 2){
          delay(del); 
            clack();
                }
lc.setChar(0,1,(hour()%10),true);
       if( StartUp == 2){
          delay(del); 
            clack();
                }
lc.setChar(0,2,(minute()/10),false);
       if( StartUp == 2){
          delay(del); 
            clack();                 
                }
lc.setChar(0,3,(minute()%10),true);
       if( StartUp == 2){
          delay(del); 
            clack();
                }
lc.setChar(0,4,(second()/10),false);
          if( StartUp == 2){
              delay(del);
                clack();
                 
                  }
lc.setChar(0,5,(second()%10),false);

////////////////////////////////////////////////////////////

//////// DisplayState 1 = DEFAULT STATE OF MIDDLE PUSH BUTTON //////////////
//                            IT SHOWS THE DATE

if (DisplayState==1){

//////////////SEND DATE TO LOWER DISPLAY - ON MAX7219 1 ////////////
 ////////////////  First the  DAY     ///////

if (day() < 10){     // blank the leading digit if Day less than 10
  lc.setRow(1,0,B00000000);
   if( StartUp == 2){
              delay(del);
               delay(del);
                clack();                 
                  }
  lc.setChar(1,1,(day()),false);
  }
else{

   if( StartUp == 2){
              delay(del);
             // Serial.print("Day = ");
              delay(del);              
                clack();                 
                  }
lc.setChar(1,0,(day()/10),false);

 if( StartUp == 2){
              delay(del);
               delay(del);
             //  Serial.print("Day = ");
                clack(); 
                                     
                  }

lc.setChar(1,1,(day()%10),false);
}
          
      //////////////////  now the    MONTH      ////////////
if (month() < 10){
  lc.setRow(1,2,B00000000);  //Blank digit if month less than 10
  if( StartUp == 2){
              delay(del);
               delay(del);
                clack();  
                  StartUp=0;               
                  }
   
  lc.setChar(1,3,(month()),false);
  }
else{

   if( StartUp == 2){
              delay(del);
               delay(del);
                clack();                 
                  }
lc.setChar(1,2,(month()/10),false);

 if( StartUp == 2){
              delay(del);
                clack();  
                  StartUp=0;               
                  }
lc.setChar(1,3,(month()%10),false);
}
}}
//////////////////////////////////////////////////////////////////

///////////////  THE STATE OF THE MIDDLE PUSHBUTTON    ///////////////////      
     }
      if (DisplayState == 2) {  //The State of middle push button
      ///  BLANK THE LOWER DISPLAY LIKE ON ORIGINAL 744H Clock//////

lc.setRow(1,0,B00000000);
lc.setRow(1,1,B00000000);
lc.setRow(1,2,B00000000);
lc.setRow(1,3,B00000000);   

  StopTime=0;  //reset stopwatch
 StopMin=0;
 Display2State = 1; //reset the lower button to its default state
}

/////////////// STOPWATCH is DisplayState 3 ON MIDDLE PUSH BUTTON //////////
/////////////////FUNCTIONS FOR STOPWATCH/////////////////////////
//// THIS SECTION READS THE STATE OF THE LOWER PUSHBUTTON (DisplayState2)PROVIDING THE LOWER
//             DISPLAY IS SHOWING THE STOPWATCH (DisplayState 5).
//            IT CYCLES THROUGH 3 STATES FOR START STOP & RESET
 
 if (DisplayState == 3) { //only run stopwatch if the correct window is displayed

 newStopState = digitalRead(StopWatchpin);//Read the state of the stopwatch button
 if ( newStopState != oldStopState )
 {
    // has the button been pressed ?
    if ( newStopState == HIGH )
    {
      // increase the value of state
      Display2State++;
      if (Display2State > 3) {
        Display2State = 1;                  
      }
}}}
  if (DisplayState == 3) {
 
//////////////////////////////////////////////////////////////////////

 ////////////////////////////////stopwatch lower button
  if (Display2State == 1) {    //SHOW STOPWATCH  00.00    OR RESET STOPWATCH

 StopTime=0;
  StopMin=0;
lc.setChar(1,0,(0),false);// 

lc.setChar(1,1,(0),true);//

lc.setChar(1,2,(0),false);// 

lc.setChar(1,3,(0),false);//
  
 }
 
 if (Display2State == 2) {   //START STOPWATCH

StopTime=StopTime+1;  
  
if (StopFlag1==0){
StopTime=0;
  
Stop1= StopTime;
StopFlag1=1;
}
if (StopTime==60){ //reset seconds to zero after 60 seconds
  StopTime=0;
  StopMin=StopMin+1;
}
if (StopMin==60){ //reset minutes to zero after 60
  StopMin=0;     // because we only have mins and secs digits
}

lc.setChar(1,0,(StopMin/10),false);// DISPLAY THE STOPWATCH

lc.setChar(1,1,(StopMin%10),true);//

lc.setChar(1,2,(StopTime/10),false);// 

lc.setChar(1,3,(StopTime%10),false);//

  }
 
 if (Display2State == 3) {

 // STOP UPDATING THE STOPWATCH AND FREEZE THE DISPLAY
  
 }  
 oldStopState = newStopState; //UPDATE THE STATE IF LOWER BUTTON IS PUSHED
 }     

 //if (FirstPass==2){ //Allow 3 loops for time to settle
  // FirstPass=3; 
 // }
  if (FirstPass==1){
   FirstPass=2; 
  }
  if (FirstPass==0){
   FirstPass=1; 
  }
  
/////////////////////////////////////////////////////////////////
///////////////////DEBUG SECTION. Send to serial monitor

//  printInt(gps.satellites.value(), gps.satellites.isValid(), 5);

//  Serial.print("Day = ");
//  Serial.println(day());

//  Serial.print("Month = ");
//  Serial.println(month());
  
 // Serial.print("Hour = ");
 // Serial.println(hour());

 // Serial.print("Minutes = ");
 // Serial.println(minute());

 // Serial.print("Seconds = ");
 // Serial.println(second());

 //  Serial.print ("StopTime = ");
 // Serial.println (StopTime);

 // Serial.print("Display state = ");
 //  Serial.println(DisplayState);

 //  Serial.print("Display2 state = ");
 // Serial.println(Display2State);

 //  Serial.print("ClackState  = "); // 1=ON 0=OFF
 //  Serial.print(ClackState);
   
 // Serial.print("TimeStore = "); // Store for the seconds value
 //Serial.println(TimeStore);
 
 // Serial.print("timeStatus 2=Locked ");
 //Serial.print(timeStatus());
 //Serial.print(" ");
// Serial.print("offset hrs = ");
// Serial.println(offset);

}
 //////////////////////////   END OF DISPLAY VOID   /////////////////////
 
void clack(){   /////////SOUNDS THE CLACKER  VOID   ////////////////////

digitalWrite(Clacker, HIGH);
delay (12); // 12ms pulse for relay. Adjust for a nice tick but make it as short as possible.
digitalWrite(Clacker, LOW);
}
//     Thank goodness that's done. © David Whitty 2021
