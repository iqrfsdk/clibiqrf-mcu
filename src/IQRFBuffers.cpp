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

#include "IQRFBuffers.h"

/**
 * Get Rx buffer
 * @return Rx buffer
 */
uint8_t* IQRFBuffers::getRxBuffer() {
	return this->rxBuffer;
}

/**
 * Get Rx buffer size
 * @return Rx buffer size
 */
uint8_t IQRFBuffers::getRxBufferSize() {
	return sizeof(this->rxBuffer);
}

/**
 * Get data from Rx buffer
 * @param position Position in buffer
 * @return Rx data
 */
uint8_t IQRFBuffers::getRxData(uint8_t position) {
	return this->rxBuffer[position];
}

/**
 * Set data to Rx buffer
 * @param position Position in Rx buffer
 * @param data Data in Rx buffer
 */
void IQRFBuffers::setRxData(uint8_t position, uint8_t data) {
	this->rxBuffer[position] = data;
}

/**
 * Get Tx buffer
 * @return Tx buffer
 */
uint8_t* IQRFBuffers::getTxBuffer() {
	return this->txBuffer;
}

/**
 * Get Tx buffer size
 * @return Tx buffer size
 */
uint8_t IQRFBuffers::getTxBufferSize() {
	return sizeof(this->txBuffer);
}

/**
 * Get data from Tx buffer
 * @param position Position in buffer
 * @return Tx data
 */
uint8_t IQRFBuffers::getTxData(uint8_t position) {
	return this->txBuffer[position];
}

/**
 * Set data to Tx buffer
 * @param position Position in Rx buffer
 * @param data Data in Tx buffer
 */
void IQRFBuffers::setTxData(uint8_t position, uint8_t data) {
	this->txBuffer[position] = data;
}