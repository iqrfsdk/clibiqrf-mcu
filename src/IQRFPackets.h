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

#ifndef IQRFPACKETS_H
#define IQRFPACKETS_H

#include <stdint.h>

/**
 * IQRF Packets
 */
class IQRFPackets {
public:
	void setId(uint8_t id);
	uint8_t getId();
	void setIdCount(uint8_t count);
	uint8_t getIdCount();
	void setLength(uint8_t length);
	uint8_t getLength();
	/**
	 * Tx packet statuses
	 */
	enum statuses {
		OK = 1, //!< Packet sent OK
		ERROR = 2 //!< Packet sent with ERROR
	};
private:
	/// Actual Tx packet ID
	uint8_t id;
	/// Counter of Tx packet IDs
	uint8_t idCounter;
	/// Packet length
	uint8_t length;
};

#endif
