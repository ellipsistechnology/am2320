#include <Wire.h>
//#include <AM2320.h>
// 
//AM2320 th;

#define AM2320_address (0xB8 >> 1) 

float t;
float h;

unsigned int CRC16(byte *ptr, byte length) 
{ 
      unsigned int crc = 0xFFFF; 
      uint8_t s = 0x00; 

      while(length--) {
        crc ^= *ptr++; 
        for(s = 0; s < 8; s++) {
          if((crc & 0x01) != 0) {
            crc >>= 1; 
            crc ^= 0xA001; 
          } else crc >>= 1; 
        } 
      } 
      return crc; 
} 

int Read()
{
  byte buf[8];
  for(int s = 0; s < 8; s++) buf[s] = 0x00; 

  Wire.beginTransmission(AM2320_address);
  Wire.endTransmission();
  // запрос 4 байт (температуры и влажности)
  Wire.beginTransmission(AM2320_address);
  Wire.write(0x03);// запрос
  Wire.write(0x00); // с 0-го адреса
  Wire.write(0x04); // 4 байта
  int response = Wire.endTransmission(1);
  Serial.println(response);
  if (response != 0) return 1;
  delayMicroseconds(1600); //>1.5ms
  // считываем результаты запроса
  Wire.requestFrom(AM2320_address, 0x08); 
  for (int i = 0; i < 0x08; i++) buf[i] = Wire.read();

  // CRC check
  unsigned int Rcrc = buf[7] << 8;
  Rcrc += buf[6];
  if (Rcrc == CRC16(buf, 6)) {
    unsigned int temperature = ((buf[4] & 0x7F) << 8) + buf[5];
    t = temperature / 10.0;
    t = ((buf[4] & 0x80) >> 7) == 1 ? t * (-1) : t;

    unsigned int humidity = (buf[2] << 8) + buf[3];
    h = humidity / 10.0;
    return 0;
  }
        return 2;
}
 
void setup() {
Serial.begin(9600);
Wire.begin(2, 14);
}
 
void loop() {
Serial.println("Light:");
Serial.println(analogRead(A0));
Serial.println("Chip = AM2320");
switch(Read()) {
case 2:
Serial.println(" CRC failed");
break;
case 1:
Serial.println(" Sensor offline");
break;
case 0:
Serial.print(" Humidity = ");
Serial.print(h);
Serial.println("%");
Serial.print(" Temperature = ");
Serial.print(t);
Serial.println("*C");
Serial.println();
break;
}
delay(2000);
}
