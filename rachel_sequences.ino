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
uint8_t normPotVal = 0;

uint8_t redVal = 0;
uint8_t grnVal = 0;
uint8_t bluVal = 0;

uint8_t tmpRedVal = 0;
uint8_t tmpGrnVal = 0;
uint8_t tmpBluVal = 0;
uint8_t mixedRedVal = redVal;
uint8_t mixedBluVal = bluVal;
float percent_bright = 0;
uint8_t fade_max = 0;
uint8_t delayx = 10;


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

  stripRight.setBrightness(200);  // Adjust as necessary to avoid blinding
  stripLeft.setBrightness(200);
  stripCenter.setBrightness(250);

  stripRight.show();   // Initialize all pixels to 'off'
  stripLeft.show();    // Initialize all pixels to 'off'
  stripCenter.show();  // Initialize all pixels to 'off'
}

void loop() {
  
  potVal = analogRead(potPin);  // read the potentiometer value at the input pin
  normPotVal = potVal / 4;  // Normalize to 0-255
  get_rgb_from_pot(normPotVal, redVal, grnVal, bluVal);

  // Some default values in case we aren't doing any pot logic:
  // redVal = (0 + normPotVal);
  // grnVal = 0;
  // bluVal = (255 - normPotVal);

  // Don't fade at min, max, & mid
  if ((normPotVal < 2 or normPotVal > 253) or (normPotVal > 120 and normPotVal < 136 )) {
    get_rgb_from_pot(normPotVal, redVal, grnVal, bluVal);

    hold_check(10);
  }
  else{
    //refresh pot values
    get_rgb_from_pot(normPotVal, redVal, grnVal, bluVal);

    // fade in
    fade_max = (255 - normPotVal);
    fade_out(fade_max);

    //refresh pot values
    get_rgb_from_pot(normPotVal, redVal, grnVal, bluVal);

    // fade out
    fade_max = (255 - normPotVal);
    fade_in(fade_max);

    //refresh pot values
    get_rgb_from_pot(normPotVal, redVal, grnVal, bluVal);

    // hold
    hold_check(10);
    // set_all_leds(range, redVal, grnVal, bluVal);
    // delay(delayx*10);

  }

}

void hold_check(int iterations) {
  // hold color but check for pot updates so color changes:
  for (int i = 0; i <= iterations; i+=1){
  Serial.print(iterations);Serial.print(",");
  Serial.println("testing2");
    get_rgb_from_pot(normPotVal, redVal, grnVal, bluVal);

    set_all_leds(range, redVal, grnVal, bluVal);
    delay(delayx);
    if (!((normPotVal < 2 or normPotVal > 253) or (normPotVal > 120 and normPotVal < 136 ))) {
      break;     
    }
  }
}

void fade_out(uint8_t fade_max){
  for(int fadeValue = fade_max ; fadeValue >= 0; fadeValue -=1) { 
    get_rgb_from_pot(normPotVal, redVal, grnVal, bluVal);
    if ((normPotVal < 2 or normPotVal > 253) or (normPotVal > 120 and normPotVal < 136 )) {
      break;     
    }

    fade_max = (255 - normPotVal);
    percent_bright = (fadeValue*100)/fade_max;

    tmpRedVal = (redVal * percent_bright)/100;
    tmpGrnVal = (grnVal * percent_bright)/100;
    tmpBluVal = (bluVal * percent_bright)/100; 
    mixedBluVal = tmpBluVal;
    mixedRedVal = tmpRedVal;
    set_all_leds(range, tmpRedVal, tmpGrnVal, tmpBluVal);
    delay(delayx);                            
  } 
}
void fade_in(uint8_t fade_max){
    for(int fadeValue = 1 ; fadeValue <= fade_max; fadeValue +=1) { 
      get_rgb_from_pot(normPotVal, redVal, grnVal, bluVal);
      if ((normPotVal < 2 or normPotVal > 253) or (normPotVal > 120 and normPotVal < 136 )) {
        break;     
      }

      fade_max = (255 - normPotVal);
      percent_bright = (fadeValue*100)/fade_max;

      tmpRedVal = (redVal * percent_bright)/100;
      tmpGrnVal = (grnVal * percent_bright)/100;
      tmpBluVal = (bluVal * percent_bright)/100;
      mixedBluVal = tmpBluVal;
      mixedRedVal = tmpRedVal;
      set_all_leds(range, tmpRedVal, tmpGrnVal, tmpBluVal);
      delay(delayx);                            
      } 
}



void get_rgb_from_pot(uint8_t & normPotVal, uint8_t & redVal, uint8_t & grnVal, uint8_t & bluVal){
  potVal = analogRead(potPin);
  normPotVal = potVal / 4; 
  if (normPotVal < 64){  
    redVal = 0;
    grnVal = (-3 * normPotVal)+ 254;
    bluVal = 0;
    mixedRedVal = redVal;
    mixedBluVal = bluVal;

  } else if (normPotVal < 96){
    redVal = (8 * normPotVal) -511;
    grnVal = (8 * normPotVal) -511;
    bluVal = (8 * normPotVal) -511;
    mixedRedVal = 0;
    mixedBluVal = 0;

  } else if (normPotVal > 96 and normPotVal < 160){ //middle
    redVal = 255;
    grnVal = 255;
    bluVal = 255;
    mixedRedVal = 0;
    mixedBluVal = 0;

  } else if (normPotVal < 192){
    redVal = (-8 * normPotVal) +1535;
    grnVal = (-8 * normPotVal) +1535;
    bluVal = (-8 * normPotVal) +1535;
    mixedRedVal = 0;
    mixedBluVal = 0;

  } else if (normPotVal < 256){ 
    redVal = 0;
    grnVal = (3 * normPotVal)- 254;
    bluVal = (3 * normPotVal)- 254;
    mixedRedVal = redVal;
    mixedBluVal = bluVal;
  } 
}

void set_all_leds(uint8_t led_range[], uint8_t redVal, uint8_t grnVal, uint8_t bluVal) {
  // It flashes if we input 128, & 64.
  // Also a few other numbers, not sure why. 
  // Seems to have something to do with brightness setting?
  // Additional numbers found during fade_in & fade_out /"breath cycle"
  switch (redVal) {
    case 164: redVal = 162; break;
    case 163: redVal = 162; break;
    case 128: redVal = 127; break;
    case 111: redVal = 108; break;
    case 110: redVal = 108; break;
    case 109: redVal = 108; break;
    case 82:  redVal = 80;  break;
    case 64:  redVal = 63;  break;
    default: break;
  }
  switch (grnVal) {
    case 164: grnVal = 162; break;
    case 163: grnVal = 162; break;
    case 128: grnVal = 127; break;
    case 111: grnVal = 108; break;
    case 110: grnVal = 108; break;
    case 109: grnVal = 108; break;
    case 82:  grnVal = 80;  break;
    case 64:  grnVal = 63;  break;
    default: break;
  }
  switch (bluVal) {
    case 164: bluVal = 162; break;
    case 163: bluVal = 162; break;
    case 128: bluVal = 127; break;
    case 111: bluVal = 108; break;
    case 110: bluVal = 108; break;
    case 109: bluVal = 108; break;
    case 82:  bluVal = 80;  break;
    case 64:  bluVal = 63;  break;
    default: break;
  }
  Serial.print(redVal);Serial.print(",");
  Serial.print(grnVal);Serial.print(",");
  Serial.print(bluVal);Serial.print(",");
  Serial.print(normPotVal);Serial.print(",");
  Serial.println(potVal);
  for (int i = 0; i < NUMPIXELS; i+=2) {  // for each element in the array
    stripRight.setPixelColor(i, redVal, grnVal, bluVal);
    stripLeft.setPixelColor(i, redVal, grnVal, bluVal);
  }
  for (int i = 1; i < NUMPIXELS; i+=2) {  // for each element in the array
    stripRight.setPixelColor(i, mixedRedVal, grnVal, mixedBluVal);
    stripLeft.setPixelColor(i, mixedRedVal, grnVal, mixedBluVal);
  }
  stripCenter.setPixelColor(0, redVal, grnVal, bluVal);
  stripRight.show();
  stripLeft.show();
  stripCenter.show();
}
