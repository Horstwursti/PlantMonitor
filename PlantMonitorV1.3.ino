char title[] = "PlantMonitor";
char version[] = "V1.3";
char owner[] = "by Horstwursti";
char contact[] = "Mail: horstwursti@hackerspace-bielefeld.de / Mastodon: @horstwursti";


#include <LiquidCrystal.h>
#include <LcdBarGraph.h>

const byte sensorOnOffPin = 6;
const byte analogPlantSensorPin = A0;
const byte digitalPlantSensorPin = 10;
const byte lightSensorPin = A1;
const byte ledPin = 7;
const byte backlightPin = 9;
const byte buttonPin = 8;

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
LcdBarGraph lbga(&lcd, 7, 9, 0);
LcdBarGraph lbgb(&lcd, 7, 9, 1);


int sensorValueWet = 0;
int sensorValueLux = 0;
boolean ledState = false;
int percentWet = 0;
int percentLux = 0;
int barGraphWet = 0;
int barGraphLux = 0;
unsigned long buttonPushedMillis;
unsigned long lcdTurnedOnAt;
bool lcdReady = false;
bool lcdState = false;

unsigned long lastSerial;
unsigned long lastSensor;

unsigned long turnOnDelay = 5;
unsigned long turnOffDelay = 10000;
unsigned long serialDelay = 1000;
unsigned long sensorDelay = 60000;
unsigned long sensorReadDelay = 2000;

unsigned long time;
unsigned long seconds;

void setup() {
  
  Serial.begin(9600);
  Serial.print(title);
  Serial.println(version);
  Serial.println(owner);
  Serial.println(contact);
  Serial.println("starting...");
  pinMode(sensorOnOffPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(backlightPin, OUTPUT);
  pinMode(digitalPlantSensorPin, INPUT_PULLUP);
  pinMode(buttonPin, INPUT_PULLUP);
  lcd.begin(16, 2);
  digitalWrite(backlightPin, HIGH);
  lcd.print(title);
  //lcd.setCursor(0, 13);
  lcd.print(version);
  lcd.setCursor(1, 1);
  lcd.print(owner);
  delay(3000);
  lcd.clear();
  digitalWrite(backlightPin, LOW);
  lcd.noDisplay();
  delay(100);
  wet();
  Serial.println("running!");

}

void loop() {
  unsigned long currentMillis = millis();


  if (digitalRead(buttonPin) == LOW) {
    buttonPushedMillis = currentMillis;
    lcdReady = true;
  }

  if (lcdReady) {
    if ((unsigned long)(currentMillis - buttonPushedMillis) >= turnOnDelay) {
      digitalWrite(backlightPin, HIGH);
      lcd.display();
      wet();
      Serial.println("++++++++ Manual H2O Sensor Update ++++++++");
      
      lcdState = true;
      lcdTurnedOnAt = currentMillis;
      lcdReady = false;
    }
  }

  if (lcdState) {
    if ((unsigned long)(currentMillis - lcdTurnedOnAt) >= turnOffDelay) {
      lcdState = false;
      digitalWrite(backlightPin, LOW);
      lcd.noDisplay();
    }
  }
  
  digitalWrite(ledPin, LOW);
    
  if (ledState == true) {
    lcd.clear();
    alarm();
    lcdState = true;
  }

  if ((unsigned long)(currentMillis - lastSensor) >= sensorDelay) {
    wet();
    Serial.println("++++++++ Automatic H2O Sensor Update ++++++++");

    lastSensor = currentMillis;
  }
  
  clock();
  lux();
  
  if ((unsigned long)(currentMillis - lastSerial) >= serialDelay) {
    Serial.print("H20:");
    Serial.print(percentWet);
    Serial.print("%");
    Serial.print(" - ");
    Serial.print("Lux:");
    Serial.print(percentLux);
    Serial.print("%");
    Serial.print(" Runtime:");
    Serial.print(seconds); 
    Serial.println(" seconds");
    
    lastSerial = currentMillis;
  }
  
}

void wet() {
  digitalWrite(sensorOnOffPin, HIGH);
  delay(sensorReadDelay);
  ledState = digitalRead(digitalPlantSensorPin);
  sensorValueWet = analogRead(analogPlantSensorPin);
  barGraphWet = map(sensorValueWet, 0, 1023, 1023, 0);
  percentWet = map(sensorValueWet, 0, 1023, 100, 0);
  lbga.drawValue(barGraphWet, 1023);
  delay(100);
  lcd.setCursor(0, 0);
  lcd.print("H2O:");
  lcd.setCursor(5, 0);
  lcd.print(percentWet);
  //lcd.setCursor(7, 0);
  lcd.print("% ");
  digitalWrite(sensorOnOffPin, LOW);
}

void lux() {
  sensorValueLux = analogRead(lightSensorPin);
  barGraphLux = map(sensorValueLux, 0, 1023, 0, 1023);
  percentLux = map(sensorValueLux, 0, 1023, 0, 100); 
  lbgb.drawValue(barGraphLux, 1023);
  delay(100);
  lcd.setCursor(0, 1);
  lcd.print("Lux:");
  lcd.setCursor(5, 1);
  lcd.print(percentLux);
  //lcd.setCursor(7, 1);
  lcd.print("% ");
  
}

void alarm() {
  lcd.display();
  digitalWrite(ledPin, HIGH);
  digitalWrite(backlightPin, HIGH);
  lcd.setCursor(5, 0);
  lcd.print("DURST!");
  Serial.print("ALARM!!! ");
  lcd.setCursor(1, 1);
  lcd.print("Wasser bitte!!");
  Serial.println("Wasser bitte!!");
  delay(2000);
  digitalWrite(backlightPin, LOW);
  lcd.clear();
  delay(500);
}

void clock() {
  time = millis();
  seconds = (time / 1000);
}

