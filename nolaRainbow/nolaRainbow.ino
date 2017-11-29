// Import library
#include <Adafruit_NeoPixel.h>

#define NEO_PIXEL_PIN 6
#define INTERRUPT_PIN 0  // RX pin on Flora
#define PIXELS 120

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXELS, NEO_PIXEL_PIN, NEO_GRB + NEO_KHZ800);

//Configuration
uint8_t Brightness = 50;
uint8_t SingleColorCycleSpeed = 20;
uint8_t RainbowColorCycleSpeed = 2;

uint32_t red = strip.Color(255, 0, 0);
uint32_t green = strip.Color(0, 255, 0);
uint32_t blue = strip.Color(0, 0, 255);
uint32_t yellow = strip.Color(255, 255, 0);
uint32_t cyan = strip.Color(0, 255, 255);
uint32_t magenta = strip.Color(255, 0, 255);
uint32_t white = strip.Color(255, 255, 255);

uint32_t saved_color;
boolean skip_to_next = false;


void setup() {
  /* Interrupt setup 
   * http://gammon.com.au/interrupts // interrupt code
   * https://learn.adafruit.com/assets/2845 // flora pin map
   * https://learn.adafruit.com/flora-tv-b-gone/pushbutton // adafruit button help
   */
  pinMode(INTERRUPT_PIN, INPUT_PULLUP); // configure pin to behave as input
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), switch_pressed, CHANGE);
  
  strip.begin();
  strip.setBrightness(Brightness);
  strip.show(); // Initialize all pixels to 'off'
}

// Interrupt Service Routine (ISR)
void switch_pressed()
{
    skip_to_next = true;
    flash(7);
}

void loop() {  
  rainbowCycle(RainbowColorCycleSpeed);
  colorWipe(green, SingleColorCycleSpeed);
  
  rainbow(RainbowColorCycleSpeed);
  colorWipe(blue, SingleColorCycleSpeed);
  
  rainbowCycle(RainbowColorCycleSpeed*2);
  colorWipe(yellow, SingleColorCycleSpeed);
  
  rainbow(RainbowColorCycleSpeed); 
  colorWipe(cyan, SingleColorCycleSpeed);
  
  rainbowCycle(RainbowColorCycleSpeed*2);
  colorWipe(magenta, SingleColorCycleSpeed);
  
  rainbow(RainbowColorCycleSpeed*2); 
  colorWipe(red, SingleColorCycleSpeed);
}

void flash(uint8_t speed) {
  uint16_t pixel, count, brightness;
  
  for(count=0; count<4; count++) 
  {    
    brightness = 255 * (count % 2);
    for(pixel=0; pixel<strip.numPixels(); pixel++) 
    {
      strip.setPixelColor(pixel, strip.Color(brightness, brightness, brightness));
    }
    strip.show();
    delay(20);
  }
}

void solid_color(uint32_t color, uint8_t wait) 
{
  for(uint16_t i=0; i<strip.numPixels(); i++) 
  {
    strip.setPixelColor(i, color);
  }
  strip.show();
  delay(wait);
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) 
{
  for(uint16_t i=0; i<strip.numPixels(); i++) 
  {
    // if we come back from flash, rainbow is ok because they are all set. 
    // can't stay on color wipe due since there'd be lots of black lights.
    if(skip_to_next){
      skip_to_next = false;
      break;
    }
  
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) 
  {
    for(i=0; i<strip.numPixels(); i++) 
    {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Equally distributed rainbow throughout rolling backward
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) 
  {
    for(i=0; i< strip.numPixels(); i++) 
    {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) 
{
  if(WheelPos < 85) 
  {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } 
  else if(WheelPos < 170) 
  {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } 
  else 
  {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

