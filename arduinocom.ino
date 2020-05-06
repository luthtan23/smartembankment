#include <SoftwareSerial.h>
#include <ArduinoJson.h>
SoftwareSerial s(5,6);

#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 2

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

int pinSensor = A0;
float tegangan;
float ntu;
float Po1;

//Ph
const int phSensorPin  = A1; //membuat variabel pH
float Po = 0;

float temp;
float ntu1;
void setup() {
  s.begin(115200);
   pinMode (phSensorPin, INPUT);
  sensors.begin();  // Start up the library
  delay(1000);
}
 
void loop() {
  StaticJsonBuffer<1000> jsonBuffer;
 JsonObject& root = jsonBuffer.createObject();
 sensors.requestTemperatures(); 

  Serial.print("Temperature: ");
  Serial.print(sensors.getTempCByIndex(0));
  temp = sensors.getTempCByIndex(0);
  Serial.print("C  ");
  
    tegangan = 00;
    for(int i=00; i<800; i++)
    {
        tegangan += ((float)analogRead(pinSensor)/1023)*5;
    }
    tegangan = tegangan/800;
    tegangan = round_to_dp(tegangan,1);
 
    if(tegangan < 2.5){
      ntu = 3000;
      }
    else{
      ntu = -1120.4*square(tegangan)+5742.3*tegangan-4353.8;
      }
    Serial.print("ADC ");
    Serial.println(tegangan);
    Serial.print("NTU ");
    Serial.println(ntu);

   int nilaiPengukuranPh = analogRead(phSensorPin);
   Serial.print("Nilai ADC Ph: ");
   Serial.println(nilaiPengukuranPh);
   double TeganganPh = 5 / 1024.0 * nilaiPengukuranPh;
   Serial.print("TeganganPh: ");
   Serial.println(TeganganPh, 3);
   Po = 6.86 + ((3.8 - TeganganPh) / 0.14);
   Serial.print("Nilai PH cairan: ");
   Serial.println(Po, 3);
   if (Po <0){
    Po1 = 7;
  } else {
    Po1 = Po;
  }
  if (ntu>2000){
    ntu1 = 2;
  } else {
    ntu1 = ntu;
  }
  root["datatemp"] = sensors.getTempCByIndex(0);
  root["datapH"] = Po1;
  root["dataturbidity"] = ntu1;
  root["dataex"] = 100;
  
if(s.available()>0)
  {
    root.printTo(s);
  }
}

float round_to_dp( float nilaibaca, int desimal)
{
  float multiplier = powf( 10.0f, desimal );
  nilaibaca = roundf( nilaibaca * multiplier ) / multiplier;
  return nilaibaca;
  Serial.println(nilaibaca);
}
