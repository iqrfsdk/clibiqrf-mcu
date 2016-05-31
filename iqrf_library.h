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

//uint8(16, 32)_t and NULL defines
#include <stddef.h>
#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "IQRFCRC.h"
#include "IQRFSettings.h"
#include "IQRFSPI.h"
#include "IQRFTR.h"

/**
 * Tx packet statuses
 */
enum txPacketStatuses {
	OK = 1, //!< Packet sent OK
	ERROR = 2 //!< Packet sent with ERROR
};

/// SPI RX data callback function type
typedef void (*RX_CALLBACK)(void);

/// SPI TX data callback function type
typedef void (*TX_CALLBACK)(uint8_t packetId, uint8_t packetResult);

/**
 * Item of SPI TX packet buffer
 */
typedef struct {
	uint8_t packetId; //!< Packet ID
	uint8_t spiCmd; //!< SPI command
	uint8_t *dataBuffer; //!< Pointer to data buffrt
	uint8_t dataLength; //!< Data lenght
	uint8_t unallocationFlag; //!< Unallocation flag
} PACKET_BUFFER;

extern uint8_t dataLength;

void IQRF_Init(RX_CALLBACK rx_call_back_fn, TX_CALLBACK tx_call_back_fn);
void IQRF_Driver();
uint8_t IQRF_SendData(uint8_t *dataBuffer, uint8_t dataLength, uint8_t unallocationFlag);
void IQRF_GetRxData(uint8_t *dataBuffer, uint8_t dataSize);
uint8_t TR_SendSpiPacket(uint8_t spiCmd, uint8_t *dataBuffer, uint8_t dataLength, uint8_t unallocationFlag);

/**
 * Get size of Rx data
 * @return Number of bytes recieved from TR module
 */
#define IQRF_GetRxDataSize() dataLength

#endif
