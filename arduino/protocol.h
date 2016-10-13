#define LOBYTE(x) ((unsigned char) ((x) & 0xff))
#define HIBYTE(x) ((unsigned char) ((x) >> 8 & 0xff))

#define PROT_SIZE 6

#define PROT_WRITE 27
#define PROT_READ  72

#define PROT_T_TEMP     1
#define PROT_T_LUX      2
#define PROT_T_PLANT    4
#define PROT_T_STATUS   8
#define PROT_T_TIME     16
#define PROT_T_HISTORY  64

typedef struct ProtocolData {
  byte header;
  byte type;
  word size;
  byte* data;
  word checksum;
} ProtocolData;
