// Reads
#define LM35    1
#define LDR     2

// Writes
#define LED     44 
#define FAN     46

#define SAMPLES 10

typedef struct ControlStruct {
  char sensor;
  int value;  
  int maxVal;
  int minVal;
  long unsigned lastRead;
  int sampleCount;
  int sampleTemp;
} ControlStruct;



struct ControlStruct temp = {LM35, 0, -999, 999, 0, 0, 0};
struct ControlStruct light = {LDR, 0, -999, 999, 0, 0, 0};

void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(FAN, OUTPUT);
}

void maxMin(struct ControlStruct* ctrlStruct) {
    if(ctrlStruct->value > ctrlStruct->maxVal) {
      ctrlStruct->maxVal = ctrlStruct->value;
    }   
    if(ctrlStruct->value < ctrlStruct->minVal) {
      ctrlStruct->minVal = ctrlStruct->value;
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
