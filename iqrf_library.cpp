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
void trInfoTask(void);
void TR_process_id_packet_com(uint8_t pktId, uint8_t pktResult);
void TR_process_id_packet_pgm(void);
void TR_dummy_func_com(void);
void TR_dummy_func_pgm(uint8_t pktId, uint8_t pktResult);

/*
 * Public variable declarations
 */
/// SPI Tx buffer
uint8_t spiTxBuffer[IQ_PKT_SIZE];
/// SPI Rx buffer
uint8_t spiRxBuffer[IQ_PKT_SIZE];
uint8_t PTYPE;
/// Number of attempts to send data
uint8_t repCnt;
/// Counts number of send/receive bytes
uint8_t tmpCnt;
/// Packet length
uint8_t packetLength;
uint8_t trInfoReading;
/// Data length
uint8_t dataLength;
uint8_t spiIqBusy;
/// Actual Tx packet ID
uint8_t txPacketId;
/// Tx packet ID counter
uint8_t txPacketIdCounter;
/// Microsecond couter
unsigned long iqrfCheckMicros;
/// Microsecond variable
unsigned long iqrfMicros;
/// TR info structure
TR_INFO_STRUCT trInfoStruct;
/// IQRF packet buffer
IQRF_PACKET_BUFFER iqrfPacketBuffer[PACKET_BUFFER_SIZE];
/// Packet input buffer
uint16_t packetBufferInPtr;
/// Packet output buffer
uint16_t packetBufferOutPtr;
/// IQRF Rx callback
IQRF_RX_CALLBACK rxCallback;
/// IQRF Tx callback
IQRF_TX_CALLBACK txCallback;
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
void IQRF_Init(IQRF_RX_CALLBACK rx_call_back_fn, IQRF_TX_CALLBACK tx_call_back_fn) {
	spiIqBusy = 0;
	spi->setStatus(spi->statuses::DISABLED);
	iqrfCheckMicros = 0;
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
	if (IQRF_GetModuleType() == tr->types::TR_72D || IQRF_GetModuleType() == tr->types::TR_76D) {
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
void IQRF_Driver(void) {
	// SPI Master enabled
	if (spi->isMasterEnabled()) {
		iqrfMicros = micros();
		// is anything to send in spiTxBuffer?
		if (spiIqBusy != spi->masterStatuses::FREE) {
			// send 1 byte every defined time interval via SPI
			if ((iqrfMicros - iqrfCheckMicros) > spi->getBytePause()) {
				// reset counter
				iqrfCheckMicros = iqrfMicros;
				// send/receive 1 byte via SPI
				spiRxBuffer[tmpCnt] = spi->byte(spiTxBuffer[tmpCnt]);
				// counts number of send/receive bytes, it must be zeroing on packet preparing
				tmpCnt++;
				// pacLen contains length of whole packet it must be set on packet preparing sent everything? + buffer overflow protection
				if (tmpCnt == packetLength || tmpCnt == IQ_PKT_SIZE) {
					// CS - deactive
					//digitalWrite(TR_SS_IO, HIGH);
					// CRC ok
					if ((spiRxBuffer[dataLength + 3] == spi->statuses::CRCM_OK) &&
							crc->check(spiRxBuffer, dataLength, PTYPE)) {
						if (spiIqBusy == spi->masterStatuses::WRITE) {
							txCallback(txPacketId, txPacketStatuses::OK);
						}
						if (spiIqBusy == spi->masterStatuses::READ) {
							rxCallback();
						}
						spiIqBusy = spi->masterStatuses::FREE;
					} else { // CRC error
						// rep_cnt - must be set on packet preparing
						if (--repCnt) {
							// another attempt to send data
							tmpCnt = 0;
						} else {
							if (spiIqBusy == spi->masterStatuses::WRITE) {
								txCallback(txPacketId, txPacketStatuses::ERROR);
							}
							spiIqBusy = spi->masterStatuses::FREE;
						}
					}
				}
			}
		} else { // no data to send => SPI status will be updated every 10ms
			if ((iqrfMicros - iqrfCheckMicros) > (MICRO_SECOND / 100)) {
				// reset counter
				iqrfCheckMicros = iqrfMicros;
				// get SPI status of TR module
				spi->setStatus(spi->byte(spi->commands::CHECK));
				// CS - deactive
				//digitalWrite(TR_SS_IO, HIGH);      
				// if the status is dataready prepare packet to read it
				if ((spi->getStatus() & 0xC0) == 0x40) {
					memset(spiTxBuffer, 0, sizeof (spiTxBuffer));
					// state 0x40 means 64B
					if (spi->getStatus() == 0x40) {
						dataLength = 64;
					} else {
						// clear bit 7,6 - rest is length (1 az 63B)
						dataLength = spi->getStatus() & 0x3F;
					}
					PTYPE = dataLength;
					spiTxBuffer[0] = spi->commands::WR_RD;
					spiTxBuffer[1] = PTYPE;
					// CRC
					spiTxBuffer[dataLength + 2] = crc->calculate(spiTxBuffer, dataLength);
					// length of whole packet + (CMD, PTYPE, CRCM, 0)
					packetLength = dataLength + 4;
					// counter of sent bytes
					tmpCnt = 0;
					// number of attempts to send data
					repCnt = 1;
					// reading from buffer COM of TR module
					spiIqBusy = spi->masterStatuses::READ;
					// current SPI status must be updated
					spi->setStatus(spi->statuses::DATA_TRANSFER);
				}
				// if TR module ready and no data in module pending
				if (!spiIqBusy) {
					// check if packet to send ready
					if (packetBufferInPtr != packetBufferOutPtr) {
						memset(spiTxBuffer, 0, sizeof (spiTxBuffer));
						dataLength = iqrfPacketBuffer[packetBufferOutPtr].dataLength;
						// PBYTE set bit7 - write to buffer COM of TR module
						PTYPE = (dataLength | 0x80);
						spiTxBuffer[0] = iqrfPacketBuffer[packetBufferOutPtr].spiCmd;
						if (spiTxBuffer[0] == spi->commands::MODULE_INFO && dataLength == 16) {
							PTYPE = 0x10;
						}
						spiTxBuffer[1] = PTYPE;
						memcpy(&spiTxBuffer[2], iqrfPacketBuffer[packetBufferOutPtr].dataBuffer, dataLength);
						// CRCM
						spiTxBuffer[dataLength + 2] = crc->calculate(spiTxBuffer, dataLength);
						// length of whole packet + (CMD, PTYPE, CRCM, 0)
						packetLength = dataLength + 4;
						// set actual TX packet ID
						txPacketId = iqrfPacketBuffer[packetBufferOutPtr].packetId;
						// counter of sent bytes
						tmpCnt = 0;
						// number of attempts to send data
						repCnt = 3;
						// writing to buffer COM of TR module
						spiIqBusy = spi->masterStatuses::WRITE;
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
	memcpy(userDataBuffer, &spiRxBuffer[2], rxDataSize);
}

/**
 * Read Module Info from TR module, uses SPI master implementation
 */
void trInfoTask(void) {
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
			rxCallback = TR_dummy_func_com;
			// set call back function after data were sent
			txCallback = TR_process_id_packet_com;
			trInfoStruct.mcuType = tr->mcuTypes::UNKNOWN;
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
			txCallback = TR_dummy_func_pgm;
			timeoutMilli = millis();
			trInfoTaskStatus = SEND_REQUEST;
			break;
		case SEND_REQUEST:
			if (spi->getStatus() == spi->statuses::COMMUNICATION_MODE && spiIqBusy == 0) {
				TR_SendSpiPacket(spi->commands::MODULE_INFO, &dataToModule[0], 16, 0);
				// initialize timeout timer
				timeoutMilli = millis();
				trInfoTaskStatus = WAIT_INFO;
			} else {
				if (spi->getStatus() == spi->statuses::PROGRAMMING_MODE && spiIqBusy == 0) {
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
			if (packetBufferInPtr == packetBufferOutPtr && spiIqBusy == 0) {
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
void TR_process_id_packet_pgm(void) {
	memcpy((uint8_t *) & trInfoStruct.moduleInfoRawData, (uint8_t *) & spiRxBuffer[2], 8);
	trInfoStruct.moduleId = (uint32_t) spiRxBuffer[2] << 24 | (uint32_t) spiRxBuffer[3] << 16 | (uint32_t) spiRxBuffer[4] << 8 | spiRxBuffer[5];
	trInfoStruct.osVersion = (uint16_t) (spiRxBuffer[6] / 16) << 8 | (spiRxBuffer[6] % 16);
	trInfoStruct.mcuType = spiRxBuffer[7] & 0x07;
	trInfoStruct.fcc = (spiRxBuffer[7] & 0x08) >> 3;
	trInfoStruct.moduleType = spiRxBuffer[7] >> 4;
	trInfoStruct.osBuild = (uint16_t) spiRxBuffer[9] << 8 | spiRxBuffer[8];
	// TR info data processed
	trInfoReading--;
}

/**
 * Function called after TR module identification request were sent
 * @param pktId Packet ID
 * @param pktResult Operation result
 */
void TR_dummy_func_pgm(uint8_t pktId, uint8_t pktResult) {
	__asm__("nop\n\t");
}

/**
 * Function called after TR module identification request were sent
 */
void TR_dummy_func_com(void) {
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
uint8_t TR_SendSpiPacket(uint8_t spiCmd, uint8_t *pDataBuffer, uint8_t dataLength, uint8_t unallocationFlag) {
	if (dataLength == 0) {
		return 0;
	}
	if (dataLength > IQ_PKT_SIZE - 4) {
		dataLength = IQ_PKT_SIZE - 4;
	}
	if ((++txPacketIdCounter) == 0) {
		txPacketIdCounter++;
	}
	iqrfPacketBuffer[packetBufferInPtr].packetId = txPacketIdCounter;
	iqrfPacketBuffer[packetBufferInPtr].spiCmd = spiCmd;
	iqrfPacketBuffer[packetBufferInPtr].dataBuffer = pDataBuffer;
	iqrfPacketBuffer[packetBufferInPtr].dataLength = dataLength;
	iqrfPacketBuffer[packetBufferInPtr].unallocationFlag = unallocationFlag;
	if (++packetBufferInPtr >= PACKET_BUFFER_SIZE) {
		packetBufferInPtr = 0;
	}
	return txPacketIdCounter;
}