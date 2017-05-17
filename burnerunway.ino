#include <FastLED.h>

//Must be an even number. If you change it make sure you change mapLedToPir
#define NUM_OF_CUBES 10
#define NUM_LEDS_PER_STRIP 15
#define NUM_OF_LEDS_PER_SIDE (NUM_OF_CUBES/2)
#define BRIGHTNESS  90
#define TURN_CUBE_OFF_AFTER_MS 7000
#define END_OF_COURSE_ANIMATION_LENGTH 12000
#define END_OF_COURSE_DELAY 20

#define LED_PIN_1 20
#define LED_PIN_2 19

#define RED_BTN_PIN 8
#define ONLY_RED_BTN_MODE false //Set this if nothing works on the playa

bool isRedButtonPressed = false;
unsigned long redButtonModeStartMs = 0;
#define RED_BTN_ANIMATE_TIME_PER_CUBE 1500
CRGBPalette16 animationPallete;

// The PIR sensor pins
int pirs[] = { 27, 29, 33, 35, 39, 41, 45, 47, 51, 53};
//int pirs[] = { 27, 29, 33, 35};

#define SERIAL_SPEED 9600
#define LED_TYPE    WS2812B
#define COLOR_ORDER RGB

bool isEndOfCourseAnimation = false;
unsigned long endOfCourseAnimationStartMs = 0;

// How the led are sorted on each pir and mapped
uint8_t mapLedToPir[] = {60, 45, 30, 15, 0, 0, 15, 30, 45, 60};
//uint8_t mapLedToPir[] = {15, 0, 0, 15};

uint8_t currentAnimationPerLed[NUM_OF_CUBES];

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

// Gradient palette "spring_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/h5/tn/spring.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 64 bytes of program space.

DEFINE_GRADIENT_PALETTE( SPRING ) {
    0, 255,  0,255,
   17, 255,  1,212,
   33, 255,  2,178,
   51, 255,  7,145,
   68, 255, 13,115,
   84, 255, 22, 92,
  102, 255, 33, 71,
  119, 255, 47, 52,
  135, 255, 62, 37,
  153, 255, 82, 25,
  170, 255,104, 15,
  186, 255,127,  9,
  204, 255,156,  4,
  221, 255,186,  1,
  237, 255,217,  1,
  255, 255,255,  0};

// Gradient palette "bhw2_38_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw2/tn/bhw2_38.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 32 bytes of program space.

DEFINE_GRADIENT_PALETTE( PURPLE_UNICORN ) {
    0, 104,205,212,
   28,  32,184,203,
   61,  39, 39,168,
   96,  46,  1,144,
  130,   8,  1, 22,
  163,  17,  3, 28,
  196,  24,  4, 31,
  255, 213,  9, 89};

int pirStates[NUM_OF_CUBES];

CRGB ledsFirst[NUM_LEDS_PER_STRIP * NUM_OF_LEDS_PER_SIDE];
CRGB ledsSecond[NUM_LEDS_PER_STRIP * NUM_OF_LEDS_PER_SIDE];
CRGB *currentArr = NULL;
bool isLedOn[NUM_OF_CUBES];

CRGBPalette16 currentPalette;
int UPDATES_PER_SECOND = 102;

void setup() {
  Serial.begin(SERIAL_SPEED);
  Serial.println("Initializing");
  setupLedStrips();
  Serial.print(NUM_OF_CUBES);
  Serial.println(" Strips started");
  setupPirs();
  Serial.print(NUM_OF_CUBES);
  Serial.println(" Sensors started");
  //pinMode(RED_BTN_PIN, INPUT);
  Serial.print(NUM_OF_CUBES);
  Serial.println(" All Started");
  animationPallete = getRandomPallete();
}

void setupLedStrips() {
  Serial.print("Init leds:");
  Serial.println(NUM_LEDS_PER_STRIP * NUM_OF_LEDS_PER_SIDE);
  FastLED.addLeds<LED_TYPE, LED_PIN_1, COLOR_ORDER>(ledsFirst, NUM_LEDS_PER_STRIP * NUM_OF_LEDS_PER_SIDE).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE, LED_PIN_2, COLOR_ORDER>(ledsSecond, NUM_LEDS_PER_STRIP * NUM_OF_LEDS_PER_SIDE).setCorrection( TypicalLEDStrip );
  for(int stripIndex=0; stripIndex<NUM_OF_CUBES; stripIndex++) {
    uint8_t ledToStartFrom = mapLedToPir[stripIndex];
    if(stripIndex < NUM_OF_LEDS_PER_SIDE ) {
      currentArr = ledsFirst;
    }else {
      currentArr = ledsSecond;
    }
    for(int i=0;i<NUM_LEDS_PER_STRIP;i++) {
      currentArr[i + mapLedToPir[stripIndex]] = CRGB::Black;
    }
    delay(100);
    FastLED.show();
    for(uint8_t i=0;i< NUM_LEDS_PER_STRIP;i++) {
      currentArr[i + ledToStartFrom] = CRGB(142,  40,  1);
    }
    FastLED.show();
    delay(100);
    for(int i=0;i<NUM_LEDS_PER_STRIP;i++) {
      currentArr[i + mapLedToPir[stripIndex]] = CRGB::Black;
    }
    delay(100);
    FastLED.show();
  }
  
  FastLED.show();
}

void setupPirs() {
  for(int i=0;i< NUM_OF_CUBES;i++) {
    pirStates[i] = LOW;
    pinMode(pirs[i], INPUT);
  }
}

static uint8_t startIndex = 0;
void loop(){
  unsigned long time = millis(); // TODO: More accurate to call it each time beforeCheckIfStripShouldGoOff but may cost more
   startIndex = startIndex + 1; /* motion speed */
   //TODO - perhaps this line instead 
   //static uint8_t startIndex = 0;
//  if(checkAndPlayRedButtonMode(startIndex, time)) {
//    return;
//  }
//
  if(checkAndSetEndOfCourseAnimation(time)) {
    return;
  }

  // Not end of course animatino
  for(int i=0;i<NUM_OF_CUBES; i++ ) {
    checkPir(i);

    //TODO: perhaps first check all pirs and then activate led program - not sure what is better
    if(pirStates[i] == HIGH) {
      FillLEDsFromPaletteColors(startIndex, Geek_Black_White, i, false);
      if(pirTurnedOnTimeInMs[i] == 0) {
        Serial.print("Turned cube on ");
        Serial.println(i);
        pirTurnedOnTimeInMs[i] = time; //TODO perhaps should be millis();
        isLedOn[i] = true;
      }
    }
    //TODO - can check those only if was set in the last couple of seconds - Right now test for HIGH AND LOW
    //Set cube just turned on
    if(isLedOn[i]) {
      checkIfStripShouldGoOff(time, i);
    }
  }

  // Check last pir
  if(pirStates[NUM_OF_CUBES - 1] == HIGH) {
    isEndOfCourseAnimation = true;
    endOfCourseAnimationStartMs = millis();
    animationPallete = getRandomPallete();
  }

  FastLED.show();
  //FastLED.delay(1000 / UPDATES_PER_SECOND); //TODO - This will cause a delay in receiving input
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

bool checkAndPlayRedButtonMode(uint8_t colorIndex, unsigned long time) {
  if(!isRedButtonPressed) {
    return isReadRedButtonOn();
  }

  // Animate cube 0 - NUM_OF_CUBES -> RED_BTN_ANIMATE_TIME_PER_CUBE seconds per cube
  if(time - redButtonModeStartMs < (NUM_OF_CUBES * RED_BTN_ANIMATE_TIME_PER_CUBE)) {
    //Cube animation
    int cubeToAnimate = (int)((time - redButtonModeStartMs) / RED_BTN_ANIMATE_TIME_PER_CUBE);
    for(int i=0;i<NUM_OF_CUBES;i++) {
      FillLEDsFromPaletteColors(colorIndex, animationPallete, cubeToAnimate, true);
    }
  }else {
    isEndOfCourseAnimation = true;
    endOfCourseAnimationStartMs = millis();
    return false; //So the code will contine to end of course animation
  }

  FastLED.show();

  return true;
}

bool isReadRedButtonOn() {
  int val = digitalRead(RED_BTN_PIN);
  if(val == LOW) {
    return false;
  }

  Serial.println("Red button!");
  isRedButtonPressed = true;
  animationPallete = getRandomPallete();
  redButtonModeStartMs = millis();
  return true;
}

void FillLEDsFromPaletteColors(uint8_t colorIndex, CRGBPalette16 currentPalette, uint8_t stripIndex, bool isFullPattern)
{
  currentAnimationPerLed[stripIndex]++;
  //isFullPattern = true;
  // Map the cube - leds from one side and the other side
  if(stripIndex < (NUM_OF_CUBES / 2) ) {
    currentArr = ledsFirst;
  }else {
    currentArr = ledsSecond;
  }
  uint8_t ledStartLocation = mapLedToPir[stripIndex];
  for( int i = 0; i < NUM_LEDS_PER_STRIP; i++) {
    if(isFullPattern) {
      currentArr[ledStartLocation + i] = ColorFromPalette( currentPalette, currentAnimationPerLed[stripIndex] + colorIndex, BRIGHTNESS);
    }else {
      currentArr[ledStartLocation + i] = CRGB::Purple;
    }
    colorIndex += 1; //TODO: check this
  }
  delay(END_OF_COURSE_DELAY);
  //Serial.println("End of Fill leds");
}


void checkIfStripShouldGoOff(unsigned long time, int stripIndex) {
  //TODO
  if(time - pirTurnedOnTimeInMs[stripIndex] > TURN_CUBE_OFF_AFTER_MS) {
    pirTurnedOnTimeInMs[stripIndex] = 0;
    Serial.print("Turning cube off ");
    Serial.println(stripIndex);
    uint8_t ledStartLocation = mapLedToPir[stripIndex];
    // Map the cube - leds from one side and the other side
    if(stripIndex < (NUM_OF_CUBES / 2) ) {
      currentArr = ledsFirst;
    }else {
      currentArr = ledsSecond;
    }
    for(int i=0;i<NUM_LEDS_PER_STRIP;i++) {
      currentArr[ledStartLocation + i] = CRGB::Black;
    }
    isLedOn[stripIndex] = false;
  }
  //Serial.println("End Check strip");
}


bool checkAndSetEndOfCourseAnimation(unsigned long time) {
  if(!isEndOfCourseAnimation) {
    return false;
  }
  
  if( (time - endOfCourseAnimationStartMs) > END_OF_COURSE_ANIMATION_LENGTH) {
    isEndOfCourseAnimation = false;
    isRedButtonPressed = false; //In case the red button triggerd the animation

    for(int i=0;i<NUM_OF_CUBES;i++) {
      isLedOn[i] = true;
    }
  }
  
  for(int i=0;i<NUM_OF_CUBES;i++) {
    FillLEDsFromPaletteColors(startIndex, animationPallete, i, true);
  }

  FastLED.show();
  return true;
}

CRGBPalette16 getRandomPallete() {
  switch(random(0, 3)) {
    case 0:
      return SPRING;
    case 1:
      return PURPLE_HAZE;
    case 2:
      return PURPLE_UNICORN;
  }
}


