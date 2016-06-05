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

#ifndef INFOTASK_H
#define INFOTASK_H

#include <stdint.h>

#include "CallbackFunctions.h"
#include "IQRFCallbacks.h"
#include "IQRFPackets.h"
#include "IQRFSPI.h"
#include "IQRFTR.h"

using namespace std;

/**
 * TR info task
 */
class InfoTask {
public:
	void task();

	/**
	 * TR info task statuses
	 */
	enum statuses {
		INIT_TASK = 0, //!< Init task
		ENTER_PROG_MODE = 1, //!< Enter to programming mode
		SEND_REQUEST = 2, //!< Send request
		WAIT_INFO = 3, //!< Wait for info
		DONE = 4 //!< Done
	};
private:
	/// TR info task status
	uint8_t status;
	/// Data to module
	uint8_t dataToModule[16];
	/// Attemps
	uint8_t attempts;
	/// Timeout in Ms
	unsigned long timeoutMs;
	/// idf mode status
	uint8_t idfMode;
	/// Packet to end program mode
	uint8_t endPgmMode[3] = {0xDE, 0x01, 0xFF};
	/// Instance of IQRFCallbacks class
	IQRFCallbacks* callbacks = new IQRFCallbacks;
	/// Instance of IQRFPackets class
	IQRFPackets* packets = new IQRFPackets;
	/// Instance of IQRFSPI class
	IQRFSPI* spi = new IQRFSPI;
	/// Instance of IQRFTR class
	IQRFTR* tr = new IQRFTR;
};

#endif
