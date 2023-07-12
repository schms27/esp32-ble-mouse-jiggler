# ESP32 bluetooth mouse jiggler

This project is heavily inspired by [tornado67/DroChill](https://github.com/tornado67/DroChill).
The current code is optimized for a [Firebeetle ESP32-E](https://wiki.dfrobot.com/FireBeetle_Board_ESP32_E_SKU_DFR0654)
but it should be adaptable to nearly every [ESP-32 Board supported by PlatformIO](https://docs.platformio.org/en/latest/boards/index.html#espressif-32).

![N|ESP32](https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcRxBzNca25vVHpixpe43H1G64hQvBPdRDXByCEZrv8dkgpu5QUOVWH4v44LM2_hidzS2V8&usqp=CAU)

Do you ever take a break from work and find that your computer has gone to sleep or locked itself due to inactivity, causing your boss to get pissed on you for not being online? 


This enables you to build a tiny, undetectable ESP32 powered Bluetooth mouse jiggler that helps your Slack or Microsoft Teams to stay awake while you're chilling. 


## Features

- Undetectable
- Moves your mouse every x seconds and randomly performs a right mouse click
- Prevents software like Slack or Microsoft Teams from setting you status to "away"
- No setup required
- Simple and reliable


## Setup
- Clone the repo. 
- Open the repo with VisualStudio Code with PlatformIO installed.
- Press upload button and wait for upload to finish.
- Power your board on
- Connect to device called like DEVICE_NAME in main.cpp (default: Logitech MJ-1000). 
- ✨You're awesome ✨

## License

[MIT](https://choosealicense.com/licenses/mit/)
