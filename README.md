# IQRF SPI library for Arduino

[![Build Status](https://travis-ci.org/iqrfsdk/clibiqrf-arduino.svg?branch=master)](https://travis-ci.org/iqrfsdk/clibiqrf-arduino)
[![Apache License](http://img.shields.io/badge/license-APACHE2-blue.svg)](LICENSE)

## Supported IQRF TR modules
 * IQRF TR-72
 * IQRF TR-52

## Supported platforms
This library is supported on AVR-based Arduino compatable platforms (e.g. Arduino Uno, Leonardo, Mega, Nano).

A library for Chipkit is [there](https://github.com/iqrfsdk/clibiqrf-chipkit).

## Installation
The best way how to install this library is to [download a latest package](https://github.com/iqrfsdk/clibiqrf-arduino/releases) or use a [platformio](https://github.com/platformio/platformio):

```
platformio lib install ID
```
## Pin layout
The following table shows the typical pin layout used:

|  Signal   | [IQRF BB-01](http://eshop.iqrf.org/products/detail/IQRF-BB-01) |     Uno     | Mega | Nano v3 | Leonardo / Micro | Pro Micro |
| --------- | -------------------------------------------------------------- | ----------- | ---- | ------- | ---------------- | --------- |
| Reset [1] |                             –TRPWR                             |      9      |   5  |    D9   |      RESET       |    RST    |
|  SPI SS   |                               SS                               |      10     |  5   |   D10   |        10        |    10     |
|  SPI MOSI |                              MOSI                              | 11 / ICSP-4 |  5   |   D11   |      ICSP-4      |    16     |
|  SPI MISO |                              MISO                              | 12 / ICSP-1 |  50  |   D12   |      ICSP-1      |    14     |
|  SPI SCK  |                              SCK                               | 13 / ICSP-3 |  52  |   D13   |      ICSP-3      |    15     |

[1] Configurable, typically defined as TR_RESET_IO in sketch/program.

## Dependecies of this library
 * [MsTimer2](https://github.com/PaulStoffregen/MsTimer2)

## Documentation
Documentation you can found on [this page](https://roman3349.github.io/clibiqrf-arduino/).

## License
This library is licensed under Apache License 2.0:

 > Copyright 2015 MICRORISC s.r.o.
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
