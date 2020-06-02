#include <AccelStepper.h>
#include <Wire.h>
#include "RTClib.h"

DS1307 rtc;

AccelStepper stepper(1, 3, 2); // mode, step, direction
#define SLEEP 4 // sleep pin

#define OPEN  0
#define CLOSE -22000

void setup()
{  
  Serial.begin(57600);
#ifdef AVR
  Wire.begin();
#else
  Wire1.begin(); // Shield I2C pins connect to alt I2C bus on Arduino Due
#endif
  rtc.begin();

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(__DATE__, __TIME__));
  }
  
  stepper.setMaxSpeed(4000.0);
  stepper.setAcceleration(1000.0);
  pinMode(SLEEP, OUTPUT);
  digitalWrite(SLEEP, HIGH);
}

void runStepperTo(int pos){
  digitalWrite(SLEEP, LOW);
  delay(10);    
  stepper.runToNewPosition(pos);
  digitalWrite(SLEEP, HIGH);
}

bool found = false;

void loop(){    
    DateTime now = rtc.now();

    if (Serial.available()){
      char c = Serial.read();
      if (c == 'o') runStepperTo(OPEN);
      if (c == 'c') runStepperTo(CLOSE);
    } else if ((now.hour() == 4 && now.minute() == 30) || (now.hour()==16 && now.minute()==0)) {
      if (!found)  {
        // open the blinds
        Serial.println("opening");
        runStepperTo(OPEN);
      }
      found = true;
    } else if ((now.hour() == 21 && now.minute() == 15) || (now.hour()==11 && now.minute()==0)) {
      if (!found) {
        // close the blinds
        Serial.println("closing");
        runStepperTo(CLOSE);
      }
      found = true;
    } else {
      found = false;
    }

    
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    
    delay(20000);
}
