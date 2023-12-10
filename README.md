
![alt text](https://embeddedproto.com/wp-content/uploads/2022/04/Embedded_Proto.png "Embedded Proto Logo")


Copyrights 2020-2024 Embedded AMS B.V. Amsterdam, [www.EmbeddedAMS.nl](https://www.EmbeddedAMS.nl), [info@EmbeddedAMS.nl](mailto:info@EmbeddedAMS.nl)


Looking for a more elaborate description of this example? Please visit: https://embeddedproto.com/ble-example-with-embedded-proto/


# Introduction

This repository hosts example code for Embedded Proto, the embedded implementation of Google Protocol Buffers. It is a simple example showing how a microcontroller and desktop pc can communicate using Bluetooth Low Energy (BLE). Command messages are send from a desktop script to the MCU using BLE. 

![alt text](https://embeddedproto.com/wp-content/uploads/2020/05/PC_to_MCU_over_UART.png "PC to MCU over BLE")

This example uses BLE to control a led on the NUCLEO board and also retrieve a sensor value. We use of a NUCLEO-F446RE and a X-NUCLEO-IDB05A1 bluetooth evaluation board both made by ST Microelectronics. The microcontroller is based on an ARM Cortex-M4 processor. To build the source code and program the hardware STM32CubeIDE has been used. 

The desktop program is a simple python script. From this script you can turn on/off the led on the NUCLEO board, and also retrieve a random value from an emulated sensor.


# Installation

1. Install STM32CubeIDE if you have not already.
2. Install the dependencies required by Embedded Proto. They are listed [here](https://github.com/Embedded-AMS/EmbeddedProto), please match the version used in this example.
3. Checkout this example repository including the submodule of Embedded Proto: `git clone --recursive https://github.com/Embedded-AMS/EmbeddedProto_Example_STM32_BLE.git`.
4. Setup the environment required for Embedded Proto and the desktop script by running the setup script: `python ./setup.sh`.

The setup script already does it for you but you can regenerate the source code using the `./generate_source_files.sh` or `generate_source_files.bat` script. This is required when you have changed the \*.proto file.


# Running the code

First connect the BLE X-NUCLEO-IDB05A1 to the NUCLEO. Then connect the NUCLEO via the usb programmer and use STM32CubeIDE to build and program the microcontroller on it. 

Next go to the desktop folder, activate the virtual environment and run the script. 

On Linux:
```bash
cd desktop
source venv/bin/activate
python3 main.py
```

On windows
```bash
cd desktop
.\venv\Scripts\activate
python main.py
```


Have fun!
