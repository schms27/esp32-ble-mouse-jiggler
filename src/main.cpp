#include <Arduino.h>
#include <BleMouse.h>
#include <TimerEvent.h>
#include "driver/touch_pad.h "

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

#define DEBOUNCE_TIME SECOND * 1

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long button_time = 0;  
unsigned long last_button_time = 0; 

bool isMouseActive = false;
bool wasConnectedBefore = false;
bool shouldCheckBluetooth = true;

uint32_t cycleCounter = 0;

BleMouse bleMouse(DEVICE_NAME);

TimerEvent mouseTimer;
TimerEvent systemCheckTimer;


void blinkLED( int cycles = 3, int onTimespanMs = 500, int offTimespanMs = 250)
{
  for(int i=0;i<cycles;i++)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(onTimespanMs);

    digitalWrite(LED_BUILTIN, LOW);
    delay(offTimespanMs);
  }
}

void onExecuteMouseTimer(){
  if(bleMouse.isConnected()) 
  {
    shouldCheckBluetooth = false;
    if(isMouseActive)
    {
      blinkLED(6, 100, 50);
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
      digitalWrite(LED_BUILTIN, HIGH);
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
      digitalWrite(LED_BUILTIN, LOW);
      shouldCheckBluetooth = false;
    }
    else
    {
      digitalWrite(LED_BUILTIN, HIGH);
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
  button_time = millis();
  if (button_time - last_button_time > DEBOUNCE_TIME)
  {

    const char* newState = isMouseActive ? "deactivating!" : "activating!";
    Serial.printf("Button pressed - %s", newState);
    isMouseActive = !isMouseActive;
    if(isMouseActive)
    {
      Serial.printf(" Update interval: %i ms", MOUSE_TIMER_INTERVAL);
      blinkLED(5, 300, 100);
    }
    else
    {
      blinkLED(3);
      cycleCounter = 0;
    }
    Serial.println("");
    Serial.println("-----------------------");
    last_button_time = button_time;
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.printf("Starting BLE mouse, device name: '%s'\r\n", DEVICE_NAME);
  
  bleMouse.begin();

  pinMode(LED_BUILTIN, OUTPUT);

  esp_err_t return_value;
  return_value = touch_pad_init();
  if (return_value != ESP_OK) {
    printf("Touch pad init error\n");
  }
  return_value = touch_pad_config(TOUCH_PAD_NUM1);
  if (return_value != ESP_OK) {
    printf("Touch pad config fail\n");
  }

  mouseTimer.set(MOUSE_TIMER_INTERVAL, onExecuteMouseTimer);
  touchAttachInterrupt(TOUCH_PAD_NUM1, updateMouseActive, 70000);
  systemCheckTimer.set(SYSTEM_TIMER_INTERVAL, onSystemCheck);
}

void loop()
{
  mouseTimer.update();
  systemCheckTimer.update();
}
