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

#ifndef _IQRFLIBRARY_H
#define _IQRFLIBRARY_H

#if defined(__PIC32MX__)
#include <WProgram.h>
#else
#include <Arduino.h>
#endif

#include <stddef.h>
#include <stdint.h>

#include "CallbackFunctions.h"
#include "IQRF.h"
#include "IQRFBuffers.h"
#include "IQRFCallbacks.h"
#include "IQRFCRC.h"
#include "IQRFPackets.h"
#include "IQRFSettings.h"
#include "IQRFSPI.h"
#include "IQRFTR.h"
#include "IQSPI.h"

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
} trInfo_t;

/**
 * Item of SPI TX packet buffer
 */
typedef struct {
	uint8_t packetId; //!< Packet ID
	uint8_t spiCmd; //!< SPI command
	uint8_t *dataBuffer; //!< Pointer to data buffrt
	uint8_t dataLength; //!< Data lenght
	uint8_t unallocationFlag; //!< Unallocation flag
} packetBuffer_t;

extern uint8_t dataLength;
extern trInfo_t trInfo;

void IQRF_Init(IQRFCallbacks::rxCallback_t rxCallback, IQRFCallbacks::txCallback_t txCallback);
void IQRF_Driver();
void IQRF_GetRxData(uint8_t *dataBuffer, uint8_t dataLength);
uint8_t TR_SendSpiPacket(uint8_t spiCmd, uint8_t *dataBuffer, uint8_t dataLength, uint8_t unallocationFlag);
void trIdentify();

#endif
