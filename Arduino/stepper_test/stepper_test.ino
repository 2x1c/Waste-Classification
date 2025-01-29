
#define stepPin 10
#define dirPin 11
 
void setup() {
  Serial.begin(9600);
  Serial.println("start");
  // Sets the two pins as Outputs
  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);
}
void loop() {
  Serial.println("Away");
  digitalWrite(dirPin,HIGH); // Enables the motor to move in a particular direction (menjauh)
  // Makes 200 pulses for making one full cycle rotation
  for(int x = 0; x < 8000; x++) {
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(400);    // by changing this time delay between the steps we can change the rotation speed
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(400); 
  }
  delay(3000); // One second delay
  
  Serial.println("Closer");
  digitalWrite(dirPin,LOW); //Changes the rotations direction (mendekat)
  // Makes 400 pulses for making two full cycle rotation
  for(int x = 0; x < 8000; x++) {
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(400);
    digitalWrite(stepPin,LOW);
    delayMicroseconds(400);
  }
  delay(6000);
}