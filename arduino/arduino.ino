#include "data_structures.h"
#include "protocol.h"


char selectedPlant = ASPLENIO;

struct Plant plants[7];

struct ControlStruct temp = {LM35, 0, -999, 999, 0, 0, 0};
struct ControlStruct light = {LDR, 0, -999, 999, 0, 0, 0};


void setup() {  
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(FAN, OUTPUT);

  plants[ASPLENIO]        = {{18, 22}, {30000, 32000}};
  plants[AZALEIA]         = {{16, 19}, {20000, 22000}};
  plants[BROMELIA]        = {{22, 25}, {21000, 25000}};
  plants[CAMEDOREA]       = {{26, 28}, {15000, 17000}};
  plants[CAMELIA]         = {{27, 29}, {20000, 23000}};
  plants[DRACENAS]        = {{22, 25}, {24000, 26000}};
  plants[MINI_SAMAMBAIA]  = {{22, 25}, {20000, 22000}};
}

void maxMin(struct ControlStruct* ctrlStruct) {
    int val = ctrlStruct->value;
    int maxVal = ctrlStruct->maxVal;
    
    if(val > maxVal) {
      ctrlStruct->maxVal = val;
    }   

    int minVal = ctrlStruct->minVal;
    if(val < minVal) {
      ctrlStruct->minVal = val;
    } 
}

void temperatureSensor(unsigned long cTime) {
  if((cTime - temp.lastRead) > 100) { 
    temp.sampleTemp += ( 5.0 * analogRead(LM35) * 100.0) / 1024.0;
    temp.sampleCount++;
    temp.lastRead = cTime;
  }

  if(temp.sampleCount == SAMPLES) {
    temp.value = temp.sampleTemp/(SAMPLES+1);

    maxMin(&temp);
    
    temp.sampleCount = 0;    
    temp.sampleTemp = 0;
    
    Serial.print(temp.value,DEC);
    Serial.print(" C, ");
    Serial.print(" Min : ");
    Serial.print(temp.minVal,DEC);
    Serial.print("  Max: ");
    Serial.println(temp.maxVal,DEC);    
    analogWrite(FAN, 255); 
  }
}


void lightSensor(unsigned long cTime) {
  if((cTime - light.lastRead) > 100) { 
    light.sampleTemp += analogRead(LDR);
    light.sampleCount++;
    light.lastRead = cTime;    
  }

  if(light.sampleCount == SAMPLES) {
    light.value = light.sampleTemp/(SAMPLES+1);

    maxMin(&light);
    
    light.sampleCount = 0;    
    light.sampleTemp = 0;
    
    Serial.print(light.value,DEC);
    Serial.print(" LDR, ");
    Serial.print(" Min : ");
    Serial.print(light.minVal,DEC);
    Serial.print("  Max: ");
    Serial.println(light.maxVal,DEC);     
    
    analogWrite(LED, (255 - light.value));
  }
}

void loop() {
  unsigned long cTime = millis();  
  temperatureSensor(cTime);
  lightSensor(cTime);
}