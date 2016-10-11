#define APP_ID 127
// Reads
#define LM35    1
#define LDR     2

// Writes
#define LED     44 
#define FAN     46

// Value Sampling
#define T_SAMPLE 50
#define SAMPLES 20

#define PWM_DUTY_MODES 9


// Data Structures
typedef struct ControlStruct {
  char sensor;
  unsigned int value;  
  int maxVal;
  int minVal;
  long unsigned lastRead;
  int sampleCount;
  unsigned long sampleTemp;
  unsigned char currentPwm;
} ControlStruct;

typedef struct Range {
  unsigned int minVal;
  unsigned int maxVal;
} Range;

typedef struct Plant {
  struct Range temp;
  struct Range lux;
  String plantName;
} Plant;

#define UI_REFRESH_TIME 250
#define VIEW_STATUS 0

typedef struct UI {
  char view;
  unsigned long lastUpdate;  
} UI;

#define CLOCK_MIN_TICK 1000

typedef struct Clock {
  char h;
  char m;
  unsigned long lastUpdate;
} Clock;

// Plants
#define TOTAL_PLANTS 7
#define ASPLENIO        0
#define AZALEIA         1
#define BROMELIA        2
#define CAMEDOREA       3
#define CAMELIA         4
#define DRACENAS        5
#define MINI_SAMAMBAIA  6


/*
  Tabela botões LCD Shield:

  1023  - 5V    - Nada
  0     - 0V    - Right
  143   - 0,7V  - Up
  329   - 1,61V - Down
  451   - 2,4V  - Left
  696   - 3.4V  - Select

*/
#define KEYBOARD_PORT 0

#define KEY_READ_TIME 500

#define LEFT    'l'
#define RIGHT   'r'
#define UP      'u'
#define DOWN    'd'
#define SELECT  's'
#define NONE     0

typedef struct Keyb {
  unsigned char btn;
  unsigned long lastRead;
} Keyb;

