//#include <Servo.h>

#include <ServoTimer2.h>
//#define TRIG_PIN1 9
//#define ECHO_PIN1 10
//#define TRIG_PIN2 11
//#define ECHO_PIN2 12
//#define SERVO_PIN1 5
//#define SERVO_PIN2 6
//#define stepPin 2
//#define dirPin 3

#define TRIG_PIN1 2    // TRIG_PIN1 pindah ke pin 2
#define ECHO_PIN1 3    // ECHO_PIN1 pindah ke pin 3
#define TRIG_PIN2 4    // TRIG_PIN2 pindah ke pin 4
#define ECHO_PIN2 5    // ECHO_PIN2 pindah ke pin 5
#define SERVO_PIN1 6   // SERVO_PIN1 pindah ke pin 6
#define SERVO_PIN2 9   // SERVO_PIN2 pindah ke pin 9 (PWM pada Nano)
#define stepPin 10     // stepPin pindah ke pin 10
#define dirPin 11      // dirPin pindah ke pin 11

//Servo myServo1;
//Servo myServo2;

 
ServoTimer2 myServo1;
ServoTimer2 myServo2;

bool objectDetected = false;      // Flag untuk mendeteksi objek
bool waitingFeedback = false;  // Flag untuk menunggu feedback dari Python

float distance1;
float distance2;

const int DATA_SIZE = 5;
int distance1History[DATA_SIZE] = { 0 };
int distance2History[DATA_SIZE] = { 0 };
int distance1Index = 0;
int distance2Index = 0;

void setup() {
  Serial.begin(9600);

  Serial.println("Start Program");


  pinMode(TRIG_PIN1, OUTPUT);
  pinMode(ECHO_PIN1, INPUT);
  pinMode(TRIG_PIN2, OUTPUT);
  pinMode(ECHO_PIN2, INPUT);

  myServo1.attach(SERVO_PIN1);
  myServo2.attach(SERVO_PIN2);

  myServo1.write(750);
  myServo2.write(2250);

  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
}

void updateDistances() {
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= 50 && !objectDetected) {
    lastUpdate = millis();

    int newDistance1 = readDistance(TRIG_PIN1, ECHO_PIN1);
    int newDistance2 = readDistance(TRIG_PIN2, ECHO_PIN2);

    distance1History[distance1Index] = newDistance1;
    distance2History[distance2Index] = newDistance2;

    distance1Index = (distance1Index + 1) % DATA_SIZE;
    distance2Index = (distance2Index + 1) % DATA_SIZE;

    Serial.print("D1 : ");
    for (int i = 0; i < DATA_SIZE; i++) {
      Serial.print(distance1History[i]);
      Serial.print(" ");
    }
    Serial.print(", D2 : ");
    for (int i = 0; i < DATA_SIZE; i++) {
      Serial.print(distance2History[i]);
      Serial.print(" ");
    }
    Serial.println();
  }
}

float readDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  return (duration * 0.0343) / 2;
}

bool checkCondition(int* array1, int* array2, int size) {
  bool result = false;
  bool allArray1 = false;
    for (int i = 0; i < size; i++) {
      if (array1[i] > 0 && array1[i] < 15) {
          allArray1 = true;
          break;
      }
    }

    bool allArray2 = false;
    for (int i = 0; i < size; i++) {
      if (array2[i] > 0 && array2[i] < 15) {
          allArray2 = true;
          break;
      }
    }

    if (allArray1 || allArray2) {
      result = true;
    }
    
  return result;
}


void checkObjectDetection() {
  if (checkCondition(distance1History, distance2History, DATA_SIZE) && !objectDetected && !waitingFeedback) {
    Serial.println("Object detected");
    objectDetected = true;
    waitingFeedback = true;

    delay(1000);

    myServo1.write(2250);
    delay(2000);
  }
}

void reset() {

  for (int i = 0; i < DATA_SIZE; i++) {
    distance1History[i] = 0;
    distance2History[i] = 0;
  }

  objectDetected = false;
  waitingFeedback = false;

  delay(1000);

  myServo1.write(750);

  delay(1000);

  Serial.println("Reset data");
}

void stepperAway(int box) {
  Serial.println("Stepper move away - START");
  int zeroPoint = 0;

  if (box == 1 || (box > 6 || box < 1)) {
    return;
  }

  int target = (box * 4400) - 4400;

  digitalWrite(dirPin, HIGH);  // Enables the motor to move in a particular direction (menjauh)
  // Makes 200 pulses for making one full cycle rotation
  for (int x = zeroPoint; x < target; x++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(400);  // by changing this time delay between the steps we can change the rotation speed
    digitalWrite(stepPin, LOW);
    delayMicroseconds(400);
  }

  Serial.println("Stepper move away - FINISH");
}

void stepperCloser(int boxes) {
  Serial.println("Stepper move closer - START");
  int zeroPoint = 0;

  if (boxes == 1 || (boxes > 6 || boxes < 1)) {
    return;
  }

  int target = (boxes * 4400) - 4400;

  digitalWrite(dirPin, LOW);  //Changes the rotations direction (mendekat)
  // Makes 400 pulses for making two full cycle rotation
  for (int x = zeroPoint; x < target; x++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(400);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(400);
  }

  Serial.println("Stepper move closer - FINISH");
}

void moveServo() {
  myServo2.write(750);  // Move servo 2
  delay(3000);
  myServo2.write(2250);  // Move servo 2
  delay(100);

  Serial.println("Servo 2 move");
}


void handleSerialInput() {
  if (Serial.available() > 0) {
    char received = Serial.read();
    Serial.print("Received: ");
    Serial.println(received);

    int multiplier = received - '0';

    if (multiplier >= 1 && multiplier <= 6) {
      handleCase(received);
    } else {
      Serial.println("Unknown signal");
    }
  }
}


void handleCase(char multiplier) {
set:
  switch (multiplier) {
    case '1':
      Serial.println("Glass detected");

      stepperAway(1);

      delay(100);
      moveServo();
      delay(1000);

      stepperCloser(1);
      reset();

      break;

    case '2':
      Serial.println("Metal detected");

      stepperAway(2);

      delay(100);
      moveServo();
      delay(1000);

      stepperCloser(2);
      reset();

      break;

    case '3':
      Serial.println("Organic detected");

      stepperAway(3);

      delay(100);
      moveServo();
      delay(1000);

      stepperCloser(3);
      reset();

      break;

    case '4':
      Serial.println("Paper detected");

      stepperAway(4);

      delay(100);
      moveServo();
      delay(1000);

      stepperCloser(4);
      reset();

      break;

    case '5':
      Serial.println("Plastic detected");

      stepperAway(5);

      delay(100);
      moveServo();
      delay(1000);

      stepperCloser(5);
      reset();

      break;

    case '6':
      Serial.println("Textile detected");

      stepperAway(6);

      delay(100);
      moveServo();
      delay(1000);

      stepperCloser(6);
      reset();

      break;

    default:
      Serial.println("Unknown signal");
      goto set;
      break;
  }
}

void loop() {
  updateDistances();
  checkObjectDetection();
  while (objectDetected){
    handleSerialInput();
  }
}
