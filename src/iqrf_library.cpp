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

/// Instance of IQRF class
IQRF* iqrf = new IQRF;
/// Instance of IQRFPackets class
IQRFPackets* packets = new IQRFPackets;
/// Instance of IQRFSPI class
IQRFSPI* spi = new IQRFSPI;
/// Instance of IQRFTR class
IQRFTR* tr = new IQRFTR;


/**
 * Function perform a TR-module driver initialization
 * Function performes initialization of trInfoStruct identification data structure
 * @param rxCallback Pointer to callback function. Function is called when the driver receives data from the TR module
 * @param txCallback Pointer to callback function. unction is called when the driver sent data to the TR module
 * @deprecated Please use IQRF::init
 */
void IQRF_Init(IQRFCallbacks::rx_callback rxCallback, IQRFCallbacks::tx_callback txCallback) {
	iqrf->init(rxCallback, txCallback);
}

/**
 * Periodically called IQRF_Driver
 * @deprecated Please use IQRF::driver
 */
void IQRF_Driver() {
	iqrf->driver();
}

/**
 * Function perform a TR-module reset
 * @deprecated Please use IQRFTR::reset
 */
void IQRF_TR_Reset() {
	tr->reset();
}

/**
 * Function switch TR-module to programming mode
 * @deprecated Please use IQRFTR::enterProgramMode
 */
void IQRF_TR_EnterProgMode() {
	tr->enterProgramMode();
}

/**
 * Function sends data from buffer to TR module
 * @param dataBuffer Pointer to a buffer that contains data that I want to send to TR module
 * @param dataLength Number of bytes to send
 * @param unallocationFlag If the pDataBuffer is dynamically allocated using malloc function.
   If you wish to unallocate buffer after data is sent, set the unallocationFlag to 1, otherwise to 0.
 * @return Tx packet ID (number 1-255)
 * @deprecated Please use IQRF::sendData
 */
uint8_t IQRF_SendData(uint8_t* dataBuffer, uint8_t dataLength, uint8_t unallocationFlag) {
	return iqrf->sendData(dataBuffer, dataLength, unallocationFlag);
}

/**
 * Function is usually called inside the callback function, whitch is called when the driver receives data from TR module
 * @param dataBuffer Pointer to my buffer, to which I want to load data received from the TR module
 * @param dataSize Number of bytes I want to read
 * @deprecated Please use IQRF::getData
 */
void IQRF_GetRxData(uint8_t *dataBuffer, uint8_t dataSize) {
	return iqrf->getData(dataBuffer, dataSize);
}

/**
 * Get size of Rx data
 * @return Number of bytes recieved from TR module
 * @deprecated Please use IQRF::getDataLength
 */
uint8_t IQRF_GetRxDataSize() {
	return iqrf->getDataLength();
}

/**
 * Get OS version
 * @return Version of OS used inside of TR module
 * @deprecated Please use IQRFTR::getOsVersion
 */
uint8_t IQRF_GetOsVersion() {
	return tr->getOsVersion();
}

/**
 * Get OS build
 * @return Build of OS used inside of TR module
 * @deprecated Please use IQRFTR::getOsBuild
 */
uint8_t IQRF_GetOsBuild() {
	return tr->getOsBuild();
}

/**
 * Get TR module ID
 * @return Unique 32 bit identifier data word of TR module
 * @deprecated Please use IQRFTR::getModuleId
 */
uint32_t IQRF_GetModuleId() {
	return tr->getModuleId();
}

/**
 * Get MCU model inside TR module
 * @return MCU type
 * @deprecated Please use IQRFTR::getMcuType
 */
uint16_t IQRF_GetMcuType() {
	return tr->getMcuType();
}

/**
 * Get TR module type
 * @return TR module type
 * @deprecated Please use IQRFTR::getModuleType
 */
uint16_t IQRF_GetModuleType() {
	return tr->getModuleType();
}

/**
 * Get raw info data about TR module
 * @param position Position in info raw buffer
 * @return Data byte from info raw buffer
 * @deprecated Please use IQRFTR::getInfoRawData
 */
uint8_t IQRF_GetModuleInfoRawData(uint8_t position) {
	return tr->getInfoRawData(position);
}

/**
 * Get TR module comunication status
 * @return TR module cominication status
 * @deprecated Please use IQRFSPI::getStatus
 */
uint8_t IQRF_GetStatus() {
	return spi->getStatus();
}

/**
 * Enable SPI Master function in IQRF driver
 * @deprecated Please use IQRFSPI::enableMaster
 */
void IQRF_SPIMasterEnable() {
	spi->enableMaster();
}

/**
 * Disable SPI Master function in IQRF driver
 * @deprecated Please use IQRFSPI::disableMaster
 */
void IQRF_SPIMasterDisable() {
	spi->disableMaster();
}

/**
 * Returns thw state of SPI Master function in IQRF driver
 * @return State of IQRF SPI master (boolean)
 * @deprecated Please use IQRFSPI::getMasterStatus
 */
bool IQRF_GetSPIMasterState() {
	return spi->getMasterStatus();
}

/**
 * Send and receive single byte over SPI
 * @param byte Character to be send via SPI
 * @return Byte received via SPI
 * @deprecated Please use IQRFSPI::byte
 */
uint8_t IQRF_SPI_Byte(uint8_t byte) {
	return spi->byte(byte);
}

/**
 * Prepare SPI packet to packet buffer
 * @param spiCmd Command that I want to send to TR module
 * @param dataBuffer Pointer to a buffer that contains data that I want to send to TR module
 * @param dataLength Number of bytes to send
 * @param unallocationFlag If the pDataBuffer is dynamically allocated using malloc function.
   If you wish to unallocate buffer after data is sent, set the unallocationFlag to 1, otherwise to 0.
 * @return Packet ID
 * @deprecated Please use IQRFPackets::sendPacket
 */
uint8_t TR_SendSpiPacket(uint8_t spiCmd, uint8_t *dataBuffer, uint8_t dataLength, uint8_t unallocationFlag) {
	return packets->sendPacket(spiCmd, dataBuffer, dataLength, unallocationFlag);
}