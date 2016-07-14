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

#ifndef IQRFTR_H
#define IQRFTR_H

#if defined(__AVR__)
#include <Arduino.h>
#elif defined(__PIC32MX__)
#include <WProgram.h>
#endif

#include "IQRFSettings.h"
#include "IQRFSPI.h"
#include "IQSPI.h"

/**
 * IQRF TR
 */
class IQRFTR {
public:
	void reset();
	void enterProgramMode();
	void turnOn();
	void turnOff();
	uint8_t getControlStatus();
	void setControlStatus(uint8_t status);
	void enableProgramFlag();
	void disableProgramFlag();
	bool getProgramFlag();
	void controlTask();

	/**
	 * TR control statuses
	 */
	enum controlStatuses {
		READY = 0, //!< TR ready state
		RESET = 1, //!< TR reset process
		WAIT = 2, //!< TR wait state
		PROG_MODE = 3 //!< TR programming mode
	};

	/**
	 * TR module types
	 */
	enum types {
		TR_52D = 0, //!< TR module IQRF TR-52D
		TR_58D_RJ = 1, //!< TR module IQRF TR-85D-RJ
		TR_72D = 2, //!< TR module IQRF TR-72D
		TR_53D = 3, //!< TR module IQRF TR-53D
		TR_54D = 8, //!< TR module IQRF TR-54D
		TR_55D = 9, //!< TR module IQRF TR-55D
		TR_56D = 10, //!< TR module IQRF TR-56D
		TR_76D = 11 //!< TR module IQRF TR-76D
	};

	/**
	 * TR MCU types
	 */
	enum mcuTypes {
		UNKNOWN = 0, //!< MCU used in uknown module
		PIC16LF819 = 1, //!< MCU used in TR-xxx-11A not supported
		PIC16LF88 = 2, //!< MCU used in TR-xxx-21A
		PIC16F886 = 3, //!< MCU used in TR-31B, TR-52B, TR-53B
		PIC16LF1938 = 4 //!< MCU used in TR-52D, TR-54D
	};
private:
	/// TR control status
	uint8_t controlStatus;
	/// TR programming flag
	bool programFlag;
	/// Instance of IQRFSPI class
	IQRFSPI* spi = new IQRFSPI;
	/// Instance of IQSPI class
	IQSPI* iqSpi = new IQSPI;
};

#endif
