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

#include "iqrf_library.h"

/*
 * Locally used function prototypes
 */
void trInfoTask();
void TR_process_id_packet_com(uint8_t pktId, uint8_t pktResult);
void TR_process_id_packet_pgm();
void doNothing(uint8_t pktId, uint8_t pktResult);

/*
 * Public variable declarations
 */
/// Packet type?
uint8_t PTYPE;
/// Number of attempts to send data
uint8_t repCnt;
/// Counts number of send/receive bytes
uint8_t tmpCnt;
/// Packet length
uint8_t packetLength;
/// Info reading status
uint8_t trInfoReading;
/// Data length
uint8_t dataLength;
/// Actual Tx packet ID
uint8_t txPacketId;
/// Tx packet ID counter
uint8_t txPacketIdCounter;
/// Microsecond couter
unsigned long iqrfCheckMs;
/// Microsecond variable
unsigned long iqrfMs;
/// IQRF packet buffer
PACKET_BUFFER packetBuffer[PACKET_BUFFER_SIZE];
/// Packet input buffer
uint16_t packetBufferInPtr;
/// Packet output buffer
uint16_t packetBufferOutPtr;
/// IQRF Rx callback
RX_CALLBACK rxCallback;
/// IQRF Tx callback
TX_CALLBACK txCallback;
/// Packet to end program mode
const uint8_t endPgmMode[] = {0xDE, 0x01, 0xFF};

/// Instance of IQRFCRC class
IQRFCRC* crc = new IQRFCRC;
/// Instance of IQRFSPI class
IQRFSPI* spi = new IQRFSPI;
/// Instance of IQRFTR class
IQRFTR* tr = new IQRFTR;

/**
 * Function perform a TR-module driver initialization
 * Function performes initialization of trInfoStruct identification data structure
 * @param rx_call_back_fn Pointer to callback function. Function is called when the driver receives data from the TR module
 * @param tx_call_back_fn Pointer to callback function. unction is called when the driver sent data to the TR module
 */
void IQRF_Init(RX_CALLBACK rx_call_back_fn, TX_CALLBACK tx_call_back_fn) {
	spi->setMasterStatus(spi->masterStatuses::FREE);
	spi->setStatus(spi->statuses::DISABLED);
	iqrfCheckMs = 0;
	// normal SPI communication
	spi->disableFastSpi();
	tr->turnOn();
	pinMode(TR_SS_IO, OUTPUT);
	digitalWrite(TR_SS_IO, HIGH);
	SPI.begin();
	// enable SPI master function in driver
	spi->enableMaster();
	// read TR module info
	trInfoReading = 2;
	// wait for TR module ID reading
	while (trInfoReading) {
		// IQRF SPI communication driver
		IQRF_Driver();
		// TR module info reading task
		trInfoTask();
	}
	// if TR72D or TR76D is conected
	if (tr->getModuleType() == tr->types::TR_72D || tr->getModuleType() == tr->types::TR_76D) {
		spi->enableFastSpi();
		Serial.println("IQRF_Init - set fast spi");
	}
	rxCallback = rx_call_back_fn;
	txCallback = tx_call_back_fn;
	tr->setControlStatus(tr->controlStatuses::READY);
}

/**
 * Periodically called IQRF_Driver
 */
void IQRF_Driver() {
	// SPI Master enabled
	if (spi->isMasterEnabled()) {
		iqrfMs = micros();
		// is anything to send in Tx buffer?
		if (spi->getMasterStatus() != spi->masterStatuses::FREE) {
			// send 1 byte every defined time interval via SPI
			if ((iqrfMs - iqrfCheckMs) > spi->getBytePause()) {
				// reset counter
				iqrfCheckMs = iqrfMs;
				// send/receive 1 byte via SPI
				spi->setRxData(tmpCnt, spi->byte(spi->getTxData(tmpCnt)));
				// counts number of send/receive bytes, it must be zeroing on packet preparing
				tmpCnt++;
				// pacLen contains length of whole packet it must be set on packet preparing sent everything? + buffer overflow protection
				if (tmpCnt == packetLength || tmpCnt == PACKET_SIZE) {
					// CS - deactive
					//digitalWrite(TR_SS_IO, HIGH);
					// CRC ok
					if ((spi->getRxData(dataLength + 3) == spi->statuses::CRCM_OK) &&
						crc->check(spi->getRxBuffer(), dataLength, PTYPE)) {
						if (spi->getMasterStatus() == spi->masterStatuses::WRITE) {
							txCallback(txPacketId, txPacketStatuses::OK);
						}
						if (spi->getMasterStatus() == spi->masterStatuses::READ) {
							rxCallback();
						}
						spi->setMasterStatus(spi->masterStatuses::FREE);
					} else { // CRC error
						// rep_cnt - must be set on packet preparing
						if (--repCnt) {
							// another attempt to send data
							tmpCnt = 0;
						} else {
							if (spi->getMasterStatus() == spi->masterStatuses::WRITE) {
								txCallback(txPacketId, txPacketStatuses::ERROR);
							}
							spi->setMasterStatus(spi->masterStatuses::FREE);
						}
					}
				}
			}
		} else { // no data to send => SPI status will be updated every 10ms
			if ((iqrfMs - iqrfCheckMs) > (MICRO_SECOND / 100)) {
				// reset counter
				iqrfCheckMs = iqrfMs;
				// get SPI status of TR module
				spi->setStatus(spi->byte(spi->commands::CHECK));
				// CS - deactive
				//digitalWrite(TR_SS_IO, HIGH);      
				// if the status is dataready prepare packet to read it
				if ((spi->getStatus() & 0xC0) == 0x40) {
					memset(spi->getTxBuffer(), 0, sizeof(spi->getTxBuffer()));
					// state 0x40 means 64B
					if (spi->getStatus() == 0x40) {
						dataLength = 64;
					} else {
						// clear bit 7,6 - rest is length (from 1 to 63B)
						dataLength = spi->getStatus() & 0x3F;
					}
					PTYPE = dataLength;
					spi->setTxData(0, spi->commands::WR_RD);
					spi->setTxData(1, PTYPE);
					// CRC
					spi->setTxData(dataLength + 2, crc->calculate(spi->getTxBuffer(), dataLength))
					// length of whole packet + (CMD, PTYPE, CRCM, 0)
					packetLength = dataLength + 4;
					// counter of sent bytes
					tmpCnt = 0;
					// number of attempts to send data
					repCnt = 1;
					// reading from buffer COM of TR module
					spi->setMasterStatus(spi->masterStatuses::READ);
					// current SPI status must be updated
					spi->setStatus(spi->statuses::DATA_TRANSFER);
				}
				// if TR module ready and no data in module pending
				if (!spi->getMasterStatus()) {
					// check if packet to send ready
					if (packetBufferInPtr != packetBufferOutPtr) {
						memset(spi->getTxBuffer(), 0, sizeof(spi->getTxBuffer()));
						dataLength = packetBuffer[packetBufferOutPtr].dataLength;
						// PBYTE set bit7 - write to buffer COM of TR module
						PTYPE = (dataLength | 0x80);
						spi->setTxData(0, packetBuffer[packetBufferOutPtr].spiCmd);
						if (spi->getTxData(0) == spi->commands::MODULE_INFO && dataLength == 16) {
							PTYPE = 0x10;
						}
						spi->setTxData(1, PTYPE);
						memcpy(&spi->getTxData(2), packetBuffer[packetBufferOutPtr].dataBuffer, dataLength);
						// CRCM
						spi->setTxData(dataLength + 2, crc->calculate(spi->getTxBuffer(), dataLength));
						// length of whole packet + (CMD, PTYPE, CRCM, 0)
						packetLength = dataLength + 4;
						// set actual TX packet ID
						txPacketId = packetBuffer[packetBufferOutPtr].packetId;
						// counter of sent bytes
						tmpCnt = 0;
						// number of attempts to send data
						repCnt = 3;
						// writing to buffer COM of TR module
						spi->setMasterStatus(spi->masterStatuses::WRITE);
						if (packetBuffer[packetBufferOutPtr].unallocationFlag) {
							// unallocate temporary TX data buffer
							free(packetBuffer[packetBufferOutPtr].dataBuffer);
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
 * Function sends data from buffer to TR module
 * @param pDataBuffer Pointer to a buffer that contains data that I want to send to TR module
 * @param dataLength Number of bytes to send
 * @param unallocationFlag If the pDataBuffer is dynamically allocated using malloc function.
   If you wish to unallocate buffer after data is sent, set the unallocationFlag to 1, otherwise to 0.
 * @return TX packet ID (number 1-255)
 */
uint8_t IQRF_SendData(uint8_t *pDataBuffer, uint8_t dataLength, uint8_t unallocationFlag) {
	return TR_SendSpiPacket(spi->commands::WR_RD, pDataBuffer, dataLength, unallocationFlag);
}

/**
 * Function is usually called inside the callback function, whitch is called when the driver receives data from TR module
 * @param userDataBuffer Pointer to my buffer, to which I want to load data received from the TR module
 * @param rxDataSize Number of bytes I want to read
 */
void IQRF_GetRxData(uint8_t *userDataBuffer, uint8_t rxDataSize) {
	memcpy(userDataBuffer, &spi->getRxData(2), rxDataSize);
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
			rxCallback = doNothing;
			// set call back function after data were sent
			txCallback = TR_process_id_packet_com;
			tr->setMcuType(tr->mcuTypes::UNKNOWN);
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
			rxCallback = TR_process_id_packet_pgm;
			// set call back function after data were sent
			txCallback = doNothing;
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
			if ((trInfoReading == 1) || (millis() - timeoutMilli >= MILLI_SECOND / 2)) {
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
				trInfoReading = 0;
			}
			break;
	}
}

/**
 * Process identification data packet from TR module
 * @param pktId Packet ID
 * @param pktResult Operation result
 */
void TR_process_id_packet_com(uint8_t pktId, uint8_t pktResult) {
	TR_process_id_packet_pgm();
}

/**
 * Process identification data packet from TR module
 */
void TR_process_id_packet_pgm() {
	tr->identify();
	// TR info data processed
	trInfoReading--;
}

/**
 * Function called after TR module identification request were sent
 * @param pktId Packet ID
 * @param pktResult Operation result
 */
void doNothing(uint8_t pktId = 0, uint8_t pktResult = 0) {
	__asm__("nop\n\t");
}

/**
 * Prepare SPI packet to packet buffer
 * @param spiCmd Command that I want to send to TR module
 * @param pDataBuffer Pointer to a buffer that contains data that I want to send to TR module
 * @param dataLength Number of bytes to send
 * @param unallocationFlag If the pDataBuffer is dynamically allocated using malloc function.
   If you wish to unallocate buffer after data is sent, set the unallocationFlag to 1, otherwise to 0.
 * @return Packet ID
 */
uint8_t TR_SendSpiPacket(uint8_t spiCmd, uint8_t *dataBuffer, uint8_t dataLength, uint8_t unallocationFlag) {
	if (dataLength == 0) {
		return 0;
	}
	if (dataLength > PACKET_SIZE - 4) {
		dataLength = PACKET_SIZE - 4;
	}
	if ((++txPacketIdCounter) == 0) {
		txPacketIdCounter++;
	}
	packetBuffer[packetBufferInPtr].packetId = txPacketIdCounter;
	packetBuffer[packetBufferInPtr].spiCmd = spiCmd;
	packetBuffer[packetBufferInPtr].dataBuffer = dataBuffer;
	packetBuffer[packetBufferInPtr].dataLength = dataLength;
	packetBuffer[packetBufferInPtr].unallocationFlag = unallocationFlag;
	if (++packetBufferInPtr >= PACKET_BUFFER_SIZE) {
		packetBufferInPtr = 0;
	}
	return txPacketIdCounter;
}