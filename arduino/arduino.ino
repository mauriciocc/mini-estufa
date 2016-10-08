#include <LiquidCrystal.h>
#include <EEPROM.h>
#include "data_structures.h"
#include "protocol.h"

LiquidCrystal lcd(8, 9, 4, 5, 6, 7); // 8 e 9 Pinos do backlight - 4,5,6,7 Pinos de dados

char selectedPlant = ASPLENIO;

struct Plant plants[7];

struct ControlStruct temp = {LM35, 0, 0, 999, 0, 0, 0, 0};
struct ControlStruct light = {LDR, 0, -999, 999, 0, 0, 0, 0};
struct UI ui = {0, 0};
struct Clock appClock = {0,0,0};
Keyb keyb = {0,0};

const unsigned char PWM_DUTY[] = {0, 25, 50, 75, 100, 125, 150, 175, 200, 225, 255};


char readKey;

void setup() {  

  // Limpa EEPROM caso não seja identificada app, limpa memória
  if(EEPROM.read(EEPROM_APP_ID_IDX) != APP_ID) {
    for (int i = 0 ; i < EEPROM.length() ; i++) {
      EEPROM.write(i, 0);
    }
    EEPROM.write(EEPROM_APP_ID_IDX, APP_ID);
  }
  
  Serial.begin(9600);
  while (!Serial);
  randomSeed(millis());
    
  appClock.lastUpdate = millis();
  
  pinMode(LM35, INPUT);
  pinMode(LDR, INPUT);
  
  pinMode(LED, OUTPUT);
  pinMode(FAN, OUTPUT);

  plants[ASPLENIO]        = {{18, 22}, {30000, 32000}, "Asplenio"};
  plants[AZALEIA]         = {{16, 19}, {20000, 22000}, "Azaleia"};
  plants[BROMELIA]        = {{22, 25}, {21000, 25000}, "Bromelia"};
  plants[CAMEDOREA]       = {{26, 28}, {15000, 17000}, "Camedorea"};
  plants[CAMELIA]         = {{27, 29}, {20000, 23000}, "Camelia"};
  plants[DRACENAS]        = {{22, 25}, {24000, 26000}, "Dracenas"};
  plants[MINI_SAMAMBAIA]  = {{22, 25}, {20000, 22000}, "Samambaia"};

  lcd.begin(16, 2);
  lcd.clear();
  
  //appClock.h = EEPROM.read(EEPROM_CLOCK_H);
  //appClock.m = EEPROM.read(EEPROM_CLOCK_M);
}

boolean systemNeedToOperate() {
  if(appClock.h >= 7 && (appClock.h <= 18 && appClock.m <= 59) ) {
    return true;    
  } else {
    return false;
  }
}

int toLux(int rawReading){  
  /*
   * VS = (5*1k)/(Rx+1k)
   */
  rawReading = 1023 - rawReading; //return rawReading;
  double Vstep = (5.0/1024.0);
  double Vs = Vstep * (double)rawReading;
  double Rx = 5000/1000*Vs;
  unsigned int lux = (500.0/Rx);//*1000;
  return lux;//*250;
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
  if(!systemNeedToOperate()) {
    ctrlStruct->currentPwm = 0;
    analogWrite(pin, 0);
    return;
  }
  
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

    boolean isAbove = isAboveLimit(temp.value, &(plants[selectedPlant].temp));
    adjustPwm(FAN, isAbove, &temp);    
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

    boolean isBelow = isBelowLimit(light.value, &(plants[selectedPlant].lux));
    adjustPwm(LED, isBelow, &light);
  }
}

void refreshLCD(unsigned long cTime) {
  if((cTime - ui.lastUpdate) > UI_REFRESH_TIME) { 
    ui.lastUpdate = cTime;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(plants[selectedPlant].plantName);      
    lcd.setCursor(11,0);
    char* t = (char*) malloc(64);
    sprintf(t, "%.2d:%.2d",appClock.h, appClock.m); 
    lcd.print(t);  
    free(t);
    lcd.setCursor(0,1);
    lcd.print("T:"+ String(temp.value)+"C "+String(temp.currentPwm));
    lcd.print(" L:"+ String(light.value)+" "+String(light.currentPwm));
  }
}

void incrementHour() {
  if(appClock.h == 23) {
    appClock.h = 0;
  } else {
    appClock.h++;
  }
}

void decrementHour() {
  if(appClock.h == 0) {
    appClock.h = 23;
  } else {
    appClock.h--;
  }
}

void incrementMinute() {
  if(appClock.m == 59) {
    appClock.m = 0;
    incrementHour();
  } else {
    appClock.m++;
  }
}

void decrementMinute() {
  if(appClock.m == 0) {
    appClock.m = 59;
    decrementHour();
  } else {
    appClock.m--;
  }
}

void updateClock(unsigned long cTime) {
  if((cTime - appClock.lastUpdate) > CLOCK_MIN_TICK) {
    appClock.lastUpdate = cTime;
    incrementMinute();
  }
}

char readKeyboard(unsigned long cTime) {
  char key = NONE;

  Serial.println("millis: "+ String(cTime));
  Serial.println("lastRead: "+ String(keyb.lastRead));
  
  if(cTime < keyb.lastRead) {
    return key;
  }
  
  int val = analogRead(KEYBOARD_PORT);
  
  if (val < 100) {
    key = RIGHT;
  } else if (val < 200) {
    key = UP;
  } else if (val < 400) {
    key = DOWN;
  } else if (val < 600) {
    key = LEFT;
  } else if (val < 800) {
    key = SELECT;
  }
  
  if(key != NONE) {
    keyb.lastRead = cTime+KEY_READ_TIME;
  }
  
  return key;
}

void nextPlant() {
  if(selectedPlant < 6 ) {
    selectedPlant++;
  } else {
    selectedPlant = 0;
  }
}
void loop() {
  unsigned long cTime = millis();  
  updateClock(cTime);
  temperatureSensor(cTime);
  lightSensor(cTime);
  refreshLCD(cTime);
  
  switch(readKeyboard(cTime)) {
    case SELECT: {
      nextPlant();
      break;
    }
    case UP: {
      incrementHour();      
      break;
    }
    case DOWN: {
      decrementHour();      
      break;
    }
    case RIGHT: {
      incrementMinute();      
      break;
    }
    case LEFT: {
      decrementMinute();      
      break;
    }
  }
}
