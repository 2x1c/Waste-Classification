#include <Servo.h>

Servo myservo,myservo1;  // create servo object to control a servo

void setup() {
  Serial.begin(9600);
  Serial.println("start");

  myservo.attach(6);  // attaches the servo on pin 9 to the servo object, servo 1 start (0) end (180)
  myservo1.attach(9);  // attaches the servo on pin 9 to the servo object, servo 2 start (180) end (0)
}

void loop() {
  delay(2000);  
  myservo.write(0);                  // sets the servo position according to the scaled value
  //myservo1.write(0);                         // waits for the servo to get there
  Serial.println("0");                 // sets the servo position according to the scaled value
  delay(1000);
  //myservo.write(90);
  //myservo1.write(90);
  //delay(2000);                           // waits for the servo to get there
  
  Serial.println("180");
  //myservo.write(180);                  // sets the servo position according to the scaled value
  //myservo1.write(180);  
  delay(1000);                     // waits for the servo to get there
}
