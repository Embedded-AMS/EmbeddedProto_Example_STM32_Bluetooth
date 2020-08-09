/*
 *  Copyright (C) 2020 Embedded AMS B.V. - All Rights Reserved
 *
 *  This file is part of Embedded Proto.
 *
 *  Embedded Proto is open source software: you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as published
 *  by the Free Software Foundation, version 3 of the license.
 *
 *  Embedded Proto  is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Embedded Proto. If not, see <https://www.gnu.org/licenses/>.
 *
 *  For commercial and closed source application please visit:
 *  <https://EmbeddedProto.com/license/>.
 *
 *  Embedded AMS B.V.
 *  Info:
 *    info at EmbeddedProto dot com
 *
 *  Postal adress:
 *    Johan Huizingalaan 763a
 *    1066 VH, Amsterdam
 *    the Netherlands
 */

//#include "EmbeddedProto_service.h"
#include "BLEProcessCommand.h"
#include "ble_messages.h"
#include "BLEReadBuffer.h"
#include "BLEWriteBuffer.h"
#include <Errors.h>

#include "bluenrg_def.h"

extern "C" tBleStatus Sensor_Update(uint8_t *data, uint8_t len);

#include "stm32f4xx_nucleo.h"

void process_command(const Command& command);

BLEReadBuffer read_buffer;
BLEWriteBuffer write_buffer;
Command received_command;
Sensor sensor;

//! Function receives command and is deserialized
/*!
 * \param[in] byte Protobuf serialized command .
 * \param[in] data_length Length of BLE message characteristic.
 */
void ble_receive_command(uint8_t *byte, uint8_t data_length){
	//Protobuf message length is stored in first byte
	uint8_t n_bytes = byte[0]+1;

	if(n_bytes < data_length){

		for(uint8_t i=1; i<n_bytes; i++){
			read_buffer.push(byte[i]);
		}

		// Deserialize the data received.
		auto deserialize_status = received_command.deserialize(read_buffer);
		if(::EmbeddedProto::Error::NO_ERRORS == deserialize_status) {
			// Process the command.
			process_command(received_command);
		}
	}
}

//! Function sends serialized protobuf reply message.
/*!
 */
void ble_send_sensor(void)
{
	uint32_t message_len_index = write_buffer.get_size();
	write_buffer.push(0); //placeholder for buffer length

	//Update the sensor value
	uint32_t val = rand()%100;
	sensor.set_light_sensor(val);

	// Serialize the data.
	auto serialization_status = sensor.serialize(write_buffer);
	if(::EmbeddedProto::Error::NO_ERRORS == serialization_status)
	{
		write_buffer.get_data()[message_len_index] = write_buffer.get_size()-1;
		Sensor_Update(write_buffer.get_data(), write_buffer.get_size());
	}

	  // Clear the buffers after we are done.
	  read_buffer.clear();
	  write_buffer.clear();
}

//! The functions takes a command and responds to it.
/*!
 * \param[in] command The received command.
 * \param[out] reply The reply to be send over BLE.
 */
void process_command(const Command& command)
{
	switch(command.get_led())
	{
		case Command::LED::On:
			BSP_LED_On(LED2);
			break;

		case Command::LED::Off:

			BSP_LED_Off(LED2);
			break;

		default:
			// By default send back the current state.
			break;
	}
}




