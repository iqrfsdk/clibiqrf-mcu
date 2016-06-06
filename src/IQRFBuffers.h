/**
 * @file
 * @author Rostislav Špinar <rostislav.spinar@microrisc.com>
 * @author Roman Ondráček <ondracek.roman@centrum.cz>
 * @version 2.0
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

#ifndef IQRFBUFFERS_H
#define IQRFBUFFERS_H

#include <stdint.h>

#include "IQRFPackets.h"

using namespace std;

/**
 * IQRF Buffers
 */
class IQRFBuffers {
public:
	uint8_t* getTxBuffer();
	uint8_t getTxBufferSize();
	uint8_t getTxData(uint8_t position);
	void setTxData(uint8_t position, uint8_t data);
	uint8_t* getRxBuffer();
	uint8_t getRxBufferSize();
	uint8_t getRxData(uint8_t position);
	void setRxData(uint8_t position, uint8_t data);
private:
	/// Tx buffer
	uint8_t txBuffer[PACKET_SIZE];
	/// Rx buffer
	uint8_t rxBuffer[PACKET_SIZE];
};

#endif
