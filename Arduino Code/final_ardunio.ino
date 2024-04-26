#include <WiFiS3.h>
#include "MQ135.h" ////////////////////////// UPDATED CUSTOM HEADER FILE ////////////////////////// 
#include <DHT.h>
#include <LiquidCrystal.h>
 // ngrok http 5002 --host-header="localhost:5002" --scheme=http

////////////////////////// WIFI DETAILS ////////////////////////// 
const char *ssid = "";
const char *password = "";
const char *server = "pulkith.ngrok.app";
const int httpsPort = 80;

WiFiClient client;

////////////////////////// VARS  ////////////////////////// 
float temp = 0, humi = 0, index = 0, dlevel = 0, ppm = 0;
bool fan1 = false, fan2 = false;
float regtemp = 0;

////////////////////////// PORT DETAILS  ////////////////////////// 
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
#define dhtp 6
DHT dht(dhtp, DHT11);
const int buzzer = 13;
const int redPin = 8;
const int greenPin = 9;
const int bluePin = 10;
const int motorControl = 7;
const int gasPin=0;
const int threshpin=1;

MQ135 gasSensor = MQ135(gasPin);

////////////////////////// SAFE RANGES ////////////////////////// 

int UNSAFE = 400;
int WARNTHRESH = 100;
// int WARNING = UNSAFE - WARNTHRESH;

const int callib = 0;


void setup()
{
  ////////////////////////// BOARD SETUP ////////////////////////// 
  pinMode(motorControl, OUTPUT);  
  pinMode(buzzer, OUTPUT); 
  dht.begin();
  lcd.begin(16, 2);

  ////////////////////////// WIFI CONNECTION ////////////////////////// 
    Serial.begin(9600);
    Serial.println("Connecting to WiFi");

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("Connected to WiFi");

    // Check if connection to the server is successful
    if (!client.connect(server, httpsPort)){
        Serial.println("Connection to server failed!");
    }
    else{
        Serial.println("Connected to server!");
    }


}
////////////////////////// UPDATE CONSTS ////////////////////////// 
//push updates
float lastupdate = 0;
float updatechange = 500;
//sensor reads
float lastread = 0;
float updateread = 500;
//motor constants
bool onMotors = true;
int motorSpeed = 150;
//threshold read
float lastpotent = 0;
float updatepotent = 500;
//zero post to Serial
float lastRZEROprint = 0;
float updateRZero = 2500;
//show threshold on LCD
String updateThresh = "Threshold: ";
float lasttreshUpdate = -50000;
float keepThreshUpdate = 3000;

void loop(){

  ////////////////////////// UPDATE THRESH ////////////////////////// 
  if(millis() > lastpotent + updatepotent) {
      int last = UNSAFE;
      int potentthresh = (int)(analogRead(A1) / 1.5) + 100;
      if(abs(potentthresh - last) > 10) {
          Serial.println("THRESH: " + String(potentthresh));
          UNSAFE = potentthresh;
          updateThresh = "Threshold: " + String(potentthresh);
          lasttreshUpdate = millis();
      }
  }

  ////////////////////////// GET DATA ////////////////////////// 
   if (millis() > lastread + updateread){
      readAndParseData();
    }

  ////////////////////////// SEND CHANGES  ////////////////////////// 
    if (millis() > lastupdate + updatechange){
        sendData();
    }
  ////////////////////////// UPDATE THRESH ON LCD  ////////////////////////// 
      if(lasttreshUpdate < millis() && millis() < lasttreshUpdate + keepThreshUpdate) {
        updateLCDThreshUpdate();
      }
}

void readAndParseData() {
    ////////////////////////// GET PPM ////////////////////////// 
      // float ppm = gasSensor.getPPM();
      ppm = gasSensor.getCorrectedPPM(regtemp, humi);
      // Serial.println(cppm);
      float rzero = gasSensor.getRZero();
      // gasSensor.updateRZero(rzero);
      index = ppm;
      // Serial.println(ppm);

      ////////////////////////// PUSH TO SERIAL  ////////////////////////// 
      if(millis() > lastRZEROprint + updateRZero) {
        Serial.println(rzero);
        lastRZEROprint = millis();
      }
      ////////////////////////// READ TEMP ////////////////////////// 
      readDHT();

      ////////////////////////// UPDATE LCD AND MOTORS  //////////////////////////       
      int revlevel = dlevel;

      updatePhysical();

      lastread = millis();

    ////////////////////////// SEND DATE IF DANGER LEVEL CHANGE ////////////////////////// 
      if(dlevel != revlevel) {
          sendData();
      }
}

////////////////////////// SHOW THRESHOLD IF CHANGE RECENTLY ////////////////////////// 
void updateLCDThreshUpdate() {
    lcd.setCursor(0, 0);
    lcd.print(updateThresh);
}

////////////////////////// UPDATE LCD, MOTORS ////////////////////////// 

void updatePhysical() {
    ////////////////////////// CLEAR LCD AFTER THRESH ////////////////////////// 
    lcd.setCursor(0, 0);
    lcd.print("                " );

    if(ppm < UNSAFE - WARNTHRESH) {
        ////////////////////////// GOOD ////////////////////////// 
        dlevel = 0;
        setColor(0, 255, 0);
        
        lcd.setCursor(0, 0);
        lcd.print("SAFE: " + String(ppm));
        lcd.setCursor(0, 1);
        lcd.print("T: " + String((int) temp) + " - NO FAN"); 
        
        motors(false);
      }
      else if(ppm < UNSAFE) {
        ////////////////////////// WARNING ////////////////////////// 
        dlevel = 1;
        setColor(255, 255, 0);

        lcd.setCursor(0, 0);
        lcd.print("CAT: " + String(ppm));
        lcd.setCursor(0, 1);
        lcd.print("T: " + String((int) temp) + " - NO FAN"); 

        motors(false);
      } else {
        ////////////////////////// DANGER ////////////////////////// 
        dlevel = 2;
        setColor(255, 0, 0);

        lcd.setCursor(0, 0);
        lcd.print("RUN!: " + String(ppm));
        lcd.setCursor(0, 1);
        lcd.print("T: " + String((int) temp) + " - FAN ON"); 

        motors(true);

        playDangerSound();
      }
}

////////////////////////// PLAY QUICK BUZZER FOR DANGER ////////////////////////// 
void playDangerSound() {
  for (int i = 0; i < 4; ++i) {
      tone(buzzer, 1000); // Send 1KHz sound signal...
      delay(200);        // ...for 1 sec
      noTone(buzzer);     // Stop sound...
    }
}

////////////////////////// CONTROL MOTORS ////////////////////////// 
void motors(bool on) {
  if(on && onMotors) {
    analogWrite(motorControl, motorSpeed);
    fan1 = true;
    fan2 = true;
  } else {
    analogWrite(motorControl, 0);
    fan1 = on;
    fan2 = on;
  }
}

////////////////////////// SEND DATA TO SERVER ////////////////////////// 
void sendData()
{
    ////////////////////////// RECONNECT IF DISCONNECTED ////////////////////////// 
    if(!client.connected()) {
        if (client.connect(server, httpsPort)) {
            // Serial.println("Reconnected to server");
        } else {
            // Serial.println("Failed to reconnect");
        }
    }

    if (client.connected()) {
        //Update MONGODB Document
        String update = "/6628251b88b68938c4cfde6d";
        //JSON Payload
        String jsonData = "{\"temperature\": "+String(temp)+", \"humidity\": "+String(humi)+", \"index\": "+String(index)+", \"dangerLevel\": "+String(dlevel)+", \"fan1\": " +String(fan1)+", \"fan2\": "+String(fan2)+"}";
        //Headers Below
        client.println("PUT /update" + update + " HTTP/1.1");
        client.println("Host: " + String(server));
        client.println("Content-Type: application/json");
        client.println("Connection: close");
        client.println("Content-Length: " + String(jsonData.length()));
        client.println();         // End of headers
        client.println(jsonData); // Send the JSON data
        // Serial.println("UPDATING SERVER");
        //RESPONSE NOT NEEDED
        // while (client.available()){
        //     String line = client.readStringUntil('\n');
        //     Serial.println(line);
        // }
        lastupdate = millis();
    }
}

////////////////////////// READ DHT SENSOR ////////////////////////// 
void readDHT() {
  humi = dht.readHumidity();
  regtemp = dht.readTemperature();
  //convert C to F
  temp = (regtemp * 9.0) / 5.0 + 32;
  // Serial.print("Temperature: ");
  // Serial.println(temp);
  
  // Serial.print("Humidity: ");
  // Serial.println(humi);
}
////////////////////////// CHANGE LED COLOR ////////////////////////// 
void setColor(int redValue, int greenValue, int blueValue) {
  analogWrite(redPin, redValue);
  analogWrite(greenPin, greenValue);
  analogWrite(bluePin, blueValue);
}