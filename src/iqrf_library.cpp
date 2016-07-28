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
IQRF _iqrf;
/// Instance of IQRFBuffers class
IQRFBuffers _buffers;
/// Instance of IQRFCRC class
IQRFCRC _crc;
/// Instance of IQRFCallbacks class
IQRFCallbacks _callbacks;
/// Instance of IQRFPackets class
IQRFPackets _packets;
/// Instance of IQRFSPI class
IQRFSPI _spi;
/// Instance of IQRFTR class
IQRFTR _tr;
/// Instance of IQSPI class
IQSPI _iqSpi;

/**
 * Function perform a TR-module driver initialization
 * Function performes initialization of trInfo identification data structure
 * @param rxCallback Pointer to callback function. Function is called when the driver receives data from the TR module
 * @param txCallback Pointer to callback function. unction is called when the driver sent data to the TR module
 */
void IQRF_Init(IQRFCallbacks::rxCallback_t rxCallback, IQRFCallbacks::txCallback_t txCallback) {
	_spi.setMasterStatus(_spi.masterStatuses::FREE);
	_spi.setStatus(_spi.statuses::DISABLED);
	_iqrf.setUsCount0(0);
	// normal SPI communication
	_spi.disableFastSpi();
	_tr.turnOn();
	_iqSpi.begin();
	// enable SPI master function in driver
	_spi.enableMaster();
	// read TR module info
	_tr.setInfoReadingStatus(2);
	// wait for TR module ID reading
	while (_tr.getInfoReadingStatus()) {
		// IQRF SPI communication driver
		_iqrf.driver();
		// TR module info reading task
		trInfoTask();
	}
	// if TR72D or TR76D is conected
	if (trInfo.moduleType == _tr.types::TR_72D || trInfo.moduleType == _tr.types::TR_76D) {
		_spi.enableFastSpi();
		Serial.println("IQRF_Init - set fast spi");
	}
	_callbacks.setRxCallback(rxCallback);
	_callbacks.setTxCallback(txCallback);
	_tr.setControlStatus(_tr.controlStatuses::READY);
}

/**
 * Periodically called IQRF_Driver
 */
void IQRF_Driver() {
	// SPI Master enabled
	if (_spi.isMasterEnabled()) {
		_iqrf.setUsCount1(micros());
		// is anything to send in Tx buffer?
		if (_spi.getMasterStatus() != _spi.masterStatuses::FREE) {
			// send 1 byte every defined time interval via SPI
			if ((_iqrf.getUsCount1() - _iqrf.getUsCount0()) > _spi.getBytePause()) {
				// reset counter
				_iqrf.setUsCount0(_iqrf.getUsCount1());
				// send/receive 1 byte via SPI
				_buffers.setRxData(_iqrf.getByteCount(), _iqSpi.transfer(_buffers.getTxData(_iqrf.getByteCount())));
				// counts number of send/receive bytes, it must be zeroing on packet preparing
				_iqrf.setByteCount(_iqrf.getByteCount() + 1);
				// pacLen contains length of whole packet it must be set on packet preparing sent everything? + buffer overflow protection
				if (_iqrf.getByteCount() == _packets.getLength() || _iqrf.getByteCount() == PACKET_SIZE) {
					// CS - deactive
					//digitalWrite(TR_SS_PIN, HIGH);
					// CRC ok
					if ((_buffers.getRxData(dataLength + 3) == _spi.statuses::CRCM_OK) &&
						_crc.check(_buffers.getRxBuffer(), dataLength, _iqrf.getPTYPE())) {
						if (_spi.getMasterStatus() == _spi.masterStatuses::WRITE) {
							_callbacks.callTxCallback(_packets.getId(), _packets.statuses::OK);
						}
						if (_spi.getMasterStatus() == _spi.masterStatuses::READ) {
							_callbacks.callRxCallback();
						}
						_spi.setMasterStatus(_spi.masterStatuses::FREE);
					} else { // CRC error
						// rep_cnt - must be set on packet preparing
						if (_iqrf.getAttepmtsCount() - 1) {
							// another attempt to send data
							_iqrf.setByteCount(0);
						} else {
							if (_spi.getMasterStatus() == _spi.masterStatuses::WRITE) {
								_callbacks.callTxCallback(_packets.getId(), _packets.statuses::ERROR);
							}
							_spi.setMasterStatus(_spi.masterStatuses::FREE);
						}
					}
				}
			}
		} else { // no data to send => SPI status will be updated every 10ms
			if ((_iqrf.getUsCount1() - _iqrf.getUsCount0()) > (MICRO_SECOND / 100)) {
				// reset counter
				_iqrf.setUsCount0(_iqrf.getUsCount1());
				// get SPI status of TR module
				_spi.setStatus(_iqSpi.transfer(_spi.commands::CHECK));
				// CS - deactive
				//digitalWrite(TR_SS_PIN, HIGH);      
				// if the status is dataready prepare packet to read it
				if ((_spi.getStatus() & 0xC0) == 0x40) {
					memset(_buffers.getTxBuffer(), 0, _buffers.getTxBufferSize());
					// state 0x40 means 64B
					if (_spi.getStatus() == 0x40) {
						dataLength = 64;
					} else {
						// clear bit 7,6 - rest is length (from 1 to 63B)
						dataLength = _spi.getStatus() & 0x3F;
					}
					_iqrf.setPTYPE(dataLength);
					_buffers.setTxData(0, _spi.commands::WR_RD);
					_buffers.setTxData(1, _iqrf.getPTYPE());
					// CRC
					_buffers.setTxData(dataLength + 2, _crc.calculate(_buffers.getTxBuffer(), dataLength));
					// length of whole packet + (CMD, PTYPE, CRCM, 0)
					_packets.setLength(dataLength + 4);
					// counter of sent bytes
					_iqrf.setByteCount(0);
					// number of attempts to send data
					_iqrf.setAttepmtsCount(1);
					// reading from buffer COM of TR module
					_spi.setMasterStatus(_spi.masterStatuses::READ);
					// current SPI status must be updated
					_spi.setStatus(_spi.statuses::DATA_TRANSFER);
				}
				// if TR module ready and no data in module pending
				if (!_spi.getMasterStatus()) {
					// check if packet to send ready
					if (packetBufferInPtr != packetBufferOutPtr) {
						memset(_buffers.getTxBuffer(), 0, _buffers.getTxBufferSize());
						dataLength = iqrfPacketBuffer[packetBufferOutPtr].dataLength;
						// PBYTE set bit7 - write to buffer COM of TR module
						_iqrf.setPTYPE(dataLength | 0x80);
						_buffers.setTxData(0, iqrfPacketBuffer[packetBufferOutPtr].spiCmd);
						if (_buffers.getTxData(0) == _spi.commands::MODULE_INFO && dataLength == 16) {
							_iqrf.setPTYPE(0x10);
						}
						_buffers.setTxData(1, _iqrf.getPTYPE());
						memcpy(&_buffers.getTxBuffer()[2], iqrfPacketBuffer[packetBufferOutPtr].dataBuffer, dataLength);
						// CRCM
						_buffers.setTxData(dataLength + 2, _crc.calculate(_buffers.getTxBuffer(), dataLength));
						// length of whole packet + (CMD, PTYPE, CRCM, 0)
						_packets.setLength(dataLength + 4);
						// set actual TX packet ID
						_packets.setId(iqrfPacketBuffer[packetBufferOutPtr].packetId);
						// counter of sent bytes
						_iqrf.setByteCount(0);
						// number of attempts to send data
						_iqrf.setAttepmtsCount(3);
						// writing to buffer COM of TR module
						_spi.setMasterStatus(_spi.masterStatuses::WRITE);
						if (iqrfPacketBuffer[packetBufferOutPtr].unallocationFlag) {
							// unallocate temporary TX data buffer
							free(iqrfPacketBuffer[packetBufferOutPtr].dataBuffer);
						}
						if (++packetBufferOutPtr >= PACKET_BUFFER_SIZE) {
							packetBufferOutPtr = 0;
						}
						// current SPI status must be updated
						_spi.setStatus(_spi.statuses::DATA_TRANSFER);
					}
				}
			}
		}
	} else {
		// SPI master is disabled
		_tr.controlTask();
	}
}

/**
 * Function is usually called inside the callback function, whitch is called when the driver receives data from TR module
 * @param dataBuffer Pointer to my buffer, to which I want to load data received from the TR module
 * @param dataLength Number of bytes I want to read
 */
void IQRF_GetRxData(uint8_t *dataBuffer, uint8_t dataLength) {
	memcpy(dataBuffer, &_buffers.getRxBuffer()[2], dataLength);
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
			_callbacks.setRxCallback(doNothingRx);
			// set call back function after data were sent
			_callbacks.setTxCallback(identifyTx);
			trInfo.mcuType = _tr.mcuTypes::UNKNOWN;
			memset(&dataToModule[0], 0, 16);
			timeoutMilli = millis();
			// next state - will read info in PGM mode or /* in COM mode */
			trInfoTaskStatus = ENTER_PROG_MODE /* SEND_REQUEST */;
			break;
		case ENTER_PROG_MODE:
			_tr.enterProgramMode();
			// try to read idf in pgm mode
			idfMode = 1;
			// set call back function to process id data
			_callbacks.setRxCallback(identifyRx);
			// set call back function after data were sent
			_callbacks.setTxCallback(doNothingTx);
			timeoutMilli = millis();
			trInfoTaskStatus = SEND_REQUEST;
			break;
		case SEND_REQUEST:
			if (_spi.getStatus() == _spi.statuses::COMMUNICATION_MODE &&
				_spi.getMasterStatus() == _spi.masterStatuses::FREE) {
				TR_SendSpiPacket(_spi.commands::MODULE_INFO, &dataToModule[0], 16, 0);
				// initialize timeout timer
				timeoutMilli = millis();
				trInfoTaskStatus = WAIT_INFO;
			} else {
				if (_spi.getStatus() == _spi.statuses::PROGRAMMING_MODE &&
					_spi.getMasterStatus() == _spi.masterStatuses::FREE) {
					TR_SendSpiPacket(_spi.commands::MODULE_INFO, &dataToModule[0], 1, 0);
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
			if ((_tr.getInfoReadingStatus() == 1) || (millis() - timeoutMilli >= MILLI_SECOND / 2)) {
				if (idfMode == 1) {
					// send end of PGM mode packet
					TR_SendSpiPacket(_spi.commands::EEPROM_PGM, (uint8_t *) & endPgmMode[0], 3, 0);
				}
				// next state
				trInfoTaskStatus = DONE;
			}
			break;
			// the task is finished
		case DONE:
			// if no packet is pending to send to TR module
			if (packetBufferInPtr == packetBufferOutPtr &&
				_spi.getMasterStatus() == _spi.masterStatuses::FREE) {
				_tr.setInfoReadingStatus(0);
			}
			break;
	}
}

/**
 * Process identification data packet from TR module
 */
void trIdentify() {
	memcpy((uint8_t *) & trInfo.moduleInfoRawData, (uint8_t *) & _buffers.getRxBuffer()[2], 8);
	trInfo.moduleId = (uint32_t) _buffers.getRxBuffer()[2] << 24 | (uint32_t) _buffers.getRxBuffer()[3] << 16 | (uint32_t) _buffers.getRxBuffer()[4] << 8 | _buffers.getRxBuffer()[5];
	trInfo.osVersion = (uint16_t) (_buffers.getRxBuffer()[6] / 16) << 8 | (_buffers.getRxBuffer()[6] % 16);
	trInfo.mcuType = _buffers.getRxBuffer()[7] & 0x07;
	trInfo.fcc = (_buffers.getRxBuffer()[7] & 0x08) >> 3;
	trInfo.moduleType = _buffers.getRxBuffer()[7] >> 4;
	trInfo.osBuild = (uint16_t) _buffers.getRxBuffer()[9] << 8 | _buffers.getRxBuffer()[8];
	_tr.identify();
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
	iqrfPacketBuffer[packetBufferInPtr].packetId = _packets.getIdCount();
	iqrfPacketBuffer[packetBufferInPtr].spiCmd = spiCmd;
	iqrfPacketBuffer[packetBufferInPtr].dataBuffer = dataBuffer;
	iqrfPacketBuffer[packetBufferInPtr].dataLength = dataLength;
	iqrfPacketBuffer[packetBufferInPtr].unallocationFlag = unallocationFlag;
	if (++packetBufferInPtr >= PACKET_BUFFER_SIZE) {
		packetBufferInPtr = 0;
	}
}
