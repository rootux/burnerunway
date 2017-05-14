#include <FastLED.h>

#define NUM_OF_CUBES 9
#define NUM_LEDS_PER_STRIP 22
#define BRIGHTNESS  30
#define TURN_CUBE_OFF_AFTER_MS 10000
#define END_OF_COURSE_ANIMATION_LENGTH 30000

#define LED_PIN_1 41
#define LED_PIN_2 42
#define LED_PIN_3 43
#define LED_PIN_4 44
#define LED_PIN_5 45
#define LED_PIN_6 46
#define LED_PIN_7 47
#define LED_PIN_8 48
#define LED_PIN_9 49

// The PIR sensor pins
int pirs[] = { 31, 32, 33, 34, 35, 36, 37, 38, 39};

#define SERIAL_SPEED 9600
#define LED_TYPE    WS2812B
#define COLOR_ORDER RGB

bool isEndOfCourseAnimation = false;
unsigned long endOfCourseAnimationStartMs = 0;


// Holds the time the pir was first turned on - The pir will turn off 10 seconds from that time
unsigned long pirTurnedOnTimeInMs[NUM_OF_CUBES];

// Gradient palette "Geek_Black_White", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/heine/tn/GeeK07.png.index.html
DEFINE_GRADIENT_PALETTE( Geek_Black_White ) {
  0, 255,255,255,0, 255,255,255,10, 0,  0,  0,
  10,   0,  0,  0, 20, 255,255,255, 20, 255,255,255,
  30,   0,  0,  0, 30,   0,  0,  0, 40, 255,255,255,
  40, 255,255,255, 50,   0,  0,  0, 50,   0,  0,  0,
  150, 255,255,255, 150, 255,255,255, 180,   0,  0,  0,
  180,   0,  0,  0, 220, 255,255,255, 220, 255,255,255,
  255,   0,  0,  0, 255,   0,  0,  0
 };

// Gradient palette "bhw1_28_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw1/tn/bhw1_28.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 32 bytes of program space.

DEFINE_GRADIENT_PALETTE( PURPLE_HAZE ) {
  0,  75,  1,221, 30, 252, 73,255, 48, 169,  0,242,
  119,   0,149,242, 170,  43,  0, 242, 206, 252, 73,255,
  232,  78, 12,214, 255,   0,149,242};


int pirStates[NUM_OF_CUBES];

CRGB leds[NUM_OF_CUBES][NUM_LEDS_PER_STRIP];

CRGBPalette16 currentPalette;
int UPDATES_PER_SECOND = 102;

void setup() {
  Serial.begin(SERIAL_SPEED);
  Serial.println("Initializing");
  setupLedStrips();
  setupPirs();
  Serial.print(NUM_OF_CUBES);
  Serial.println(" Sensors");
  Serial.print(NUM_OF_CUBES);
  Serial.println(" Strips");
  Serial.println("Started");
}

void setupLedStrips() {
  FastLED.addLeds<LED_TYPE, LED_PIN_1, COLOR_ORDER>(leds[0], NUM_LEDS_PER_STRIP).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE, LED_PIN_2, COLOR_ORDER>(leds[1], NUM_LEDS_PER_STRIP).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE, LED_PIN_3, COLOR_ORDER>(leds[2], NUM_LEDS_PER_STRIP).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE, LED_PIN_4, COLOR_ORDER>(leds[3], NUM_LEDS_PER_STRIP).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE, LED_PIN_5, COLOR_ORDER>(leds[4], NUM_LEDS_PER_STRIP).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE, LED_PIN_6, COLOR_ORDER>(leds[5], NUM_LEDS_PER_STRIP).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE, LED_PIN_7, COLOR_ORDER>(leds[6], NUM_LEDS_PER_STRIP).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE, LED_PIN_8, COLOR_ORDER>(leds[7], NUM_LEDS_PER_STRIP).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE, LED_PIN_9, COLOR_ORDER>(leds[8], NUM_LEDS_PER_STRIP).setCorrection( TypicalLEDStrip );   
}

void setupPirs() {
  for(int i=0;i< NUM_OF_CUBES;i++) {
    pirStates[i] = LOW;
    pinMode(pirs[i], INPUT);
  }
}

void loop(){
  unsigned long time = millis(); // TODO: More accurate to call it each time beforeCheckIfStripShouldGoOff but may cost more

  if(checkAndSetEndOfCourseAnimation(time)) {
    return;
  }

  // Not end of course animatino
  for(int i=0;i<NUM_OF_CUBES; i++ ) {
    checkPir(i);

    //TODO: perhaps first check all pirs and then activate led program - not sure what is better
    if(pirStates[i] == HIGH) {
      FillLEDsFromPaletteColors(Geek_Black_White, i);

      //Set cube just turned on
      if(pirTurnedOnTimeInMs[i] == 0) {
        pirTurnedOnTimeInMs[i] = millis();
      }
      
      checkIfStripShouldGoOff(time, i);
    }
  }

  // Check last pir
  if(pirStates[NUM_OF_CUBES - 1] == HIGH) {
    isEndOfCourseAnimation = true;
    endOfCourseAnimationStartMs = millis();
  }

  FastLED.show();
  //FastLED.delay(1000 / UPDATES_PER_SECOND); //TODO - This will cause a delay in receiving input
}

void checkIfStripShouldGoOff(unsigned long time, int stripIndex) {
  if(time - pirTurnedOnTimeInMs[stripIndex] > TURN_CUBE_OFF_AFTER_MS) {
    for(int i=0;i<NUM_LEDS_PER_STRIP;i++) {
      leds[stripIndex][i] = CRGB::Black;
    }
  }
}

void checkPir(int index) {
  int val = digitalRead(pirs[index]);
  if (val == HIGH) {
    if (pirStates[index] == LOW) {
      // we have just turned on
      Serial.print(index);
      Serial.println(" Motion detected!");
      // We only want to print on the output change, not state
      pirStates[index] = HIGH;
     }
  } else {
    if (pirStates[index] == HIGH){
      // we have just turned of
      Serial.print(index);
      Serial.println(" Motion ended!");
      // We only want to print on the output change, not state
      pirStates[index] = LOW;
    }
  }
}

void FillLEDsFromPaletteColors(CRGBPalette16 currentPalette, uint8_t stripIndex)
{
  uint8_t colorIndex = 1;
  
  for( int i = 0; i < NUM_LEDS_PER_STRIP; i++) {
      leds[stripIndex][i] = ColorFromPalette( currentPalette, colorIndex, BRIGHTNESS);
      colorIndex += 3; //TODO: check this
  }
}

bool checkAndSetEndOfCourseAnimation(unsigned long time) {
  if(!isEndOfCourseAnimation) {
    return false;
  }

  if( (time - endOfCourseAnimationStartMs) > END_OF_COURSE_ANIMATION_LENGTH) {
    isEndOfCourseAnimation = false;  
  }
    
  for(int i=0;i<NUM_OF_CUBES;i++) {
    FillLEDsFromPaletteColors(PURPLE_HAZE, i);
  }

  FastLED.show();
  return true;
}





