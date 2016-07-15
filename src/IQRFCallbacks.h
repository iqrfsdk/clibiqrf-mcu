/**
 * @file
 * @author Rostislav Špinar <rostislav.spinar@microrisc.com>
 * @author Roman Ondráček <ondracek.roman@centrum.cz>
 * @version 1.1
 *
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

#ifndef IQRFCALLBACKS_H
#define IQRFCALLBACKS_H

#include <stdint.h>

class IQRFCallbacks {
public:
	/// SPI RX data callback function type
	typedef void (*rxCallback_t)(void);
	/// SPI TX data callback function type
	typedef void (*txCallback_t)(uint8_t packetId, uint8_t packetResult);
	void setRxCallback(rxCallback_t callback);
	void callRxCallback();
	void setTxCallback(txCallback_t callback);
	void callTxCallback(uint8_t packetId, uint8_t packetResult);
private:
	/// Rx callback function
	rxCallback_t rxCallback;
	/// Tx callback function
	txCallback_t txCallback;
};

#endif
