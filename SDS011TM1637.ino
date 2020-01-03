#include <SDS011.h>
#include <TM1637Display.h>

// Config
const int DescriptionDelayMs = 777;
const int ValueDelayMs = 1333;
const int maxPmValue = 9999;
uint8_t clearDisplay[]={
  0,0,0,0 
};
  
uint8_t pm25Desc[]={
  SEG_A | SEG_B | SEG_E | SEG_F | SEG_G,         // p
  SEG_C | SEG_E | SEG_G,   // m = n
  SEG_A | SEG_B | SEG_D | SEG_E | SEG_G | 0b10000000,  // 2.
  SEG_A | SEG_C | SEG_D | SEG_G | SEG_D | SEG_F ,  //5
  };
uint8_t pm10Desc[]={
  SEG_A | SEG_B | SEG_E | SEG_F | SEG_G,         // p
  SEG_C | SEG_E | SEG_G,   // m = n
  SEG_B | SEG_C,  // 1
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,  // 0
  };
uint8_t errorDesc[]={
  0,
  SEG_A | SEG_D | SEG_E | SEG_G | SEG_F,         // E
  SEG_G | SEG_E, // r
  SEG_G | SEG_E // r
  };
// SDS011
const int TxdPin = 3;
const int RxdPin = 2;
float p10, p25;
int sdsResultCode;
int sdsConsecutiveErrors = 0;

SDS011 my_sds;

// TM1637
const int Clk = 3;
const int Dio = 4;

TM1637Display display(Clk, Dio);


void setup()
{
  Serial.begin(9600);
  // SDS setup
  my_sds.begin(TxdPin, RxdPin);

  // display setup
  display.setBrightness(0x0D);
}

void loop()
{
  
  sdsResultCode = my_sds.read(&p25, &p10);
  if (sdsResultCode)
  {
    ++sdsConsecutiveErrors;
    Serial.println("SDS Error. Pm 2.5: " + String(p25) + " Pm 10:  " + String(p10));
  }
  else
  {
    sdsConsecutiveErrors = 0;
    Serial.println("P2.5: " + String(p25));
    Serial.println("P10:  " + String(p10));
  }
  if (p25 >= maxPmValue)
  {
    p25 = maxPmValue;
  }
  if (p10 >= maxPmValue)
  {
    p10 = maxPmValue;
  }
  if (sdsConsecutiveErrors < 10)
  {
    display.setSegments(pm25Desc);
    delay(DescriptionDelayMs);
    display.showNumberDecEx(p25, false, 4, 0);
    delay(ValueDelayMs);
    display.setSegments(pm10Desc);
    delay(DescriptionDelayMs);
    display.showNumberDecEx(p10, false, 4, 0);
    delay(ValueDelayMs);
  }
  else
  {
    Serial.println("SDS " + String(sdsConsecutiveErrors) + " errors in a row");
    display.setSegments(errorDesc);
    delay(ValueDelayMs);
  }
}
