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

#include "CallbackFunctions.h"

/// Instance of IQRFTR class
IQRFTR* _tr = new IQRFTR;

/**
 * Function called after TR module identification request were sent
 */
void doNothingRx() {
	__asm__("nop\n\t");
}

/**
 * Function called after TR module identification request were sent
 * @param packetId Packet ID
 * @param packetResult Operation result
 */
void doNothingTx(uint8_t packetId, uint8_t packetResult) {
	__asm__("nop\n\t");
}

/**
 * Process identification data packet from TR module
 */
void identifyRx() {
	_tr->identify();
}

/**
 * Process identification data packet from TR module
 * @param packetId Packet ID
 * @param packetResult Operation result
 */
void identifyTx(uint8_t packetId, uint8_t packetResult) {
	_tr->identify();
}
