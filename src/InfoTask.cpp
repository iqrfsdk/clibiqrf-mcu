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

#include "InfoTask.h"

/**
 * Read Module Info from TR module, uses SPI master implementation
 */
void InfoTask::task() {
	this->status = this->statuses::INIT_TASK;
	switch (this->status) {
		case this->statuses::INIT_TASK:
			// Try enter to programming mode
			this->attempts = 1;
			// Try read idf in communication mode
			this->idfMode = 0;
			// Set callback function to process id data
			callbacks->setRx(&doNothingRx);
			// Set callback function after data were sent
			callbacks->setTx(&identifyTx);
			tr->setMcuType(tr->mcuTypes::UNKNOWN);
			memset(&this->dataToModule[0], 0, 16);
			this->timeoutMs = millis();
			// Next state - will read info in PGM mode or /* in COM mode */
			this->status = this->statuses::ENTER_PROG_MODE /* this->statuses::SEND_REQUEST */;
			break;
		case this->statuses::ENTER_PROG_MODE:
			tr->enterProgramMode();
			// Try read idf in programming mode
			this->idfMode = 1;
			// Set callback function to process id data
			callbacks->setRx(&identifyRx);
			// Set callback function after data were sent
			callbacks->setTx(&doNothingTx);
			this->timeoutMs = millis();
			this->status = this->statuses::SEND_REQUEST;
			break;
		case this->statuses::SEND_REQUEST:
			if (spi->getStatus() == spi->statuses::COMMUNICATION_MODE &&
				spi->getMasterStatus() == spi->masterStatuses::FREE) {
				packets->sendPacket(spi->commands::MODULE_INFO, &this->dataToModule[0], 16, 0);
				// Initialize timeout timer
				this->timeoutMs = millis();
				this->status = this->statuses::WAIT_INFO;
			} else {
				if (spi->getStatus() == spi->statuses::PROGRAMMING_MODE &&
					spi->getMasterStatus() == spi->masterStatuses::FREE) {
					packets->sendPacket(spi->commands::MODULE_INFO, &this->dataToModule[0], 1, 0);
					// Initialize timeout timer
					this->timeoutMs = millis();
					this->status = this->statuses::WAIT_INFO;
				} else {
					if (millis() - this->timeoutMs >= 500) {
						// In a case, try it twice to enter programming mode
						if (this->attempts) {
							this->attempts--;
							this->status = this->statuses::ENTER_PROG_MODE;
						} else {
							// TR module probably does not work
							this->status = this->statuses::DONE;
						}
					}
				}
			}
			break;
		case this->statuses::WAIT_INFO:
			if ((tr->getInfoReadingStatus() == 1) || (millis() - timeoutMs >= 500)) {
				if (idfMode == 1) {
					// Send end of PGM mode packet
					packets->sendPacket(spi->commands::EEPROM_PGM, (uint8_t *) & this->endPgmMode[0], 3, 0);
				}
				this->status = this->statuses::DONE;
			}
			break;
		case this->statuses::DONE:
			// If no packet is pending to send to TR module
			if (packets->getInputCounter() == packets->getOutputCounter() &&
				spi->getMasterStatus() == spi->masterStatuses::FREE) {
				tr->setInfoReadingStatus(0);
			}
			break;
	}
}
