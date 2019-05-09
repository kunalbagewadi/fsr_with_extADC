#include <Wire.h>
#include "Adafruit_ADS1015.h"

// Scale 12-bit value to 10-bit and scale for full-scale of 12.288V to 5V.
// We do this because Arduino ADC values are with 10-bit ADC and 5V full-scale
// and ADS1015 ADC chip is 12-bit with 12.288 V full-scale because ads.setGain(GAIN_TWOTHIRDS) by default
#define SCALING 0.6144  // (12.288/4096) * (1024/5)

#define FSR_SIGNATURE 0xA7
#define DATA_LENGTH 25 //17
byte inData[DATA_LENGTH];
int iter = 0;

Adafruit_ADS1015 ads;     /* Use this for the 12-bit version */
int16_t seq=1;

void setup(void) 
{
  Serial.begin(115200);
  //Serial.println("Hello!");
  
  //Serial.println("Getting single-ended readings from AIN0..3");
  // Serial.println("ADC Range: +/- 6.144V (1 bit = 3mV/ADS1015, 0.1875mV/ADS1115)");
  
  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
   //ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  
   //ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV   
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
  
  ads.begin();
  
 // Serial.print("Setup done! Scaling= "); Serial.println(SCALING);
}
//*********************************** main loop to write data into serial port 1************************

void loop(){
 // read smart shoe data and send through serial port 1
    ssRead(iter);
    Serial.write(inData,DATA_LENGTH);
    iter++;
    delay(50);  // 20Hz
}

// *************** subfunction for reading FSR data **************************************

void ssRead(int iter) 
{
  int fsr0 = analogRead(0);
  int fsr1 = analogRead(1); 
  int fsr2 = analogRead(2);
  int fsr3 = analogRead(3);
  int fsr4 = analogRead(4);
  int fsr5 = analogRead(5); 
  
  int16_t adc0, adc1, adc2, adc3;

// Read from external ADC
  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  adc2 = ads.readADC_SingleEnded(2);
  adc3 = ads.readADC_SingleEnded(3);

  int fsr6 = adc0* SCALING;
  int fsr7 = adc1* SCALING;
  int fsr8 = adc2* SCALING;
  int fsr9 = adc3* SCALING;
/*
  Serial.println(seq++);
  Serial.print("AIN0: "); Serial.println(adc0);
  Serial.print("AIN1: "); Serial.println(adc1);
  Serial.print("AIN2: "); Serial.println(adc2);
  Serial.print("AIN3: "); Serial.println(adc3);


// Read from A0 channel
  fsrReading = analogRead(fsrPin);
  Serial.print("A0 reading = ");
  Serial.println(fsrReading);     // raw analog reading
  Serial.println(" ");
*/
int rawRead[] = {fsr0, fsr1, fsr2, fsr3, fsr4, fsr5,fsr6, fsr7, fsr8, fsr9};
  inData[0] = 0xFF;
  inData[1] = 0xFF;
  inData[2] = FSR_SIGNATURE;
  inData[3] = highByte(iter);
  inData[4] = lowByte(iter);
  for (int j = 0; j < 10; j++){   // 10= #ints in rawRead[]
    inData[5+2*j] = highByte(rawRead[j]);
    inData[6+2*j] = lowByte(rawRead[j]);
  }

}
