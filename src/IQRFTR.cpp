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

#include "IQRFTR.h"

/**
 * Reset TR module
 */
void IQRFTR::reset() {
	if (spi->isMasterEnabled()) {
		this->turnOff();
		// RESET pause
		delay(100);
		this->turnOn();
		delay(1);
	} else {
		spi->setStatus(spi->statuses::BUSY);
	}
}

/**
 * TR module switch to programming mode
 */
void IQRFTR::enterProgramMode() {
	if (spi->isMasterEnabled()) {
		iqSpi->end();
		this->reset();
		pinMode(TR_SS_PIN, OUTPUT);
		pinMode(TR_MOSI_PIN, OUTPUT);
		pinMode(TR_MISO_PIN, INPUT);
		digitalWrite(TR_SS_PIN, LOW);
		unsigned long enterMs = millis();
		do {
			// Copy MOSI to MISO for approx. 500ms => TR into programming mode
			digitalWrite(TR_MOSI_PIN, digitalRead(TR_MISO_PIN));
		} while ((millis() - enterMs) < (MILLI_SECOND / 2));
		iqSpi->begin();
	} else {
		this->setControlStatus(controlStatuses::RESET);
		this->enableProgramFlag();
		spi->setStatus(spi->statuses::BUSY);
	}
}

/**
 * Enter TR module into ON state
 */
void IQRFTR::turnOn() {
	pinMode(TR_SS_PIN, OUTPUT);
	pinMode(TR_RESET_PIN, OUTPUT);
	digitalWrite(TR_SS_PIN, HIGH);
	digitalWrite(TR_RESET_PIN, LOW);
}

/**
 * Enter TR module into OFF state
 */
void IQRFTR::turnOff() {
	pinMode(TR_SS_PIN, OUTPUT);
	pinMode(TR_RESET_PIN, OUTPUT);
	digitalWrite(TR_SS_PIN, LOW);
	digitalWrite(TR_RESET_PIN, HIGH);
}

/**
 * Get TR control status
 * @return TR control status
 */
uint8_t IQRFTR::getControlStatus() {
	return this->controlStatus;
}

/**
 * Set TR control status
 * @param status TR control status
 */
void IQRFTR::setControlStatus(uint8_t status) {
	this->controlStatus = status;
}

/**
 * Enable programming flag
 */
void IQRFTR::enableProgramFlag() {
	this->programFlag = true;
}

/**
 * Disable programming flag
 */
void IQRFTR::disableProgramFlag() {
	this->programFlag = false;
}

/**
 * Get programming flag
 * @return Programming flag
 */
bool IQRFTR::getProgramFlag() {
	return this->programFlag;
}

/**
 * Make TR module reset or switch to prog mode when SPI master is disabled
 */
void IQRFTR::controlTask() {
	static unsigned long timeoutMs;
	switch (this->getControlStatus()) {
		case controlStatuses::READY:
			spi->setStatus(spi->statuses::DISABLED);
			this->disableProgramFlag();
			break;
		case controlStatuses::RESET:
			spi->setStatus(spi->statuses::BUSY);
			iqSpi->end();
			this->turnOff();
			timeoutMs = millis();
			this->setControlStatus(controlStatuses::WAIT);
			break;
		case controlStatuses::WAIT:
			spi->setStatus(spi->statuses::BUSY);
			if (millis() - timeoutMs >= MILLI_SECOND / 3) {
				this->setControlStatus(controlStatuses::PROG_MODE);
			} else {
				iqSpi->begin();
				this->setControlStatus(controlStatuses::READY);
			}
			break;
		case controlStatuses::PROG_MODE:
			this->enterProgramMode();
			this->setControlStatus(controlStatuses::READY);
			break;
	}
}

/**
 * Set TR info reading status
 * @param status TR info reading status
 */
void IQRFTR::setInfoReadingStatus(uint8_t status) {
	this->infoReading = status;
}

/**
 * Get TR info reading status
 * @return TR info reading status
 */
uint8_t IQRFTR::getInfoReadingStatus() {
	return this->infoReading;
}

void IQRFTR::identify() {
	// TR info data processed
	this->infoReading--;
}
