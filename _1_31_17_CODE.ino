#include <Servo.h>
#include "FastLED.h"
#include <Adafruit_NeoPixel.h>
#include <RBD_Timer.h>
#include <avr/power.h>
#include <Entropy.h>

int busyPinState = HIGH;

const int resetPin  = 8;
const int clockPin  = 6;
const int dataPin   = 4;
const int busyPin   = 5;
const int hit_switch = 12;

const int FORWARD = 15;    //controls FET that moves backward

const int LED_POWER  = 18;                  //Analog pin A0 supplies power to the LED button on top
const int SERVO_POWER = 11;

int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button
int loop_A = 0;              //Counter loop for the switch

long randNumber;


#define DATA_PIN 7      //LED data pin
#define BRIGHTNESS  40
#define NUM_LEDS 56

CRGB leds[NUM_LEDS];

Adafruit_NeoPixel strip = Adafruit_NeoPixel(56, DATA_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800);
//make each scene a function
//make loop a global variable
//have arm hit the button multiple time
//speaker on FET for longer songs?
//change delay to milli(); to add in LED's
//fire code LED?

Servo arm_servo;  // create servo object to control a servo
// twelve servo objects can be created on most boards
Servo open_servo;

int volume = 7;  //Set the volume

//positions for arm to hit swtich
int pos = 180;    // variable to store the servo position
int START_ARM = 180;  //Arm min position is 180
int END_ARM = 90;   //Arm max to hit switch is 90


//Positions for door opening servo
int START_OPEN = 130;   //Door max height is position 130
int END_OPEN = 165;    //Door all the way down is position 180

void setup() {
  Entropy.Initialize();

  arm_servo.attach(9);   // ATTACHES SERVO ARM TO PIN 9
  open_servo.attach(10); //ATTACHES DOOR OPENING ARM TO PIN 10
  pinMode(SERVO_POWER, OUTPUT);   //Fet to turn Servo on
  pinMode(LED_POWER, OUTPUT);                      //Analog pin set to output for green LED button
  pinMode(resetPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(busyPin, INPUT);
  pinMode(hit_switch, INPUT_PULLUP);
  pinMode(FORWARD, OUTPUT);
  //pinMode(A1, OUTPUT);
  delay(1000); // * Wait for the Pins *

  resetModule();

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  LEDS.setBrightness(BRIGHTNESS);


}

//INITIALIZE VARIABLES
void INITIALIZE_VARIABLES()
{
  arm_servo.write(START_ARM); //Initiates servos to the correct positions
  open_servo.write(END_OPEN); //Initiates servos to the correct positions

  FastLED.clear (); //TURN LED's off
  FastLED.show();

  setVolume(volume);
  sendCommand(0xfff7);
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}


// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle1(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 99 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}






//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j = 0; j < 256; j++) {   // cycle all 256 colors in the wheel
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, Wheel( (i + j) % 255)); //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}


void Gold()
{
  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::Gold;
  }
  FastLED.show();

}

void Purple()
{
  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::Purple;
  }
  FastLED.show();
}


void Blue()
{
  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::Blue;
  }
  FastLED.show();
}


void Magenta()
{
  for (int i = 0; i < NUM_LEDS; ++i)        //ON
  {
    leds[i] = CRGB::Magenta;
  }
  FastLED.show();
}

void Red()
{

  for (int i = 0; i < NUM_LEDS; ++i)              //ON
  {
    leds[i] = CRGB::Red;
  }
  FastLED.show();
}

void Aqua()
{
  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::Aqua;
  }
  FastLED.show();
}

void Green()
{
  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::Green;
  }
  FastLED.show();

}



uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
    WheelPos -= 170;
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}


void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void   HOLD_SWITCH()
{
  int x = 5; //delay time for box

  //OPEN DOOR
  for (pos = END_OPEN; pos >= START_OPEN; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(x);                       // waits 15ms for the servo to reach the position
  }

  // ARM TURN OFF BUTTON
  for (pos = START_ARM; pos >= END_ARM; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    arm_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(x);                       // waits 15ms for the servo to reach the position
  }
  delay(1000);
  for (pos = END_ARM; pos <= START_ARM; pos += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    arm_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(x);                       // waits 15ms for the servo to reach the position
  }

  //CLOSE DOOR
  for (pos = START_OPEN ; pos <= END_OPEN; pos += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(x);                       // waits 15ms for the servo to reach the position
  }
  delay(1000);
}


void PEAK()
{
  //////////////OPEN DOOR
  for (pos = END_OPEN; pos >= 145; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(1);                       // waits 15ms for the servo to reach the position
  }

  delay(1000);
  ////////////////////////// CLOSE DOOR
  for (pos = 145 ; pos <= END_OPEN; pos += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(40);                       // waits 15ms for the servo to reach the position
  }
}

void HIT_SWITCH()
{
  // ARM TURN OFF BUTTON
  for (pos = START_ARM; pos >= END_ARM; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    arm_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(10);                       // waits 15ms for the servo to reach the position
  }
  delay(1000);
  for (pos = END_ARM; pos <= START_ARM; pos += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    arm_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(10);                       // waits 15ms for the servo to reach the position
  }
}


///OPEN HOOD MULTIPLE TIMES
void FAKE_ONCE()
{ int loop1 = 0;

  {
    int x1 = 10; //delay time for box
    int x2 = 10;

    //////////////OPEN DOOR
    for (pos = END_OPEN; pos >= START_OPEN; pos -= 1) // goes from 180 degrees to 0 degrees
    {

      open_servo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(x1);                       // waits 15ms for the servo to reach the position
    }

    ////////////////////////// CLOSE DOOR
    for (pos = START_OPEN ; pos <= END_OPEN; pos += 1) // goes from 0 degrees to 180 degrees
    { // in steps of 1 degree
      open_servo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(x2);                       // waits 15ms for the servo to reach the position
    }

    //OPEN DOOR
    for (pos = END_OPEN; pos >= START_OPEN; pos -= 1) // goes from 180 degrees to 0 degrees
    {

      open_servo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(x1);                       // waits 15ms for the servo to reach the position
    }


    //////////////////////// ARM TURN OFF BUTTON
    for (pos = START_ARM; pos >= END_ARM; pos -= 1) // goes from 180 degrees to 0 degrees
    {
      arm_servo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(x1);                       // waits 15ms for the servo to reach the position
    }

    for (pos = END_ARM; pos <= START_ARM; pos += 1) // goes from 0 degrees to 180 degrees
    { // in steps of 1 degree
      arm_servo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(x1);                       // waits 15ms for the servo to reach the position
    }
    ////////////////////////// CLOSE DOOR
    for (pos = START_OPEN ; pos <= END_OPEN; pos += 1) // goes from 0 degrees to 180 degrees
    { // in steps of 1 degree
      open_servo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(x1);                       // waits 15ms for the servo to reach the position
    }
    loop1++;
  }

}


void  HOLD_OPEN_DOOR()
{
  int x = 15; //delay time for box

  //OPEN DOOR
  for (pos = END_OPEN; pos >= START_OPEN; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(x);                       // waits 15ms for the servo to reach the position
  }

  //MOVE ARM PART WAY OUT TO GET READY TO HIT SWITCH
  for (pos = START_ARM; pos >= 120; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    arm_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(x);                       // waits 15ms for the servo to reach the position
  }
}


//function to open switch quickly
void SWITCH_FAST()
{
  int x = 5; //delay time for box

  //////////////OPEN DOOR
  for (pos = END_OPEN; pos >= START_OPEN; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(x);                       // waits 15ms for the servo to reach the position
  }

  //////////////////////// ARM TURN OFF BUTTON

  for (pos = START_ARM; pos >= END_ARM; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    arm_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(x);                       // waits 15ms for the servo to reach the position
  }
  for (pos = END_ARM; pos <= START_ARM; pos += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    arm_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(x);                       // waits 15ms for the servo to reach the position
  }

  ////////////////////////// CLOSE DOOR
  for (pos = START_OPEN ; pos <= END_OPEN; pos += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(x);                       // waits 15ms for the servo to reach the position
  }
}

void OPEN_HOOD_MULTIPLE_TIMES()
{ int loop1 = 0;

  {
    int x1 = 15; //delay time for box
    int x2 = 15;
    delay(3000);    //starts at 3 seconds in
    while (loop1 <= 15)
    {
      for (int i = 0; i < NUM_LEDS; ++i)
      {
        leds[i] = CRGB::Yellow;
      }
      FastLED.show();

      //////////////OPEN DOOR
      for (pos = END_OPEN; pos >= START_OPEN; pos -= 1) // goes from 180 degrees to 0 degrees
      {

        open_servo.write(pos);              // tell servo to go to position in variable 'pos'
        delay(x1);                       // waits 15ms for the servo to reach the position
      }

      FastLED.clear (); //TURN LED's off
      FastLED.show();

      ////////////////////////// CLOSE DOOR
      for (pos = START_OPEN ; pos <= END_OPEN; pos += 1) // goes from 0 degrees to 180 degrees
      { // in steps of 1 degree
        open_servo.write(pos);              // tell servo to go to position in variable 'pos'
        delay(x2);                       // waits 15ms for the servo to reach the position
      }
      loop1++;
    }
    delay(500);
  }
  FastLED.clear (); //TURN LED's off
  FastLED.show();
}

//HIT THE SWITCH SLOW
//function to open box slowely
void SWITCH_SLOW()
{
  int x = 40; //delay time for box

  //////////////OPEN DOOR
  for (pos = END_OPEN; pos >= START_OPEN; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(x);                       // waits 15ms for the servo to reach the position
  }

  //////////////////////// ARM TURN OFF BUTTON

  for (pos = START_ARM; pos >= END_ARM; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    arm_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(x);                       // waits 15ms for the servo to reach the position
  }
  for (pos = END_ARM; pos <= START_ARM; pos += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    arm_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(x);                       // waits 15ms for the servo to reach the position
  }

  ////////////////////////// CLOSE DOOR
  for (pos = START_OPEN ; pos <= END_OPEN; pos += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(x);                       // waits 15ms for the servo to reach the position
  }

}


void MOVE_FORWARD()      ///Moves the car forward for two seconds
{
  digitalWrite(FORWARD, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(2000);
  digitalWrite(FORWARD, LOW);   // turn the LED on (HIGH is the voltage level)
}


void Scene0()
{
  SWITCH_FAST();
}



/////////////////////////////////PLAY THE FIRST TRACK: ANGELS SINGING
void Scene1()
{

  INITIALIZE_VARIABLES();
  // Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800);
  startSoundInBackground(0);

  for (int i = 0; i < NUM_LEDS; i++)
  {

    pixels.setPixelColor(i, pixels.Color(30, 30, 30)); // Moderately bright green color.

    pixels.show();

  }


  delay(1000);


  for (pos = END_OPEN; pos >= START_OPEN; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(100);                       // waits 15ms for the servo to reach the position
  }

  delay(4000);

  for (int i = 0; i < NUM_LEDS; i++)
  {

    pixels.setPixelColor(i, pixels.Color(50, 50, 50)); // Moderately bright green color.

    pixels.show();
  }

  for (pos = 130; pos >= 100; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(100);                       // waits 15ms for the servo to reach the position
  }


  for (pos = START_ARM; pos >= END_ARM; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    arm_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(20);                       // waits 15ms for the servo to reach the position
  }
  for (pos = END_ARM; pos <= START_ARM; pos += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    arm_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(30);                       // waits 15ms for the servo to reach the position
  }

  ////////////////////////// CLOSE DOOR
  for (pos = 100 ; pos <= END_OPEN; pos += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(20);                       // waits 15ms for the servo to reach the position
  }
  FastLED.clear (); //TURN LED's off
  FastLED.show();
}


//Scene2
/////////////////////////////////PLAY THE SECOND TRACK: BABY GOT BACK
void Scene2()
{

  INITIALIZE_VARIABLES();
  startSoundInBackground(1);

  int loop0 = 0;

  OPEN_HOOD_MULTIPLE_TIMES();
  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::Yellow;
  }
  FastLED.show();

  delay(2000);
  SWITCH_FAST();

  FastLED.clear (); //TURN LED's off
  FastLED.show();
}

//Scene_3
/////////////////////////////////PLAY THE THIRD TRACK: CAN"T TOUCH THIS
void Scene3()
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(2);

  delay(11000);


  MOVE_FORWARD();

  SWITCH_FAST();
}

/////////////////////////////////PLAY THE FOURTH TRACK: KEEP YA HEAD UP
void Scene4()
{

  INITIALIZE_VARIABLES();
  startSoundInBackground(3);

  int loop_2 = 0;

  Purple();

  delay(13000);
  SWITCH_SLOW();
  delay(1000);

  FastLED.clear (); //TURN LED's off
  FastLED.show();
}


/////////////////////////////////PLAY THE FITH TRACK: 101 DALMATIONS
void Scene5()
{

  INITIALIZE_VARIABLES();
  startSoundInBackground(4);

  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::Teal;
  }
  FastLED.show();


  delay(34500);
  SWITCH_FAST();


  FastLED.clear (); //TURN LED's off
  FastLED.show();

}



/////////////////////////////////PLAY THE SIXTH TRACK: BABY IT'S COLD OUTSIDE
void Scene6()
{

  INITIALIZE_VARIABLES();
  startSoundInBackground(5);

  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::Teal;
  }
  FastLED.show();

  delay(46000);
  HOLD_OPEN_DOOR();
  delay(7000);

  // ARM TURN OFF BUTTON
  for (pos = 120; pos >= END_ARM; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    arm_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(20);                       // waits 15ms for the servo to reach the position
  }
  delay(1000);
  for (pos = END_ARM; pos <= START_ARM; pos += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    arm_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(20);                       // waits 15ms for the servo to reach the position
  }
  FastLED.clear (); //TURN LED's off
  FastLED.show();

}

/////////////////////////////////PLAY THE SEVENTH TRACK: BIBIO LOVERS'S CARVINGS
void Scene7()
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(6);

  Blue();

  //delay(22000);
  delay(10000);
  PEAK();
  delay(10000);
  FAKE_ONCE();
  FastLED.clear (); //TURN LED's off
  FastLED.show();
}

//////////////////////////////////////////////PLAY BLOW MOVIE QUOTE
void Scene8()
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(7);

  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::Violet;
  }
  FastLED.show();

  delay(1000);

  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::Amethyst;
  }
  FastLED.show();

  delay(2500);

  Aqua();

  delay(5000);
  HOLD_SWITCH();
  delay(1000);
  FastLED.clear (); //TURN LED's off
  FastLED.show();

}

////////////////////////////////////////////BRUINS GOAL
void Scene9()
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(8);

  delay(12000);

  Gold();

  delay(11000);

  SWITCH_FAST();

  FastLED.clear (); //TURN LED's off
  FastLED.show();
}

//////////////////////////////////////////////CALIFORNIA- LED ZEPLIN
void Scene10()
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(9);

  Green();


  delay(8000);
  SWITCH_SLOW();
  FastLED.clear (); //TURN LED's off
  FastLED.show();
}

//////////////////////////////////////////////CHANLDER VS PHOEBE
void Scene11()
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(10);

  delay(28500);
  SWITCH_FAST();
}

//////////////////////////////////////////////COMING HOME
void Scene12()
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(11);

  Purple();

  int x = 5; //delay time for box

  //////////////OPEN DOOR
  for (pos = END_OPEN; pos >= START_OPEN; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(x);                       // waits 15ms for the servo to reach the position
  }

  //////////////////////// ARM TURN OFF BUTTON

  for (pos = START_ARM; pos >= 96; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    arm_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(x);                       // waits 15ms for the servo to reach the position
  }

  delay(23000);

  for (pos = 96; pos >= 90; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    arm_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(x);                       // waits 15ms for the servo to reach the position
  }

  ////
  for (pos = 90; pos <= START_ARM; pos += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    arm_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(x);                       // waits 15ms for the servo to reach the position
  }

  ////////////////////////// CLOSE DOOR
  for (pos = START_OPEN ; pos <= END_OPEN; pos += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(x);                       // waits 15ms for the servo to reach the position
  }
  FastLED.clear (); //TURN LED's off
  FastLED.show();

}



//////////////////////////////////////////////ONE MORE TIME DAFT PUNK
void Scene13()
{

  INITIALIZE_VARIABLES();
  startSoundInBackground(12);

  digitalWrite(SERVO_POWER, LOW);
  theaterChaseRainbow(10);
  digitalWrite(SERVO_POWER, HIGH);

  SWITCH_FAST();

  FastLED.clear (); //TURN LED's off
  FastLED.show();
}

///////////////////////////////////////////////DARK_NECESSITIES_RED_HOT_HILLPPS
void Scene14()
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(59);

  delay(4500);

  Blue();

  delay(2500);

  Green();

  delay(2700);

  FastLED.clear (); //TURN LED's off
  FastLED.show();


  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::DarkTurquoise;
  }
  FastLED.show();

  delay(2400);

  Magenta();

  delay(2800);

  //OPEN DOOR
  for (pos = END_OPEN; pos >= 110; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(30);                       // waits 15ms for the servo to reach the position
  }

  delay(1000);

  for (pos = START_ARM; pos >= END_ARM; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    arm_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(7);                       // waits 15ms for the servo to reach the position
  }
  for (pos = END_ARM; pos <= START_ARM; pos += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    arm_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(7);                       // waits 15ms for the servo to reach the position
  }

  delay(1500);

  //CLOSE DOOR
  for (pos = 110 ; pos <= END_OPEN; pos += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(30);                       // waits 15ms for the servo to reach the position
  }
  delay(800);

  Red();

  delay(2000);

  FastLED.clear (); //TURN LED's off
  FastLED.show();
}

void Scene15()///////////////////////DROPS OF JUPITER
{

  INITIALIZE_VARIABLES();
  startSoundInBackground(14);

  colorWipe(strip.Color(55, 0, 0), 50); // Red
  colorWipe(strip.Color(0, 55, 0), 50); // Green
  colorWipe(strip.Color(0, 0, 55), 50); // Blue

  //////////////OPEN DOOR
  for (pos = END_OPEN; pos >= 130; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(40);                       // waits 15ms for the servo to reach the position
  }

  colorWipe(strip.Color(55, 0, 0), 30); // Red
  colorWipe(strip.Color(0, 55, 0), 30); // Green
  colorWipe(strip.Color(0, 0, 55), 30); // Blue

  //////////////////////// ARM TURN OFF BUTTON

  for (pos = START_ARM; pos >= END_ARM; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    arm_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(20);                       // waits 15ms for the servo to reach the position
  }
  for (pos = END_ARM; pos <= START_ARM; pos += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    arm_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(20);                       // waits 15ms for the servo to reach the position
  }

  ////////////////////////// CLOSE DOOR
  for (pos = START_OPEN ; pos <= END_OPEN; pos += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(20);                       // waits 15ms for the servo to reach the position
  }

  FastLED.clear (); //TURN LED's off
  FastLED.show();

}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j = 0; j < 10; j++) { //do 10 cycles of chasing
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, c);  //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}



//////////////////////////////////////////////EMINEM LOSE YYOURSELF
void Scene16()
{

  INITIALIZE_VARIABLES();
  startSoundInBackground(15);

  theaterChase(strip.Color(100, 100, 100), 50); // White      //Should use a timer here to flash light and move servo
  theaterChase(strip.Color(100, 100, 100), 50); // White
  theaterChase(strip.Color(100, 100, 100), 50); // White
  theaterChase(strip.Color(100, 100, 100), 50); // White
  theaterChase(strip.Color(100, 100, 100), 50); // White

  SWITCH_FAST();
  theaterChase(strip.Color(100, 100, 100), 50); // White
  strip.Color(100, 100, 100); // White
  delay(200);
  FastLED.clear (); //TURN LED's off
  FastLED.show();
  delay(2000);

}



//////////////////////////////////////////////EYE OF THE TIGER
void Scene17()
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(16);
  delay(8900);

  Green();
  delay(300);

  FastLED.clear (); //TURN LED's off
  FastLED.show();
  delay(900);

  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::White;
  }
  FastLED.show();
  delay(150);

  FastLED.clear (); //TURN LED's off
  FastLED.show();
  delay(200);

  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::White;
  }
  FastLED.show();
  delay(150);

  FastLED.clear (); //TURN LED's off
  FastLED.show();
  delay(200);

  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::White;
  }
  FastLED.show();
  delay(1500);


  open_servo.write(140);
  delay(400);
  open_servo.write(150);
  delay(300);
  open_servo.write(165);

  delay(1500);
  open_servo.write(130);
  delay(500);
  open_servo.write(115);
  delay(200);

  for (pos = 115 ; pos <= END_OPEN; pos += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(50);                       // waits 15ms for the servo to reach the position
  }

  delay(250);
  Green();

  delay(1000);

  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::White;
  }
  FastLED.show();
  delay(150);

  FastLED.clear (); //TURN LED's off
  FastLED.show();
  delay(200);

  Blue();
  delay(150);

  FastLED.clear (); //TURN LED's off
  FastLED.show();
  delay(200);

  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::Gold;
  }
  FastLED.show();
  delay(1800);

  open_servo.write(140);
  delay(400);
  open_servo.write(150);
  delay(300);
  open_servo.write(165);


  delay(1200);
  open_servo.write(130);
  delay(500);
  open_servo.write(115);
  delay(200);

  for (pos = 115 ; pos <= END_OPEN; pos += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(30);                       // waits 15ms for the servo to reach the position
  }
  delay(300);
  SWITCH_FAST();

  FastLED.clear (); //TURN LED's off
  FastLED.show();
}


//////////////////////////////////////////////FIRE AND RAIN JAMES TAYLOR
void Scene18()
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(17);

  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::FireBrick;
  }
  FastLED.show();

  delay(25000);
  SWITCH_SLOW();

  FastLED.clear (); //TURN LED's off
  FastLED.show();
}



//////////////////////////////////////////////LIGHTNING ROUND FRIENDS
void Scene19()
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(18);

  delay(4000);

  Gold();

  delay(300);

  FastLED.clear (); //TURN LED's off
  FastLED.show();

  delay(2600);

  Red();

  delay(2000);

  SWITCH_FAST();

  FastLED.clear (); //TURN LED's off
  FastLED.show();
}

//////////////////////////////////////////////FRIENDS THEME SONG
void Scene20()
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(19);

  Gold();
  delay(7200);

  Gold();
  FastLED.show();
  delay(100);

  FastLED.clear ();                    //TURN LED's off
  FastLED.show();
  delay(100);

  Gold();
  delay(100);


  FastLED.clear ();                   //TURN LED's off
  FastLED.show();
  delay(100);

  Gold();
  delay(100);

  FastLED.clear ();                   //TURN LED's off
  FastLED.show();
  delay(100);

  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::LawnGreen;
  }
  FastLED.show();

  delay(15000);
  //OPEN DOOR
  for (pos = END_OPEN; pos >= 145; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(50);                       // waits 15ms for the servo to reach the position
  }

  delay(3000);

  //CLOSE DOOR
  for (pos = 145 ; pos <= END_OPEN; pos += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(50);                       // waits 15ms for the servo to reach the position
  }

  delay(10000);
  FAKE_ONCE();
  SWITCH_FAST();
}


void Scene21()////////////////////////////////GOOD FRIDAY-KANYE
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(20);

  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::Cyan;
  }
  FastLED.show();


  delay(22000);
  SWITCH_FAST();

  FastLED.clear (); //TURN LED's off
  FastLED.show();
}


void Scene22()////////////////////////////////GOT SOUL BUT IM NOT A SOLDIER
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(21);

  theaterChaseRainbow(34);
  SWITCH_FAST();

  FastLED.clear (); //TURN LED's off
  FastLED.show();
}


void Scene23()////////////////////////////////GREATEST SPEECH
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(22);

  delay(1500);

  Red();
  delay(4200);

  Purple();
  delay(3200);


  Gold();
  delay(3000);

  Green();

  delay(700);

  Blue();
  delay(3000);
  HOLD_SWITCH();
  FastLED.clear (); //TURN LED's off
  FastLED.show();
}

void Scene24()////////////////////////////////Hercules meets Meg
{ INITIALIZE_VARIABLES();
  startSoundInBackground(23);

  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::Navy;

  }
  FastLED.show();


  delay(23000);
  SWITCH_FAST();
  FastLED.clear (); //TURN LED's off
  FastLED.show();
}

void Scene25()////////////////////////////////I'M JOEY
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(64);
  delay(9000);
  MOVE_FORWARD();
  SWITCH_FAST();
}


void Scene26()////////////////////////////////INNER NINJA
{


  INITIALIZE_VARIABLES();

  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::Amethyst;

  }
  FastLED.show();

  startSoundInBackground(25);


  //OPEN DOOR
  for (pos = END_OPEN; pos >= START_OPEN; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(100);                       // waits 15ms for the servo to reach the position
  }

  delay(7000);

  arm_servo.write(100);
  delay(400);
  arm_servo.write(180);

  delay(1700);

  arm_servo.write(100);
  delay(400);
  arm_servo.write(180);

  delay(1700);

  arm_servo.write(100);
  delay(400);
  arm_servo.write(180);

  delay(1700);

  arm_servo.write(90);
  delay(400);
  arm_servo.write(180);

  delay(200);
  open_servo.write(165);


  digitalWrite(FORWARD, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(4000);
  digitalWrite(FORWARD, LOW);   // turn the LED on (HIGH is the voltage level)

  FastLED.clear (); //TURN LED's off
  FastLED.show();
}

void Scene27()////////////////////////////////IMAGINE-JOHN LENAN
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(26);
  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::Amethyst;
    leds[i].fadeToBlackBy( 100 );
  }
  FastLED.show();

  delay(20000);
  SWITCH_SLOW();
  delay(1000);
  FastLED.clear (); //TURN LED's off
  FastLED.show();
}


void Scene28()////////////////////////////////KANYE_HOMCOMING
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(27);

  Green();


  delay(10500);
  SWITCH_FAST();
  FastLED.clear (); //TURN LED's off
  FastLED.show();
}


void Scene29()//////////////////////////////// KUNG FOO FIGHTING
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(28);
  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::Yellow;
  }
  FastLED.show();

  delay(18000);

  SWITCH_FAST();
  delay(2000);

  FastLED.clear (); //TURN LED's off
  FastLED.show();
}

void Scene30()////////////////////////////////LILO AND STITCH
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(29);

  delay(23000);
  SWITCH_FAST();
}



void Scene31()////////////////////////////////MACKLEMORE_SAME.
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(30);

  digitalWrite(SERVO_POWER, LOW);
  rainbowCycle(5);
  digitalWrite(SERVO_POWER, HIGH);

  SWITCH_SLOW();
  digitalWrite(SERVO_POWER, LOW);
  rainbowCycle1(1);
  digitalWrite(SERVO_POWER, HIGH);

  FastLED.clear (); //TURN LED's off
  FastLED.show();
}


void Scene32()////////////////////////////////
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(31);
  theaterChase(strip.Color(0, 0, 50), 50); // Blue flashing
  theaterChase(strip.Color(0, 0, 50), 50); // Blue
  theaterChase(strip.Color(0, 0, 50), 50); // Blue
  theaterChase(strip.Color(0, 0, 50), 50); // Blue
  theaterChase(strip.Color(0, 0, 50), 50); // Blue

  theaterChase(strip.Color(0, 50, 0), 50); // Green flashing

  SWITCH_FAST();
  FastLED.clear (); //TURN LED's off
  FastLED.show();
}



void Scene33()////////////////////////////////mulan -  I'll make a man out of you SECOND REPRINT
{
  int loop1 = 0;

  INITIALIZE_VARIABLES();
  startSoundInBackground(32);
  while (loop1 < 29)
  {
    theaterChase(strip.Color(127,   0,   0), 50); // Red
    loop1++;
  }



  SWITCH_FAST();

  FastLED.clear (); //TURN LED's off
  FastLED.show();
}


void Scene34()////////////////////////////////MULAN_BLACK ADN WHITE
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(33);
  Red();


  delay(25000);
  SWITCH_FAST();

  FastLED.clear (); //TURN LED's off
  FastLED.show();
}

void Scene35()////////////////////////////////MULAN-YOU'RE NOT LUCKY?
{
  INITIALIZE_VARIABLES();

  int x = 5; //delay time for box

  //OPEN DOOR
  for (pos = END_OPEN; pos >= START_OPEN; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(x);                       // waits 15ms for the servo to reach the position
  }

  // ARM TURN OFF BUTTON
  for (pos = START_ARM; pos >= END_ARM; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    arm_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(x);                       // waits 15ms for the servo to reach the position
  }


  startSoundInBackground(34);

  Red();

  delay(10600);


  for (pos = END_ARM; pos <= START_ARM; pos += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    arm_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(x);                       // waits 15ms for the servo to reach the position
  }

  //CLOSE DOOR
  for (pos = START_OPEN ; pos <= END_OPEN; pos += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(x);                       // waits 15ms for the servo to reach the position
  }
  delay(1000);

  FastLED.clear (); //TURN LED's off
  FastLED.show();
}


void Scene36()////////////////////////////////NOTHIN ON YOU
{

  INITIALIZE_VARIABLES();
  startSoundInBackground(35);

  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::DarkTurquoise;
  }
  FastLED.show();



  delay(10000);
  SWITCH_SLOW();

  FastLED.clear (); //TURN LED's off
  FastLED.show();
}


void Scene37()////////////////////////////////ONE MAN CAN CHANGE THE WORLD
{
  INITIALIZE_VARIABLES();
  //Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800);
  startSoundInBackground(36);
  pixels.begin();

  for (int i = 0; i < NUM_LEDS; i++) {

    pixels.setPixelColor(i, pixels.Color(0, 100, 0)); // Moderately bright green color.

    pixels.show(); // This sends the updated pixel color to the hardware.

    delay(590); // Delay for a period of time (in milliseconds).
  }
  SWITCH_SLOW();

  FastLED.clear (); //TURN LED's off
  FastLED.show();
}

void Scene38()////////////////////////////////PETER PAN CAN FLY
{
  int loop1 = 0;
  int x = 5;

  INITIALIZE_VARIABLES();
  startSoundInBackground(37);
  while (loop1 < 17)
  {
    theaterChase(strip.Color(0, x, 0), 50); // Green

    loop1++;
    x = 8 * loop1;
  }
  SWITCH_FAST();
  FastLED.clear (); //TURN LED's off
  FastLED.show();
}


void Scene39()////////////////////////////////US AND THEM PINK FLOYD
{
  INITIALIZE_VARIABLES();

  startSoundInBackground(38);
  digitalWrite(SERVO_POWER, LOW);
  rainbowCycle(25);
  digitalWrite(SERVO_POWER, HIGH);

  SWITCH_FAST();

  FastLED.clear (); //TURN LED's off
  FastLED.show();

}


void Scene40()///////////////////////////////RAY LEMONTANGE-EMPTY
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(39);


  delay(2500);

  Red();
  delay(6000);

  FastLED.clear (); //TURN LED's off
  FastLED.show();

  delay(5700);

  Red();

  delay(500);

  Gold();

  delay(4000);

  HOLD_SWITCH();

  FastLED.clear (); //TURN LED's off
  FastLED.show();

}


void Scene41()///////////////////////////////RENT MUSICAL -HOW DO YOU MEASURE A YEAR
{
  INITIALIZE_VARIABLES();

  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::DeepPink;
  }
  FastLED.show();

  startSoundInBackground(40);
  delay(45000);

  SWITCH_FAST();

  FastLED.clear (); //TURN LED's off
  FastLED.show();
}

void Scene42()///////////////////////////////RIO 2-CLARA THE CAPIBARRA
{

  INITIALIZE_VARIABLES();
  startSoundInBackground(71);

  Blue();

  delay(12800);

  open_servo.write(START_OPEN); //Initiates servos to the correct positions
  delay(500);
  open_servo.write(END_OPEN); //Initiates servos to the correct positions


  FastLED.clear (); //TURN LED's off
  FastLED.show();
  delay(4200);


  open_servo.write(START_OPEN); //Initiates servos to the correct positions

  Gold();


  delay(7000);

  SWITCH_FAST();

  MOVE_FORWARD();
  FastLED.clear (); //TURN LED's off
  FastLED.show();

}

void Scene43()/////////////////////////////// TUPAC-ROSE THAT GREW FROM CONCRETE
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(42);

  theaterChaseRainbow(49);

  SWITCH_SLOW();
  delay(400);

  FastLED.clear (); //TURN LED's off
  FastLED.show();
}

void Scene44()///////////////////////////////SUN_QUEEN
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(43);

  HOLD_OPEN_DOOR();


  Gold();


  delay(9000);

  for (pos = END_ARM; pos <= START_ARM; pos += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    arm_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(20);                       // waits 15ms for the servo to reach the position
  }


  //CLOSE DOOR
  for (pos = START_OPEN ; pos <= END_OPEN; pos += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(20);                       // waits 15ms for the servo to reach the position
  }

  delay(1500);
  SWITCH_FAST();


  FastLED.clear (); //TURN LED's off
  FastLED.show();
}


void Scene45()///////////////////////////////The_drunk_and_Monk.ad4
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(44);
  delay(20500);

  Red();

  delay(2700);

  Magenta();
  delay(6000);

  Blue();
  delay(2500);

  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::White;
  }
  FastLED.show();
  delay(300);

  Blue();
  delay(7000);

  Gold();

  delay(12000);
  SWITCH_FAST();
  FastLED.clear (); //TURN LED's off
  FastLED.show();

}

void Scene46()///////////////////////////////PETER PAN TWENTY BOYS QUOTE
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(45);


  delay(1500);

  for (int i = 0; i < NUM_LEDS; ++i)      //Goodnight
  {
    leds[i] = CRGB::Pink;
  }
  FastLED.show();

  delay(600);

  FastLED.clear (); //TURN LED's off
  FastLED.show();

  delay(7000);

  Green();
  delay(500);


  FastLED.clear (); //TURN LED's off
  FastLED.show();

  delay(3000);

  Green();

  delay(1800);


  FastLED.clear (); //TURN LED's off
  FastLED.show();
  delay(1000);

  for (int i = 0; i < NUM_LEDS; ++i)      //You really think so?
  {
    leds[i] = CRGB::Pink;
  }
  FastLED.show();
  delay(1300);

  FastLED.clear (); //TURN LED's off
  FastLED.show();

  delay(1200);

  Green();

  delay(2000);

  FastLED.clear (); //TURN LED's off
  FastLED.show();

  SWITCH_FAST();


}

void Scene47()///////////////////////////////UNDER PRESSURE
{
  INITIALIZE_VARIABLES();

  //OPEN DOOR
  for (pos = END_OPEN; pos >= 135; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(40);                       // waits 15ms for the servo to reach the position
  }

  //CLOSE DOOR
  for (pos = 135 ; pos <= END_OPEN; pos += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(40);
  }

  startSoundInBackground(46);

  delay(1900);

  for (int i = 0; i < NUM_LEDS; ++i)              //ON
  {
    leds[i] = CRGB::Crimson;
  }
  FastLED.show();

  delay(2200);

  for (int i = 0; i < NUM_LEDS; ++i)              //ON
  {
    leds[i] = CRGB::Aquamarine;
  }
  FastLED.show();
  delay(2200);

  Red();
  delay(2000);

  Blue();
  delay(2200);

  for (int i = 0; i < NUM_LEDS; ++i)              //ON
  {
    leds[i] = CRGB::DarkGreen;
  }
  FastLED.show();
  delay(2200);

  Gold();

  delay(2000);

  for (int i = 0; i < NUM_LEDS; ++i)              //ON
  {
    leds[i] = CRGB::Cyan;
  }
  FastLED.show();

  delay(2000);
  for (int i = 0; i < NUM_LEDS; ++i)              //ON
  {
    leds[i] = CRGB::DarkOrchid;
  }
  FastLED.show();

  delay(2200);

  Purple();                                      //ON

  delay(2200);
  for (int i = 0; i < NUM_LEDS; ++i)              //ON
  {
    leds[i] = CRGB::DarkRed;
  }
  FastLED.show();

  delay(2000);
  for (int i = 0; i < NUM_LEDS; ++i)              //ON
  {
    leds[i] = CRGB::MediumSpringGreen;
  }
  FastLED.show();

  SWITCH_FAST();

}

void Scene48()///////////////////////////////UNDERDOG SPEECH
{
  INITIALIZE_VARIABLES();

  Blue();



  startSoundInBackground(47);
  delay(55000);
  SWITCH_FAST();

  FastLED.clear (); //TURN LED's off            //OFF
  FastLED.show();

}


void Scene49()///////////////////////////////WILL WILL ROCK YOU
{

  int loop1 = 0;

  INITIALIZE_VARIABLES();
  startSoundInBackground(48);

  delay(800);

  while (loop1 < 4)
  {

    for (int i = 0; i < NUM_LEDS; ++i)              //ON
    {
      leds[i] = CRGB::DarkTurquoise;
    }
    FastLED.show();
    delay(150);


    FastLED.clear (); //TURN LED's off            //OFF
    FastLED.show();
    delay(150);

    for (int i = 0; i < NUM_LEDS; ++i)        //ON
    {
      leds[i] = CRGB::DarkTurquoise;
    }
    FastLED.show();
    delay(150);

    FastLED.clear (); //TURN LED's off        //OFF
    FastLED.show();
    delay(100);

    for (int i = 0; i < NUM_LEDS; ++i)         //ON
    {
      leds[i] = CRGB::DarkTurquoise;
    }
    FastLED.show();
    delay(400);

    FastLED.clear (); //TURN LED's off          /OFF
    FastLED.show();
    delay(510);

    ////////////////////////////////////
    Gold();
    delay(150);


    FastLED.clear (); //TURN LED's off            //OFF
    FastLED.show();
    delay(150);

    Gold();
    delay(150);

    FastLED.clear (); //TURN LED's off        //OFF
    FastLED.show();
    delay(100);

    Gold();
    delay(400);

    FastLED.clear (); //TURN LED's off          /OFF
    FastLED.show();
    delay(510);
    ////////////////////////////////////
    ////////////////////////////////////
    for (int i = 0; i < NUM_LEDS; ++i)              //ON
    {
      leds[i] = CRGB::Lime;
    }
    FastLED.show();
    delay(150);


    FastLED.clear (); //TURN LED's off            //OFF
    FastLED.show();
    delay(150);

    for (int i = 0; i < NUM_LEDS; ++i)        //ON
    {
      leds[i] = CRGB::Lime;
    }
    FastLED.show();
    delay(150);

    FastLED.clear (); //TURN LED's off        //OFF
    FastLED.show();
    delay(100);

    for (int i = 0; i < NUM_LEDS; ++i)         //ON
    {
      leds[i] = CRGB::Lime;
    }
    FastLED.show();
    delay(400);

    FastLED.clear (); //TURN LED's off          /OFF
    FastLED.show();
    delay(510);
    ///////////////////////////////////
    Magenta();
    delay(150);


    FastLED.clear (); //TURN LED's off            //OFF
    FastLED.show();
    delay(150);

    Magenta();
    delay(150);

    FastLED.clear (); //TURN LED's off        //OFF
    FastLED.show();
    delay(100);

    Magenta();
    delay(400);

    FastLED.clear (); //TURN LED's off          /OFF
    FastLED.show();
    delay(510);
    ///////////////////////////////////
    loop1++;
  }

  for (int i = 0; i < NUM_LEDS; ++i)              //ON
  {
    leds[i] = CRGB::DarkTurquoise;
  }
  FastLED.show();
  delay(150);


  FastLED.clear (); //TURN LED's off            //OFF
  FastLED.show();
  delay(150);

  for (int i = 0; i < NUM_LEDS; ++i)        //ON
  {
    leds[i] = CRGB::DarkTurquoise;
  }
  FastLED.show();
  delay(150);

  FastLED.clear (); //TURN LED's off        //OFF
  FastLED.show();
  delay(100);

  for (int i = 0; i < NUM_LEDS; ++i)         //ON
  {
    leds[i] = CRGB::DarkTurquoise;
  }
  FastLED.show();
  delay(400);

  FastLED.clear (); //TURN LED's off          /OFF
  FastLED.show();
  delay(510);

  ////////////////////////////////////
  for (int i = 0; i < NUM_LEDS; ++i)              //ON
  {
    leds[i] = CRGB::Gold;
  }
  FastLED.show();
  delay(150);


  FastLED.clear (); //TURN LED's off            //OFF
  FastLED.show();
  delay(150);

  for (int i = 0; i < NUM_LEDS; ++i)        //ON
  {
    leds[i] = CRGB::Gold;
  }
  FastLED.show();
  delay(150);

  FastLED.clear (); //TURN LED's off        //OFF
  FastLED.show();
  delay(100);

  for (int i = 0; i < NUM_LEDS; ++i)         //ON
  {
    leds[i] = CRGB::Gold;
  }
  FastLED.show();
  delay(400);

  FastLED.clear (); //TURN LED's off          /OFF
  FastLED.show();
  delay(510);
  ////////////////////////////////////
  ////////////////////////////////////
  for (int i = 0; i < NUM_LEDS; ++i)              //ON
  {
    leds[i] = CRGB::Lime;
  }
  FastLED.show();
  delay(150);


  FastLED.clear (); //TURN LED's off            //OFF
  FastLED.show();
  delay(150);

  for (int i = 0; i < NUM_LEDS; ++i)        //ON
  {
    leds[i] = CRGB::Lime;
  }
  FastLED.show();
  delay(150);

  FastLED.clear (); //TURN LED's off        //OFF
  FastLED.show();
  delay(100);

  for (int i = 0; i < NUM_LEDS; ++i)         //ON
  {
    leds[i] = CRGB::Lime;
  }
  FastLED.show();
  delay(400);

  FastLED.clear (); //TURN LED's off          /OFF
  FastLED.show();

  SWITCH_FAST();
  delay(5000);
  FastLED.clear (); //TURN LED's off
  FastLED.show();
}


void Scene50()///////////////////////////////WOULD'T GET FAR
{
  int loop1 = 0;
  INITIALIZE_VARIABLES();
  startSoundInBackground(49);

  //OPEN DOOR
  for (pos = END_OPEN; pos >= START_OPEN; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 15ms for the servo to reach the position
  }

  // ARM TURN OFF BUTTON
  for (pos = START_ARM; pos >= END_ARM; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    arm_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 15ms for the servo to reach the position
  }
  digitalWrite(SERVO_POWER, LOW);

  while (loop1 < 5)
  {
    colorWipe(strip.Color(100, 0, 0), 10); // Red
    delay(50);
    colorWipe(strip.Color(0, 100, 0), 10); // Green
    delay(50);
    colorWipe(strip.Color(0, 0, 100), 10); // Blue
    delay(50);
    loop1++;
  }
  digitalWrite(SERVO_POWER, HIGH);

  for (pos = END_ARM; pos <= START_ARM; pos += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    arm_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 15ms for the servo to reach the position
  }

  //CLOSE DOOR
  for (pos = 150 ; pos <= END_OPEN; pos += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 15ms for the servo to reach the position
  }

  FastLED.clear (); //TURN LED's off
  FastLED.show();

}


void Scene51()///////////////////////////////YOU'VE GOT A FRIEND ON ME
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(50);
  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::Lime;
  }
  FastLED.show();

  SWITCH_SLOW();

  delay(8000);
  //OPEN DOOR
  for (pos = END_OPEN; pos >= 150; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(40);                       // waits 15ms for the servo to reach the position
  }

  delay(1500);

  //CLOSE DOOR
  for (pos = 150 ; pos <= END_OPEN; pos += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(40);                       // waits 15ms for the servo to reach the position
  }

  delay(27000);

  FastLED.clear (); //TURN LED's off
  FastLED.show();
}



void Scene52()///////////////////////////////upside down jack johnson
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(76);

  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::Gold;
  }
  FastLED.show();


  delay(21000);
  SWITCH_FAST();
  FastLED.clear (); //TURN LED's off
  FastLED.show();
}


void Scene53()///////////////////////////////R Kelly - Ignition Remix
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(74);

  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::Orange;
  }
  FastLED.show();


  delay(3000);
  SWITCH_FAST();

  FastLED.clear (); //TURN LED's off
  FastLED.show();
}

void Scene54()///////////////////////////////Friends-Pile of Coats
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(53);
  delay(5000);
  SWITCH_FAST();

  digitalWrite(FORWARD, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);
  digitalWrite(FORWARD, LOW);   // turn the LED on (HIGH is the voltage level)
}


void Scene55()///////////////////////////////Wall-E Ending.ad4
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(54);
  delay(6500);
  Red();

  delay(10000);

  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::Yellow;
  }
  FastLED.show();


  SWITCH_SLOW();
  delay(12500);

  FastLED.clear (); //TURN LED's off
  FastLED.show();

}


void Scene56()///////////////////////////////KANYE-Street Lights
{
  int loop1 = 0;

  INITIALIZE_VARIABLES();
  startSoundInBackground(55);


  while (loop1 < 17)
  {
    for (int i = 0; i < NUM_LEDS; ++i)
    {
      leds[i] = CRGB::Gold;
    }
    FastLED.show();
    delay(500);

    for (int i = 0; i < NUM_LEDS; ++i)
    {
      leds[i].fadeToBlackBy( 64 );
    }
    FastLED.show();
    delay(500);

    for (int i = 0; i < NUM_LEDS; ++i)
    {
      leds[i].fadeToBlackBy( 125 );
    }
    FastLED.show();
    delay(500);

    for (int i = 0; i < NUM_LEDS; ++i)
    {
      leds[i].fadeToBlackBy( 175 );
    }
    FastLED.show();
    delay(500);

    for (int i = 0; i < NUM_LEDS; ++i)
    {
      leds[i].fadeToBlackBy( 200 );
    }
    FastLED.show();
    delay(500);
    loop1++;
  }

  SWITCH_FAST();
}


void Scene57()/////////////////////////////// Where The Streets Have No Name.ad4
{


  int loop1 = 0;

  INITIALIZE_VARIABLES();
  startSoundInBackground(56);

  while (loop1 < 3)
  {
    theaterChase(strip.Color(0, 0, 1), 50); // Green
    loop1++;
  }
  loop1 = 0;
  while (loop1 < 5)
  {
    theaterChase(strip.Color(0, 0, 5), 50); // Green
    loop1++;
  }
  loop1 = 0;
  while (loop1 < 5)
  {
    theaterChase(strip.Color(0, 0, 25), 50); // Green
    loop1++;
  }
  loop1 = 0;
  while (loop1 < 5)
  {
    theaterChase(strip.Color(0, 0, 50), 50); // Green
    loop1++;
  }

  loop1 = 0;
  while (loop1 < 5)
  {
    theaterChase(strip.Color(0, 0, 75), 50); // Green
    loop1++;
  }


  loop1 = 0;
  while (loop1 < 5)
  {
    theaterChase(strip.Color(0, 0, 175), 50); // Green
    loop1++;
  }

  loop1 = 0;
  while (loop1 < 3)
  {
    theaterChase(strip.Color(0, 0, 255), 50); // Green
    loop1++;
  }
  loop1 = 0;
  while (loop1 < 4)
  {
    theaterChase(strip.Color(0, 0, 175), 50); // Green
    loop1++;
  }


  loop1 = 0;
  while (loop1 < 1)
  {
    theaterChase(strip.Color(0, 0, 75), 50); // Green
    loop1++;
  }

  loop1 = 0;
  while (loop1 < 1)
  {
    theaterChase(strip.Color(0, 0, 25), 50); // Green
    loop1++;
  }


  loop1 = 0;
  while (loop1 < 1)
  {
    theaterChase(strip.Color(0, 0, 5), 50); // Green
    loop1++;
  }


  SWITCH_FAST();
  FastLED.clear (); //TURN LED's off
  FastLED.show();
}


void Scene58()///////////////////////////////Austin Powers Theme Song
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(57);

  rainbowCycle(2);

  delay(200);
  //colorWipe(strip.Color(255, 0, 0), 50); // Red

  MOVE_FORWARD();

  SWITCH_FAST();
}


void Scene59()///////////////////////////////Busy P - Rainbow Man.ad4
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(58);

  delay(2500);

  //OPEN DOOR
  for (pos = END_OPEN; pos >= 100; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 15ms for the servo to reach the position
  }

  delay(1900);


  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::DarkOrchid;
  }
  FastLED.show();
  delay(2400);


  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::Lime;
  }
  FastLED.show();

  delay(500);

  open_servo.write(100); //Initiates servos to the correct positions
  delay(150);
  open_servo.write(104); //Initiates servos to the correct positions
  delay(150);
  open_servo.write(108); //Initiates servos to the correct positions
  delay(150);
  open_servo.write(112); //Initiates servos to the correct positions
  delay(150);
  open_servo.write(116); //Initiates servos to the correct positions
  delay(150);
  open_servo.write(120); //Initiates servos to the correct positions
  delay(150);
  open_servo.write(124); //Initiates servos to the correct positions
  delay(150);
  open_servo.write(128); //Initiates servos to the correct positions
  delay(150);
  open_servo.write(132); //Initiates servos to the correct positions
  delay(150);
  open_servo.write(136); //Initiates servos to the correct positions
  delay(150);
  open_servo.write(140); //Initiates servos to the correct positions
  delay(150);
  open_servo.write(144); //Initiates servos to the correct positions
  delay(150);
  open_servo.write(148); //Initiates servos to the correct positions
  delay(150);
  open_servo.write(152); //Initiates servos to the correct positions
  delay(150);
  open_servo.write(157); //Initiates servos to the correct positions
  delay(150);
  open_servo.write(162); //Initiates servos to the correct positions
  delay(150);
  open_servo.write(165); //Initiates servos to the correct positions


  delay(1800);

  //OPEN DOOR
  for (pos = END_OPEN; pos >= 100; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 15ms for the servo to reach the position
  }

  delay(1900);


  Aqua();
  delay(2400);


  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::Chartreuse;
  }
  FastLED.show();


  Blue();
  delay(2400);

  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::Crimson;
  }
  FastLED.show();
  delay(2100);


  // ARM TURN OFF BUTTON
  for (pos = START_ARM; pos >= END_ARM; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    arm_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(4);                       // waits 15ms for the servo to reach the position
  }

  for (pos = END_ARM; pos <= START_ARM; pos += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    arm_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(4);                       // waits 15ms for the servo to reach the position
  }

  //CLOSE DOOR
  for (pos = 100 ; pos <= END_OPEN; pos += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(6);                       // waits 15ms for the servo to reach the position
  }



  FastLED.clear (); //TURN LED's off
  FastLED.show();
}


void Scene60()///////////////////////////////DJ Earworm - United State of pop
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(60);
  theaterChaseRainbow(50);
  SWITCH_FAST();
}

void Scene61()///////////////////////////////Emperor's New Groove - Tell Us Where The Talking Llama IS.ad4
{

  MOVE_FORWARD();
  INITIALIZE_VARIABLES();
  startSoundInBackground(61);

  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::DodgerBlue;
  }
  FastLED.show();

  delay(25500);
  SWITCH_FAST();

  FastLED.clear (); //TURN LED's off
  FastLED.show();
}

void Scene62()///////////////////////////////Hi, I'm ChandlerI Make Jokes When Im Uncomfortable.ad4
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(62);
  delay(2000);
  SWITCH_FAST();
}


void Scene63()///////////////////////////////ALRIGHTY THEN
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(63);
  int loop1 = 0;

  Aqua();

  delay(1600);

  while (loop1 < 10)
  {
    for (int i = 0; i < NUM_LEDS; ++i)
    {
      leds[i] = CRGB::SpringGreen;
    }
    FastLED.show();
    delay(60);
    loop1++;

    FastLED.clear (); //TURN LED's off
    FastLED.show();

    delay(60);
  }

  Blue();

  delay(2000);
  SWITCH_FAST();
  FastLED.clear (); //TURN LED's off
  FastLED.show();


}

void Scene64()///////////////////////////////It's So Fluffy!.ad4
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(65);
  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::OrangeRed;
  }
  FastLED.show();

  delay(4000);
  SWITCH_FAST();

  MOVE_FORWARD();

  FastLED.clear (); //TURN LED's off
  FastLED.show();
}



void Scene65()///////////////////////////////Jack Johnson - All At Once.ad4
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(66);
  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::DarkOrchid;
  }
  FastLED.show();
  delay(37000);

  SWITCH_FAST();

  FastLED.clear (); //TURN LED's off
  FastLED.show();
}


void Scene66()///////////////////////////////Louis Armstrong- What a wonderful world.ad4
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(67);
  delay(4200);

  Blue();
  delay(4400);
  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::White;
  }
  FastLED.show();
  delay(3500);

  Gold();

  delay(3000);
  FastLED.clear (); //TURN LED's off
  FastLED.show();

  delay(6000);

  Purple();
  delay(9000);
  SWITCH_FAST();
  FastLED.clear (); //TURN LED's off
  FastLED.show();
}

void Scene67()///////////////////////////////Pongo judges other dogs - 101 Dalmatians
{


  int loop1 = 0;
  INITIALIZE_VARIABLES();
  delay(2000);

  //OPEN DOOR
  for (pos = END_OPEN; pos >= 150; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(30);                       // waits 15ms for the servo to reach the position
  }

  startSoundInBackground(68);
  digitalWrite(SERVO_POWER, LOW);

  while (loop1 < 5) {
    rainbow(20);
    loop1++;
  }

  digitalWrite(SERVO_POWER, HIGH);

  SWITCH_FAST();
  FastLED.clear (); //TURN LED's off
  FastLED.show();
}


void Scene68()///////////////////////////////Monsters Inc - All Roz Scenes!.ad4
{
  INITIALIZE_VARIABLES();

  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::Navy;
  }
  FastLED.show();

  startSoundInBackground(69);
  delay(25000);
  SWITCH_FAST();

  FastLED.clear (); //TURN LED's off
  FastLED.show();
}


void Scene69()///////////////////////////////Rachel Platten - Stand By You Lyrics.ad4
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(70);
  delay(2000);
  PEAK();
  delay(4000);
  SWITCH_FAST();
}

void Scene70()///////////////////////////////Ruth B - Lost Boy
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(72);

  delay(8300);
  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::LawnGreen;
  }
  FastLED.show();
  delay(4000);
  FastLED.clear (); //TURN LED's off
  FastLED.show();


  delay(8500);
  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::LawnGreen;
  }
  FastLED.show();

  delay(9000);
  SWITCH_FAST();
  FastLED.clear (); //TURN LED's off
  FastLED.show();

}


void Scene71()///////////////////////////////Tiger Roar & Growl SOUND EFFECT.ad4
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(73);

  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::SpringGreen;
  }
  FastLED.show();

  //////////////OPEN DOOR
  delay(1000);
  for (pos = END_OPEN; pos >= 110; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(65);                       // waits 15ms for the servo to reach the position
  }
  open_servo.write(120); //Initiates servos to the correct positions

  delay(1500);

  for (pos = END_OPEN; pos >= 90; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(30);                       // waits 15ms for the servo to reach the position
  }

  for (pos = 90 ; pos <= 135; pos += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(60);                       // waits 15ms for the servo to reach the position
  }

  delay(1000);
  SWITCH_FAST();
  FastLED.clear (); //TURN LED's off
  FastLED.show();
  delay(500);
}



void Scene72()////////////////////////////////mulan -  I'll make a man out of you SECOND REPRINT
{
  Scene33();
}


void Scene73()////////////////////////////////Thunderstorm and Rain
{
  int loop1 = 0;

  INITIALIZE_VARIABLES();
  startSoundInBackground(75);

  delay(3300);
  while (loop1 < 2)
  {
    for (int i = 0; i < NUM_LEDS; ++i)
    {
      leds[i] = CRGB::White;
      leds[i].fadeToBlackBy( 240 );
    }
    FastLED.show();


    delay(100);

    FastLED.clear (); //TURN LED's off
    FastLED.show();
    delay(100);
    loop1++;
  }
  loop1 = 0;

  delay(8000);

  while (loop1 < 2)
  {
    for (int i = 0; i < NUM_LEDS; ++i)
    {
      leds[i] = CRGB::White;
      leds[i].fadeToBlackBy( 200 );
    }
    FastLED.show();

    delay(100);

    FastLED.clear (); //TURN LED's off
    FastLED.show();
    delay(100);
    loop1++;
  }


  delay(3800);
  loop1 = 0;

  while (loop1 < 2)
  {
    for (int i = 0; i < NUM_LEDS; ++i)
    {
      leds[i] = CRGB::White;
    }
    FastLED.show();
    delay(100);

    FastLED.clear (); //TURN LED's off
    FastLED.show();
    delay(100);
    loop1++;
  }

  delay(10200);


  SWITCH_FAST();

  FastLED.clear (); //TURN LED's off
  FastLED.show();
}

void Scene74()///////////////////////////////Common loon
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(77);

  delay(10000);
  SWITCH_FAST();

}


void Scene75()///////////////////////////////This will destroy you
{
  int loop1 = 0;

  INITIALIZE_VARIABLES();
  startSoundInBackground(78);
  delay(6000);
  while (loop1 < 33)
  {
    Gold();
    delay(500);

    FastLED.clear (); //TURN LED's off
    FastLED.show();
    delay(500);

    loop1++;
  }

  SWITCH_FAST();
  FastLED.clear (); //TURN LED's off
  FastLED.show();

}

void Scene76()  //Finding Nemo Dory speaking whale
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(79);
  delay(27000);

  SWITCH_FAST();
}



void Scene77()    //Fleetwood Mac Dont stop
{
  int loop1 = 0;
  INITIALIZE_VARIABLES();
  startSoundInBackground(81);
  delay(6000);
  while (loop1 < 21)
  {
    Gold();

    delay(75);

    FastLED.clear (); //TURN LED's off
    FastLED.show();
    delay(75);
    loop1++;
  }

  delay(10000);

  SWITCH_FAST();
  FastLED.clear (); //TURN LED's off
  FastLED.show();
}

//
void Scene78()          //Efterklang - Hands Playing Butterfly
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(82);

  delay(27000);
  SWITCH_SLOW();
}

//
void Scene79()          //Friends - Oh My God!
{
  int loop1 = 0;

  INITIALIZE_VARIABLES();
  startSoundInBackground(85);

  delay(2000);
  MOVE_FORWARD();
  SWITCH_FAST();
}

//
void Scene80()          //Iron giant My favorite Part
{
  int loop1 = 0;

  INITIALIZE_VARIABLES();
  startSoundInBackground(86);

  delay(4000);
  open_servo.write(140);
  delay(600);
  open_servo.write(150);
  delay(400);
  open_servo.write(165);

  delay(1800);

  open_servo.write(130);
  delay(1200);

  colorWipe(strip.Color(25, 25, 25), 10); // White
  colorWipe(strip.Color(50, 0, 0), 10); // Red
  colorWipe(strip.Color(0, 50, 0), 10); // Green
  colorWipe(strip.Color(0, 0, 50), 10); // Blue
  colorWipe(strip.Color(50, 0, 0), 10); // Red
  delay(1000);

  Gold();

  while (loop1 < 5)
  {
    //close door
    for (pos = 130 ; pos <= END_OPEN; pos += 1) // goes from 0 degrees to 180 degrees
    { // in steps of 1 degree
      open_servo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(20);                       // waits 15ms for the servo to reach the position
    }


    //open door
    for (pos = END_OPEN; pos >= START_OPEN; pos -= 1) // goes from 180 degrees to 0 degrees
    {
      open_servo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(60);                       // waits 15ms for the servo to reach the position
    }
    loop1++;
  }
  SWITCH_FAST();
  delay(1000);
  FastLED.clear (); //TURN LED's off
  FastLED.show();
}


void Scene81()          //Storks Covering for myself
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(87);
  Gold();

  delay(4000);
  MOVE_FORWARD();

  SWITCH_FAST();

  FastLED.clear (); //TURN LED's off
  FastLED.show();
}


void Scene82()          //A Dog's Purpose - Official Trailer (HD)
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(89);
  Red();

  delay(14000);
  SWITCH_FAST();

  FastLED.clear (); //TURN LED's off
  FastLED.show();
}

void Scene83()          //Adele When We Were Young Lyrics
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(90);

  Aqua();

  delay(56000);
  SWITCH_FAST();

  FastLED.clear (); //TURN LED's off
  FastLED.show();
}



void Scene84()          //BAILY_BAILY_BAILY
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(91);
  Aqua();

  delay(7000);
  SWITCH_FAST();

  FastLED.clear (); //TURN LED's off
  FastLED.show();
}

void Scene85()          //Blackbird
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(92);
  Aqua();

  delay(19000);
  SWITCH_SLOW();

  FastLED.clear (); //TURN LED's off
  FastLED.show();
}

void Scene86()          //BFG don't eat me
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(94);
  Blue();

  delay(11000);
  SWITCH_FAST();

  FastLED.clear (); //TURN LED's off
  FastLED.show();
}


void Scene87()          //Ghost in shell song
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(93);
  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::Pink;
  }
  FastLED.show();

  delay(27000);
  SWITCH_FAST();

  FastLED.clear (); //TURN LED's off
  FastLED.show();
}


void Scene88()          //Walk on the wild side
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(95);
  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::DeepPink;
  }
  FastLED.show();

  delay(31000);
  SWITCH_FAST();
  MOVE_FORWARD();
  FastLED.clear (); //TURN LED's off
  FastLED.show();
}


void Scene89()          //STORKS_teach.ad4
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(96);
  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::LightSkyBlue;
  }
  FastLED.show();

  delay(7000);
  SWITCH_FAST();

  FastLED.clear (); //TURN LED's off
  FastLED.show();
}


void Scene90()          //Storks (2016)  funny wolf transformation.ad4
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(97);

  delay(6000);
  SWITCH_FAST();
}


void Scene91()          //STORKS_KEEPING_IT
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(98);

  delay(9000);
  SWITCH_FAST();

}


void Scene92()          // STORKS_ALPHA.mp3
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(99);
  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::MediumSpringGreen;
  }
  FastLED.show();



  delay(55000);
  SWITCH_FAST();

  FastLED.clear (); //TURN LED's off
  FastLED.show();
}

void Scene93()          //Storks (1080p HD) - Wolf Transfomer - Best Scene
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(100);
  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::Turquoise;
  }
  FastLED.show();



  delay(6000);
  SWITCH_FAST();
  FastLED.clear (); //TURN LED's off
  FastLED.show();
}




void Scene94()          //Storks (1080p HD) - Wolf Transfomer - Best Scene
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(102);
  Blue();

  delay(31000);
  SWITCH_FAST();
  FastLED.clear (); //TURN LED's off
  FastLED.show();
}

void Scene95()          //Silent fight Gentrifying neighborhood
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(103);

  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::Fuchsia;
  }
  FastLED.show();



  delay(12000);
  SWITCH_FAST();
  FastLED.clear (); //TURN LED's off
  FastLED.show();
}

//////
void Scene96()          //Castle theme song
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(104);

  Gold();

  delay(6000);
  SWITCH_FAST();
  FastLED.clear (); //TURN LED's off
  FastLED.show();

}


void Scene97()          //castle rugged and handsom
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(105);

  Gold();

  delay(4000);
  SWITCH_FAST();
  FastLED.clear (); //TURN LED's off
  FastLED.show();

}


void Scene98()          //Kevin MacLeod ~ Future Gladiator
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(106);

  Red();

  delay(27000);

  SWITCH_FAST();

  FastLED.clear (); //TURN LED's off
  FastLED.show();
}

/////////////////////////////////////////////////////////////////////////
void Scene99()          //Scene96_BREATHE
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(88);
  Green();

  delay(56000);
  SWITCH_FAST();

  FastLED.clear (); //TURN LED's off
  FastLED.show();
}

/////////////////////////////////////////////////////////////////////////
void Scene101()        //BONUS TRACK ONE SCARS
{
  INITIALIZE_VARIABLES();
  startSoundInBackground(80);

  delay(5500);

  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::White;
  }
  FastLED.show();

  delay(2500);

  Gold();
  delay(1500);

  Purple();
  delay(10500);

  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::White;
  }
  FastLED.show();
  delay(1800);

  Gold();
  delay(2000);

  //OPEN DOOR
  for (pos = END_OPEN; pos >= START_OPEN; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(20);                       // waits 15ms for the servo to reach the position
  }
  digitalWrite(SERVO_POWER, LOW);

  rainbowCycle(5);
  rainbowCycle(5);
  rainbowCycle(5);

  digitalWrite(SERVO_POWER, HIGH);

  //////////////////////// ARM TURN OFF BUTTON

  for (pos = START_ARM; pos >= END_ARM; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    arm_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(10);                       // waits 15ms for the servo to reach the position
  }
  for (pos = END_ARM; pos <= START_ARM; pos += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    arm_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(10);                       // waits 15ms for the servo to reach the position
  }

  ////////////////////////// CLOSE DOOR
  for (pos = START_OPEN ; pos <= END_OPEN; pos += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(20);                       // waits 15ms for the servo to reach the position
  }

  FastLED.clear (); //TURN LED's off
  FastLED.show();

}


/////////////////////////////////////////////////////////////////////////
void Scene100()          //Final_bonus_Beth Hart_Leave the light on 1.ad4
{

  INITIALIZE_VARIABLES();
  startSoundInBackground(83);
  delay(32000);

  for (pos = END_OPEN; pos >= START_OPEN; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(60);                       // waits 15ms for the servo to reach the position
  }
  delay(8000);

  digitalWrite(SERVO_POWER, LOW);
  Gold();
  delay(247000);

  digitalWrite(SERVO_POWER, HIGH);
  //////////////////////// ARM TURN OFF BUTTON

  for (pos = START_ARM; pos >= END_ARM; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    arm_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(30);                       // waits 15ms for the servo to reach the position
  }
  for (pos = END_ARM; pos <= START_ARM; pos += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    arm_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(30);                       // waits 15ms for the servo to reach the position
  }

  ////////////////////////// CLOSE DOOR
  for (pos = START_OPEN ; pos <= END_OPEN; pos += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    open_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(20);                       // waits 15ms for the servo to reach the position
  }
}



void resetModule() {

  digitalWrite(resetPin, HIGH);
  digitalWrite(resetPin, LOW);
  delay(5);

  digitalWrite(resetPin, HIGH);
  digitalWrite(clockPin, HIGH);

  // * Wait *
  delay(700);

  // * Volume has to be "FULL VOLUME" 7  ... must be set, because after Power on or Reset Button sometimes the sound will not start! *
  sendCommand(0xfff7);
}


void startSoundInBackground(int soundNumber) {

  sendCommand(soundNumber);
}

void startSoundAndWaitTilEnd(int soundNumber) {

  sendCommand(soundNumber);

  busyPinState = digitalRead(busyPin);
  while (busyPinState == HIGH) busyPinState = digitalRead(busyPin);
}

void stopSound() {
  sendCommand(0xFFFF);
}


// * Volume 0-4 (0 = Off) *
void setVolume(int targetVol) {

  // * the value can not be lower 4, volume < 4 = random volume modulation => distrotion *
  if (targetVol >= 0 && targetVol < 5) {

    if (targetVol == 0)   sendCommand(0xFFF0);
    else {

      if (targetVol > volume) {

        int steps = targetVol - volume;
        for (int i = 0; i < steps; i++) {
          volume++;
          sendCommand(0xFFF0 + 3 + volume);
          delay(800); // * without this delay... the sound will destroyed *
        }
      }
      else if (targetVol < volume) {

        int steps = volume - targetVol;
        for (int i = 0; i < steps; i++) {
          volume--;
          sendCommand(0xFFF0 + 3 + volume);
          delay(800); // * without this delay... the sound will destroyed *
        }
      }
    }
  }
}


void sendCommand(unsigned int command) {

  //Start bit Low level pulse.
  digitalWrite(clockPin, LOW);
  delay(2);


  // * 0x8000 = 100000000 00000000 = 16 Bits, Send Command as Serial Transfer *
  for (unsigned int mask = 0x8000; mask > 0; mask >>= 1) {

    digitalWrite(clockPin, LOW);
    delayMicroseconds(50);

    // * Set Data-Bit on the Line *
    if (command & mask) {


      digitalWrite(dataPin, HIGH);
    }
    else  {


      digitalWrite(dataPin, LOW);
    }

    // * Wait 50us *
    delayMicroseconds(50);

    // * Clock: High ... time to get Data-Bit *
    digitalWrite(clockPin, HIGH);

    // * Wait some time to give time to read the bit (possible values: 5-6500us) *
    delayMicroseconds(50);
  }

  // * Give Busy time to go "High" *
  delay(50);
}


////////////////////////////////////////////////////////////////
void (*doScene[])()
{
  Scene0,
  Scene1,
  Scene2,
  Scene3,
  Scene4,
  Scene5,
  Scene6,
  Scene7,
  Scene8,
  Scene9,
  Scene10,
  Scene11,
  Scene12,
  Scene13,
  Scene14,
  Scene15,
  Scene16,
  Scene17,
  Scene18,
  Scene19,
  Scene20,
  Scene21,
  Scene22,
  Scene23,
  Scene24,
  Scene25,
  Scene26,
  Scene27,
  Scene28,
  Scene29,
  Scene30,
  Scene31,
  Scene32,
  Scene33,
  Scene34,
  Scene35,
  Scene36,
  Scene37,
  Scene38,
  Scene39,
  Scene40,
  Scene41,
  Scene42,
  Scene43,
  Scene44,
  Scene45,
  Scene46,
  Scene47,
  Scene48,
  Scene49,
  Scene50,
  Scene51,
  Scene52,
  Scene53,
  Scene54,
  Scene55,
  Scene56,
  Scene57,
  Scene58,
  Scene59,
  Scene60,
  Scene61,
  Scene62,
  Scene63,
  Scene64,
  Scene65,
  Scene66,
  Scene67,
  Scene68,
  Scene69,
  Scene70,
  Scene71,
  Scene72,
  Scene73,
  Scene74,
  Scene75,
  Scene76,
  Scene77,
  Scene78,
  Scene79,
  Scene80,
  Scene81,
  Scene82,
  Scene83,
  Scene84,
  Scene85,
  Scene86,
  Scene87,
  Scene88,
  Scene89,
  Scene90,
  Scene91,
  Scene92,
  Scene93,
  Scene94,
  Scene95,    //Storks gentrifying neighborhood   //so it's zero to 95 without bonus tracks


  Scene96,
  Scene97,
  Scene98,




  Scene101,    //BONUS TRACK ONE    //plays at 94
  Scene99,    //BONUS TRACK TWO
  Scene100    //BONUS TRACK THREE   //leave the light on


  //BONUS TRACKS ONLY PLAY WHEN SWITCH HAS BEEN HIT _ NUMBER OF TIMES
};


void loop()
{
  int loop1 = 0, loop2 = 0, loop3 = 0, loop4 = 0, loop5 = 0, loop6 = 0, loop7 = 0, loop8 = 0, loop9 = 0, loop10 = 0, loop11 = 0, loop12 = 0, loop13 = 0;

  digitalWrite (LED_POWER, HIGH);
  INITIALIZE_VARIABLES();
  digitalWrite (SERVO_POWER, HIGH);

  buttonState = digitalRead(hit_switch);

  if (buttonState == LOW)
  {
    delay(1000);

    if (loop_A == 65)
    {

      Scene101();             //FIRST BONUS SCENE            DONE
    }


    else if (loop_A == 85)
    {
      Scene99();               //Just Breathe            DONE
    }

    else if (loop_A == 100)
    {

      Scene100();              //Leave the light on           DONE
      loop_A = 0;
    }

    else if (buttonState == LOW )
    {
      randNumber = Entropy.random(99);   //pick a random number from 0 to 98

      if (randNumber == 17)  //eye of the tiger
      {
        loop1++;
      }

      if (randNumber == 71)   //roaring tiger
      {
        loop2++;
      }

      if (randNumber == 80)  //iron giant
      {
        loop3++;
      }


      if (randNumber == 1)  //angels
      {
        loop4++;
      }

      if (randNumber == 31)  //Same macklemore
      {
        loop5++;
      }

      if (randNumber == 35)  //Not lucky
      {
        loop6++;
      }

      if (randNumber == 49)  //we will rock you
      {
        loop7++;
      }


      if (randNumber == 45)  //monk
      {
        loop9++;
      }


      if (randNumber == 48)  //monk
      {
        loop10++;
      }


      if (randNumber == 72)  //Man out of you
      {
        loop8++;
      }


      if (randNumber == 76)  //Finding dory
      {
        loop11++;
      }


      if (randNumber == 92)  //storks alpha
      {
        loop12++;
      }


      if (randNumber == 59)  //rinbow music
      {
        loop13++;
      }

      ///////////////////////////////////////////////////////
      if ( loop_A == 30 && loop8 == 0)
      {
        randNumber = 72;
      }

      if ( loop_A == 55 && loop6 == 0)
      {
        randNumber = 35;
      }

      if ( loop_A == 78 && loop7 == 0)
      {
        randNumber = 35; //not lucky
      }

      if ( loop_A == 73 && loop5 == 0)
      {
        randNumber = 31; //maclemore
      }


      if ( loop_A == 81 && loop2 == 0)
      {
        randNumber = 71; //Tiger
      }


      if ( loop_A == 90 && loop3 == 0)
      {
        randNumber = 80; //iron giant
      }


      if ( loop_A == 93 && loop13 == 0)
      {
        randNumber = 59; //Rainbow music
      }


      if ( loop_A == 61 && loop1 == 0)
      {
        randNumber = 17; //eye of the tiger
      }

      if ( loop_A == 71 && loop4 == 0)
      {
        randNumber = 1; //angels
      }

      if (randNumber == 45 && loop9 > 0)
      {
        randNumber = Entropy.random(99);
      }


      if (randNumber == 48 && loop10 > 0)
      {
        randNumber = Entropy.random(99);
      }

      if (randNumber == 76 && loop11 > 0)
      {
        randNumber = Entropy.random(99);
      }

      if (randNumber == 92 && loop12 > 0)
      {
        randNumber = Entropy.random(99);
      }

      doScene[randNumber]();
    }


    loop_A++;
    delay(1000);
    
    /*{
      //int i = validly_constrained_random_number;
      // doScene[i]();
      for (uint8_t i = 0; i < 3; i++)
      {
        (*myPointer[i])();
      }
    }
    */

  }

}


//Scene99();               //Just Breathe
//Scene100();              //Leave the light on           DONE
//Scene101();             //FIRST BONUS SCENE            DONE

/*
    Scene1();                     //Angels                        DONE
    Scene2();                     //Baby Got Back                 DONE
    Scene3();                     //Can't Touch this              DONE
    Scene4();                     //Keep Ya head up               DONE
    Scene5();                     //101 Dalmation Puppies         DONE
    Scene6();                     //Baby it's cold Outside        DONE
    Scene7();                     //BIBIO LOVERS'S CARVINGS       DONE
    Scene8();                     //Blow movie quote              DONE
    Scene9();                     //Bruins goal                   DONE
    Scene10();                    //Led Zeplin California         DONE
    Scene11();                    //Chanler VS Phoebe             DONE
    Scene12();                    //P Diddy Coming home           DONE
    Scene13();                    //Daft Punk One more Time       DONE
    Scene14();                    //Red hot Dark Neccesitiyes     DONE
    Scene15();                    //Drops of Jupiter              DONE
    Scene16();                    //Eminem Lose Yourself          DONE
    Scene17();                    //Eye of the Tiger              DONE  //first bonus track here  7:30 so far
    Scene18();                    //james Taylor Fire and Rain    DONE
    Scene19();                    //Friends Lighting round        DONE
    Scene20();                    //Friends theme song            DONE
    Scene21();                    //Kanye Good Friday             DONE
    Scene22();                    //Soul but not a Soldier        DONE  //11 minutes in
    Scene23();                    //Greatest speech               DONE
    Scene24();                    //Meg meets Herculees           DONE
    Scene25();                    //I'm Joey                      DONE
    Scene26();                    //Inner ninja                   DONE
    Scene27();                    //Imagine John Lenon            DONE
    Scene28();                    //Kanye Homecoming              DONE
    Scene29();                    //Kung Fu fighting              DONE
    Scene30();                    //Lilo and Stitch               DONE
    Scene31();                    //Same Macklemore               DONE
    Scene32();                    //Macklemore 10,000 hours       DONE
    Scene33();                    //Mulan Make a Man out of you   DONE        15 minutes in
    Scene34();                    //Mulan Panda scene             DONE            //Make car move on this scene
    Scene35();                    //Mulan-youre not lucky?        DONE
    Scene36();                    //Beautiful girls               DONE
    Scene37();                    //One man can change the world  DONE
    Scene38();                    //Peter fan Fly                 DONE
    Scene39();                    //Pink Floyd Us and Them        DONE
    Scene40();                    //Ray Lamontange empty          DONE
    Scene41();                    //Rent Measure a year           DONE
    Scene42();                    //Rio2                          DONE
    Scene43();                    //Tuac rose from concrete       DONE
    Scene44();                    //Sun Queen                     DONE
    Scene45();                    //The Monk                      DONE        22 minutes in
    Scene46();                    //Peter Pan 20 boys             DONE
    Scene47();                    //Queen Under Pressure          DONE
    Scene48();                    //Underdog speech               DONE
    Scene49();                    //We will rock you              DONE
    Scene50();                    //Wouldn't get far              DONE
    Scene51();                    //you've got a friend           DONE
    Scene52();                    //Jack Johnson Upside Down      DONE
    Scene53();                    //REMIX TO IGNITION             DONE
    Scene54();                    //I was the Pile of Coats       DONE
    Scene55();                    //WALL-E ending                 DONE
    Scene56();                    //Kanye-Streetlights            DONE
    Scene57();                    //U2 Dartmouth opening          DONE
    Scene58();                    //Austin powers theme song      DONE
    Scene59();                    //Rainbow MUSIC                 DONE
    Scene60();                    //United states of POP          DONE
    Scene61();                    //Talking llama                 DONE  33 minutes in    ///30 minutes in the second time
    Scene62();                    //Chanlder uncomfortable        DONE
    Scene63();                    //Alrighty then                 DONE
    Scene64();                    //So fluffly                    DONE
    Scene65();                    //Jack Johnson-All at once      DONE
    Scene66();                    //Louis Armstrong-Wndfl world   DONE
    Scene67();                    //101 Dalmatians-Dog judging    DONE
    Scene68();                    //Monsters Inc                  DONE
    Scene69();                    //Stand By You Lyrics           DONE
    Scene70();                    //Ruth B - Lost Boy             DONE
    Scene71();                    //TIGER ROARING                 DONE
    Scene72();                    //Mulan-MAN out of you#2        DONE
    Scene73();                    //Thunderstorm and Rain         DONE
    Scene74();                    //Common Loon                   DONE 40 minutes in here
    Scene75();                    //THis will destory you         DONE
    Scene76();                    //Finding_nemo                  DONE       //second time through 36 seconds through
    Scene77();                    //Fleetwood mac Dont stop       DONE
    Scene78();                    //Efterklang- Butterflys        DONE
    Scene79();                    //Friends - Oh My God!          DONE
    Scene80();                    //Iron giant My favorite Part   DONE   42 minutes
    Scene81();                    //Storks Covering for myself
    Scene82();                    //A Dog's Purpose - Official Trailer (HD)
    Scene83();                    //Adele When We Were Young Lyrics
    Scene84();                    //BAILY_BAILY_BAILY
    Scene85();                    //Blackbird                          //45 minutes  ///40 minutes in secodn time
    Scene86();                    //BFG this I'd never
    Scene87();                    //Ghost in shell song
    Scene88();                    //Walk on the wild side
    Scene89();                    //STORKS_teach.ad4
    Scene90();                    //Storks (2016)  funny wolf transformation.ad4      //where is storks neighborhood?
    Scene91();                    //STORKS_KEEPING_IT
    Scene92();                    //STORKS_ALPHA.mp3
    Scene93();                    //Storks (1080p HD) - Wolf Transfomer - Best Scene  //running into glass
    Scene94();                    //SLide goo goo dolls              //48 mins  here
    Scene95();                    //STORKS Gentrifying neighborhood
    Scene96();                    //castle theme song
    Scene97();                    //rugged and handsome from castle
    Scene98();                    //Kevin Mcleod Gladiator drums

    Scene99();                    //Just Breathe BONUS TRACK ONE
    Scene100();                    //Leave the light on           DONE
    Scene101();                    //FIRST BONUS SCENE            DONE

*/

/*   //plays highlight reel...(The Top 13 scenes)

    else if (buttonState == LOW && loop_A == 0)
    {
      Scene1(); //angels
    }

    else if (buttonState == LOW && loop_A == 1)
    {
      Scene17();  //eye of the tiger
    }

    else if (buttonState == LOW && loop_A == 2)
    {
      Scene73();        ///thunderstorm
    }

    else if (buttonState == LOW && loop_A == 3)
    {
      Scene16();   ///eminem
    }

    else if (buttonState == LOW && loop_A == 4)
    {
      Scene71();  //tiger roaring
    }

    else if (buttonState == LOW && loop_A == 5)
    {
      Scene79();  //Friends oh my god//Move forward
    }

    else if (buttonState == LOW && loop_A == 6)
    {
      Scene80();   //iron giant
    }

    else if (buttonState == LOW && loop_A == 7)
    {
      Scene59(); ///rainbow music
    }

    else if (buttonState == LOW && loop_A == 8)
    {
      Scene26(); //inner ninja
    }

    else if (buttonState == LOW && loop_A == 9)
    {
      Scene13(); //One more time
    }

    else if (buttonState == LOW && loop_A == 10)
    {
      Scene63();  //alrighty then
    }

    else if (buttonState == LOW && loop_A == 11)
    {
      Scene31();    //macklemore SAME
    }

    else if (buttonState == LOW && loop_A == 12)
    {
      Scene35();
      loop_A = 0;
    }


    else if (buttonState == LOW && loop_A == 13)
    {
      Scene49();
      loop_A = 0;
    }
*/



