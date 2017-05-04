#include <Adafruit_NeoPixel.h>

// The PIR sensors
int pirs[] = { 34, 35 };
// The led strips
int leds[] = { 38, 39}; 
// The default pir states
int pirStates[] = {LOW, LOW};

int NUM_OF_CUBES = 2;

Adafruit_NeoPixel strips[] = {
  Adafruit_NeoPixel(22, leds[0], NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(22, leds[1], NEO_GRB + NEO_KHZ800)
};

void setup() {
  Serial.begin(9600);
  Serial.println("Initializing");
  for(int i=0;i<NUM_OF_CUBES;i++) {
    pinMode(pirs[i], INPUT);
  }
  Serial.print(NUM_OF_CUBES);
  Serial.println(" Sensors");

  for(int i=0;i<NUM_OF_CUBES;i++) {
    strips[i].begin();
    strips[i].show();
  }
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.print(NUM_OF_CUBES);
  Serial.println(" strips");
  Serial.println("Started");
}

void loop(){
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(LED_BUILTIN, LOW);
  for(int i=0;i<NUM_OF_CUBES; i++ ) {
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
      Serial.print(index);
      Serial.println(" Motion detected!");
      // We only want to print on the output change, not state
      pirStates[index] = HIGH;
     }
  } else {
    isOn = false;
    if (pirStates[index] == HIGH){
      // we have just turned of
      Serial.print(index);
      Serial.println(" Motion ended!");
      // We only want to print on the output change, not state
      pirStates[index] = LOW;
    }
  }

  if(isOn) {
    colorWipe(index, Adafruit_NeoPixel::Color(83, 100, 46), 50); // Red
  }else {
    colorWipe(index, Adafruit_NeoPixel::Color(0, 0, 0), 50); //Black
  }
}

void colorWipe(int index, uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strips[index].numPixels(); i++) {
    strips[index].setPixelColor(i, c);
    strips[index].show();
    delay(wait);
  }
}
