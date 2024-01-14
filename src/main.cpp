#include <Arduino.h>
#include <BleMouse.h>
#include <TimerEvent.h>

#define SECOND 1000
#define X_RANDOM_RANGE 3
#define Y_RANDOM_RANGE 3

#ifndef DEVICE_NAME
#define DEVICE_NAME "Logitech MJ-1000"
#endif

#ifndef MIN_DISTANCE
#define MIN_DISTANCE 100
#endif
#ifndef MAX_DISTANCE
#define MAX_DISTANCE 1000
#endif

#ifndef MIN_SPEED
#define MIN_SPEED 1
#endif

#ifndef MAX_SPEED
#define MAX_SPEED 5
#endif

#ifndef MOUSE_TIMER_INTERVAL
#define MOUSE_TIMER_INTERVAL SECOND * 30
#endif

#ifndef RANDOM_MOUSE_CLICK
#define RANDOM_MOUSE_CLICK 2
#endif

#define MOVE_DELAY 12 // with lower interval  notify(): << esp_ble_gatts_send_ notify: rc=-1 Unknown ESP_ERR error appears
#define SYSTEM_TIMER_INTERVAL SECOND * 5

bool wasConnectedBefore = false;
bool shouldCheckBluetooth = true;

uint32_t cycleCounter = 0;
uint32_t totalDistanceMoved = 0;

BleMouse bleMouse(DEVICE_NAME);

TimerEvent mouseTimer;
TimerEvent systemCheckTimer;

void onExecuteMouseTimer(){
  if(bleMouse.isConnected()) 
  {
    shouldCheckBluetooth = false;

    int mouseSpeed = random(MIN_SPEED, MAX_SPEED);
    int distance = random(MIN_DISTANCE, MAX_DISTANCE);
    int x = (random(X_RANDOM_RANGE) - 1) * mouseSpeed;
    int y = (random(Y_RANDOM_RANGE) - 1) * mouseSpeed;
    Serial.printf("Moving mouse: x=%i, y=%i, dist=%i, speed=%i, cycle=%i, total_dist=%i\r\n", x, y, distance, mouseSpeed, cycleCounter, totalDistanceMoved);
    for (int i = 0; i < distance; i+=mouseSpeed)
    {
      bleMouse.move(x, y);
      delay(MOVE_DELAY);
    }
    if (RANDOM_MOUSE_CLICK != 0 && random(0, 7) == 3)
    {
      Serial.printf("Executing mouse click: %i\r\n", RANDOM_MOUSE_CLICK);
      bleMouse.click(RANDOM_MOUSE_CLICK);
    }
    cycleCounter++;
    totalDistanceMoved += distance;
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
      Serial.printf("Device name: %s\r\n", DEVICE_NAME);
      Serial.printf("Update interval: %i ms\r\n", MOUSE_TIMER_INTERVAL);
      Serial.printf("Minimal moving speed: %i\r\n", MIN_SPEED);
      Serial.printf("Maximal moving speed: %i\r\n", MAX_SPEED);
      Serial.printf("Minimal moving distance: %i px\r\n", MIN_DISTANCE);
      Serial.printf("Maximal moving distance: %i px\r\n", MAX_DISTANCE);

      
      shouldCheckBluetooth = false;
    }
    else
    {
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

void setup()
{
  Serial.begin(115200);
  Serial.printf("Starting BLE mouse, device name: '%s'\r\n", DEVICE_NAME);
  
  bleMouse.begin();

  mouseTimer.set(MOUSE_TIMER_INTERVAL, onExecuteMouseTimer);
  systemCheckTimer.set(SYSTEM_TIMER_INTERVAL, onSystemCheck);
}

void loop()
{
  mouseTimer.update();
  systemCheckTimer.update();
}
