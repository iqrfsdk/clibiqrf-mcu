/**
 * @file
 * @author Rostislav Špinar <rostislav.spinar@microrisc.com>
 * @author Roman Ondráček <ondracek.roman@centrum.cz>
 * @version 1.1
 *
 * Copyright 2015 MICRORISC s.r.o.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef IQRFSettings_H
#define IQRFSettings_H

#define PACKET_SIZE        68          //!< Size of SPI TX and RX buffer
#define PACKET_BUFFER_SIZE 32          //!< Size of SPI TX packet buffer

// Timing
#define MICRO_SECOND       1000000      //!< Microsecond
#define MILLI_SECOND       1000         //!< Milisecond

// Pins
#if !defined(TR_RESET_PIN)
#define TR_RESET_PIN        6           //!< TR reset pin
#endif
#if !defined(TR_SS_PIN)
#define TR_SS_PIN           10          //!< SPI SS pin
#endif

// Arduino Leonardo, Micro
#if defined(__AVR_ATmega32U4__)
#define TR_MOSI_PIN         16          //!< SPI MOSI pin
#define TR_MISO_PIN         14          //!< SPI MISO pin
// Arduino Mega, Mega 2560
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define TR_MOSI_PIN         51          //!< SPI MOSI pin
#define TR_MISO_PIN         50          //!< SPI MISO pin
// Arduino Due or Zero, MKR1000
#elif defined(__SAM3X8E__) || defined(__SAMD21G18A__)
#define TR_MOSI_PIN         PIN_SPI_MOSI //!< SPI MOSI pin
#define TR_MISO_PIN         PIN_SPI_MISO //!< SPI MISO pin
#else
#define TR_MOSI_PIN         11          //!< SPI MOSI pin
#define TR_MISO_PIN         12          //!< SPI MISO pin
#endif

#endif
