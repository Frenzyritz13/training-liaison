#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "Button2.h"; 
#include "ESPRotary.h";
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>

// char ssid[] = "Mishra"; // your network SSID (name)
// char password[] = "password7";  // your network key
#define BOTtoken "1212253230:AAHMVxFTecnDXduCNVVQiaBKXvSyLW0XqeI" // your Bot Token (Get from Botfather)

// String Mychat_id = "";

// String bot_name = "Pleock"; // you can change this to whichever is your bot name

// WiFiSSLClient client;

// UniversalTelegramBot bot(BOTtoken, client);

// int Bot_mtbs = 1000; //mean time between scan messages
// long Bot_lasttime;   //last time messages' scan has been done

#define FIREBASE_HOST "https://athenahacks2021-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "AZ2YXuQUPeKrEUfVDTMwsXP7pDW4AG73Vc1u9B1D"
#define WIFI_SSID "Mishra"
#define WIFI_PASSWORD "password7"
#define API_KEY "AIzaSyAzz1b19d4bGmvMSnsJvbLjuY2PiwvWaCE"


const int FLEX_PIN = A0; // Pin connected to voltage divider output
#define ROTARY_PIN1  D7
#define ROTARY_PIN2 D6
#define BUTTON_PIN  D4

#define CLICKS_PER_STEP   4

const float VCC =4.98; // Measured voltage of Ardunio 5V line
const float R_DIV =47500.0; // Measured resistance of 3.3k resistor
const float STRAIGHT_RESISTANCE =37300.0; // resistance when straight
const float BEND_RESISTANCE =90000.0; // resistance at 90 deg

ESPRotary r = ESPRotary(ROTARY_PIN1, ROTARY_PIN2, CLICKS_PER_STEP);
Button2 b = Button2(BUTTON_PIN);

FirebaseData fbdo;
Adafruit_MPU6050 mpu;
int count =0;

void setup()
{
  Serial.begin(9600);
  pinMode(FLEX_PIN, INPUT);
  Serial.println("Init");
  pinMode(A0, INPUT);
//  pinMode(LED_PIN, OUTPUT);
//  pinMode(FLOWMETER_INPUT_PIN, INPUT_PULLUP);
  rotarySetup();
  WifiSetup();

  // String list = "Hi, I just woke up and this is what can I do for you \n";
  // list = list + "tell you when you consumed the pill and used a distraction";

  // bot.sendMessage(Mychat_id, list, "Markdown");

}

void loop()
{
  readFlex();
  getAccelValues();
  printAccelValues();
  rotaryUpdates();
  firebaseUpdate("True");
  count++;
  if(count>7){
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  if(Firebase.setString(fbdo, "/trigger", "1"))
  {
    //Success
     Serial.println("Set int data success");

  }else{
    //Failed?, get the error reason from fbdo

    Serial.print("Error in setInt, ");
    Serial.println(fbdo.errorReason());
  }

if(Firebase.pushString(fbdo, "/LED_Status", "2"))
  {
    //Success
     Serial.println("Set int data success");

  }else{
    //Failed?, get the error reason from fbdo

    Serial.print("Error in setInt, ");
    Serial.println(fbdo.errorReason());
  }
  } 
}
void readFlex() 
{
// Read the ADC, and calculate voltage and resistance from it
int flexADC=analogRead(FLEX_PIN);
float flexV=flexADC* VCC / 1023.0;
float flexR= R_DIV * (VCC / flexV-1.0);
Serial.println("Resistance: "+String(flexR) +" ohms");

// Use the calculated resistance to estimate the sensor's
// bend angle:
float angle = map(flexR, STRAIGHT_RESISTANCE, BEND_RESISTANCE,
0, 90.0);
Serial.println("Bend: "+String(angle) +" degrees");
Serial.println();

  delay(500);
}
void getAccelValues() {

  Serial.println("MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  Serial.println("");
  delay(100);
}
void printAccelValues() {
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* Print out the values */
  Serial.print("Acceleration X: ");
  Serial.print(a.acceleration.x);
  Serial.print(", Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(", Z: ");
  Serial.print(a.acceleration.z);
  Serial.println(" m/s^2");

  Serial.print("Rotation X: ");
  Serial.print(g.gyro.x);
  Serial.print(", Y: ");
  Serial.print(g.gyro.y);
  Serial.print(", Z: ");
  Serial.print(g.gyro.z);
  Serial.println(" rad/s");

//  Serial.print("Temperature: ");
//  Serial.print(temp.temperature);
//  Serial.println(" degC");

  Serial.println("");
  delay(500);
}
void environmentCheck() 
{
  float sensorVoltage; 
  float sensorValue;
 
  sensorValue = analogRead(A0);
  sensorVoltage = sensorValue/1024*5.0;
 
  Serial.print("sensor voltage = ");
  Serial.print(sensorVoltage);
  Serial.println(" V");
  delay(1000);
}
void rotarySetup() {
  r.setChangedHandler(rotate);
  r.setLeftRotationHandler(showDirection);
  r.setRightRotationHandler(showDirection);

  b.setTapHandler(click);
  b.setLongClickHandler(resetPosition);
}

void rotaryUpdates() {
  Serial.println(r.directionToString(r.getDirection()));
  Serial.println(r.getPosition());
}

void firebaseUpdate(String a) {
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  if(Firebase.setString(fbdo, "/LED_Status", a))
  {
    //Success
     Serial.println("Set int data success");

  }else{
    //Failed?, get the error reason from fbdo

    Serial.print("Error in setInt, ");
    Serial.println(fbdo.errorReason());
  }

if(Firebase.pushString(fbdo, "/LED_Status", a))
  {
    //Success
     Serial.println("Set int data success");

  }else{
    //Failed?, get the error reason from fbdo

    Serial.print("Error in setInt, ");
    Serial.println(fbdo.errorReason());
  }

//  if(Firebase.getInt(fbdo, "/LED_Status"))
//  {
//    //Success
//    Serial.print("Get int data success, int = ");
//    Serial.println(fbdo.intData());
//
//  }else{
//    //Failed?, get the error reason from fbdo
//
//    Serial.print("Error in getInt, ");
//    Serial.println(fbdo.errorReason());
//  }

// if(Firebase.get(fbdo, "/LED_Status"))
//  {
//    //Success
//    Serial.print("Get variant data success, type = ");
//    Serial.println(fbdo.dataType());
//
//    if(fbdo.dataType() == "int"){
//      Serial.print("data = ");
//      Serial.println(fbdo.intData());
//    }else if(fbdo.dataType() == "bool"){
//      if(fbdo.boolData())
//        Serial.println("data = true");
//      else
//        Serial.println("data = false");
//    }

//  }else{
//
//    Serial.print("Error in get, ");
//    Serial.println(fbdo.errorReason());
//  }

}

void firebaseUpdateSet(String a) {
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  if(Firebase.setString(fbdo, "/LED_Status", a))
  {
    //Success
     Serial.println("Set int data success");

  }else{
    //Failed?, get the error reason from fbdo

    Serial.print("Error in setInt, ");
    Serial.println(fbdo.errorReason());
  }

//  if(Firebase.getInt(fbdo, "/LED_Status"))
//  {
//    //Success
//    Serial.print("Get int data success, int = ");
//    Serial.println(fbdo.intData());
//
//  }else{
//    //Failed?, get the error reason from fbdo
//
//    Serial.print("Error in getInt, ");
//    Serial.println(fbdo.errorReason());
//  }

// if(Firebase.get(fbdo, "/LED_Status"))
//  {
//    //Success
//    Serial.print("Get variant data success, type = ");
//    Serial.println(fbdo.dataType());
//
//    if(fbdo.dataType() == "int"){
//      Serial.print("data = ");
//      Serial.println(fbdo.intData());
//    }else if(fbdo.dataType() == "bool"){
//      if(fbdo.boolData())
//        Serial.println("data = true");
//      else
//        Serial.println("data = false");
//    }

//  }else{
//
//    Serial.print("Error in get, ");
//    Serial.println(fbdo.errorReason());
//  }
}

void firebaseUpdatePush(String a) {
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

if(Firebase.pushString(fbdo, "/LED_Status", a))
  {
    //Success
     Serial.println("Set int data success");

  }else{
    //Failed?, get the error reason from fbdo

    Serial.print("Error in setInt, ");
    Serial.println(fbdo.errorReason());
  }

  if(Firebase.pushString(fbdo, "/makeHarvard/notify", "True"))
  {
    //Success
     Serial.println("Set int data success");

  }else{
    //Failed?, get the error reason from fbdo

    Serial.print("Error in setInt, ");
    Serial.println(fbdo.errorReason());
  }

//  if(Firebase.getInt(fbdo, "/LED_Status"))
//  {
//    //Success
//    Serial.print("Get int data success, int = ");
//    Serial.println(fbdo.intData());
//
//  }else{
//    //Failed?, get the error reason from fbdo
//
//    Serial.print("Error in getInt, ");
//    Serial.println(fbdo.errorReason());
//  }

// if(Firebase.get(fbdo, "/LED_Status"))
//  {
//    //Success
//    Serial.print("Get variant data success, type = ");
//    Serial.println(fbdo.dataType());
//
//    if(fbdo.dataType() == "int"){
//      Serial.print("data = ");
//      Serial.println(fbdo.intData());
//    }else if(fbdo.dataType() == "bool"){
//      if(fbdo.boolData())
//        Serial.println("data = true");
//      else
//        Serial.println("data = false");
//    }

//  }else{
//
//    Serial.print("Error in get, ");
//    Serial.println(fbdo.errorReason());
//  }

}

void WifiSetup() {

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

void rotate(ESPRotary& r) {
   Serial.println(r.getPosition());
}

// on left or right rotation
void showDirection(ESPRotary& r) {
  Serial.println(r.directionToString(r.getDirection()));
}
 
// single click
void click(Button2& btn) {
  Serial.println("Click!");
}

// long click
void resetPosition(Button2& btn) {
  r.resetPosition();
  Serial.println("Reset!");
}

// void sendTelegram() {
//       // distract();
//   if(Firebase.get(fbdo, "/Distract"))
//  {
//    //Success
//    Serial.print("Get variant data success, type = ");
//    Serial.println(fbdo.dataType());

//    if(fbdo.dataType() == "String"){
//      Serial.print("data = ");
//      Serial.println(fbdo.stringData());
//    }else if(fbdo.dataType() == "bool"){
//      if(fbdo.boolData())
//        Serial.println("data = true");
//      else
//        Serial.println("data = false");
//    }
//     }
// }