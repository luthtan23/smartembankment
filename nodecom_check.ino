#include <SoftwareSerial.h>
SoftwareSerial s(D6,D5);
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseArduino.h>
#include <Wire.h>

String apiKey = "JD7AMM8F457Z1FNP";
const char *ssid =  "JohnBrad";
const char *pass =  "abcdefgh";
#define FIREBASE_HOST "altlantaproject.firebaseio.com"
#define FIREBASE_AUTH "BCzKYhIiICymsTRDpRhtq5NfPNn2iGaZYOe4s3wf"
const char* server = "api.thingspeak.com";

int pos = 0;
float pHd;
int turbd;
WiFiClient client;

int led1;
int led2;
int salinitas;
 
void setup() 
{
       Serial.begin(115200);
       s.begin(115200);
       while (!Serial) continue;
       delay(10);
       pinMode (D1, OUTPUT);
       pinMode (D2, OUTPUT);
       pinMode (D3, OUTPUT);
       Serial.println("Connecting to ");
       WiFi.begin(ssid, pass);
      while (WiFi.status() != WL_CONNECTED) 
     {
            delay(500);
            Serial.print(".");
     }
      Serial.println("");
      Serial.println("WiFi connected");
      Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
      Firebase.setInt("harvest",2);
      Firebase.setFloat("suhu",0);
      Firebase.setFloat("turbidity",0);
      Firebase.setFloat("salinitas",0);
      Firebase.setFloat("pH",0);
      Firebase.setInt("salinitas",0);
      Firebase.setInt("lampu1",0);
      Firebase.setInt("lampu2",0);
      digitalWrite(D1, LOW);
      digitalWrite(D2, LOW);
      digitalWrite(D3, LOW);
}

void loop() 
{
  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(s);
  if (root == JsonObject::invalid())
  {
    return;
  }
  root.prettyPrintTo(Serial);
  int dataex = root["dataex"];
  int suhu = root["datatemp"];
  String suhustr = String(suhu);
  float pH = root["datapH"];
  if (pH<0){
    float pHd = 7;
  } else {
    float pHd = pH;
  }
  String pHstr = String(pHd);
  int turb = root["dataturbidity"];
  if (turb >2000){
    int turbd = 2;
  } else {
    int turbd = turb;
  }
  String turbstr = String(turb);
  salinitas = Firebase.getInt("salinitas");
  String salinitasstr = String(salinitas);
  Firebase.setInt("dataex",dataex);
  Firebase.setInt("suhu", suhu);
  Firebase.setFloat("pH", pHd);
  Firebase.setInt("turbidity", turbd);
  if (Firebase.getInt("harvest")==1){
    digitalWrite(D1, HIGH);
    delay(10000);
    digitalWrite(D1,LOW);
    Firebase.setInt("harvest",2);
  } else if (Firebase.getInt("harvest")==0) {
    digitalWrite(D1, HIGH);
    delay(10000);
    digitalWrite(D1,LOW);
    Firebase.setInt("harvest",2);        
  } else {
    digitalWrite(D1, LOW);
  }
  led1 = Firebase.getInt("lampu1");
  Serial.print(led1);
  if (led1==1){
    digitalWrite(D2,HIGH);
  } else if (led1==0){
    digitalWrite(D2,LOW);
  }
  led2 = Firebase.getInt("lampu2");
  Serial.print(led2);
  if (led2==1){
    digitalWrite(D3,HIGH);
  } else if (led2==0){
    digitalWrite(D3,LOW);
  }
  if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
                      {  
                            String postStr = apiKey;
                             postStr +="&field1=";
                             postStr += String(pHstr);
                             postStr +="&field2=";
                             postStr += String(turbstr);
                             postStr += "\r\n\r\n";
                             postStr +="&field3=";
                             postStr += String(suhustr);
                             postStr +="&field4=";
                             postStr += String(salinitasstr);
 
                             client.print("POST /update HTTP/1.1\n");
                             client.print("Host: api.thingspeak.com\n");
                             client.print("Connection: close\n");
                             client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             client.print("Content-Type: application/x-www-form-urlencoded\n");
                             client.print("Content-Length: ");
                             client.print(postStr.length());
                             client.print("\n\n");
                             client.print(postStr);

                             Serial.print("Transferdata ke Thingspeak\n");
                              
                        }
          client.stop();
}
