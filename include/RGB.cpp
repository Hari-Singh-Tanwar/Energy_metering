#include <Arduino.h>

const uint8_t red = 36;
const uint8_t green = 39;
const uint8_t blue = 34;

const uint16_t freq = 500;   // Frequency of PWM signal
const uint8_t res = 8;       // Resolution of PWM signsl (8 -> 8bit = 0 - 255)
const uint8_t ch_red = 0;    // Channel 0 of PWM signal
const uint8_t ch_green = 1;  // Channel 1 of PWM signal
const uint8_t ch_blue = 2;   // Channel 2 of PWM signal


void RGBsetup()
{
      // RGB INIT
  // Red led setup
  pinMode(red, OUTPUT);
  ledcSetup(ch_red, freq, res); // Setting up red led
  ledcAttachPin(red, ch_red);   // Attaching pin to the red led

  // Green led setup
  pinMode(green, OUTPUT);
  ledcSetup(ch_green, freq, res);
  ledcAttachPin(green, ch_green);

  // Blue led setup
  pinMode(blue, OUTPUT);
  ledcSetup(ch_blue, freq, res);
  ledcAttachPin(blue, ch_blue);
}

void RGBOff() // shuts off RGB
{
    ledcWrite(ch_red, 0); 
    ledcWrite(ch_green, 0); 
    ledcWrite(ch_blue, 0); 
}

void ColourWhite() // Generates coloue white
{
    ledcWrite(ch_red, 255); 
    ledcWrite(ch_green, 255); 
    ledcWrite(ch_blue, 255); 
}

void ColourRed() // Generates coloue red
{
    ledcWrite(ch_red, 255); 
    ledcWrite(ch_green, 0); 
    ledcWrite(ch_blue, 0); 
}

void ColourGreen() // Generates coloue red
{
    ledcWrite(ch_red, 0); 
    ledcWrite(ch_green, 255); 
    ledcWrite(ch_blue, 0); 
}

void ColourBlue() // Generates coloue red
{
    ledcWrite(ch_red, 0); 
    ledcWrite(ch_green, 0); 
    ledcWrite(ch_blue, 255); 
}

void ColourCyan() // Generates coloue cyan
{
    ledcWrite(ch_red, 0); 
    ledcWrite(ch_green, 255); 
    ledcWrite(ch_blue, 255); 
}

void ColourMaginta() // Generates coloue Maginta
{
    ledcWrite(ch_red, 255); 
    ledcWrite(ch_green, 0); 
    ledcWrite(ch_blue, 255); 
}

void ColourYellow() // Generates coloue Yellow
{
    ledcWrite(ch_red, 255); 
    ledcWrite(ch_green, 255); 
    ledcWrite(ch_blue, 0); 
}

void CoustomRGB(uint8_t RED, uint8_t GREEN,uint8_t BLUE) // Generates coudtom colour
{
    ledcWrite(ch_red, RED); 
    ledcWrite(ch_green, GREEN); 
    ledcWrite(ch_blue, BLUE); 
}