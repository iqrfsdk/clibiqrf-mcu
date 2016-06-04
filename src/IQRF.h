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

#ifndef IQRF_H
#define IQRF_H

#include <stdint.h>
#include <Arduino.h>

#include "IQRFBuffers.h"
#include "IQRFCallbacks.h"
#include "IQRFCRC.h"
#include "IQRFPackets.h"
#include "IQRFSPI.h"
#include "IQRFTR.h"
#include "InfoTask.h"

using namespace std;

/**
 * IQRF Main class
 */
class IQRF {
public:
	IQRF(IQRFCallbacks::rx_callback rxCallback, IQRFCallbacks::tx_callback txCallback);
	void driver();
	void getData(uint8_t *dataBuffer, uint8_t dataSize);
	uint8_t getDataLength();
	uint8_t sendData(uint8_t *dataBuffer, uint8_t dataLength, uint8_t unallocationFlag);
	void identifyTR();
private:
	IQRFBuffers* buffers = new IQRFBuffers;
	IQRFCallbacks* callbacks = new IQRFCallbacks;
	IQRFCRC* crc = new IQRFCRC;
	IQRFPackets* packets = new IQRFPackets;
	IQRFSPI* spi = new IQRFSPI;
	IQRFTR* tr = new IQRFTR;
	InfoTask* infoTask = new InfoTask;
	/// Ms counter
	unsigned long checkMs1;
	/// Ms counter
	unsigned long checkMs2;
	/// Counts number of send/receive bytes
	uint8_t byteCounter;
	/// Data length
	uint8_t dataLength;
	/// Packet length
	uint8_t packetLength;
	/// Packet ID
	uint8_t packetId;
	/// Send attempts
	uint8_t sendAttempts;
};

#endif
