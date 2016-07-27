/**
 * @file
 * @author Rostislav Špinar <rostislav.spinar@microrisc.com>
 * @author Roman Ondráček <ondracek.roman@centrum.cz>
 * @version 1.1
 *
 * Copyright 2015-2016 MICRORISC s.r.o.
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

#include "IQSPI.h"

/**
 * Initialize the SPI bus
 */
void IQSPI::begin() {
	pinMode(TR_SS_PIN, OUTPUT);
	digitalWrite(TR_SS_PIN, HIGH);
#if defined(__PIC32MX__)
	spi.begin();
	spi.setSpeed(IQSPI_CLOCK);
	spi.setPinSelect(TR_SS_PIN);
#else
	SPI.begin();
#endif
}

/**
 * Disable the SPI bud
 */
void IQSPI::end() {
#if defined(__PIC32MX__)
	spi.end();
#else
	SPI.end();
#endif
}

/**
 * SPI byte transfer
 * @param txByte Transmitted byte
 * @return Received byte
 */
uint8_t IQSPI::transfer(uint8_t txByte) {
	uint8_t rxByte;
#if defined(__PIC32MX__)
	spi.setSelect(LOW);
	delayMicroseconds(10);
	spi.transfer(1, txByte, &rxByte);
	delayMicroseconds(10);
	spi.setSelect(HIGH);
#else
	pinMode(TR_SS_PIN, OUTPUT);
	digitalWrite(TR_SS_PIN, LOW);
	delayMicroseconds(10);
	SPI.beginTransaction(SPISettings(IQSPI_CLOCK, MSBFIRST, SPI_MODE0));
	rxByte = SPI.transfer(txByte);
	SPI.endTransaction();
	delayMicroseconds(10);
	digitalWrite(TR_SS_PIN, HIGH);
#endif
	return rxByte;
}