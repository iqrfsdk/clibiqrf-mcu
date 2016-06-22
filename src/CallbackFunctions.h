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

#ifndef CALLBACKFUNCTIONS_H
#define CALLBACKFUNCTIONS_H

#include <stdint.h>

#include "iqrf_library.h"

using namespace std;

void doNothingRx();
void doNothingTx(uint8_t packetId, uint8_t packetResult);
void identifyRx();
void identifyTx(uint8_t packetId, uint8_t packetResult);

#endif
