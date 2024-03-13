// This software controls the Droptomaat. 

// On weekdays (mon-sat), at midnight, the droptomaat will index to the next pocket. 
// On Sundays the Droptomaat will be empty & open to release all possible content.
// On Mondays the Droptomaat can be filled. 
// The Droptomaat can be opened and the switch on the flap can be used to go into filling mode
// In this mode, the Droptomaat will beep, indicating that it is in filling mode. 

// Using Arduino Uno
// An RTC tracks time, the Arduino controls the motor and a buzzer, and handles input from the button.

#include <Wire.h>
#include <RTClib.h>
#include <Servo.h>

Servo myservo;
int servoPin = 9;
int beepPin = 2;
int buttonPin = 12;
int VPin = 13; // Additional 5V power
int positionX = 180; // Push position
int positionO = 90; // Open position
int positionY = 50; // Closed position
RTC_DS3231 rtc;
int baudrate = 9600;

void printDateTime() {
  DateTime now = rtc.now();
  Serial.begin(9600);
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
}

void setRTCTime() {
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void setup() {
  Wire.begin();
  myservo.attach(servoPin);
  pinMode(beepPin, OUTPUT);
  pinMode(VPin, OUTPUT);
  pinMode(buttonPin, INPUT); 
  digitalWrite(VPin, HIGH);    
  myservo.write(positionY);     // Close the servo initially
  
  if (rtc.begin()) {
    Serial.begin(baudrate);
    Serial.println("RTC start:");
    DateTime now = rtc.now();
    printDateTime();
  }

  if (!rtc.begin()) {
    Serial.begin(baudrate);
    Serial.println("RTC not found");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.begin(baudrate);
    Serial.println("RTC lost power");
    Serial.println("Set time");
    setRTCTime();
  }

}

void loop() {
  
  // IF operational
  if (digitalRead(buttonPin) == 0) {
    DateTime now = rtc.now();

    // // Check if it's 02:00:xx (midnight)
    if (now.hour() == 2 && now.minute() == 00) {
      // Days are numbered: Sunday is 0, Monday is 1, ..., Saturday is 6
      if (now.dayOfTheWeek() >= 0 && now.dayOfTheWeek() <= 5) { // Monday to Saturday
        myservo.write(positionX); // Push
        delay(500);
        myservo.write(positionY); // Close
      } 
      else { // Sunday
        myservo.write(positionO); // Open
      }
      delay(60000); // Wait for 1 minute
    }
  }
  // IF refill
  else {
    myservo.write(positionO); // Open
    while (digitalRead(buttonPin) == 1) {
      beep();
    }
    myservo.write(positionY); // Close
  }
  printDateTime();
  delay(1000);
}


void beep() {
    digitalWrite(beepPin, HIGH);
    delay(10);            
    digitalWrite(beepPin, LOW);
    delay(1000);
}


