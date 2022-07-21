#include "FirebaseESP8266.h"  
#include <ESP8266WiFi.h>

#define FIREBASE_HOST "washing-machine-system-default-rtdb.firebaseio.com"  
#define FIREBASE_AUTH "cxqCWzBpjxFmecllOPH2NNP9qRponnUaofZrWdnd"

#define WIFI_SSID "N"
#define WIFI_PASSWORD "ioup0834"
  
short powerWrite = D0;      
bool runningStatus = 0;
short connectionStatus = 0;

// Measuring AC Current Using ACS712

const int sensorIn = A0;
int mVperAmp = 66; // use 100 for 20A Module and 66 for 30A Module
float Voltage = 0;
float VRMS = 0;
float AmpsRMS = 0;

//Defining FirebaseESP8266 data object
FirebaseData firebaseData;
FirebaseData powerData;
FirebaseJson json;

////////////////////////// For Current Sensor
float getVPP(){
  float result;
  int readValue;             //value read from the sensor
  int maxValue = 0;          // store max value here
  int minValue = 1024;          // store min value here
  
   uint32_t start_time = millis();
   while((millis()-start_time) < 1000) //sample for 1 Sec
   {
       readValue = analogRead(sensorIn);
       // see if you have a new maxValue
       if (readValue > maxValue) 
       {
           //record the maximum sensor value
           maxValue = readValue;
       }
       if (readValue < minValue) 
       {
           // record the minimum sensor value
           minValue = readValue;
       }
   }
   
   // Subtract min from max
   result = ((maxValue - minValue) * 5.0)/1024.0;
      
   return result;
 }
 
void setup(){
  Serial.begin(9600);
  pinMode(powerWrite,OUTPUT);
  pinMode(A0, INPUT);           // input for running status
  
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

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  ////////////////////////
  if (Firebase.setInt(firebaseData, "/WM_J/J400/powerWrite", 0))
  {
    Serial.println("PASSED");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());
    Serial.println("ETag: " + firebaseData.ETag());
    Serial.println("------------------------------------");
    Serial.println();
  }
  if (Firebase.setBool(firebaseData, "/WM_J/J400/runningStatus", 0))
  {
    Serial.println("PASSED");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());
    Serial.println("ETag: " + firebaseData.ETag());
    Serial.println("------------------------------------");
    Serial.println();
  }
  if (Firebase.setInt(firebaseData, "/WM_J/J400/connectionStatus", 0))
  {
    Serial.println("PASSED");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());
    Serial.println("ETag: " + firebaseData.ETag());
    Serial.println("------------------------------------");
    Serial.println();
  }
}

void machineUpdate(){
  if(AmpsRMS>=0.12){
  runningStatus = 1;
  }
  else{
    runningStatus = 0;
  }
  Serial.print("runningStatus : ");
  Serial.println(runningStatus);
  if (Firebase.setBool(firebaseData, "/WM_J/J400/runningStatus", runningStatus)){
    Serial.println("PASSED runningStatus");
    Serial.println();
  }
  else{
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
  if (Firebase.setInt(firebaseData, "/WM_J/J400/connectionStatus", connectionStatus)){
    Serial.println("PASSED connectionStatus");
    Serial.println();
    connectionStatus = connectionStatus +1;
    if (connectionStatus == 100){
      connectionStatus = 0;
    }
  }
  else{
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
}

void loop() {
 Voltage = getVPP();
 VRMS = (Voltage/2.0) *0.707;  //root2/ 2 is 0.707
 AmpsRMS = (VRMS * 1000 -8)/mVperAmp;
 Serial.print(AmpsRMS);
 Serial.println(" Amps RMS");
 
  machineUpdate();
  if (Firebase.getString(powerData, "/WM_J/J400/powerWrite")){
    Serial.println(powerData.stringData());
    if (powerData.stringData() == "1") {
    digitalWrite(powerWrite, HIGH);
    }
  else if (powerData.stringData() == "0"){
    digitalWrite(powerWrite, LOW);
    }
  }  
  delay(5000);
}

///////////////////////////////////////////////////////////////
