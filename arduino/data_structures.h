// Reads
#define LM35    1
#define LDR     2

// Writes
#define LED     44 
#define FAN     46

// Value Sampling
#define SAMPLES 10


// Data Structures
typedef struct ControlStruct {
  char sensor;
  int value;  
  int maxVal;
  int minVal;
  long unsigned lastRead;
  int sampleCount;
  int sampleTemp;
} ControlStruct;

typedef struct Range {
  unsigned int minVal;
  unsigned int maxVal;
} Range;

typedef struct Plant {
  struct Range temp;
  struct Range lux;
} Plant;

// Plants
#define ASPLENIO        0
#define AZALEIA         1
#define BROMELIA        2
#define CAMEDOREA       3
#define CAMELIA         4
#define DRACENAS        5
#define MINI_SAMAMBAIA  6
