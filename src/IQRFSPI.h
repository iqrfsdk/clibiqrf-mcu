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

#ifndef IQRFSPI_H
#define IQRFSPI_H
#define IQRF_SPI_CLOCK 250000 //!< SPI clock 250kHz

#include <stdint.h>
#include <SPI.h>

using namespace std;

/**
 * IQRF SPI
 */
class IQRFSPI {
public:
	uint8_t getStatus();
	void setStatus(uint8_t status);
	void enableMaster();
	void disableMaster();
	bool isMasterEnabled();
	uint8_t getMasterStatus();
	void setMasterStatus(uint8_t status);
	void enableFastSpi();
	void disableFastSpi();
	bool isFastSpiEnabled();
	unsigned long getBytePause();
	void setBytePause(unsigned long time);
	uint8_t byte(uint8_t txByte);

	/**
	 * SPI status of TR module (see IQRF SPI user manual)
	 */
	enum statuses {
		NO_MODULE = 0xFF, //!< SPI not working (HW error)
		BUSY = 0xFE, //!< SPI busy in Master disabled mode
		DATA_TRANSFER = 0xFD, //!< SPI data transfer in progress
		DISABLED = 0x00, //!< SPI not working (disabled)
		CRCM_OK = 0x3F, //!< SPI not ready (full buffer, last CRCM ok)
		CRCM_ERR = 0x3E, //!< SPI not ready (full buffer, last CRCM error)
		COMMUNICATION_MODE = 0x80, //!< SPI ready (communication mode)
		PROGRAMMING_MODE = 0x81, //!< SPI ready (programming mode)
		DEBUG_MODE = 0x82, //!< SPI ready (debugging mode)
		SLOW_MODE = 0x83, //!< SPI not working in background
		USER_STOP = 0x07 //!< SPI state after stopSPI();
	};

	/**
	 * IQRF SPI master statuses
	 */
	enum masterStatuses {
		FREE = 0, //!< SPI master free
		WRITE = 1, //!< SPI master wrtite
		READ = 2 //!< SPI master read
	};

	/**
	 * SPI commands for TR module (see IQRF SPI user manual)
	 */
	enum commands {
		CHECK = 0x00, //!< Master checks the SPI status of the TR module
		WR_RD = 0xF0, //!< Master reads/writes a packet from/to TR module
		RAM_READ = 0xF1, //!< Master reads data from ram in debug mode
		EEPROM_READ = 0xF2, //!< Master reads data from eeprom in debug mode
		EEPROM_PGM = 0xF3, //!< Master writes data to eeprom in programming mode
		MODULE_INFO = 0xF5, //!< Master reads Module Info from TR module
		FLASH_PGM = 0xF6, //!< Master writes data to flash in programming mode
		PLUGIN_PGM = 0xF9 //!< Master writes plugin data to flash in programming mode
	};
private:
	/// SPI master status (ON/OFF)
	bool master;
	/// SPI master status
	uint8_t masterStatus;
	/// SPI status
	uint8_t status;
	/// Fast SPI
	bool fastSpi;
	/// SPI byte to byte pause in us
	unsigned long bytePause;
};

#endif
