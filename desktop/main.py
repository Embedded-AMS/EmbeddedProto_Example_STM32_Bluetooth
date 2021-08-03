#
# Copyright (C) 2020 Embedded AMS B.V. - All Rights Reserved
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
#   Johan Huizingalaan 763a
#   1066 VH, Amsterdam
#   the Netherlands
#

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
    send_command = False
    get_sensor = False
    quit = False
    msg = ble_messages_pb2.Command()

    char = input("Next command: ")
    if "y" == char:
        msg.led = ble_messages_pb2.Command.On
        send_command = True
    elif "n" == char:
        msg.led = ble_messages_pb2.Command.Off
        send_command = True
    elif "g" == char:
        get_sensor = True
    elif "Q" == char:
        # Stop the loop
        quit = True
    else:
        send_command = False
        get_sensor = False      
        print_control_keys()

    if send_command:
        return None, msg, quit
    elif get_sensor:
        return True, None, quit
    else:
        return None, None, quit


address = "02:80:E1:00:34:12"
SENSOR_CHAR_UUID =   "340a1b80-cf4b-11e1-ac36-0002a5d5c51b"
COMMAND_CHAR_UUID = "e23e78a0-cf4a-11e1-8ffc-0002a5d5c51b"


async def run(loop):    
    connect_attempts = 0
    while True:
    
        try:
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
                        # First send the length of the message.
                        l = len(command_str)
                        b.extend(l.to_bytes(1, byteorder='little'))
                        # Next send the actual data
                        b.extend(command_str)                     
                        await client.write_gatt_char(COMMAND_CHAR_UUID, b)
                    
                    if get_sensor:
                        # Await a reply.
                        bytes = await client.read_gatt_char(SENSOR_CHAR_UUID)
                        
                        #Print received protobuf message
                        #print(bytes)
                        
                        length = bytes[0]
                        if 0 < length:
                  
                            bytes = bytes[1:length+1]

                            # Check if we have received all bytes.
                            if length == len(bytes):
                                sensor = ble_messages_pb2.Sensor()
                                sensor.ParseFromString(bytes)
                                
                                print(str(sensor.light_sensor))
                                # Do something with the reply.
                                #if sensor.light_sensor:
                                #   print("We have a winner!")
                                #else:
                                #    print("x pos: " + str(reply.x_pos))
                                #    print("y pos: " + str(reply.y_pos))
                            
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
