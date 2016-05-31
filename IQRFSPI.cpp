/**
 * @file
 * @author Rostislav Špinar <rostislav.spinar@microrisc.com>
 * @author Roman Ondráček <ondracek.roman@centrum.cz>
 * @version 1.0
 *
 * @section LICENSE
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

#include "IQRFSPI.h"

/**
 * Get TR module SPI status
 * @return TR module SPI status
 * Status code |   Status message   |                Description                 
 *  ---------- | ------------------ | --------------------------------------------
 *     0xFF    |     NO_MODULE      | SPI not working (HW error)
 *     0xFE    |        BUSY        | SPI busy in Master disabled mode
 *     0xFD    |    DATA_TRANSFER   | SPI data transfer in progress
 *     0x00    |      DISABLED      | SPI not working (disabled)
 *     0x3F    |      CRCM_OK       | SPI not ready (full buffer, last CRCM ok)
 *     0x3E    |      CRCM_ERR      | SPI not ready (full buffer, last CRCM error)
 *     0xFF    | COMMUNICATION_MODE | SPI ready (communication mode)
 *     0x81    |  PROGRAMMING_MODE  | SPI ready (programming mode)
 *     0x82    |     DEBUG_MODE     | SPI ready (debugging mode)
 *     0x83    |      SLOW_MODE     | SPI not working in background
 *     0x07    |      USER_STOP     | SPI state after stopSPI();
 */
uint8_t IQRFSPI::getStatus() {
	return this->status;
}

/**
 * Set TR module SPI status
 * @param status SPI status
 */
void IQRFSPI::setStatus(uint8_t status) {
	this->status = status;
}

/**
 * Enable SPI master
 */
void IQRFSPI::enableMaster() {
	this->master = true;
}

/**
 * Disable SPI master
 */
void IQRFSPI::disableMaster() {
	this->master = false;
	this->status = statuses::DISABLED;
}

/**
 * Get TR SPI master status
 * @return TR SPI master status
 */
bool IQRFSPI::isMasterEnabled() {
	return this->master;
}

/**
 * Get Master status
 * @return Master status
 */
uint8_t IQRFSPI::getMasterStatus() {
	return this->masterStatus;
}

/**
 * Set master status
 * @param status Master status
 */
void IQRFSPI::setMasterStatus(uint8_t status) {
	this->masterStatus = status;
}

/**
 * Enable Fast SPI
 */
void IQRFSPI::enableFastSpi() {
	this->fastSpi = true;
	this->setBytePause(150);
}

/**
 * Disable Fast SPI
 */
void IQRFSPI::disableFastSpi() {
	this->fastSpi = false;
	this->setBytePause(1000);
}

/**
 * Get Fast SPI status
 * @return Fast SPI status 
 */
bool IQRFSPI::isFastSpiEnabled() {
	return this->fastSpi;
}

/**
 * Get SPI byte to byte pause in us
 * @return SPI byte to byte pause in us
 */
unsigned long IQRFSPI::getBytePause() {
	return this->bytePause;
}

/**
 * Set SPI byte to byte pause in us
 * @param time Byte to byte time in us
 */
void IQRFSPI::setBytePause(unsigned long time) {
	this->bytePause = time;
}

/**
 * Send and receive single byte over SPI
 * @param txByte Character to be send via SPI
 * @return Byte received via SPI
 */
uint8_t IQRFSPI::byte(uint8_t txByte) {
	uint8_t rxByte = 0;
	digitalWrite(TR_SS_IO, LOW);
	delayMicroseconds(10);
	SPI.beginTransaction(SPISettings(IQRF_SPI_CLK, MSBFIRST, SPI_MODE0));
	rxByte = SPI.transfer(txByte);
	SPI.endTransaction();
	delayMicroseconds(10);
	digitalWrite(TR_SS_IO, HIGH);
	return rxByte;
}

/**
 * Get Rx buffer
 * @return Rx data
 */
uint8_t IQRFSPI::getRxBuffer() {
	return this->rxBuffer;
}

/**
 * Get data from Rx buffer
 * @param position Position in buffer
 * @return Rx data
 */
uint8_t IQRFSPI::getRxData(uint8_t position) {
	return this->rxBuffer[position];
}

/**
 * Set data to Rx buffer
 * @param position Position in Rx buffer
 * @param data Data in Rx buffer
 */
void IQRFSPI::setRxData(uint8_t position, uint8_t data) {
	this->rxBuffer[position] = data;
}


/**
 * Get Tx buffer
 * @return Tx buffer
 */
uint8_t IQRFSPI::getTxBuffer() {
	return this->txBuffer;
}

/**
 * Get data from Tx buffer
 * @param position Position in buffer
 * @return Tx data
 */
uint8_t IQRFSPI::getTxData(uint8_t position) {
	return this->txBuffer[position];
}

/**
 * Set data to Tx buffer
 * @param position Position in Rx buffer
 * @param data Data in Tx buffer
 */
void IQRFSPI::setTxData(uint8_t position, uint8_t data) {
	this->txBuffer[position] = data;
}