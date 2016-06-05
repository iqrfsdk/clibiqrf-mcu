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

#ifndef IQRF_LIBRARY_H
#define IQRF_LIBRARY_H

#include <stdint.h>

#include "IQRF.h"
#include "IQRFCallbacks.h"
#include "IQRFPackets.h"
#include "IQRFSPI.h"
#include "IQRFTR.h"

void IQRF_Init(IQRFCallbacks::rx_callback rxCallback, IQRFCallbacks::tx_callback txCallback);
void IQRF_Driver();
void IQRF_TR_Reset();
void IQRF_TR_EnterProgMode();
uint8_t IQRF_SendData(uint8_t *dataBuffer, uint8_t dataLength, uint8_t unallocationFlag);
void IQRF_GetRxData(uint8_t *dataBuffer, uint8_t dataSize);
uint8_t IQRF_GetRxDataSize();
uint8_t IQRF_GetOsVersion();
uint8_t IQRF_GetOsBuild();
uint32_t IQRF_GetModuleId();
uint16_t IQRF_GetMcuType();
uint16_t IQRF_GetModuleType();
uint8_t IQRF_GetModuleInfoRawData(uint8_t position);
uint8_t IQRF_GetStatus();
void IQRF_SPIMasterEnable();
void IQRF_SPIMasterDisable();
bool IQRF_GetSPIMasterState();
uint8_t IQRF_SPI_Byte(uint8_t byte);
uint8_t TR_SendSpiPacket(uint8_t spiCmd, uint8_t *dataBuffer, uint8_t dataLength, uint8_t unallocationFlag);

#endif
