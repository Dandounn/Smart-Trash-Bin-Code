#include <Servo.h> //Servo library to manipulate servo motor

// Declaring variables and pin configurations
Servo servomotor;
int inputPin=5;
int outputPin=6;

// Setup function to declare pin modes and establish a serial communication
void setup() {
  Serial.begin(9600);
  pinMode(inputPin, INPUT);
  pinMode(outputPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  servomotor.attach(7);
}

// Loop function to repeat main program
void loop() {
  // Send pulse signals
  digitalWrite(outputPin, LOW);
  delayMicroseconds(2);
  
  digitalWrite(outputPin, HIGH);
  
  delayMicroseconds(10);
  digitalWrite(outputPin, LOW);
  
  // Declare distance variable to hold received pulse values
  int distance = pulseIn(inputPin, HIGH);

  // Calculating real distance value
  distance=distance/58;
  Serial.println(distance); // Printing distance value to serial monitor
  delay(50);
  
  // If statement to check if object is within given parameters
  if (distance < 7 && distance > 3) { // If true
    servomotor.write(0); // Open the lid
    delay(2500); // Wait with the lid open
  }
  else { // If false
    servomotor.write(170); // Close the lid
  }
}