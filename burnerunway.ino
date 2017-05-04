#include <Adafruit_NeoPixel.h>

// The PIR sensors
int pirs[] = { 34, 35 };
// The led strips
int leds[] = { 38, 39};
// The default pir states
int pirStates[] = {LOW, LOW};

Adafruit_NeoPixel strips[] = {
  Adafruit_NeoPixel(22, leds[0], NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(22, leds[1], NEO_GRB + NEO_KHZ800)
};

void setup() {
  for(int i=0;i<sizeof(pirs);i++) {
    pinMode(pirs[i], INPUT);
  }

  for(int i=0;i<sizeof(strips);i++) {
    strips[i].begin();
    strips[i].show();
  }
  
  Serial.begin(9600);
  Serial.println("Started");
}

void loop(){
  for(int i=0;i<sizeof(pirs); i++ ) {
    checkPir(i);
  }
}

void checkPir(int index) {
  bool isOn;
  int val = digitalRead(pirs[index]);
  if (val == HIGH) {
    isOn = true;
    if (pirStates[index] == LOW) {
      // we have just turned on
      Serial.println("Motion detected!");
      // We only want to print on the output change, not state
      pirStates[index] = HIGH;
     }
  } else {
    isOn = false;
    if (pirStates[index] == HIGH){
      // we have just turned of
      Serial.println("Motion ended!");
      // We only want to print on the output change, not state
      pirStates[index] = LOW;
    }
  }

  if(isOn) {
    colorWipe(strips[index], Adafruit_NeoPixel::Color(83, 100, 46), 50); // Red
  }else {
    colorWipe(strips[index], Adafruit_NeoPixel::Color(0, 0, 0), 50); //Black
  }
}

void colorWipe(Adafruit_NeoPixel strip, uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}
