#include "FastLED.h"

FASTLED_USING_NAMESPACE

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

// Params for width and height
const uint8_t kMatrixWidth = 16;
const uint8_t kMatrixHeight = 16;
#define MAX_DIMENSION ((kMatrixWidth>kMatrixHeight) ? kMatrixWidth : kMatrixHeight)
#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
// Param for different pixel layouts
const bool    kMatrixSerpentineLayout = true;

// The 32bit version of our coordinates
static uint16_t x;
static uint16_t y;
static uint16_t z;

// We're using the x/y dimensions to map to the x/y pixels on the matrix.  We'll
// use the z-axis for "time".  speed determines how fast time moves forward.  Try
// 1 for a very slow moving effect, or 60 for something that ends up looking like
// water.
// uint16_t speed = 1; // almost looks like a painting, moves very slowly
uint16_t speed = 66; // a nice starting speed, mixes well with a scale of 100
// uint16_t speed = 33;
// uint16_t speed = 100; // wicked fast!

// Scale determines how far apart the pixels in our noise matrix are.  Try
// changing these values around to see how it affects the motion of the display.  The
// higher the value of scale, the more "zoomed out" the noise iwll be.  A value
// of 1 will be so zoomed in, you'll mostly see solid colors.

// uint16_t scale = 1; // mostly just solid colors
// uint16_t scale = 4011; // very zoomed out and shimmery
uint16_t scale = 311;

// This is the array that we keep our computed noise values in
uint8_t noise[MAX_DIMENSION][MAX_DIMENSION];


uint16_t XY( uint8_t x, uint8_t y)
{
  uint16_t i;

  if ( kMatrixSerpentineLayout == false) {
    i = (y * kMatrixWidth) + x;
  }

  if ( kMatrixSerpentineLayout == true) {
    if ( y & 0x01) {
      // Odd rows run backwards
      uint8_t reverseX = (kMatrixWidth - 1) - x;
      i = (y * kMatrixWidth) + reverseX;
    } else {
      // Even rows run forwards
      i = (y * kMatrixWidth) + x;
    }
  }

  return i;
}

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
{
  CRGB::Orange,
  CRGB::Orange, // 'white' is too bright compared to red and blue
  CRGB::Orange,
  CRGB::Aqua,

  CRGB::Aqua,
  CRGB::Orange,
  CRGB::Orange,
  CRGB::Orange,

  CRGB::Aqua,
  CRGB::Aqua,
  CRGB::Aqua,
  CRGB::Aqua,
  CRGB::Aqua,
  CRGB::Aqua,
  CRGB::Aqua,
  CRGB::Orange


};


CRGB endclr;
CRGB midclr;

#define DATA_PIN    9
//#define CLK_PIN   4
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define NUM_LEDS    16
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

// Initialize changeable global variables.
uint8_t max_bright = 128;                                     // Overall brightness definition. It can be changed on the fly.

// Most of these variables can be changed.
int ranamount = 50;                                           // The higher the number, lowers the chance for a pixel to light up.
uint8_t thisdelay = 50;                                       // Standard delay value.
bool boolcolours = 1;                                         // True = random, False = array of colours (up to 10)
uint8_t numcolours = 3;                                       // Number of colours in the array to display.
unsigned long colours[10] = {0xff0000, 0x00ff00, 0x0000ff, 0xffffff};  // Just assign the first 3 of these selectable colours.
uint8_t maxbar = 1;                                           // Maximum # of pixels to turn on in a row
uint8_t fadeval = 224;                                        // Fade rate
int stepper = 50;





/*********************************************************************
  This is an example for our nRF51822 based Bluefruit LE modules

  Pick one up today in the adafruit shop!

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  MIT license, check LICENSE for more information
  All text above, and the splash screen below must be included in
  any redistribution
*********************************************************************/


#include <Arduino.h>
#include <SPI.h>
#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
#include <SoftwareSerial.h>
#endif

#define BLUEFRUIT_HWSERIAL_NAME           Serial1

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "BluefruitConfig.h"
int led = 7;

/*=========================================================================
    APPLICATION SETTINGS

      FACTORYRESET_ENABLE       Perform a factory reset when running this sketch
     
                                Enabling this will put your Bluefruit LE module
                              in a 'known good' state and clear any config
                              data set in previous sketches or projects, so
                                running this at least once is a good idea.
     
                                When deploying your project, however, you will
                              want to disable factory reset by setting this
                              value to 0.  If you are making changes to your
                                Bluefruit LE device via AT commands, and those
                              changes aren't persisting across resets, this
                              is the reason why.  Factory reset will erase
                              the non-volatile memory where config data is
                              stored, setting it back to factory default
                              values.
         
                                Some sketches that require you to bond to a
                              central device (HID mouse, keyboard, etc.)
  //                              won't work at all with this feature enabled
                              since the factory reset will clear all of the
                              bonding data stored on the chip, meaning the
                              central device won't be able to reconnect.
    MINIMUM_FIRMWARE_VERSION  Minimum firmware version to have some new features
    MODE_LED_BEHAVIOUR        LED activity, valid options are
                              "DISABLE" or "MODE" or "BLEUART" or
                              "HWUART"  or "SPI"  or "MANUAL"
    -----------------------------------------------------------------------*/
#define FACTORYRESET_ENABLE         1
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
#define MODE_LED_BEHAVIOUR          "MODE"
/*=========================================================================*/

// Create the bluefruit object, either software serial...uncomment these lines
/*
  SoftwareSerial bluefruitSS = SoftwareSerial(BLUEFRUIT_SWUART_TXD_PIN, BLUEFRUIT_SWUART_RXD_PIN);

  Adafruit_BluefruitLE_UART ble(bluefruitSS, BLUEFRUIT_UART_MODE_PIN,
                      BLUEFRUIT_UART_CTS_PIN, BLUEFRUIT_UART_RTS_PIN);
*/

/* ...or hardware serial, which does not need the RTS/CTS pins. Uncomment this line */
Adafruit_BluefruitLE_UART ble(BLUEFRUIT_HWSERIAL_NAME, BLUEFRUIT_UART_MODE_PIN);

/* ...hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
//Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

/* ...software SPI, using SCK/MOSI/MISO user-defined SPI pins and then user selected CS/IRQ/RST */
//Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_SCK, BLUEFRUIT_SPI_MISO,
//                             BLUEFRUIT_SPI_MOSI, BLUEFRUIT_SPI_CS,
//                             BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);


// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

/**************************************************************************/
/*!
    @brief  Sets up the HW an the BLE module (this function is called
            automatically on startup)
*/
/**************************************************************************/
void setup(void)
{

  delay(3000); // 3 second delay for recovery

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  // uncomment the following lines if you want to see FPS count information
  // Serial.begin(38400);
  // Serial.println("resetting!");



  pinMode(led, OUTPUT);
  while (!Serial);  // required for Flora & Micro
  delay(500);

  Serial.begin(115200);
  Serial.println(F("Adafruit Bluefruit Command <-> Data Mode Example"));
  Serial.println(F("------------------------------------------------"));

  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ) {
      error(F("Couldn't factory reset"));
    }
  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in UART mode"));
  Serial.println(F("Then Enter characters to send to Bluefruit"));
  Serial.println();

  ble.verbose(false);  // debug info is a little annoying after this point!

  /* Wait for connection */
  while (! ble.isConnected()) {
    delay(500);
  }

  Serial.println(F("******************************"));

  // LED Activity command is only supported from 0.6.6
  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    // Change Mode LED Activity
    Serial.println(F("Change LED activity to " MODE_LED_BEHAVIOUR));
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
  }

  // Set module to DATA mode
  Serial.println( F("Switching to DATA mode!") );
  ble.setMode(BLUEFRUIT_MODE_DATA);

  Serial.println(F("******************************"));


  // Initialize our coordinates to some random values
  x = random8(2);
  y = random8(2);
  z = random8(2);
}

void fillnoise8() {
  for (int i = 0; i < MAX_DIMENSION; i++) {
    int ioffset = scale * i;
    for (int j = 0; j < MAX_DIMENSION; j++) {
      int joffset = scale * j;
      noise[i][j] = inoise8(x + ioffset, y + joffset, z);
    }
  }
  z += speed;
}




// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
//SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

/**************************************************************************/
/*!
    @brief  Constantly poll for new command or response data
*/
/**************************************************************************/
void loop(void)
{
  // Check for user input
  char n, inputs[BUFSIZE + 1];

  if (Serial.available())
  {
    n = Serial.readBytes(inputs, BUFSIZE);
    inputs[n] = 0;
    // Send characters to Bluefruit
    Serial.print("Sending: ");
    Serial.println(inputs);

    // Send input data to host via Bluefruit
    ble.print(inputs);
  }

  // Echo received data
  while ( ble.available() )
  {
    int c = ble.read();
    String stringOne = String((char)c);
    Serial.print(stringOne);

    if (stringOne == "r") {
      for ( int i = 0; i < 200; i++) {
        juggle();
        gHue++;
        FastLED.show();
        delay(40);
      }
      for ( int i = 0; i < 1; i++) {

        black();

      }



    } else if (stringOne == "g") {
      for ( int i = 0; i < 500; i++) {
        addGlitter(40);
        gHue++;
        delay(10);

      }

    }
    else if (stringOne == "b") {
      for ( int i = 0; i < 500; i++) {
        easeMe();
        gHue++;
        FastLED.show();
        delay(5);
        FastLED.show();
      }


    } else if (stringOne == "a") {

      for ( int i = 0; i < 5; i++) {

        wavea();
        waveb();
        black();


      }
      black();

    } else if (stringOne == "l") {

      for ( int i = 0; i < 2000; i++) {

        rain();
        FastLED.show();
        delay(5);

      }
      for ( int i = 0; i < 1; i++) {

        black();

      }



    } else if (stringOne == "c") {
      for ( int i = 0; i < 1500; i++) {

        bpm();
        FastLED.show();
        delay(1);

      }

    } else if (stringOne == "h") {

      for ( int i = 0; i < 2; i++) {

        darksky();

      }
      

      for ( int i = 0; i < 2; i++) {

        sky();

      }
      for ( int i = 0; i < 2; i++) {

        dawn();

      }
      for ( int i = 0; i < 2; i++) {

        sun();

      }

      for ( int i = 0; i < 1; i++) {

        black();

      }



    } else if (stringOne == "s") {
      //
      //      for ( int i = 0; i < 500; i++) {
      //
      //        snow();
      //        FastLED.show();
      //        delay(5);
      //
      //
      //      }

    } else if (stringOne == "t") {
      sinelon();

    } else if (stringOne == "f") {

    } else if (stringOne == "m") {

    } else if (stringOne == "h") {

    } else if (stringOne == "r") {

    }


    // insert a delay to keep the framerate modest
    //  FastLED.delay(1000/FRAMES_PER_SECOND);

    // do some periodic updates
    EVERY_N_MILLISECONDS( 20 ) {
      gHue++;  // slowly cycle the "base color" through the rainbow
      fadeToBlack();
    }

  }
}
void rainbow()
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter()
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter)
{
  if ( random8() < chanceOfGlitter) {
    CRGBPalette16 palette = myRedWhiteBluePalette_p;
    leds[ random16(NUM_LEDS) ] += CRGB::White;
    leds[ random16(NUM_LEDS) ] += CRGB::Black;
  }
}

void confetti()
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + 0, random8(255), 0);
}



void sinelon()
{
  // a colored dot sweeping
  // back and forth, with
  // fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS);
  static int prevpos = 0;
  if ( pos < prevpos ) {
    fill_solid( leds + pos, (prevpos - pos) + 1, CRGB::Black);
  } else {
    fill_solid( leds + prevpos, (pos - prevpos) + 1, CRGB::DarkOrange);
  }
  prevpos = pos;
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 5;
  CRGBPalette16 palette = myRedWhiteBluePalette_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for ( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 200;
  for ( int i = 0; i < 48; i++) {
    leds[beatsin16(i + 7, 0, NUM_LEDS)] |= CRGB(dothue, dothue, dothue);
    dothue += 16;
  }
}


void easeMe() {

  static uint8_t easeOutVal = 18;
  static uint8_t easeInVal  = 6;
  static uint8_t lerpVal    = 18;

  easeOutVal = ease8InOutQuad(easeInVal);
  easeInVal++;

  lerpVal = lerp8by8(0, NUM_LEDS, easeOutVal);

  leds[lerpVal] = CRGB::Blue;
  fadeToBlackBy(leds, NUM_LEDS, 16);                     // 8 bit, 1 = slow, 255 = fast
} // loop()

void pop_fade() {
  unsigned long thiscolour;
  if (ranamount > NUM_LEDS) ranamount = NUM_LEDS;              // Make sure we're at least utilizing ALL the LED's.
  int idex = random16(0, ranamount);

  if (idex < NUM_LEDS) {                                      // Only the lowest probability twinkles will do.
    boolcolours ? thiscolour = random(0x6699FF, 0xffffff) : thiscolour =  colours[random16(0, numcolours)];
    int barlen = random16(1, maxbar);
    for (int i = 0; i < barlen; i++)
      if (idex + i < NUM_LEDS) leds[idex + i] = thiscolour;   // Make sure we don't overshoot the array.
  }
  for (int i = 0; i < NUM_LEDS; i++) leds[i].nscale8(fadeval); // Go through the array and reduce each RGB value by a percentage.
} // pop_fade()



void ChangeMe() {
  uint8_t secondHand = (millis() / 1000) % 30;                // Increase this if you want a longer demo.
  static uint8_t lastSecond = 99;                             // Static variable, means it's only defined once. This is our 'debounce' variable.


  thisdelay = 350; colours[0] = 0x0099FF; colours[1] = 0x3300CC; colours[3] = 0x33CCFF; colours[4] = 0x000099; numcolours = 5; boolcolours = 0; fadeval = 192; maxbar = 8;



  //  if (lastSecond != secondHand) {
  //    lastSecond = secondHand;
  //    switch(secondHand) {
  //      case  0: thisdelay = 50; colours[0] = 0xffffff; numcolours=1; boolcolours=0; maxbar = 1; break;          // Just white twinkles
  //      case  5: thisdelay = 20; colours[1] = 0xff0000; numcolours=2; boolcolours=0; maxbar = 4; break;          // Add red and make bars
  //      case 15: thisdelay = 50; boolcolours=1; maxbar=1; break;                                                 // A pile of colours, 1 pixel in length
  //      case 20: thisdelay = 50; fadeval = 128; break;                                                           // Slow down the fade
  //      case 25: thisdelay = 50; colours[2]= 0x0000ff; boolcolours=0; numcolours=3; fadeval = 192; maxbar = 6; break;
  //      case 30: break;
  //    }
  //  } // if lastSecond
} // ChangeMe()

void weather() {
  uint8_t speed = 1;
  endclr = blend(CRGB::Aqua, CRGB::Aquamarine, speed);
  midclr = blend(CRGB::Aquamarine, CRGB::Aqua, speed);
  cycle();
  FastLED.show();
}
void weather2() {
  uint8_t speed = 1;
  endclr = blend(CRGB::Orange, CRGB::Aqua, speed);
  midclr = blend(CRGB::Aqua, CRGB::Orange, speed);
  cycle();
  FastLED.show();
}


void weather3() {
  uint8_t speed = 1;
  endclr = blend(CRGB::Orange, CRGB::DarkOrange, speed);
  midclr = blend(CRGB::DarkOrange, CRGB::Orange, speed);
  cycle();
  FastLED.show();
}
void cycle() {

  fill_gradient_RGB(leds, 0, endclr, NUM_LEDS / 2, midclr);
  fill_gradient_RGB(leds, NUM_LEDS / 2 + 1, midclr, NUM_LEDS, endclr);
}

void darksky() {
  // Move a single white led
  for (int whiteLed = 0; whiteLed < NUM_LEDS; whiteLed = whiteLed + 2 ) {
    // Turn our current led on to white, then show the leds
    leds[whiteLed] = CRGB::DarkBlue;
    leds[whiteLed + 1] = CRGB::DarkBlue;
    // Show the leds (only one of which is set to white, from above)
    FastLED.show();

    // Wait a little bit
    delay(100);

    // Turn our current led back to black for the next loop around
    leds[whiteLed] = CRGB::DeepSkyBlue;
  }
}

void sky() {
  // Move a single white led
  for (int whiteLed = 0; whiteLed < NUM_LEDS; whiteLed = whiteLed + 2 ) {
    // Turn our current led on to white, then show the leds
    leds[whiteLed] = CRGB::RoyalBlue;
    leds[whiteLed + 1] = CRGB::RoyalBlue;
    // Show the leds (only one of which is set to white, from above)
    FastLED.show();

    // Wait a little bit
    delay(100);

    // Turn our current led back to black for the next loop around
    leds[whiteLed] = CRGB::Aqua;
  }
}

void dawn() {
  // Move a single white led
  for (int whiteLed = 0; whiteLed < NUM_LEDS; whiteLed = whiteLed + 2 ) {
    // Turn our current led on to white, then show the leds
    leds[whiteLed] = CRGB::Yellow;
    leds[whiteLed + 1] = CRGB::Yellow;
    // Show the leds (only one of which is set to white, from above)
    FastLED.show();

    // Wait a little bit
    delay(100);

    // Turn our current led back to black for the next loop around
    leds[whiteLed] = CRGB::Orange;
  }
  fadeToBlack();
}
void sun() {
  // Move a single white led
  for (int whiteLed = 0; whiteLed < NUM_LEDS; whiteLed = whiteLed + 2 ) {
    // Turn our current led on to white, then show the leds
    leds[whiteLed] = CRGB::Orange;
    leds[whiteLed + 1] = CRGB::Orange;
    // Show the leds (only one of which is set to white, from above)
    FastLED.show();

    // Wait a little bit
    delay(100);

    // Turn our current led back to black for the next loop around
    leds[whiteLed] = CRGB::OrangeRed;
  }
  fadeToBlack();
}

void wavea() {
  // Move a single white led
  for (int whiteLed = 0; whiteLed < NUM_LEDS; whiteLed = whiteLed + 1 ) {
    // Turn our current led on to white, then show the leds
    leds[whiteLed] = CRGB::Blue;
    leds[whiteLed + 1] = CRGB::Blue;
    leds[whiteLed + 2] = CRGB::Blue;
    leds[whiteLed + 3] = CRGB::Blue;

    // Show the leds (only one of which is set to white, from above)
    FastLED.show();

    // Wait a little bit
    delay(50);

    // Turn our current led back to black for the next loop around
    leds[whiteLed] = CRGB::Aqua;
  }
}

void waveb() {
  // Move a single white led
  for (int whiteLed = 0; whiteLed < NUM_LEDS; whiteLed = whiteLed + 1 ) {
    // Turn our current led on to white, then show the leds
    leds[whiteLed] = CRGB::Aqua;
    leds[whiteLed + 1] = CRGB::Aqua;
    leds[whiteLed + 2] = CRGB::Aqua;
    leds[whiteLed + 3] = CRGB::Aqua;

    // Show the leds (only one of which is set to white, from above)
    FastLED.show();

    // Wait a little bit
    delay(50);

    // Turn our current led back to black for the next loop around
    leds[whiteLed] = CRGB::Blue;
  }
}


void black() {
  // Move a single white led
  for (int whiteLed = 0; whiteLed < NUM_LEDS; whiteLed = whiteLed + 1 ) {
    // Turn our current led on to white, then show the leds
    leds[whiteLed] = CRGB::Black;
    // Show the leds (only one of which is set to white, from above)
    FastLED.show();

    // Wait a little bit
    delay(10);

    // Turn our current led back to black for the next loop around
    leds[whiteLed] = CRGB::Black;
  }
}

void rain() {
  ChangeMe();                                                 // Muck those variable around.
  EVERY_N_MILLISECONDS(thisdelay) {                           // FastLED based non-blocking delay to update/display the sequence.
    pop_fade();                                                 // Simple call to the routine.
  }
  show_at_max_brightness_for_power();
} // loop()

void fadeToBlack()
{
  fadeToBlackBy( leds, NUM_LEDS, 1);
}


