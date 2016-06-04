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

#include "IQRF.h"

/**
 * Function perform a TR-module driver initialization
 * Function performes initialization of trInfoStruct identification data structure
 * @param rxCallback Pointer to callback function. Function is called when the driver receives data from the TR module
 * @param txCallback Pointer to callback function. unction is called when the driver sent data to the TR module
 */
IQRF::IQRF(IQRFCallbacks::rx_callback rxCallback, IQRFCallbacks::tx_callback txCallback) {
	spi->setMasterStatus(spi->masterStatuses::FREE);
	spi->setStatus(spi->statuses::DISABLED);
	// Normal SPI communication
	spi->disableFastSpi();
	tr->turnOn();
	pinMode(Arduino_h::SS, OUTPUT);
	digitalWrite(Arduino_h::SS, HIGH);
	SPI.begin();
	// Enable SPI master function in driver
	spi->enableMaster();
	// Set TR info reading status
	tr->setInfoReadingStatus(2);
	// Wait for TR info reading
	while (tr->getInfoReadingStatus()) {
		// IQRF SPI communication driver
		this->driver();
		// TR module info reading task
		infoTask->task();
	}
	// If TR72D or TR76D is conected
	if (tr->getModuleType() == tr->types::TR_72D || tr->getModuleType() == tr->types::TR_76D) {
		spi->enableFastSpi();
		Serial.println("IQRF_Init - set fast spi");
	}
	callbacks->setRx(rxCallback);
	callbacks->setTx(txCallback);
	tr->setControlStatus(tr->controlStatuses::READY);
}

/**
 * IQRF Driver
 */
void IQRF::driver() {
	if (spi->isMasterEnabled()) {
		this->checkMs1 = micros();
		// is anything to send in spiTxBuffer?
		if (spi->getMasterStatus() != spi->masterStatuses::FREE) {
			// send 1 byte every defined time interval via SPI
			if ((this->checkMs1 - this->checkMs2) > spi->getBytePause()) {
				// Reset Ms counter
				this->checkMs2 = this->checkMs1;
				// Send or receive 1 byte via SPI
				buffers->setRxData(this->byteCounter, spi->byte(buffers->getTxData(this->byteCounter)));
				// Counts number of send/receive bytes, it must be zeroing on packet preparing
				this->byteCounter++;
				// packetLength contains length of whole packet it must be set on packet preparing sent everything? + buffer overflow protection
				if (this->byteCounter == this->packetLength || this->byteCounter == PACKET_SIZE) {
					// CS deactivate
					//digitalWrite(Arduino_h::SS, HIGH);
					// CRC ok
					if ((buffers->getRxData(this->dataLength + 3) == spi->statuses::CRCM_OK) &&
						crc->check(buffers->getRxBuffer(), this->dataLength, packets->getType())) {
						if (spi->getMasterStatus() == spi->masterStatuses::WRITE) {
							callbacks->getTx(this->packetId, packets->statuses::OK);
						}
						if (spi->getMasterStatus() == spi->masterStatuses::READ) {
							callbacks->getRx();
						}
						spi->setMasterStatus(spi->masterStatuses::FREE);
					} else {
						if (--this->sendAttempts) {
							this->byteCounter = 0;
						} else {
							if (spi->getMasterStatus() == spi->masterStatuses::WRITE) {
								callbacks->getTx(this->packetId, packets->statuses::ERROR);
							}
							spi->setMasterStatus(spi->masterStatuses::FREE);
						}
					}
				}
			}
		} else {
			// No data to send => SPI status will be updated every 10ms
			if ((this->checkMs1 - this->checkMs2) > 10) {
				// Reset Ms counter
				this->checkMs2 = this->checkMs1;
				spi->setStatus(spi->byte(spi->commands::CHECK));
				// CS - deactive
				//digitalWrite(Arduino_h::SS, HIGH);
				// If the status is dataready prepare packet to read it
				if ((spi->getStatus() & 0xC0) == 0x40) {
					memset(buffers->getTxBuffer(), 0, buffers->getTxBufferSize());
					// State 0x40 means 64B
					if (spi->getStatus() == 0x40) {
						this->dataLength = 64;
					} else {
						// clear bit 7,6 - rest is length (from 1 to 63B)
						this->dataLength = spi->getStatus() & 0x3F;
					}
					packets->setType(this->dataLength);
					buffers->setTxData(0, spi->commands::WR_RD);
					buffers->setTxData(1, packets->getType());
					// CRC
					buffers->setTxData(this->dataLength + 2, crc->calculate(buffers->getTxBuffer(), this->dataLength));
					// Length of whole packet + (CMD, PYTPE, CRC, 0)
					this->packetLength = this->dataLength + 4;
					this->byteCounter = 0;
					this->sendAttempts = 1;
					// Reading from buffer COM of TR module
					spi->setMasterStatus(spi->masterStatuses::READ);
					// Current SPI status must be updated
					spi->setStatus(spi->statuses::DATA_TRANSFER);
				}
				// if TR module ready and no data in module pending and packet is ready to send
				if (spi->getMasterStatus() == spi->masterStatuses::FREE &&
					packets->getInputCounter() != packets->getOutputCounter()) {
					memset(buffers->getTxBuffer(), 0, buffers->getTxBufferSize());
					this->dataLength = packets->getDataLength(packets->getOutputCounter());
					// PBYTE set bit7 - write to buffer COM of TR module
					packets->setType(dataLength | 0x80);
					buffers->setTxData(0, packets->getSpiCommand(packets->getOutputCounter()));
					if (buffers->getTxData(0) == spi->commands::MODULE_INFO && this->dataLength == 16) {
						packets->setType(0x10);
					}
					buffers->setTxData(1, packets->getType());
					memcpy(&buffers->getTxBuffer()[2], packets->getDataBuffer(packets->getOutputCounter()), this->dataLength);
					// CRC
					buffers->setTxData(this->dataLength + 2, crc->calculate(buffers->getTxBuffer(), this->dataLength));
					// Length of whole packet + (CMD, PYTPE, CRC, 0)
					this->packetLength = dataLength + 4;
					// Set actual packet ID
					this->packetId = packets->getPacketId(packets->getOutputCounter());
					this->byteCounter = 0;
					this->sendAttempts = 3;
					// Writing to buffer COM of TR module
					spi->setMasterStatus(spi->masterStatuses::WRITE);
					if (packets->getUnallocationFlag(packets->getOutputCounter())) {
						// Unallocate temporary Tx buffer
						free(packets->getDataBuffer(packets->getOutputCounter()));
					}
					if ((packets->getOutputCounter() + 1) >= PACKET_BUFFER_SIZE) {
						packets->setOutputCounter(0);
					}
					spi->setStatus(spi->statuses::DATA_TRANSFER);
				}
			}
		}
	} else {
		tr->controlTask();
	}
}

/**
 * Function is usually called inside the callback function, whitch is called when the driver receives data from TR module
 * @param dataBuffer Pointer to my buffer, to which I want to load data received from the TR module
 * @param dataSize Number of bytes I want to read
 */
void IQRF::getData(uint8_t *dataBuffer, uint8_t dataSize) {
	memcpy(dataBuffer, &buffers->getRxBuffer()[2], dataSize);
}

/**
 * Get data length
 * @return Data length
 */
uint8_t IQRF::getDataLength() {
	return this->dataLength;
}

/**
 * Function sends data from buffer to TR module
 * @param dataBuffer Pointer to a buffer that contains data that I want to send to TR module
 * @param dataLength Number of bytes to send
 * @param unallocationFlag If the pDataBuffer is dynamically allocated using malloc function.
   If you wish to unallocate buffer after data is sent, set the unallocationFlag to 1, otherwise to 0.
 * @return Tx packet ID (number 1-255)
 */
uint8_t IQRF::sendData(uint8_t* dataBuffer, uint8_t dataLength, uint8_t unallocationFlag) {
	return packets->sendPacket(spi->commands::WR_RD, dataBuffer, dataLength, unallocationFlag);
}

/**
 * Print TR module type to Serial
 */
void IQRF::identifyTR() {
	switch (tr->getModuleType()) {
		case tr->types::TR_52D:
			Serial.println("Module type: TR-52D");
			break;
		case tr->types::TR_53D:
			Serial.println("Module type: TR-53D");
			break;
		case tr->types::TR_54D:
			Serial.println("Module type: TR-54D");
			break;
		case tr->types::TR_55D:
			Serial.println("Module type: TR-55D");
			break;
		case tr->types::TR_56D:
			Serial.println("Module type: TR-56D");
			break;
		case tr->types::TR_58D_RJ:
			Serial.println("Module type: TR-58D-RJ");
			break;
		case tr->types::TR_72D:
			Serial.println("Module type: TR-72D");
			break;
		case tr->types::TR_76D:
			Serial.println("Module type: TR-76D");
			break;
		default:
			Serial.println("Module type: UKNOWN");
			break;
	}
}
