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

// For IQRF
#include "iqrf_library.h"

// 5000@1ms = 5s 
#define USER_TIMER_PERIOD 5000

// LOCAL PROTOTYPES
void MyIqrfRxHandler(void);
void MyIqrfTxHandler(uint8_t txPktId, uint8_t txPktResult);
void AppTimerHandler(void);
void cb_timer1ms(void);

// GLOBAL VARIABLES
// App data
typedef struct {
  uint8_t myIqrfRxBuf[IQ_PKT_SIZE];
  uint8_t *myIqrfTxBuf;
  uint8_t testPktId;
  volatile uint16_t appTimer;
  volatile uint8_t appTimerAck;
} app_vars_t;
app_vars_t app_vars;

// Const data
const uint8_t testBuffer[64] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64};
const uint8_t ledPin = 13;

/**
 * Init peripherals
 */
void setup(void) {
  // User LED
  pinMode(ledPin, OUTPUT);
  // Up - PC
  Serial.begin(9600);
  // Down - IQRF
  IQRF_Init(MyIqrfRxHandler, MyIqrfTxHandler);
  // Info - TR
  switch (IQRF_GetModuleType()) {
    case TR_52D:
      Serial.println("Module type: TR-52D");
      break;
    case TR_72D:
      Serial.println("Module type: TR-72D");
      break;
    default:
      Serial.println("Module type: UNKNOWN");
      break;
  }
  MsTimer2::set(1, cb_timer1ms);
  MsTimer2::start();
  // Clear variables
  memset(&app_vars, 0, sizeof(app_vars_t));
  app_vars.appTimer = USER_TIMER_PERIOD;
  // Done here
  Serial.println("Peripherals and IQRF init done");
}

/**
 * Main loop
 */
void loop(void) {
  // TR module SPI comunication driver
  IQRF_Driver();
  // Test send data every 5s
  if (app_vars.appTimerAck) {
    // Allocate memory for Tx packet
    app_vars.myIqrfTxBuf = (uint8_t *) malloc(sizeof(testBuffer));
    if (app_vars.myIqrfTxBuf != NULL) {
      // Copy data from test to IQRF TX packet
      memcpy(app_vars.myIqrfTxBuf, (uint8_t *) &testBuffer, sizeof(testBuffer));
      // Send data and unallocate data buffer
      app_vars.testPktId = IQRF_SendData(app_vars.myIqrfTxBuf, sizeof(testBuffer), 1);
    }
    app_vars.appTimerAck = false;
  }
}

/**
 * 1ms timer callback
 */
void cb_timer1ms(void) {
  // app timer, call handler
  if (app_vars.appTimer) {
    if ((--app_vars.appTimer) == 0) {
      AppTimerHandler();
      app_vars.appTimer = USER_TIMER_PERIOD;
    }
  }
}

/**
 * User timer handler
 */
void AppTimerHandler(void) {
  app_vars.appTimerAck = true;
}

/**
 * IQRF RX callback
 */
void MyIqrfRxHandler(void) {
  // Read and print received data
  IQRF_GetRxData(app_vars.myIqrfRxBuf, IQRF_GetRxDataSize());
  Serial.print("IQRF receive done: ");
  Serial.write(app_vars.myIqrfRxBuf, IQRF_GetRxDataSize());
  Serial.println();
}

/**
 * IQRF TX callback
 * @param txPktId Paket ID
 * @param txPktResult Paket writing result
 */
void MyIqrfTxHandler(uint8_t txPktId, uint8_t txPktResult) {
  Serial.println("IQRF send done");
}
