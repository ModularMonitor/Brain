# Brain (Main module)

*This is the main module code, including all centralized features such as screen and SD card.*

### What's included:

* `Display`: TFT controller with touch support
* `CPU`: Useful tools for monitoring the CPU, RAM
* `I2C`: Fancy main I2C polling to get other modules status and data
* `LOG`: Logging system to write on SD card and Serial
* `SD`: Fancy asynchronous writing/reading stuff
* `SerialInputHandler`: For terminal-like work, so you can use it to debug easily
* `QR`: Yes, QR code generator
* `WiFi`: DNS, Server & WiFi AP controls

*This almost fill all RAM, so changing resolutions or other parameters on `defaults.h` may break builds.*
