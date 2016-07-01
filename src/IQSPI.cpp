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

#include "IQSPI.h"

/**
 * Initialize the SPI bus
 */
void IQSPI::begin() {
	pinMode(this->ss, OUTPUT);
	digitalWrite(this->ss, HIGH);
#if defined(__AVR__)
	SPI.begin();
#elif defined(__PIC32MX__)
	spi->begin();
	spi->setSpeed(IQSPI_CLOCK);
	spi->setPinSelect(this->ss);
#endif
}

/**
 * Disable the SPI bud
 */
void IQSPI::end() {
#if defined(__AVR__)
	SPI.end();
#elif defined(__PIC32MX__)
	spi->end();
#endif
}

/**
 * SPI byte transfer
 * @param txByte Transmitted byte
 * @param rxByte Received byte
 */
void IQSPI::transfer(uint8_t txByte, uint8_t rxByte) {
#if defined(__AVR__)
	pinMode(this->ss, OUTPUT);
	digitalWrite(this->ss, LOW);
	delayMicroseconds(10);
	SPI.beginTransaction(SPISettings(IQSPI_CLOCK, MSBFIRST, SPI_MODE0));
	rxByte = SPI.transfer(txByte);
	SPI.endTransaction();
	delayMicroseconds(10);
	digitalWrite(this->ss, HIGH);
#elif defined(__PIC32MX__)
	spi->setSelect(LOW);
	delayMicroseconds(10);
	spi->transfer(1, txByte, &rxByte);
	delayMicroseconds(10);
	spi->setSelect(HIGH);
#endif
}

/**
 * Set SS (Slave Select) pin
 * @param pin SS (Slave Select) pin
 */
void IQSPI::setSs(uint8_t pin) {
	this->ss = ss;
}

/**
 * Get SS (Slave Select) pin
 * @return SS (Slave Select) pin
 */
uint8_t IQSPI::getSs() {
	return this->ss;
}
