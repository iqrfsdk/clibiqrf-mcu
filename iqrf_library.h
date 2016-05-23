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

#ifndef _IQRFLIBRARY_H
#define _IQRFLIBRARY_H

#include <Arduino.h>
#include <SPI.h>
#include <MsTimer2.h>

//uint8(16, 32)_t and NULL defines
#include <stddef.h>
#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "IQRFCRC.h"
#include "IQRFSPI.h"
#include "IQRFTR.h"

#define IQ_PKT_SIZE           68     //!< Size of SPI TX and RX buffer
#define PACKET_BUFFER_SIZE    32     //!< Size of SPI TX packet buffer
#define IQRF_SPI_CLK          250000 //!< SPI clock 250kHz

// Pins
#define TR_SS_IO              10     //!< SPI SS pin
#define TR_SDO_IO             11     //!< SPI SDO pin
#define TR_SDI_IO             12     //!< SPI SDI pin

// Timing
#define MICRO_SECOND          1000000 //!< Microsecond
#define MILLI_SECOND          1000    //!< Milisecond

/**
 * TR module types
 */
enum trTypes {
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
enum trMcuTypes {
	UNKNOWN = 0, //!< MCU used in uknown module
	PIC16LF819 = 1, //!< MCU used in TR-xxx-11A not supported
	PIC16LF88 = 2, //!< MCU used in TR-xxx-21A
	PIC16F886 = 3, //!< MCU used in TR-31B, TR-52B, TR-53B
	PIC16LF1938 = 4 //!< MCU used in TR-52D, TR-54D
};

/**
 * TR control statuses
 */
enum trControlStatuses {
	READY = 0, //!< TR ready state
	RESET = 1, //!< TR reset process
	WAIT = 2, //!< TR wait state
	PROG_MODE = 3 //!< TR programming mode
};

/**
 * Tx packet statuses
 */
enum txPacketStatuses {
	OK = 1, //!< Packet sent OK
	ERROR = 2 //!< Packet sent with ERROR
};

/**
 * FCC (Federal Communications Commission) certification statuses
 */
enum fccStatuses {
	NOT_CERTIFIED = 0, //!< Not certified fy FCC
	CERTIFIED = 1 //!< Certified by FCC
};

/// SPI RX data callback function type
typedef void (*IQRF_RX_CALLBACK)(void);

/// SPI TX data callback function type
typedef void (*IQRF_TX_CALLBACK)(uint8_t pktId, uint8_t pktResult);

/**
 * TR module info structure
 */
typedef struct {
	uint16_t osVersion; //!< OS version
	uint16_t osBuild; //!< OS build
	uint32_t moduleId; //!< Module ID
	uint16_t mcuType; //!< MCU tyle
	uint16_t moduleType; //!< Module type
	uint16_t fcc; //!< FCC
	uint8_t moduleInfoRawData[8]; //!< Raw data
} TR_INFO_STRUCT;

/**
 * Item of SPI TX packet buffer
 */
typedef struct {
	uint8_t pktId; //!< Packet ID
	uint8_t spiCmd; //!< SPI command
	uint8_t *pDataBuffer; //!< Pointer to data buffrt
	uint8_t dataLength; //!< Data lenght
	uint8_t unallocationFlag; //!< Unallocation flag
} IQRF_PACKET_BUFFER;

extern uint8_t dataLength;
extern uint8_t spiIqBusy;
extern TR_INFO_STRUCT trInfoStruct;

void IQRF_Init(IQRF_RX_CALLBACK rx_call_back_fn, IQRF_TX_CALLBACK tx_call_back_fn);
void IQRF_Driver(void);
void IQRF_TR_Reset(void);
void IQRF_TR_EnterProgMode(void);
uint8_t IQRF_SendData(uint8_t *pDataBuffer, uint8_t dataLength, uint8_t unallocationFlag);
void IQRF_GetRxData(uint8_t *userDataBuffer, uint8_t rxDataSize);
uint8_t IQRF_SPI_Byte(uint8_t Tx_Byte);
uint8_t TR_SendSpiPacket(uint8_t spiCmd, uint8_t *pDataBuffer, uint8_t dataLength, uint8_t unallocationFlag);

/**
 * Get size of Rx data
 * @return Number of bytes recieved from TR module
 */
#define IQRF_GetRxDataSize() dataLength

/**
 * Get OS version
 * @return Version of OS used inside of TR module
 */
#define IQRF_GetOsVersion()  trInfoStruct.osVersion

/**
 * Get OS build
 * @return Build of OS used inside of TR module
 */
#define IQRF_GetOsBuild()  trInfoStruct.osBuild

/**
 * Get TR module ID
 * @return Unique 32 bit identifier data word of TR module
 */
#define IQRF_GetModuleId()  trInfoStruct.moduleId

/**
 * Get MCU model inside TR module
 * @return MCU type
 * Code |     Type
 * ---- | ------------
 *   0  | unknown type
 *   1  |  PIC16LF819
 *   2  |  PIC16LF88
 *   3  |  PIC16F886
 *   4  | PIC16LF1938
 */
#define IQRF_GetMcuType()  trInfoStruct.mcuType

/**
 * Get TR module type
 * @return TR module type
 * Code |   Model
 * ---- | ---------
 *   0  |   TR_52D
 *   1  | TR_58D_RJ
 *   2  |   TR_72D
 *   8  |   TR_54D
 *   9  |   TR_55D
 *  10  |   TR_56D
 */
#define IQRF_GetModuleType()  trInfoStruct.moduleType

/**
 * Get raw info data about TR module
 * @param position Position in info raw buffer
 * @return Data byte from info raw buffer
 */
#define IQRF_GetModuleInfoRawData(position) trInfoStruct.moduleInfoRawData[position]

#endif
