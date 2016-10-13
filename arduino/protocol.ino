word toWord(byte b1, byte b2) {
  return (b2 << 8) | b1;
}

ProtocolData* parseMsg() {
  byte bytes[PROT_SIZE];
  Serial.readBytes(bytes, 4);
  
  ProtocolData* data = (ProtocolData*) malloc(sizeof(ProtocolData));
  
  data->header = bytes[0];
  data->type = bytes[1];
  data->size = toWord(bytes[2], bytes[3]);
  
  word ptr = 4;
  if(data->size > 0) {    
    data->data = (byte*) malloc(data->size);    
    Serial.readBytes(data->data, data->size);    
  }  

  Serial.readBytes(&bytes[ptr], 2);
  
  data->checksum = toWord(bytes[ptr], bytes[ptr+1]);
  
  return data;  
}

void freeMsg(ProtocolData* pd) {
  if(pd->size > 0) {
    free(pd->data);   
  }
  free(pd);
}

byte* toBytes(ProtocolData* data) {
  word allocation = 6+data->size;
  byte* ser = (byte*) malloc(allocation);
  ser[0] = data->header;
  ser[1] = data->type;  
  ser[2] = LOBYTE(data->size);
  ser[3] = HIBYTE(data->size);
  int ptr = 4;
  // Corpo da msg
  int i = 0;
  if(data->size > 0) {
    for(; i < data->size; i++) {
      ser[i+ptr] = data->data[i];
    }
    ptr += data->size;
  }
  ser[ptr] = LOBYTE(data->checksum);
  ser[ptr+1] = HIBYTE(data->checksum);
  return ser;
}

void sendMsg(ProtocolData* data) {
  byte* bytes = toBytes(data);
  Serial.write(bytes, data->size+PROT_SIZE);
  free(bytes);
}

void sendWord(word val) {
  byte msg[2];
  msg[0] = LOBYTE(val);
  msg[1] = HIBYTE(val);
  Serial.write(msg, 2);
}

void sendByte(byte val) {  
  Serial.write(val);
}

