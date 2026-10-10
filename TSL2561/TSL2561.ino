#include <Wire.h>
#include "TSL2561.h"

TSL2561 tsl(TSL2561_ADDR_FLOAT);

void setup(void) {
  Serial.begin(9600);

  if (tsl.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No sensor?");
    while (1);
  }

  tsl.setGain(TSL2561_GAIN_16X);      


  tsl.setTiming(TSL2561_INTEGRATIONTIME_13MS);  

}

void loop(void) {

  uint16_t x = tsl.getLuminosity(TSL2561_VISIBLE);
  //uint16_t x = tsl.getLuminosity(TSL2561_FULLSPECTRUM);
  //uint16_t x = tsl.getLuminosity(TSL2561_INFRARED);

  Serial.print(x, DEC);
  Serial.print("\t");
  uint32_t lum = tsl.getFullLuminosity();
  uint16_t ir, full;
  ir = lum >> 16;
  full = lum & 0xFFFF;
  Serial.print("IR: "); Serial.print(ir);   
  Serial.print("\t\t");
  Serial.print("Full: "); Serial.print(full);   
  Serial.print("\t");
  Serial.print("Visible: "); Serial.println(full - ir);   Serial.print("\t");
}
