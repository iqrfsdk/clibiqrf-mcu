/**
 * @file
 * @author Rostislav Špinar <rostislav.spinar@microrisc.com>
 * @author Roman Ondráček <ondracek.roman@centrum.cz>
 * @version 2.0
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

#ifndef IQRFCALLBACKS_H
#define IQRFCALLBACKS_H

#include <stdint.h>

using namespace std;

/**
 * IQRF Callbacks class
 */
class IQRFCallbacks {
public:
	/// Rx data callback function type
	typedef void (*rx_callback)();
	/// Tx data callback function type
	typedef void (*tx_callback)(uint8_t packetId, uint8_t packetResult);
	void getRx();
	void setRx(rx_callback callback);
	void getTx(uint8_t packetId, uint8_t packetResult);
	void setTx(tx_callback callback);
private:
	/// Rx callback
	rx_callback rxCallback;
	/// Tx callback
	tx_callback txCallback;
};

#endif
