#include "data_structures.h"
#include "protocol.h"


char selectedPlant = ASPLENIO;

struct Plant plants[7];

struct ControlStruct temp = {LM35, 0, 0, 999, 0, 0, 0, 0};
struct ControlStruct light = {LDR, 0, -999, 999, 0, 0, 0, 0};

const unsigned char PWM_DUTY[] = {0, 20, 51, 80, 100, 127, 142, 168, 192, 215, 255};

void setup() {  
  Serial.begin(9600);
  pinMode(LM35, INPUT);
  pinMode(LDR, INPUT);
  
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


int toLux(int rawReading){  
  /*
   * VS = (5*1k)/(Rx+1k)
   */
  rawReading = 1023 - rawReading; //return rawReading;
  double Vstep = (5.0/1024.0);
  double Vs = Vstep * (double)rawReading;
  double Rx = 5000/1000*Vs;
  unsigned int lux = (500.0/Rx)*1000;
  return lux*250;
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

boolean isAboveLimit(int val, struct Range* limit) {
  return val > limit->maxVal;
}

boolean isBelowLimit(int val, struct Range* limit) {
  return val < limit->minVal;
}

void adjustPwm(char pin, boolean condition, struct ControlStruct* ctrlStruct) {
  unsigned char currentPwm = ctrlStruct->currentPwm;
  if(condition) {
      if(currentPwm < PWM_DUTY_MODES) {
        ctrlStruct->currentPwm++;        
      }
  } else {
    if(currentPwm > 0) {
        ctrlStruct->currentPwm--;
    }
  }
  analogWrite(pin, PWM_DUTY[ctrlStruct->currentPwm]); 
}

void temperatureSensor(unsigned long cTime) {
  if((cTime - temp.lastRead) > T_SAMPLE) { 
    /*Serial.print(" LM35 - : ");
    Serial.println(analogRead(LM35),DEC);    */
    
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
    Serial.print("  Stage: ");
    Serial.print(temp.currentPwm,DEC);
    Serial.print(" Min : ");
    Serial.print(temp.minVal,DEC);
    Serial.print("  Max: ");
    Serial.println(temp.maxVal,DEC);    

    adjustPwm(
      FAN,
      isAboveLimit(temp.value, &(plants[selectedPlant].temp)),
      &temp
    );
    /*if(isAboveLimit(temp.value, &(plants[selectedPlant].temp))) {
      analogWrite(FAN, 255); 
    } else {
      analogWrite(FAN, 0); 
    }*/
    
  }
}

void lightSensor(unsigned long cTime) {
  if((cTime - light.lastRead) > T_SAMPLE) { 
    /*Serial.print(" LDR - : ");
    Serial.println(analogRead(LDR),DEC);*/
    light.sampleTemp += toLux(analogRead(LDR));
    light.sampleCount++;
    light.lastRead = cTime;    
  }

  if(light.sampleCount == SAMPLES) {
    light.value = light.sampleTemp/(SAMPLES+1);

    maxMin(&light);
    
    light.sampleCount = 0;    
    light.sampleTemp = 0;
    
    Serial.print(light.value,DEC);
    Serial.print(" Lux, ");
    Serial.print("  Stage: ");
    Serial.print(light.currentPwm,DEC);
    Serial.print(" Min : ");
    Serial.print(light.minVal,DEC);
    Serial.print("  Max: ");
    Serial.println(light.maxVal,DEC);     


    adjustPwm(
      LED,
      isBelowLimit(light.value, &(plants[selectedPlant].lux)),
      &light
    );
    /*if(isBelowLimit(light.value, &(plants[selectedPlant].lux))) {
      if(light.currentPwm < PWM_DUTY_MODES) {
        light.currentPwm++;
        analogWrite(LED, PWM_DUTY[light.currentPwm]); 
      }
    } else {
      if(light.currentPwm > 0) {
        light.currentPwm--;
        analogWrite(LED, PWM_DUTY[light.currentPwm]); 
      }
    }*/
  }
}

void loop() {
  unsigned long cTime = millis();  
  temperatureSensor(cTime);
  lightSensor(cTime);
}
