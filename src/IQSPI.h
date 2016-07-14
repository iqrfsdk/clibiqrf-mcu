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

#ifndef IQSPI_H
#define IQSPI_H

#define IQSPI_CLOCK 250000 //!< SPI clock 250kHz

#include <stdint.h>
#include "IQRFSettings.h"
#if defined(__AVR__)
#include <Arduino.h>
#include <SPI.h>
#elif defined(__PIC32MX__)
#include <WProgram.h>
#include <DSPI.h>
#endif

/**
 * SPI interface for AVR and PIC32 MCU
 */
class IQSPI {
public:
	void begin();
	void end();
	uint8_t transfer(uint8_t txByte);
private:
#if defined(__PIC32MX__)
	/// Instance of chipKIT SPI class
	DSPI0* spi = new DSPI0;
#endif
};

#endif
