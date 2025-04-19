/*
  Pixie reads data in at 115.2k serial, 8N1.
  Byte order is R1, G1, B1, R2, G2, B2, ... where the first triplet is the
  color of the LED that's closest to the controller. 1ms of silence triggers
  latch. 2 seconds silence (or overheating) triggers LED off (for safety).

  Do not look into Pixie with remaining eye!
*/

#include "Adafruit_Pixie.h"
#include <Adafruit_NeoPixel.h>
#include "SoftwareSerial.h"

#define PIXIEPINRIGHT 12  // Pin number 4 Right led wings
#define PIXIEPINLEFT 6    // Pin number 4 Left led wings
#define PIXIEPINCENTER 8  // Pin for Center led
#define NUMPIXELS 4  // Number of Pixies in the strip

uint8_t range[NUMPIXELS] = { 0, 1, 2, 3 };
// INPUT: Potentiometer should be connected to 3.3V and GND
int potPin = 9;       // Potentiometer output connected to analog pin 9
int led = 7;          // Single center LED
uint16_t potVal = 0;  // Variable to store the input from the potentiometer
uint8_t temppotVal = 0;

uint8_t redVal = 0;
uint8_t grnVal = 0;
uint8_t bluVal = 0;

SoftwareSerial pixieSerialRight(-1, PIXIEPINRIGHT);
SoftwareSerial pixieSerialLeft(-1, PIXIEPINLEFT);

Adafruit_Pixie stripLeft = Adafruit_Pixie(NUMPIXELS, &pixieSerialLeft);
Adafruit_Pixie stripRight = Adafruit_Pixie(NUMPIXELS, &pixieSerialRight);
Adafruit_NeoPixel stripCenter = Adafruit_NeoPixel(1, PIXIEPINCENTER, NEO_GRB + NEO_KHZ800);


void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(9600);

  pixieSerialRight.begin(115200);  // Pixie REQUIRES this baud rate
  pixieSerialLeft.begin(115200);   // Pixie REQUIRES this baud rate
  stripCenter.begin();             

  stripRight.setBrightness(50);  // Adjust as necessary to avoid blinding
  stripLeft.setBrightness(50);
  stripCenter.setBrightness(50);

  stripRight.show();   // Initialize all pixels to 'off'
  stripLeft.show();    // Initialize all pixels to 'off'
  stripCenter.show();  // Initialize all pixels to 'off'
}

void loop() {
  potVal = analogRead(potPin);  // read the potentiometer value at the input pin
  int temppotVal = potVal / 4;  // Normalize to 0-255

  bluVal = (255 - temppotVal);
  redVal = (0 + temppotVal);

 if (temppotVal < 64){  
    bluVal = (-3 * temppotVal)+ 254;
    redVal = 0;

  } else if (temppotVal < 96){
    bluVal = (8 * temppotVal) -511;
    redVal = (8 * temppotVal) -511;

  } else if (temppotVal > 96 and temppotVal < 160){ //middle
    bluVal = 255;
    redVal = 255;

  } else if (temppotVal < 192){
    bluVal = (-8 * temppotVal) +1535;
    redVal = (-8 * temppotVal) +1535;

  } else if (temppotVal < 256){ 
    bluVal = 0;
    redVal = (3 * temppotVal)- 254;
  } 

  set_all_leds(range, redVal, grnVal, bluVal);
}

void set_all_leds(uint8_t led_range[], uint8_t redVal, uint8_t grnVal, uint8_t bluVal) {
  // It flashes if we input 128, & 64. Also a few other numbers, not sure why
  switch (redVal) {
    case 164: redVal = 162; break;
    case 163: redVal = 162; break;
    case 128: redVal = 127; break;
    case 64: redVal = 63; break;
    default: break;
  }
  switch (bluVal) {
    case 164: bluVal = 162; break;
    case 163: bluVal = 162; break;
    case 128: bluVal = 127; break;
    case 64: bluVal = 63; break;
    default: break;
  }
  switch (grnVal) {
    case 164: grnVal = 162; break;
    case 163: grnVal = 162; break;
    case 128: grnVal = 127; break;
    case 64: grnVal = 63; break;
    default: break;
  }
  Serial.print(bluVal);Serial.print(",");
  Serial.print(redVal);Serial.print(",");
  Serial.print(temppotVal);Serial.print(",");
  Serial.println(potVal);
  for (int i = 0; i < NUMPIXELS; i++) {  // for each element in the array
    stripRight.setPixelColor(i, redVal, grnVal, bluVal);
    stripLeft.setPixelColor(i, redVal, grnVal, bluVal);
  }
  stripCenter.setPixelColor(0, redVal, grnVal, bluVal);
  stripRight.show();
  stripLeft.show();
  stripCenter.show();
  delay(150);
}
