# IQRF SPI library for microcontrollers

[![Build Status](https://travis-ci.org/iqrfsdk/clibiqrf-mcu.svg?branch=master)](https://travis-ci.org/iqrfsdk/clibiqrf-mcu)
[![Apache License](https://img.shields.io/badge/license-APACHE2-blue.svg)](https://github.com/iqrfsdk/clibiqrf-mcu/blob/master/LICENSE)

## Supported IQRF TR modules
 * IQRF TR-72
 * IQRF TR-52

## Supported platforms
This library is supported on AVR-based and SAM-based Arduino compatable platforms (e.g. Arduino Uno, Leonardo, Mega, Nano, Due) and PIC32-based Arduino compatable platforms (e.g. chipKIT).

## Installation
The best way how to install this library is to [download a latest package](https://github.com/iqrfsdk/clibiqrf-mcu/releases) or use a [platformio](http://platformio.org/lib/show/318/IQRF%20SPI/):

```
platformio lib install "IQRF SPI"
```
## Pin layout
The following table shows the typical pin layout used:

|  Signal   | [IQRF BB-01](http://eshop.iqrf.org/products/detail/IQRF-BB-01) |  Uno / Nano | Mega | Leonardo / Micro | Pro Micro |
| :-------: | :------------------------------------------------------------: | :---------: | :--: | :--------------: | :-------: |
| Reset [1] |                             –TRPWR                             |      9      |   5  |      RESET       |    RST    |
|  SPI SS   |                               SS                               |      10     |  53  |        10        |    10     |
|  SPI MOSI |                              MOSI                              | 11 / ICSP-4 |  51  |      ICSP-4      |    16     |
|  SPI MISO |                              MISO                              | 12 / ICSP-1 |  50  |      ICSP-1      |    14     |
|  SPI SCK  |                              SCK                               | 13 / ICSP-3 |  52  |      ICSP-3      |    15     |

[1] Configurable, typically defined as ```TR_RESET_IO``` in sketch/program.

## Compatibility table
|         MCU        |  Works |            Boards            |
| ------------------ | :----: | ---------------------------- |
| Atmega328 @ 16MHz  |    ✓   | Arduino Uno, Nano            |
| Atmega32u4 @ 16MHz |    ✓   | Arduino Leonardo, Micro      |
| Atmega2560 @ 16MHz |    ✓   | Arduino Mega 2560            |
| ATSAM3X8E          |    ✓   | Arduino Due                  |
| PIC32MX320F128H    |    ✓   | chipKIT Uno32                |
| PIC32MX340F512H    |    ✓   | chipKIT uC32                 |
| MK66FX1M0VMD18     |    ✓   | Teensy 3.6                   |

## Documentation
Documentation you can found on [this page](https://iqrfsdk.github.io/clibiqrf-mcu/).

## License
This library is licensed under Apache License 2.0:

 > Copyright 2015-2016 MICRORISC s.r.o.
 > 
 > Licensed under the Apache License, Version 2.0 (the "License");
 > you may not use this file except in compliance with the License.
 > You may obtain a copy of the License at
 > 
 >     http://www.apache.org/licenses/LICENSE-2.0
 > 
 > Unless required by applicable law or agreed to in writing, software
 > distributed under the License is distributed on an "AS IS" BASIS,
 > WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 > See the License for the specific language governing permissions and
 > limitations under the License.
