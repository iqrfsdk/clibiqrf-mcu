/**
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

#if defined(__PIC32MX__)
#include <WProgram.h>
#else
#include <Arduino.h>
#endif

#if defined(__AVR__) || defined(CORE_TEENSY)
#include <MsTimer2.h>
#elif defined(__SAM3X8E__)
#include <DueTimer.h>
#endif

#include <stddef.h>
#include <stdint.h>
#include <IQRF.h>

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
#if defined(__PIC32MX__)
uint32_t msTimerCallback(uint32_t currentTime);
#else
void msTimerCallback();
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
IQRF iqrf;
IQRFTR iqrfTr;

// Const data
const char testBuffer[12] = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'};

/**
 * Init peripherals
 */
void setup() {
	// User LED
	pinMode(13, OUTPUT);
	// Up - PC
	Serial.begin(9600);
	// Wait for Serial
	while (!Serial) {
	}
	// Down - IQRF
	iqrf.begin(rxHandler, txHandler);
	// Info - TR
	switch (iqrfTr.getModuleType()) {
		case iqrfTr.types::TR_52D:
			Serial.println("[IQRF] Module type: TR-52D");
			break;
		case iqrfTr.types::TR_72D:
			Serial.println("[IQRF] Module type: TR-72D");
			break;
		default:
			Serial.println("[IQRF] Module type: UNKNOWN");
			break;
	}
#if defined(__AVR__) || defined(CORE_TEENSY)
	MsTimer2::set(1, msTimerCallback);
	MsTimer2::start();
#elif defined(__SAM3X8E__)
	Timer6.attachInterrupt(msTimerCallback).start(1000);
#elif defined(__PIC32MX__)
	attachCoreTimerService(msTimerCallback);
#endif
	// Clear variables
	memset(&appVars, 0, sizeof(appVarsStruct));
	appVars.timer = USER_TIMER_PERIOD;
}

/**
 * Main loop
 */
void loop() {
	// TR module SPI comunication driver
	iqrf.driver();
	// Test send data every 5s
	if (appVars.timerAck) {
		// Allocate memory for Tx packet
		appVars.txBuffer = (uint8_t *) malloc(sizeof(testBuffer));
		if (appVars.txBuffer != NULL) {
			// Copy data from test to Tx packet
			memcpy(appVars.txBuffer, (uint8_t *) & testBuffer, sizeof(testBuffer));
			// Send data and unallocate data buffer
			appVars.packetId = iqrf.sendData(appVars.txBuffer, sizeof(testBuffer), 1);
		}
		appVars.timerAck = false;
	}
}

#if defined(__PIC32MX__)

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

#else

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

#endif

/**
 * IQRF Rx callback
 */
void rxHandler() {
	// Read and print received data
	iqrf.getData(appVars.rxBuffer, iqrf.getDataLength());
	Serial.print("[IQRF] Receive done: ");
	Serial.write(appVars.rxBuffer, iqrf.getDataLength());
	Serial.println();
}

/**
 * IQRF Tx callback
 * @param packetId Packet ID
 * @param packetResult Packet writing result
 */
void txHandler(uint8_t packetId, uint8_t packetResult) {
	Serial.println("[IQRF] Send done");
}
