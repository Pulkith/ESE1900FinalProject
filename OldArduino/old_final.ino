#include "MQ135.h"
#include <DHT.h>

// give a name to digital pin 2, which has a pushbutton attached
int pushButton = 2;

// #include "MQ135.h"
// const int ANALOGPIN=0;
// MQ135 gasSensor = MQ135(ANALOGPIN);
// void setup(){
//   Serial.begin(9600);      // sets the serial port to 9600
// }
// void loop(){
//   float rzero = gasSensor.getRZero();
//   Serial.println(rzero);
//   delay(1000);
// }


int motorControl = 9;
const int ANALOGPIN=0;
int UNSAFE = 800;
int WARNING = UNSAFE - 100;
MQ135 gasSensor = MQ135(ANALOGPIN);

int redPin= 4;
int greenPin = 7;
int bluePin = 8;

const int buzzer = 13;
#include <LiquidCrystal.h>

const int rs = 12, en = 11, d4 = 10, d5 = 6, d6 = 5, d7 = 3;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


#define dhtp 2
DHT dht(dhtp, DHT11);

float h;
float t;

// the setup routine runs once when you press reset:
void setup() {
  // make the pushbutton's pin an input:
  pinMode(pushButton, INPUT);

  // make the transistor's pin an output:
  pinMode(motorControl, OUTPUT);  

  pinMode(buzzer, OUTPUT); // Set buzzer - pin 9 as an output

  Serial.begin(9600);

  dht.begin();

  lcd.begin(16, 2);
  
}

// the loop routine runs over and over again forever:
void loop() {
  float ppm = gasSensor.getPPM();
  Serial.println(ppm);
  float rzero = gasSensor.getRZero();
  Serial.println(rzero);

  // if(ppm > 400) {
  //   analogWrite(motorControl, 100);
  // } else {
  //   analogWrite(motorControl, 0);
  // }

  if(ppm < WARNING) {
    setColor(0, 255, 0);
    lcd.setCursor(0, 0);
    lcd.print("SAFE: " + String(ppm));
    lcd.setCursor(0, 1);
    lcd.print("T: " + String((int) (t * 9.0 / 5) + 32) + " - NO FAN"); 
    analogWrite(motorControl, 0);
  }
  else if(ppm < UNSAFE) {
    setColor(255, 255, 0);
    lcd.setCursor(0, 0);
    lcd.print("CAT: " + String(ppm));
    lcd.setCursor(0, 1);
    lcd.print("T: " + String((int) (t * 9.0 / 5) + 32) + " - NO FAN"); 
      analogWrite(motorControl, 0);

  } else {
    // setColor(255, 0, 0);
    setColor(255, 0, 0);
    for (int i = 0; i < 4; ++i) {
      tone(buzzer, 1000); // Send 1KHz sound signal...
      delay(200);        // ...for 1 sec
      noTone(buzzer);     // Stop sound...
    }
    lcd.setCursor(0, 0);
    lcd.print("RUN!: " + String(ppm));
    lcd.setCursor(0, 1);
    lcd.print("T: " + String((int) (t * 9.0 / 5) + 32) + " - FAN ON"); 
      analogWrite(motorControl, 100);
  }
  readDHT();
  delay(1000);
}

void readDHT() {
  h = dht.readHumidity();
  t = dht.readTemperature();

  Serial.print("Temperature: ");
  Serial.println(t);

  delay(100);
  
  Serial.print("Humidity: ");
  Serial.println(h);
}

void setColor(int redValue, int greenValue, int blueValue) {
  analogWrite(redPin, redValue);
  analogWrite(greenPin, greenValue);
  analogWrite(bluePin, blueValue);
}
