/**
 * @file
 * @author Rostislav Špinar <rostislav.spinar@microrisc.com>
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

#define IQ_PKT_SIZE           68     //!< Size of SPI TX and RX buffer
#define PACKET_BUFFER_SIZE    32     //!< Size of SPI TX packet buffer
#define IQRF_SPI_CLK          250000 //!< SPI clock 250kHz

// MCU type of TR module
#define MCU_UNKNOWN           0      //!< MCU used in uknown module
#define PIC16LF819            1      //!< MCU used in TR-xxx-11A not supported
#define PIC16LF88             2      //!< MCU used in TR-xxx-21A
#define PIC16F886             3      //!< MCU used in TR-31B, TR-52B, TR-53B
#define PIC16LF1938           4      //!< MCU used in TR-52D, TR-54D

// TR module types
#define TR_52D                0      //!< TR module IQRF TR-52D
#define TR_58D_RJ             1      //!< TR module IQRF TR-85D-RJ
#define TR_72D                2      //!< TR module IQRF TR-72D
#define TR_53D                3      //!< TR module IQRF TR-53D
#define TR_54D                8      //!< TR module IQRF TR-54D
#define TR_55D                9      //!< TR module IQRF TR-55D
#define TR_56D                10     //!< TR module IQRF TR-56D
#define TR_76D                11     //!< TR module IQRF TR-76D

// FCC cerificate
#define FCC_NOT_CERTIFIED     0      //!< FCC not certificated
#define FCC_CERTIFIED         1      //!< FCC certificated

// SPI status of TR module (see IQRF SPI user manual)
#define NO_MODULE             0xFF   //!< SPI not working (HW error)
#define SPI_BUSY              0xFE   //!< SPI busy in Master disabled mode
#define SPI_DATA_TRANSFER     0xFD   //!< SPI data transfer in progress
#define SPI_DISABLED          0x00   //!< SPI not working (disabled)
#define SPI_CRCM_OK           0x3F   //!< SPI not ready (full buffer, last CRCM ok)
#define SPI_CRCM_ERR          0x3E   //!< SPI not ready (full buffer, last CRCM error)
#define COMMUNICATION_MODE    0x80   //!< SPI ready (communication mode)
#define PROGRAMMING_MODE      0x81   //!< SPI ready (programming mode)
#define DEBUG_MODE            0x82   //!< SPI ready (debugging mode)
#define SPI_SLOW_MODE         0x83   //!< SPI not working in background
#define SPI_USER_STOP         0x07   //!< SPI state after stopSPI();

// SPI commands for TR module (see IQRF SPI user manual)
#define SPI_CHECK             0x00   //!< Master checks the SPI status of the TR module
#define SPI_WR_RD             0xF0   //!< Master reads/writes a packet from/to TR module
#define SPI_RAM_READ          0xF1   //!< Master reads data from ram in debug mode
#define SPI_EEPROM_READ       0xF2   //!< Master reads data from eeprom in debug mode
#define SPI_EEPROM_PGM        0xF3   //!< Master writes data to eeprom in programming mode
#define SPI_MODULE_INFO       0xF5   //!< Master reads Module Info from TR module
#define SPI_FLASH_PGM         0xF6   //!< Master writes data to flash in programming mode
#define SPI_PLUGIN_PGM        0xF9   //!< Master writes plugin data to flash in programming mode

// IQRF TX packet result
#define IQRF_TX_PKT_OK        1      //!< Packet sent OK
#define IQRF_TX_PKT_ERR       2      //!< Packet sent with ERROR

// IQRF SPI master status
#define IQRF_SPI_MASTER_FREE  0      //!< SPI master free
#define IQRF_SPI_MASTER_WRITE 1      //!< SPI master wrtite
#define IQRF_SPI_MASTER_READ  2      //!< SPI master read

// Pins
#define TR_RESET_IO           9      //!< TR reset pin
#define TR_SS_IO              10     //!< SPI SS pin
#define TR_SDO_IO             11     //!< SPI SDO pin
#define TR_SDI_IO             12     //!< SPI SDI pin

// Timing
#define MICRO_SECOND          1000000 //!< Microsecond
#define MILLI_SECOND          1000    //!< Milisecond

// TR control states
#define TR_CTRL_READY     0  //!< TR ready state
#define TR_CTRL_RESET     1  //!< TR reset process
#define TR_CTRL_WAIT      2  //!< TR wait state
#define TR_CTRL_PROG_MODE 3  //!< TR programming mode

/// SPI RX data callback function type
typedef void (*IQRF_RX_CALL_BACK)(void);

/// SPI TX data callback function type
typedef void (*IQRF_TX_CALL_BACK)(uint8_t pktId, uint8_t pktResult);

/// TR module info structure
typedef struct {
	uint16_t osVersion;           //!< OS version
	uint16_t osBuild;             //!< OS build
	uint32_t moduleId;            //!< Module ID
	uint16_t mcuType;             //!< MCU tyle
	uint16_t moduleType;          //!< Module type
	uint16_t fcc;                 //!< FCC
	uint8_t moduleInfoRawData[8]; //!< Raw data
} TR_INFO_STRUCT;

/// Item of SPI TX packet buffer
typedef struct {
	uint8_t pktId;                //!< Packet ID
	uint8_t spiCmd;               //!< SPI command
	uint8_t *pDataBuffer;         //!< Pointer to data buffrt
	uint8_t dataLength;           //!< Data lenght
	uint8_t unallocationFlag;     //!< Unallocation flag
} IQRF_PACKET_BUFFER;

extern uint8_t DLEN;                  //!< Data length
extern uint8_t spiIqBusy;
extern uint8_t spiStat;               //!< SPI status
extern uint8_t iqrfSpiMasterEnable;   //!< SPI master
extern uint8_t fastIqrfSpiEnable;     //!< Fast SPI
extern TR_INFO_STRUCT trInfoStruct;   //!< TR info structure

/**
 * Function perform a TR-module driver initialization
 * Function performes initialization of trInfoStruct identification data structure
 * @param rx_call_back_fn Pointer to callback function. Function is called when the driver receives data from the TR module
 * @param tx_call_back_fn Pointer to callback function. unction is called when the driver sent data to the TR module
 */
void IQRF_Init(IQRF_RX_CALL_BACK rx_call_back_fn, IQRF_TX_CALL_BACK tx_call_back_fn);

/**
 * Periodically called IQRF_Driver
 */
void IQRF_Driver(void);

/**
 * Function perform a TR-module reset
 */
void IQRF_TR_Reset(void);

/**
 * Function switch TR-module to programming mode
 */
void IQRF_TR_EnterProgMode(void);

/**
 * Function sends data from buffer to TR module
 * @param pDataBuffer Pointer to a buffer that contains data that I want to send to TR module
 * @param dataLength Number of bytes to send
 * @param unallocationFlag If the pDataBuffer is dynamically allocated using malloc function.
   If you wish to unallocate buffer after data is sent, set the unallocationFlag to 1, otherwise to 0.
 * @return TX packet ID (number 1-255)
 */
uint8_t IQRF_SendData(uint8_t *pDataBuffer, uint8_t dataLength, uint8_t unallocationFlag);

/**
 * Function is usually called inside the callback function, whitch is called when the driver receives data from TR module
 * @param userDataBuffer Pointer to my buffer, to which I want to load data received from the TR module
 * @param rxDataSize Number of bytes I want to read
 */
void IQRF_GetRxData(uint8_t *userDataBuffer, uint8_t rxDataSize);

/**
 * Get size of Rx data
 * @return Number of bytes recieved from TR module
 */
#define IQRF_GetRxDataSize() DLEN

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
 * @param x Position in info raw buffer
 * @return Data byte from info raw buffer
 */
#define IQRF_GetModuleInfoRawData(x) trInfoStruct.moduleInfoRawData[x]

/**
 * Get TR module comunication status
 * @return TR module cominication status
 * Status code |   Status message   |                Description                 
 *  ---------- | ------------------ | --------------------------------------------
 *     0xFF    |     NO_MODULE      | SPI not working (HW error)
 *     0xFE    |      SPI_BUSY      | SPI busy in Master disabled mode
 *     0xFD    |  SPI_DATA_TRANSFER | SPI data transfer in progress
 *     0x00    |    SPI_DISABLED    | SPI not working (disabled)
 *     0x3F    |    SPI_CRCM_OK     | SPI not ready (full buffer, last CRCM ok)
 *     0x3E    |    SPI_CRCM_ERR    | SPI not ready (full buffer, last CRCM error)
 *     0xFF    | COMMUNICATION_MODE | SPI ready (communication mode)
 *     0x81    |  PROGRAMMING_MODE  | SPI ready (programming mode)
 *     0x82    |     DEBUG_MODE     | SPI ready (debugging mode)
 *     0x83    |    SPI_SLOW_MODE   | SPI not working in background
 *     0x07    |    SPI_USER_STOP   | SPI state after stopSPI();
 */
#define IQRF_GetStatus()  spiStat

/**
 * Enable SPI Master function in IQRF driver
 */
#define IQRF_SPIMasterEnable()  iqrfSpiMasterEnable = 1

/**
 * Disable SPI Master function in IQRF driver
 */
#define IQRF_SPIMasterDisable()  {iqrfSpiMasterEnable = 0; spiStat = SPI_DISABLED;}

/**
 * Returns thw state of SPI Master function in IQRF driver
 * @return State of IQRF SPI master 0 = Disabled, 1 = enabled
 */
#define IQRF_GetSPIMasterState() iqrfSpiMasterEnable


/**
 * Send and receive single byte over SPI
 * @param Tx_Byte Character to be send via SPI
 * @return Byte received via SPI
 */
uint8_t IQRF_SPI_Byte(uint8_t Tx_Byte);

/**
 * Prepare SPI packet to packet buffer
 * @param spiCmd Command that I want to send to TR module
 * @param pDataBuffer Pointer to a buffer that contains data that I want to send to TR module
 * @param dataLength Number of bytes to send
 * @param unallocationFlag If the pDataBuffer is dynamically allocated using malloc function.
   If you wish to unallocate buffer after data is sent, set the unallocationFlag to 1, otherwise to 0.
 * @return Packet ID
 */
uint8_t TR_SendSpiPacket(uint8_t spiCmd, uint8_t *pDataBuffer, uint8_t dataLength, uint8_t unallocationFlag);

/**
 * Set byte to byte pause is SPI driver
 * @param byteToByteTime byte to byte time in us
 */
void TR_SetByteToByteTime(uint16_t byteToByteTime);

#endif
