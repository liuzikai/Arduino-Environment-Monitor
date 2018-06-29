# Introduction
This is the program for an Arduino driven environment monitor kit.

The kit is based on Adeept Indoor Environment Monitor Kit, while some sensors are abandoned or replaced. Instead of displaying instant data on LCD screen, this kit store data in SD card, which enable long-term, outdoor measurement.

[data_processor.py](/data_processor.py) is used to process data retrieved from SD cards.

# Framework

![Program Framework.png](/Documents/Program%20Framework.png)

![State Transistion.png](/Documents/State%20Transistion.png)

# Authors
liuzikai, LTK P.

ZJU-UIUC

# Hardware Configuration
* Arduino UNO
* Plantower PMS7003
* DHT22
* SD Card Module
* Adeept BMP180 Module
* Adeept Photosensor Module
* Adeept MQ2 (Optional)
* Adeept Button Module
* LCD12864RSPI

# Pin Configuration
See [Environment_Monitor.ino](/Environment_Monitor.ino)

# Notice
* Arduino SD library doesn't well support SD cards that are greater than 2GB.
* MQ2, PMS7003 and LCD need large current supply, so external power supply may be needed. Notice that Arduino UNO 5V pin can only support limited current.