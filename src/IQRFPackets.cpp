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

#include "IQRFPackets.h"

/**
 * Prepare SPI packet to packet buffer
 * @param spiCmd Command that I want to send to TR module
 * @param dataBuffer Pointer to a buffer that contains data that I want to send to TR module
 * @param dataLength Number of bytes to send
 * @param unallocationFlag If the dataBuffer is dynamically allocated using malloc function.
   If you wish to unallocate buffer after data is sent, set the unallocationFlag to 1, otherwise to 0.
 * @return Packet ID
 */
uint8_t IQRFPackets::send(uint8_t spiCmd, uint8_t* dataBuffer, uint8_t dataLength, uint8_t unallocationFlag) {
	if (dataLength == 0) {
		return 0;
	}
	if (dataLength > PACKET_SIZE - 4) {
		dataLength = PACKET_SIZE - 4;
	}
	if ((++this->idCounter) == 0) {
		this->idCounter++;
	}
	TR_SendSpiPacket(spiCmd, dataBuffer, dataLength, unallocationFlag);
	return this->idCounter;
}

/**
 * Set actual Tx packet ID
 * @param id Actual Tx packet ID
 */
void IQRFPackets::setId(uint8_t id) {
	this->id = id;
}

/**
 * Get actual Tx packet ID
 * @return Actual Tx packet ID
 */
uint8_t IQRFPackets::getId() {
	return this->id;
}

/**
 * Set count of packet IDs
 * @param count Count of packet IDs
 */
void IQRFPackets::setIdCount(uint8_t count) {
	this->idCounter = count;
}

/**
 * Get count of packet IDs
 * @return Count of packet IDs
 */
uint8_t IQRFPackets::getIdCount() {
	return this->idCounter;
}

/**
 * Set packet length
 * @param length Packet length
 */
void IQRFPackets::setLength(uint8_t length) {
	this->length = length;
}

/**
 * Get packet lenght
 * @return Packet length
 */
uint8_t IQRFPackets::getLength() {
	return this->length;
}
