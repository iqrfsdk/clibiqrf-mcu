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

#ifndef IQRFSettings_H
#define IQRFSettings_H

#define IQ_PKT_SIZE        68     //!< Size of SPI TX and RX buffer
#define PACKET_BUFFER_SIZE 32     //!< Size of SPI TX packet buffer
#define IQRF_SPI_CLK       250000 //!< SPI clock 250kHz

// Timing
#define MICRO_SECOND       1000000 //!< Microsecond
#define MILLI_SECOND       1000    //!< Milisecond

// Pins
#define TR_RESET_IO        9      //!< TR reset pin

#endif
