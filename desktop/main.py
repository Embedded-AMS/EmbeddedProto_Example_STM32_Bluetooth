#
# Copyright (C) 2021-2024 Embedded AMS B.V. - All Rights Reserved
#
# This file is part of Embedded Proto.
#
# Embedded Proto is open source software: you can redistribute it and/or
# modify it under the terms of the GNU General Public License as published
# by the Free Software Foundation, version 3 of the license.
#
# Embedded Proto  is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Embedded Proto. If not, see <https://www.gnu.org/licenses/>.
#
# For commercial and closed source application please visit:
# <https://EmbeddedProto.com/license/>.
#
# Embedded AMS B.V.
# Info:
#   info at EmbeddedProto dot com
#
# Postal address:
#   Atoomweg 2
#   1627 LE, Hoorn
#   the Netherlands
#


# The sensor address defined in the embedded code.
address = "02:80:E1:00:34:12"

# The id's characteristics defined in the GATT service
SENSOR_CHAR_UUID = "340a1b80-cf4b-11e1-ac36-0002a5d5c51b"
COMMAND_CHAR_UUID = "e23e78a0-cf4a-11e1-8ffc-0002a5d5c51b"

import sys
import asyncio
from bleak import BleakClient
import bleak
import time
from generated import ble_messages_pb2


def print_control_keys():
    print("Command keys are:")
    print("y to turn on LED")
    print("n to turn off LED")
    print("g to get sensor value")
    print("")
    print("If your done press \"Q\"")


def process_cmd_input():
    get_sensor = False
    quit = False
    msg = None

    # Obtain input from the keyboard.
    char = input("Next command: ")
    
    if "y" == char:
        msg = ble_messages_pb2.Command()
        msg.led = ble_messages_pb2.Command.On
    elif "n" == char:
        msg = ble_messages_pb2.Command()
        msg.led = ble_messages_pb2.Command.Off
    elif "g" == char:
        get_sensor = True
    elif "Q" == char:
        # Stop the loop
        quit = True
    else: 
        print_control_keys()

    return get_sensor, msg, quit


async def run(loop):    
    connect_attempts = 0
    while True:
    
        try:
            print("trying to connect to: " + address)
            async with BleakClient(address, loop=loop) as client:
                connect_attempts=0
                print_control_keys()
                
                while True:
                
                    get_sensor, command, quit = process_cmd_input()                  
                    
                    if quit:
                        print("Terminating script")
                        sys.exit()

                    if command:
                        b = bytearray()

                        # Serialize the command message and send it over BLE.
                        command_str = command.SerializeToString()
                        
                        # Next frame the data with the data length infront of 
                        # the serialized data.
                        l = len(command_str)
                        b.extend(l.to_bytes(1, byteorder='little'))
                        b.extend(command_str)
                        
                        # Next send the actual data                 
                        await client.write_gatt_char(COMMAND_CHAR_UUID, b)
                    
                    if get_sensor:
                        # Await a reply.
                        bytes = await client.read_gatt_char(SENSOR_CHAR_UUID)
                         
                        # The data is framed the first byte contains the number
                        # of bytes in the protobuf serialized data.
                        length = bytes[0]
                       
                        if 0 < length:
                  
                            # Strip the length from the bytes array
                            bytes = bytes[1:length+1]

                            # Check if we have received all bytes.
                            if length == len(bytes):
                            
                                # Create a new protobuf message object and 
                                # deserialze the received data.
                                sensor = ble_messages_pb2.Sensor()
                                sensor.ParseFromString(bytes)
                                
                                # And do something with the sensor value received.
                                print(str(sensor.light_sensor))
                            
                    time.sleep(0.5)
        
        except bleak.exc.BleakError:
            if connect_attempts > 9:
                print("Cannot connect to: " + address + " terminating script")
                sys.exit()
            print("trying to connect to: " + address)
            connect_attempts = connect_attempts+1
            

if __name__ == "__main__":
    
    loop = asyncio.get_event_loop()
    loop.run_until_complete(run(loop))
