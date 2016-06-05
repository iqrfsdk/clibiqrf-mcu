/**
 * @file
 * @author Rostislav Špinar <rostislav.spinar@microrisc.com>
 * @author Roman Ondráček <ondracek.roman@centrum.cz>
 * @version 2.0
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

#ifndef IQRFPACKETS_H
#define IQRFPACKETS_H

#define PACKET_SIZE        68 //!< Size of SPI Tx and Rx buffer
#define PACKET_BUFFER_SIZE 32 //!< Size of SPI Tx packet buffer

#include <stdint.h>
#include <stddef.h>

using namespace std;

/**
 * IQRF Packets class
 */
class IQRFPackets {
public:
	uint8_t sendPacket(uint8_t spiCmd, uint8_t *dataBuffer, uint8_t dataLength, uint8_t unallocationFlag);
	uint16_t getInputCounter();
	void setInputCounter(uint16_t count);
	uint16_t getOutputCounter();
	void setOutputCounter(uint16_t count);
	uint8_t getPacketId(uint16_t counter);
	uint8_t getSpiCommand(uint16_t counter);
	uint8_t* getDataBuffer(uint16_t counter);
	uint8_t getDataLength(uint8_t counter);
	uint8_t getUnallocationFlag(uint16_t counter);
	uint8_t getType();
	void setType(uint8_t type);

	/**
	 * Tx packet statuses
	 */
	enum statuses {
		OK = 1, //!< Packet sent OK
		ERROR = 2 //!< Packet sent with ERROR
	};

	/**
	 * Item of Tx packet buffer
	 */
	typedef struct {
		uint8_t packetId; //!< Packet ID
		uint8_t spiCmd; //!< SPI command
		uint8_t *dataBuffer; //!< Pointer to data buffrt
		uint8_t dataLength; //!< Data lenght
		uint8_t unallocationFlag; //!< Unallocation flag
	} packet_buffer;
private:
	/// Packet input counter
	uint8_t inputCounter;
	/// Packet output counter
	uint8_t outputCounter;
	/// Packet counter
	uint8_t counter;
	/// Packet type
	uint8_t type;
	/// IQRF packet buffer
	packet_buffer buffer[PACKET_BUFFER_SIZE];
};

#endif
