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

#include "iqrf_library.h"

/*
 * Locally used function prototypes
 */
void trInfoTask();

/*
 * Public variable declarations
 */
/// Data length
uint8_t dataLength;
/// TR info structure
trInfo_t trInfo;
/// IQRF packet buffer
packetBuffer_t iqrfPacketBuffer[PACKET_BUFFER_SIZE];
/// Packet input buffer
uint16_t packetBufferInPtr;
/// Packet output buffer
uint16_t packetBufferOutPtr;
/// Packet to end program mode
const uint8_t endPgmMode[] = {0xDE, 0x01, 0xFF};

/// Instance of IQRF class
IQRF* iqrf = new IQRF;
/// Instance of IQRFBuffers class
IQRFBuffers* buffers = new IQRFBuffers;
/// Instance of IQRFCRC class
IQRFCRC* crc = new IQRFCRC;
/// Instance of IQRFCallbacks class
IQRFCallbacks* callbacks = new IQRFCallbacks;
/// Instance of IQRFPackets class
IQRFPackets* packets = new IQRFPackets;
/// Instance of IQRFSPI class
IQRFSPI* spi = new IQRFSPI;
/// Instance of IQRFTR class
IQRFTR* tr = new IQRFTR;
/// Instance of IQSPI class
IQSPI* iqSpi = new IQSPI;

/**
 * Function perform a TR-module driver initialization
 * Function performes initialization of trInfo identification data structure
 * @param rxCallback Pointer to callback function. Function is called when the driver receives data from the TR module
 * @param txCallback Pointer to callback function. unction is called when the driver sent data to the TR module
 */
void IQRF_Init(IQRFCallbacks::rxCallback_t rxCallback, IQRFCallbacks::txCallback_t txCallback) {
	spi->setMasterStatus(spi->masterStatuses::FREE);
	spi->setStatus(spi->statuses::DISABLED);
	iqrf->setUsCount0(0);
	// normal SPI communication
	spi->disableFastSpi();
	tr->turnOn();
	iqSpi->begin();
	// enable SPI master function in driver
	spi->enableMaster();
	// read TR module info
	tr->setInfoReadingStatus(2);
	// wait for TR module ID reading
	while (tr->getInfoReadingStatus()) {
		// IQRF SPI communication driver
		IQRF_Driver();
		// TR module info reading task
		trInfoTask();
	}
	// if TR72D or TR76D is conected
	if (IQRF_GetModuleType() == tr->types::TR_72D || IQRF_GetModuleType() == tr->types::TR_76D) {
		spi->enableFastSpi();
		Serial.println("IQRF_Init - set fast spi");
	}
	callbacks->setRxCallback(rxCallback);
	callbacks->setTxCallback(txCallback);
	tr->setControlStatus(tr->controlStatuses::READY);
}

/**
 * Periodically called IQRF_Driver
 */
void IQRF_Driver() {
	// SPI Master enabled
	if (spi->isMasterEnabled()) {
		iqrf->setUsCount1(micros());
		// is anything to send in Tx buffer?
		if (spi->getMasterStatus() != spi->masterStatuses::FREE) {
			// send 1 byte every defined time interval via SPI
			if ((iqrf->getUsCount1() - iqrf->getUsCount0()) > spi->getBytePause()) {
				// reset counter
				iqrf->setUsCount0(iqrf->getUsCount1());
				// send/receive 1 byte via SPI
				buffers->setRxData(iqrf->getByteCount(), iqSpi->transfer(buffers->getTxData(iqrf->getByteCount())));
				// counts number of send/receive bytes, it must be zeroing on packet preparing
				iqrf->setByteCount(iqrf->getByteCount() + 1);
				// pacLen contains length of whole packet it must be set on packet preparing sent everything? + buffer overflow protection
				if (iqrf->getByteCount() == packets->getLength() || iqrf->getByteCount() == PACKET_SIZE) {
					// CS - deactive
					//digitalWrite(TR_SS_PIN, HIGH);
					// CRC ok
					if ((buffers->getRxData(dataLength + 3) == spi->statuses::CRCM_OK) &&
						crc->check(buffers->getRxBuffer(), dataLength, iqrf->getPTYPE())) {
						if (spi->getMasterStatus() == spi->masterStatuses::WRITE) {
							callbacks->callTxCallback(packets->getId(), packets->statuses::OK);
						}
						if (spi->getMasterStatus() == spi->masterStatuses::READ) {
							callbacks->callRxCallback();
						}
						spi->setMasterStatus(spi->masterStatuses::FREE);
					} else { // CRC error
						// rep_cnt - must be set on packet preparing
						if (iqrf->getAttepmtsCount() - 1) {
							// another attempt to send data
							iqrf->setByteCount(0);
						} else {
							if (spi->getMasterStatus() == spi->masterStatuses::WRITE) {
								callbacks->callTxCallback(packets->getId(), packets->statuses::ERROR);
							}
							spi->setMasterStatus(spi->masterStatuses::FREE);
						}
					}
				}
			}
		} else { // no data to send => SPI status will be updated every 10ms
			if ((iqrf->getUsCount1() - iqrf->getUsCount0()) > (MICRO_SECOND / 100)) {
				// reset counter
				iqrf->setUsCount0(iqrf->getUsCount1());
				// get SPI status of TR module
				spi->setStatus(iqSpi->transfer(spi->commands::CHECK));
				// CS - deactive
				//digitalWrite(TR_SS_PIN, HIGH);      
				// if the status is dataready prepare packet to read it
				if ((spi->getStatus() & 0xC0) == 0x40) {
					memset(buffers->getTxBuffer(), 0, buffers->getTxBufferSize());
					// state 0x40 means 64B
					if (spi->getStatus() == 0x40) {
						dataLength = 64;
					} else {
						// clear bit 7,6 - rest is length (from 1 to 63B)
						dataLength = spi->getStatus() & 0x3F;
					}
					iqrf->setPTYPE(dataLength);
					buffers->setTxData(0, spi->commands::WR_RD);
					buffers->setTxData(1, iqrf->getPTYPE());
					// CRC
					buffers->setTxData(dataLength + 2, crc->calculate(buffers->getTxBuffer(), dataLength));
					// length of whole packet + (CMD, PTYPE, CRCM, 0)
					packets->setLength(dataLength + 4);
					// counter of sent bytes
					iqrf->setByteCount(0);
					// number of attempts to send data
					iqrf->setAttepmtsCount(1);
					// reading from buffer COM of TR module
					spi->setMasterStatus(spi->masterStatuses::READ);
					// current SPI status must be updated
					spi->setStatus(spi->statuses::DATA_TRANSFER);
				}
				// if TR module ready and no data in module pending
				if (!spi->getMasterStatus()) {
					// check if packet to send ready
					if (packetBufferInPtr != packetBufferOutPtr) {
						memset(buffers->getTxBuffer(), 0, buffers->getTxBufferSize());
						dataLength = iqrfPacketBuffer[packetBufferOutPtr].dataLength;
						// PBYTE set bit7 - write to buffer COM of TR module
						iqrf->setPTYPE(dataLength | 0x80);
						buffers->setTxData(0, iqrfPacketBuffer[packetBufferOutPtr].spiCmd);
						if (buffers->getTxData(0) == spi->commands::MODULE_INFO && dataLength == 16) {
							iqrf->setPTYPE(0x10);
						}
						buffers->setTxData(1, iqrf->getPTYPE());
						memcpy(&buffers->getTxBuffer()[2], iqrfPacketBuffer[packetBufferOutPtr].dataBuffer, dataLength);
						// CRCM
						buffers->setTxData(dataLength + 2, crc->calculate(buffers->getTxBuffer(), dataLength));
						// length of whole packet + (CMD, PTYPE, CRCM, 0)
						packets->setLength(dataLength + 4);
						// set actual TX packet ID
						packets->setId(iqrfPacketBuffer[packetBufferOutPtr].packetId);
						// counter of sent bytes
						iqrf->setByteCount(0);
						// number of attempts to send data
						iqrf->setAttepmtsCount(3);
						// writing to buffer COM of TR module
						spi->setMasterStatus(spi->masterStatuses::WRITE);
						if (iqrfPacketBuffer[packetBufferOutPtr].unallocationFlag) {
							// unallocate temporary TX data buffer
							free(iqrfPacketBuffer[packetBufferOutPtr].dataBuffer);
						}
						if (++packetBufferOutPtr >= PACKET_BUFFER_SIZE) {
							packetBufferOutPtr = 0;
						}
						// current SPI status must be updated
						spi->setStatus(spi->statuses::DATA_TRANSFER);
					}
				}
			}
		}
	} else {
		// SPI master is disabled
		tr->controlTask();
	}
}

/**
 * Function is usually called inside the callback function, whitch is called when the driver receives data from TR module
 * @param dataBuffer Pointer to my buffer, to which I want to load data received from the TR module
 * @param dataLength Number of bytes I want to read
 */
void IQRF_GetRxData(uint8_t *dataBuffer, uint8_t dataLength) {
	memcpy(dataBuffer, &buffers->getRxBuffer()[2], dataLength);
}

/**
 * Read Module Info from TR module, uses SPI master implementation
 */
void trInfoTask() {
	static uint8_t dataToModule[16];
	static uint8_t attempts;
	static unsigned long timeoutMilli;
	static uint8_t idfMode;

	static enum {
		INIT_TASK = 0,
		ENTER_PROG_MODE,
		SEND_REQUEST,
		WAIT_INFO,
		DONE
	} trInfoTaskStatus = INIT_TASK;
	switch (trInfoTaskStatus) {
		case INIT_TASK:
			// try enter to programming mode
			attempts = 1;
			// try to read idf in com mode
			idfMode = 0;
			// set call back function to process id data
			callbacks->setRxCallback(doNothingRx);
			// set call back function after data were sent
			callbacks->setTxCallback(identifyTx);
			trInfo.mcuType = tr->mcuTypes::UNKNOWN;
			memset(&dataToModule[0], 0, 16);
			timeoutMilli = millis();
			// next state - will read info in PGM mode or /* in COM mode */
			trInfoTaskStatus = ENTER_PROG_MODE /* SEND_REQUEST */;
			break;
		case ENTER_PROG_MODE:
			tr->enterProgramMode();
			// try to read idf in pgm mode
			idfMode = 1;
			// set call back function to process id data
			callbacks->setRxCallback(identifyRx);
			// set call back function after data were sent
			callbacks->setTxCallback(doNothingTx);
			timeoutMilli = millis();
			trInfoTaskStatus = SEND_REQUEST;
			break;
		case SEND_REQUEST:
			if (spi->getStatus() == spi->statuses::COMMUNICATION_MODE &&
				spi->getMasterStatus() == spi->masterStatuses::FREE) {
				TR_SendSpiPacket(spi->commands::MODULE_INFO, &dataToModule[0], 16, 0);
				// initialize timeout timer
				timeoutMilli = millis();
				trInfoTaskStatus = WAIT_INFO;
			} else {
				if (spi->getStatus() == spi->statuses::PROGRAMMING_MODE &&
					spi->getMasterStatus() == spi->masterStatuses::FREE) {
					TR_SendSpiPacket(spi->commands::MODULE_INFO, &dataToModule[0], 1, 0);
					// initialize timeout timer
					timeoutMilli = millis();
					trInfoTaskStatus = WAIT_INFO;
				} else {
					if (millis() - timeoutMilli >= MILLI_SECOND / 2) {
						// in a case, try it twice to enter programming mode
						if (attempts) {
							attempts--;
							trInfoTaskStatus = ENTER_PROG_MODE;
						} else {
							// TR module probably does not work
							trInfoTaskStatus = DONE;
						}
					}
				}
			}
			break;
			// wait for info data from TR module
		case WAIT_INFO:
			if ((tr->getInfoReadingStatus() == 1) || (millis() - timeoutMilli >= MILLI_SECOND / 2)) {
				if (idfMode == 1) {
					// send end of PGM mode packet
					TR_SendSpiPacket(spi->commands::EEPROM_PGM, (uint8_t *) & endPgmMode[0], 3, 0);
				}
				// next state
				trInfoTaskStatus = DONE;
			}
			break;
			// the task is finished
		case DONE:
			// if no packet is pending to send to TR module
			if (packetBufferInPtr == packetBufferOutPtr &&
				spi->getMasterStatus() == spi->masterStatuses::FREE) {
				tr->setInfoReadingStatus(0);
			}
			break;
	}
}

/**
 * Process identification data packet from TR module
 */
void trIdentify() {
	memcpy((uint8_t *) & trInfo.moduleInfoRawData, (uint8_t *) & buffers->getRxBuffer()[2], 8);
	trInfo.moduleId = (uint32_t) buffers->getRxBuffer()[2] << 24 | (uint32_t) buffers->getRxBuffer()[3] << 16 | (uint32_t) buffers->getRxBuffer()[4] << 8 | buffers->getRxBuffer()[5];
	trInfo.osVersion = (uint16_t) (buffers->getRxBuffer()[6] / 16) << 8 | (buffers->getRxBuffer()[6] % 16);
	trInfo.mcuType = buffers->getRxBuffer()[7] & 0x07;
	trInfo.fcc = (buffers->getRxBuffer()[7] & 0x08) >> 3;
	trInfo.moduleType = buffers->getRxBuffer()[7] >> 4;
	trInfo.osBuild = (uint16_t) buffers->getRxBuffer()[9] << 8 | buffers->getRxBuffer()[8];
	// TR info data processed
	tr->setInfoReadingStatus(tr->getInfoReadingStatus() - 1);
}

/**
 * Prepare SPI packet to packet buffer
 * @param spiCmd Command that I want to send to TR module
 * @param dataBuffer Pointer to a buffer that contains data that I want to send to TR module
 * @param dataLength Number of bytes to send
 * @param unallocationFlag If the dataBuffer is dynamically allocated using malloc function.
   If you wish to unallocate buffer after data is sent, set the unallocationFlag to 1, otherwise to 0.
 * @return Packet ID
 */
uint8_t TR_SendSpiPacket(uint8_t spiCmd, uint8_t *dataBuffer, uint8_t dataLength, uint8_t unallocationFlag) {
	iqrfPacketBuffer[packetBufferInPtr].packetId = packets->getIdCount();
	iqrfPacketBuffer[packetBufferInPtr].spiCmd = spiCmd;
	iqrfPacketBuffer[packetBufferInPtr].dataBuffer = dataBuffer;
	iqrfPacketBuffer[packetBufferInPtr].dataLength = dataLength;
	iqrfPacketBuffer[packetBufferInPtr].unallocationFlag = unallocationFlag;
	if (++packetBufferInPtr >= PACKET_BUFFER_SIZE) {
		packetBufferInPtr = 0;
	}
}
