#include <Arduino.h>
#include "WiFi.h"
#include "ArduinoOTA.h"
// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// Released under the GPLv3 license to match the rest of the
// Adafruit NeoPixel library
#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define EEPROM_SIZE 12

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN_D D8 // On Trinket or Gemma, suggest changing this to 1
#define PIN_G D9 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS_DROITE 45 // Popular NeoPixel ring size
#define NUMPIXELS_GAUCHE 30 // Popular NeoPixel ring size

int bootcount; // remember number of boots in RTC Memory
int bootcountnext;
// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels_droit(NUMPIXELS_DROITE, PIN_D, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels_gauche(NUMPIXELS_GAUCHE, PIN_G, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 20 // Time (in milliseconds) to pause between pixels
int address = 0;

String wifi_ssid = "Joelette_2";

void premierBandeau(int r, int g, int b)
{
  for (int i = 0; i < 15; i++)
  {
    pixels_droit.setPixelColor(i, r, g, b); // turn every third pixel on
  }
  pixels_droit.show();
}

uint32_t WheelDroite(byte WheelPos)
{
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85)
  {
    return pixels_droit.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170)
  {
    WheelPos -= 85;
    return pixels_droit.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels_droit.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void rainbowCycleDroite(uint8_t wait)
{
  uint16_t i, j;
  for (j = 0; j < 256 * 5; j++)
  { // 5 cycles of all colors on wheel
    for (i = 15; i < NUMPIXELS_DROITE; i++)
    {
      pixels_droit.setPixelColor(i, WheelDroite(((i - 15 * 256 / 30) + j) & 255));
    }
    pixels_droit.show();
    delay(wait);
  }
}

uint32_t WheelGauche(byte WheelPos)
{
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85)
  {
    return pixels_gauche.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170)
  {
    WheelPos -= 85;
    return pixels_gauche.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels_gauche.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void rainbowCycleGauche(uint8_t wait)
{
  uint16_t i, j;
  for (j = 0; j < 256 * 5; j++)
  { // 5 cycles of all colors on wheel
    for (i = 0; i < NUMPIXELS_GAUCHE; i++)
    {
      pixels_gauche.setPixelColor(i, WheelGauche(((i * 256 / 30) + j) & 255));
    }
    pixels_gauche.show();
    delay(wait);
  }
}

void rainbowDouble(uint8_t wait)
{
  premierBandeau(0, 100, 0);
  uint16_t i, j;
  for (j = 0; j < 256 * 5; j++)
  { // 5 cycles of all colors on wheel
    for (i = 0; i < NUMPIXELS_GAUCHE; i++)
    {
      pixels_gauche.setPixelColor(i, WheelGauche(((i * 256 / 30) + j) & 255));
      pixels_droit.setPixelColor(i + 15, WheelGauche(((i * 256 / 30) + j) & 255));
    }
    pixels_gauche.show();
    pixels_droit.show();
    delay(wait);
  }
}

void RunningLights(byte red, byte green, byte blue, int WaveDelay)
{
  int Position = 0;
  premierBandeau(red, green, blue);

  for (int j = 0; j < 30 * 2; j++)
  {
    Position++; // = 0; //Position + Rate;
    for (int i = 0; i < 30; i++)
    {
      // sine wave, 3 offset waves make a rainbow!
      // float level = sin(i+Position) * 127 + 128;
      // setPixel(i,level,0,0);
      // float level = sin(i+Position) * 127 + 128;
      pixels_gauche.setPixelColor(i, ((sin(i + Position) * 127 + 128 / 2) / 255) * red,
                                  ((sin(i + Position) * 127 + 128 / 2) / 255) * green,
                                  ((sin(i + Position) * 127 + 128 / 2) / 255) * blue);
      pixels_droit.setPixelColor(i + 15, ((sin(i + Position) * 127 + 128 / 2) / 255) * red,
                                 ((sin(i + Position) * 127 + 128 / 2) / 255) * green,
                                 ((sin(i + Position) * 127 + 128 / 2) / 255) * blue);
    }

    pixels_droit.show();
    pixels_gauche.show();
    delay(WaveDelay);
  }
}

void theaterChase(byte red, byte green, byte blue, int SpeedDelay)
{
  premierBandeau(red, green, blue);
  for (int j = 0; j < 10; j++)
  { // do 10 cycles of chasing
    for (int q = 0; q < 3; q++)
    {
      for (int i = 0; i < 30; i = i + 3)
      {
        pixels_droit.setPixelColor(i + q + 15, red, green, blue); // turn every third pixel on
        pixels_gauche.setPixelColor(i + q, red, green, blue);     // turn every third pixel on
      }
      pixels_droit.show();
      pixels_gauche.show();

      delay(SpeedDelay);

      for (int i = 0; i < 30; i = i + 3)
      {
        pixels_droit.setPixelColor(i + q + 15, 0, 0, 0); // turn every third pixel off
        pixels_gauche.setPixelColor(i + q, 0, 0, 0);     // turn every third pixel off
      }
    }
  }
}

void allOn(int r, int g, int b)
{
  for (int i = 0; i < 45; i++)
  {
    pixels_droit.setPixelColor(i, r, g, b);  // turn every third pixel on
    pixels_gauche.setPixelColor(i, r, g, b); // turn every third pixel on
  }
  pixels_droit.show();
  pixels_gauche.show();
}

void initOTA()
{
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(wifi_ssid);

  Serial.print("local IP address: ");
  Serial.println(WiFi.softAPIP());

  // Port defaults to 3232
  ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  ArduinoOTA.setHostname("myesp32-OTA");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");
  ArduinoOTA
      .onStart([]()
               {
			String type;
			if (ArduinoOTA.getCommand() == U_FLASH)
				type = "sketch";
			else // U_SPIFFS
				type = "filesystem";

			// NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
			Serial.println("Start updating " + type); })
      .onEnd([]()
             { Serial.println("\nEnd"); })
      .onProgress([](unsigned int progress, unsigned int total)
                  { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); })
      .onError([](ota_error_t error)
               {
			Serial.printf("Error[%u]: ", error);
			if (error == OTA_AUTH_ERROR)
				Serial.println("Auth Failed");
			else if (error == OTA_BEGIN_ERROR)
				Serial.println("Begin Failed");
			else if (error == OTA_CONNECT_ERROR)
				Serial.println("Connect Failed");
			else if (error == OTA_RECEIVE_ERROR)
				Serial.println("Receive Failed");
			else if (error == OTA_END_ERROR)
				Serial.println("End Failed"); });

  ArduinoOTA.setTimeout(60000);
  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup()
{
  Serial.begin(115200);

  initOTA();

  // Mise en mémoire du bootcount qui sert à choisir l'animation.
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.get(address, bootcount);

  // Si le bootcount est supérieur à 4 on le remet à zero pour cycler sur les animations
  if (bootcount > 4 || bootcount < 0)
  {
    bootcount = 0;
    bootcountnext = bootcount + 1;
    EEPROM.put(address, bootcountnext);
    EEPROM.commit();
  }
  else
  {
    bootcountnext = bootcount + 1;
    EEPROM.put(address, bootcountnext);
    EEPROM.commit();
  }

  pixels_droit.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels_gauche.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels_droit.clear();
  pixels_gauche.clear();
}

void loop()
{
	ArduinoOTA.handle();
  // Animation à modifier selon envie.
  switch (bootcount)
  {
  case 0:
    rainbowDouble(10);
    break;

  case 1:
    RunningLights(100, 0, 0, 50);
    break;

  case 2:
    allOn(100, 0, 0);
    break;

  case 3:
    allOn(0, 100, 0);
    break;

  case 4:
    allOn(0, 0, 100);
    break;

  default:
    break;
  }
}