#include <Arduino.h>
#include <BleMouse.h>
#include <JC_Button.h> 
#include <TimerEvent.h>
#include <FastLED.h>

#define DEVICE_NAME "Logitech MJ-1000"

#define X_RANDOM_RANGE 3
#define Y_RANDOM_RANGE 3
#define MIN_DISTANCE 100
#define MAX_DISTANCE 1000
#define MIN_SPEED 1
#define MAX_SPEED 5
#define MOVE_DELAY 12 // with lower interval  notify(): << esp_ble_gatts_send_ notify: rc=-1 Unknown ESP_ERR error appears

#define SECOND 1000
#define MOUSE_TIMER_INTERVAL SECOND * 30
#define SYSTEM_TIMER_INTERVAL SECOND * 5

#define BUTTON_PIN 27
#define LED_PIN 5

#define NUM_LEDS 1
#define BRIGHTNESS 40
#define COLOR_ACTIVATING CRGB::DarkGreen
#define COLOR_DEACTIVATING CRGB::OrangeRed
#define COLOR_RUNNING CRGB::RoyalBlue
#define COLOR_ACTIVE CRGB::DarkGoldenrod

bool isMouseActive = false;
bool wasConnectedBefore = false;
bool shouldCheckBluetooth = true;

uint32_t cycleCounter = 0;

BleMouse bleMouse(DEVICE_NAME);
Button button(BUTTON_PIN);
CRGB leds[NUM_LEDS];

TimerEvent mouseTimer;
TimerEvent systemCheckTimer;

void setLED(CRGB::HTMLColorCode colorOn, uint8_t brightness = BRIGHTNESS)
{
  leds[0] = colorOn;
  FastLED.setBrightness(brightness);
  FastLED.show();
}

void blinkLED(CRGB::HTMLColorCode colorOn, int cycles = 3, int onTimespanMs = 500, int offTimespanMs = 250)
{
  for(int i=0;i<cycles;i++)
  {
    setLED(colorOn);
    delay(onTimespanMs);

    setLED(CRGB::Black);
    delay(offTimespanMs);
  }
}

void onExecuteMouseTimer(){
  if(bleMouse.isConnected()) 
  {
    shouldCheckBluetooth = false;
    if(isMouseActive)
    {
      setLED(COLOR_RUNNING, 100);
      int mouseSpeed = random(MIN_SPEED, MAX_SPEED);
      int distance = random(MIN_DISTANCE, MAX_DISTANCE);
      int x = (random(X_RANDOM_RANGE) - 1) * mouseSpeed;
      int y = (random(Y_RANDOM_RANGE) - 1) * mouseSpeed;
      Serial.printf("Moving mouse: x=%i, y=%i, dist=%i, speed=%i, cycle=%i\r\n", x, y, distance, mouseSpeed, cycleCounter);
      for (int i = 0; i < distance; i+=mouseSpeed)
      {
        bleMouse.move(x, y);
        delay(MOVE_DELAY);
      }
      if (random(0, 7) == 3)
      {
        Serial.println("Executing right click");
        bleMouse.click(MOUSE_RIGHT);
      }
      setLED(COLOR_ACTIVE);
      cycleCounter++;
    }
  }
  else
  {
    shouldCheckBluetooth = true;
  }
}

void onSystemCheck()
{
  if(shouldCheckBluetooth)
  {
    if(bleMouse.isConnected()) 
    {
      wasConnectedBefore = true;
      Serial.println("Bluetooth connected!");
      setLED(CRGB::Black);
      shouldCheckBluetooth = false;
    }
    else
    {
      setLED(CRGB::Red);
      if(wasConnectedBefore)
      {
        Serial.println("Bluetooth disconnected, resetting...");
        //unfortunately, bleMouse.end(), bleMouse.begin() does not work, therefore we simply restart the whole board...
        ESP.restart(); 
      }
      else
      {
        Serial.println("Bluetooth not connected, ready to connect!");
      }
    }
  }
}

void updateMouseActive()
{
  const char* newState = isMouseActive ? "deactivating!" : "activating!";
  Serial.printf("Button pressed - %s", newState);
  isMouseActive = !isMouseActive;
  if(isMouseActive)
  {
    Serial.printf(" Update interval: %i ms", MOUSE_TIMER_INTERVAL);
    blinkLED(COLOR_ACTIVATING, 5, 300, 100);
    setLED(COLOR_ACTIVE);
  }
  else
  {
    blinkLED(COLOR_DEACTIVATING, 3);
    cycleCounter = 0;
  }
  Serial.println("");
  Serial.println("-----------------------");
}

void setup()
{
  Serial.begin(115200);
  Serial.printf("Starting BLE mouse, device name: '%s'\r\n", DEVICE_NAME);
  
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

  bleMouse.begin();
  button.begin();

  mouseTimer.set(MOUSE_TIMER_INTERVAL, onExecuteMouseTimer);
  systemCheckTimer.set(SYSTEM_TIMER_INTERVAL, onSystemCheck);
}

void loop()
{
  button.read();
  if(button.wasReleased())
  {
    updateMouseActive();
  }

  mouseTimer.update();
  systemCheckTimer.update();
}
