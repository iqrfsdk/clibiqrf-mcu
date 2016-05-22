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
#include "IQRFCRC.h"

/**
 * Calculate CRC before master's send
 * @param buffer SPI Tx buffer
 * @param dataLength Data length
 * @return boolean
 */
uint8_t IQRFCRC::calculate(uint8_t *buffer, uint8_t dataLength) {
	uint8_t crc = 0x5F;
	for (uint8_t i = 0; i < (dataLength + 2); i++) {
		crc ^= buffer[i];
	}
	return crc;
}

/**
 * Confirm CRC from SPI slave upon received data
 * @param buffer SPI Rx buffer
 * @param dataLength Data length
 * @param type Type
 * @return CRC
 */
bool IQRFCRC::check(uint8_t *buffer, uint8_t dataLength, uint8_t type) {
	uint8_t crc = 0x5F ^ type;
	for (uint8_t i = 2; i < (dataLength + 2); i++) {
		crc ^= buffer[i];
	}
	if (buffer[dataLength + 2] == crc) {
		// CRCS ok
		return true;
	} else {
		// CRCS error
		return false;
	}
}
