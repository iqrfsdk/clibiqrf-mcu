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

#ifndef IQRF_H
#define IQRF_H

#include <stdint.h>

#include "IQRFCallbacks.h"
#include "IQRFSPI.h"
#include "iqrf_library.h"

/**
 * IQRF
 */
class IQRF {
public:
	void begin(IQRFCallbacks::rxCallback_t rxCallback, IQRFCallbacks::txCallback_t txCallback);
	void driver();
	uint8_t getDataLength();
	uint8_t sendData(uint8_t *dataBuffer, uint8_t dataLength, uint8_t unallocationFlag);
	void setPTYPE(uint8_t PTYPE);
	uint8_t getPTYPE();
	void setAttepmtsCount(uint8_t attepmts);
	uint8_t getAttepmtsCount();
	void setByteCount(uint8_t count);
	uint8_t getByteCount();
	void setUsCount0(unsigned long us);
	unsigned long getUsCount0();
	void setUsCount1(unsigned long us);
	unsigned long getUsCount1();
private:
	/// PTYPE
	uint8_t PTYPE;
	/// Count of attempts to send data
	uint8_t attepmtsCounter;
	/// Count number of send/receive bytes
	uint8_t byteCounter;
	/// Microsecond counter 0
	unsigned long usCounter0;
	/// Microsecond counter 1
	unsigned long usCounter1;
	/// Instance of IQRFSPI class
	IQRFSPI spi;
};

#endif
