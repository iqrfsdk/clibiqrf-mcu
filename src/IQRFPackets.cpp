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

#include "IQRFPackets.h"

/**
 * Prepare SPI packet to packet buffer
 * @param spiCmd Command that I want to send to TR module
 * @param dataBuffer Pointer to a buffer that contains data that I want to send to TR module
 * @param dataLength Number of bytes to send
 * @param unallocationFlag If the pDataBuffer is dynamically allocated using malloc function.
   If you wish to unallocate buffer after data is sent, set the unallocationFlag to 1, otherwise to 0.
 * @return Packet ID
 */
uint8_t IQRFPackets::sendPacket(uint8_t spiCmd, uint8_t* dataBuffer, uint8_t dataLength, uint8_t unallocationFlag) {
	if (dataLength == 0) {
		return 0;
	}
	if (dataLength > PACKET_SIZE - 4) {
		dataLength = PACKET_SIZE - 4;
	}
	if ((++this->counter) == 0) {
		this->counter++;
	}
	this->buffer[this->inputCounter].dataBuffer = dataBuffer;
	this->buffer[this->inputCounter].dataLength = dataLength;
	this->buffer[this->inputCounter].packetId = this->counter;
	this->buffer[this->inputCounter].spiCmd = spiCmd;
	this->buffer[this->inputCounter].unallocationFlag = unallocationFlag;
	if (++this->inputCounter >= PACKET_BUFFER_SIZE) {
		this->inputCounter = 0;
	}
	return this->counter;
}

/**
 * Get count of input packets
 * @return Count of input packets
 */
uint16_t IQRFPackets::getInputCounter() {
	return this->inputCounter;
}

/**
 * Set count of unput packets
 * @param count Count of input packets
 */
void IQRFPackets::setInputCounter(uint16_t count) {
	this->inputCounter = count;
}

/**
 * Get count of output packets
 * @return Count of output packets
 */
uint16_t IQRFPackets::getOutputCounter() {
	return this->outputCounter;
}

/**
 * Set count of output packets
 * @param count Count of output packets
 */
void IQRFPackets::setOutputCounter(uint16_t count) {
	this->outputCounter = count;
}

/**
 * Get temporary data buffer
 * @param counter Packet counter
 * @return Pointer to temporary data buffer
 */
uint8_t* IQRFPackets::getDataBuffer(uint16_t counter) {
	return this->buffer[counter].dataBuffer;
}

/**
 * Get data length
 * @param counter Packet counter
 * @return Data length
 */
uint8_t IQRFPackets::getDataLength(uint8_t counter) {
	return this->buffer[counter].dataLength;
}

/**
 * Get packet ID
 * @param counter Packet counter
 * @return Packet ID
 */
uint8_t IQRFPackets::getPacketId(uint16_t counter) {
	return this->buffer[counter].packetId;
}

/**
 * Get SPI command
 * @param counter Packet counter
 * @return SPI command
 */
uint8_t IQRFPackets::getSpiCommand(uint16_t counter) {
	return this->buffer[counter].spiCmd;
}

/**
 * Get unallocation flag
 * @param counter Packet sounter
 * @return Unallocation flag
 */
uint8_t IQRFPackets::getUnallocationFlag(uint16_t counter) {
	return this->buffer[counter].unallocationFlag;
}

/**
 * Get packet type
 * @return Packet type
 */
uint8_t IQRFPackets::getType() {
	return this->type;
}

/**
 * Set packet type
 * @param type Packet type
 */
void IQRFPackets::setType(uint8_t type) {
	this->type = type;
}