/**
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

#if defined(__AVR__)
#include <Arduino.h>
#include <MsTimer2.h>
#elif defined(__PIC32MX__)
#include <WProgram.h>
#endif

#include <stddef.h>
#include <stdint.h>
#include <iqrf_library.h>

// 5000@1ms = 5s
#define USER_TIMER_PERIOD 5000

// TR MODULE PINS
#define TR_RESET_PIN  6   //!< TR reset pin
#define TR_SS_PIN     10  //!< SPI SS pin

// LOCAL PROTOTYPES
void setup();
void loop();
void rxHandler();
void txHandler(uint8_t packetId, uint8_t packetResult);
#if defined(__AVR__)
void msTimerCallback();
#elif defined(__PIC32MX__)
uint32_t msTimerCallback(uint32_t currentTime);
#endif

// GLOBAL VARIABLES

/**
 * App data structure
 */
typedef struct {
	uint8_t rxBuffer[PACKET_SIZE]; //!< Rx buffer
	uint8_t *txBuffer; //!< Tx buffer
	uint8_t packetId; //!< Packet ID
	volatile uint16_t timer; //!< Timer
	volatile bool timerAck; //!< Timer action
} appVarsStruct;
appVarsStruct appVars;

/// Instances
IQRFTR* iqrfTr = new IQRFTR;

// Const data
const uint8_t testBuffer[64] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64};

/**
 * Init peripherals
 */
void setup() {
	// User LED
	pinMode(13, OUTPUT);
	// Up - PC
	Serial.begin(9600);
	// Down - IQRF
	IQRF_Init(rxHandler, txHandler);
	// Info - TR
	switch (IQRF_GetModuleType()) {
		case iqrfTr->types::TR_52D:
			Serial.println("Module type: TR-52D");
			break;
		case iqrfTr->types::TR_72D:
			Serial.println("Module type: TR-72D");
			break;
		default:
			Serial.println("Module type: UNKNOWN");
			break;
	}
#if defined(__AVR__)
	MsTimer2::set(1, msTimerCallback);
	MsTimer2::start();
#elif defined(__PIC32MX__)
	attachCoreTimerService(msTimerCallback);
#endif
	// Clear variables
	memset(&appVars, 0, sizeof(appVarsStruct));
	appVars.timer = USER_TIMER_PERIOD;
	// Done here
	Serial.println("Peripherals and IQRF init done");
}

/**
 * Main loop
 */
void loop() {
	// TR module SPI comunication driver
	IQRF_Driver();
	// Test send data every 5s
	if (appVars.timerAck) {
		// Allocate memory for Tx packet
		appVars.txBuffer = (uint8_t *) malloc(sizeof(testBuffer));
		if (appVars.txBuffer != NULL) {
			// Copy data from test to Tx packet
			memcpy(appVars.txBuffer, (uint8_t *) & testBuffer, sizeof(testBuffer));
			// Send data and unallocate data buffer
			appVars.packetId = IQRF_SendData(appVars.txBuffer, sizeof(testBuffer), 1);
		}
		appVars.timerAck = false;
	}
}

#if defined(__AVR__)

/**
 * 1ms timer callback
 */
void msTimerCallback() {
	// App timer, call handler
	if (appVars.timer) {
		if ((--appVars.timer) == 0) {
			appVars.timerAck = true;
			appVars.timer = USER_TIMER_PERIOD;
		}
	}
}
#elif defined(__PIC32MX__)

/**
 * 1ms timer callback
 * @param currentTime Current time
 * @return Next time
 */
uint32_t msTimerCallback(uint32_t currentTime) {
	// App timer, call handler
	if (appVars.timer) {
		if ((--appVars.timer) == 0) {
			appVars.timerAck = true;
			appVars.timer = USER_TIMER_PERIOD;
		}
	}
	return(currentTime + CORE_TICK_RATE);
}
#endif

/**
 * IQRF Rx callback
 */
void rxHandler() {
	// Read and print received data
	IQRF_GetRxData(appVars.rxBuffer, IQRF_GetRxDataSize());
	Serial.print("IQRF receive done: ");
	Serial.write(appVars.rxBuffer, IQRF_GetRxDataSize());
	Serial.println();
}

/**
 * IQRF Tx callback
 * @param packetId Packet ID
 * @param packetResult Packet writing result
 */
void txHandler(uint8_t packetId, uint8_t packetResult) {
	Serial.println("IQRF send done");
}
