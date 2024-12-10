#include <WiFi.h> // WiFi module to connect to WiFi
#include "ThingSpeak.h" // ThingSpeak module to send data to ThingSpeak
#include <DHT.h> // Temperature module to read and output temperature
#include <Wire.h> // Wire module for I2C communication
#include <LiquidCrystal_I2C.h> // LCD module

// Defining variables for sensors
#define Threshold 2000
#define gasPin 35
#define ultrasonicEcho 25
#define ultrasonicTrig 33
#define tempPin 14
#define tiltPin 13
#define ledBlue 15
#define ledRed 23
#define buzzerPin 4
#define DHTTYPE DHT11

LiquidCrystal_I2C lcd(0x27, 16, 2); // Defining LCD

const char* ssid = "";  // WiFi network SSID
const char* password = "";   // WiFi network password

WiFiClient client;
unsigned long channel = ;       // Thingspeak channel number
const char* API = "";  // Channel write API

DHT dht(tempPin, DHTTYPE); // Specifying temperature model
unsigned long lastTime = 0;
unsigned long timerDelay = 15000;

// Setup which sets up:
// 1. Serial Communication
// 2. WiFi mode
// 3. ThingSpeak start-up
// 4. Pin configurations
// 5. LCD configurations
void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
  dht.begin();
  pinMode(tiltPin, INPUT);
  pinMode(ultrasonicEcho, INPUT);
  pinMode(ultrasonicTrig, OUTPUT);
  pinMode(ledBlue, OUTPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  delay(20000);
}

// Loop function
void loop() {
  // Read tilt sensor every iteration
  int tiltState = digitalRead(tiltPin);

  // If tilt is HIGH, flash LEDs back and forth continuously
  // This happens regardless of the 30-second interval
  if (tiltState == HIGH) {
    // Flash Blue LED & turn on buzzer
    digitalWrite(ledBlue, HIGH);
    digitalWrite(ledRed, LOW);
    digitalWrite(buzzerPin, HIGH);
    delay(200);
    // Flash Red LED & turn off buzzer
    digitalWrite(ledBlue, LOW);
    digitalWrite(ledRed, HIGH);
    digitalWrite(buzzerPin, LOW);
    delay(200);
  } else {
    // If tilt is not active, ensure LEDs and buzzer are off here
    digitalWrite(ledBlue, LOW);
    digitalWrite(ledRed, LOW);
    digitalWrite(buzzerPin, LOW);
  }

  // Perform the timed operations (ThingSpeak update, sensor readings)
  if ((millis() - lastTime) > timerDelay) {
    // Connect or reconnect to WiFi
    if (WiFi.status() != WL_CONNECTED) {
      Serial.print("Attempting to connect");
      while (WiFi.status() != WL_CONNECTED) {
        WiFi.begin(ssid, password);
        delay(5000);
      }
      Serial.println("\nConnected.");
    }

    float tempValue = dht.readTemperature(); // Read temperature
    float gasValue = analogRead(gasPin); // Read gas value

    lcd.clear(); // Clear LCD screen
    // If tempValue is greater than 40
    if (tempValue > 40) { // If true
      Serial.print("Temperature: ");
      Serial.print(tempValue);
      Serial.println(" - Heat detected!"); // Heat detected message
    }
    else {
      // If false
      Serial.print("Temperature in Celsius: ");
      Serial.println(tempValue);
    }
    // If gasValue greater than Threshold
    if (gasValue > Threshold) { // If true
      Serial.print("Gas value: ");
      Serial.print(gasValue);
      Serial.println(" - Gas detected!"); // Gas detected message
    }
    else {
      // If false
      Serial.print("Gas value: ");
      Serial.println(gasValue);
    }
    // Ultrasonic distance measurement
    digitalWrite(ultrasonicTrig, LOW);
    delayMicroseconds(2);
    digitalWrite(ultrasonicTrig, HIGH);
    delayMicroseconds(10);
    digitalWrite(ultrasonicTrig, LOW);

    // Calcualte distance
    long duration = pulseIn(ultrasonicEcho, HIGH);
    int distance = duration / 58;
    Serial.print("Distance: ");
    Serial.println(distance);

    // If distance is equal or less than 8
    if (distance <= 8) { // If true, print "I am full."
      lcd.setCursor(6, 0);
      lcd.print("I am");
      lcd.setCursor(6, 1);
      lcd.print("full.");
    } else if (distance > 10) { // Else if distance is greater than 10
      lcd.setCursor(4, 0); // If true, print "Feed me trash"
      lcd.print("Feed me");
      lcd.setCursor(5, 1);
      lcd.print("trash");
    } else { // Else 
      lcd.setCursor(0, 0); // Print "No reading!"
      lcd.print("No reading!");
    }

    // Update ThingSpeak fields
    ThingSpeak.setField(1, gasValue);      // Field 1: Gas value
    ThingSpeak.setField(2, tempValue);     // Field 2: Temperature
    ThingSpeak.setField(3, (distance <= 8 ? 1 : 0)); //Field 3: Full

    int resp = ThingSpeak.writeFields(channel, API);

    if (resp == 200) {  // if resp == OK response
      Serial.println("Channel update successful."); // Successful message
    } else {  // Others
      Serial.println("Problem updating channel. HTTP error code " + String(resp)); // Error message
    }

    lastTime = millis();
  }

  // Short delay to prevent excessive loop speed
  delay(50);
}
