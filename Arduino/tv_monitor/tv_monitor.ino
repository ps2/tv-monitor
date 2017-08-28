#include <SoftwareSerial.h>
#include "Goldelox_Serial_4DLib.h"
#include "Goldelox_Const4D.h"
#include <RCSwitch.h>

#define RESETLINE 4

// Screen time
unsigned long screenTimeStart = millis();
unsigned long screenTimeDuration = 0;

// Switch
#define POWER_ON true
#define POWER_OFF false
boolean powerState = false;
RCSwitch mySwitch = RCSwitch();

// Variables for command processing
String inputCommand = "";
boolean commandAvailable = false;

SoftwareSerial displaySerial(2,3) ; // pin 2 = TX of display, pin3 = RX
Goldelox_Serial_4DLib Display(&displaySerial);

void displayScreenTime(unsigned int seconds)
{
  Display.txt_MoveCursor(0, 0) ;
  Display.txt_FGcolour(WHITE);
  Display.putstr("Screen Time") ;
  Display.txt_Xgap(3) ;
  Display.txt_Ygap(3) ;
  if (seconds > 10 * 60) {
    Display.txt_FGcolour(GREEN);
  } else if (seconds > 5 * 60) {
    Display.txt_FGcolour(YELLOW);    
  } else {
    Display.txt_FGcolour(RED);
  }
  Display.txt_FontID(SYSTEM) ;
  Display.txt_MoveCursor(2, 0) ;
  Display.txt_Height(4) ;
  Display.txt_Width(4) ;
  if (seconds > 60) {
    Display.putstr((String(seconds / 60 + 1) + "m  ").c_str()) ;    
  } else {
    Display.putstr((String(seconds) + "s  ").c_str()) ;    
  }
  Display.txt_Height(1) ;
  Display.txt_Width(1) ;  
}

void Callback(int ErrCode, unsigned char ErrByte)
{
  const char *Error4DText[] = {"OK\0", "Timeout\0", "NAK\0", "Length\0", "Invalid\0"};
  Serial.print(F("Serial 4D Library reports error "));
  Serial.print(Error4DText[ErrCode]);

  if (ErrCode == Err4D_NAK)
  {
    Serial.print(F(" returned data= "));
    Serial.println(ErrByte);
  }
  else {
    Serial.println(F(""));
  }
// Pin 13 has an LED connected on most Arduino boards. Just give it a name
#define led 13
  while(1)
  {
    digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(200);               // wait for a second
    digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
    delay(200);               // wait for a second
  }
}

void processCommand() {
  if (inputCommand.startsWith("T")) {
    unsigned long newDuration = inputCommand.substring(1).toInt();
    if (newDuration > 0) {
      screenTimeDuration = newDuration*1000;
      screenTimeStart = millis();
    }
  } else if (inputCommand.startsWith("M")) {
    displayMessage(inputCommand.substring(1));
  } else if (inputCommand.startsWith("C")) {
    Display.gfx_Cls();
  }
  inputCommand.remove(0);
}

void setup()
{
  pinMode(13,OUTPUT);
  pinMode(RESETLINE, OUTPUT);  // Set D4 on Arduino to Output (4D Arduino Adaptor V2 - Display Reset)
  digitalWrite(RESETLINE, 0);  // Reset the Display via D4
  delay(100);
  digitalWrite(RESETLINE, 1);  // unReset the Display via D4
  delay(5000);

  Display.TimeLimit4D   = 5000 ; // 2 second timeout on all commands
  Display.Callback4D = Callback ; // NULL ;
  displaySerial.begin(9600) ;

  Display.SSTimeout(0); // NULL ;
  Display.gfx_Cls();

  Serial.begin(19200);
  inputCommand.reserve(200);

  // Optional set number of transmission repetitions.
  // mySwitch.setRepeatTransmit(15);
  mySwitch.enableTransmit(5);
  
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
}

void setPowerState(boolean newState) {
  if (newState != powerState) {
    powerState = newState;
    if (powerState == POWER_ON) {
      mySwitch.send("10001000010001100000010100");
    } else {
      Display.gfx_Cls();
      mySwitch.send("01001000010001100000010100");
    }
  }
}

void displayMessage(String msg) {
  Display.gfx_Cls();
  Display.txt_MoveCursor(6, 0);
  Display.txt_FGcolour(WHITE);
  Display.putstr((msg).c_str()) ;
}

void loop()
{
  unsigned long elapsed = millis() - screenTimeStart;
  long remaining = screenTimeDuration - elapsed;
  if (remaining > 0) {
    setPowerState(POWER_ON);
    displayScreenTime(remaining / 1000);
  } else {
    setPowerState(POWER_OFF);
  }

  if (commandAvailable) {
    commandAvailable = false;
    processCommand(); 
  }
  
  delay(100);
}

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() {
  while (!commandAvailable && Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputCommand:
    inputCommand += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      commandAvailable = true;
    }
  }
}
