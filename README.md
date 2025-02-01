# [WIP] screw_drive_unit_pico
Code for Controlling Screw-Drive Unit by Using Raspberry Pi Pico, URL: https://www.raspberrypi.com/products/raspberry-pi-pico/

<img src="https://github.com/Xiaoyu0601-Wang/my_robots/blob/main/amphibious_snake_like_robot/picture/amphibious_snake_like_robot_screw_drive_unit.jpg" alt="Screw Drive Mechanism" width="400">

## Environment Configuration
Install build tools
```sh
sudo apt-get install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential
sudo apt-get install pkg-config libusb-1.0-0-dev
```

Get the source with the following directory structure:
```sh
screw_drive_unit_pico/
  |->pico-sdk/
  |->pico-examples/
  |->screw_drive_unit_pico/
```

Update submodules
```sh
git submodule update --init
cd pico-sdk
git submodule update --init
```

Set up the user environment variable, and modify $HOME with the path of the repository:
```sh
cd ~
echo "export PICO_SDK_PATH=$HOME/(to your directory path)/screw_drive_unit_pico/pico-sdk" >> ~/.bashrc
. ~/.bashrc
```

How to build
```sh
cd ~/screw_drive_unit_pico/screw_drive_unit_pico
mkdir build
cd build
cmake .. -DPICO_BOARD=pico_w -DCMAKE_BUILD_TYPE=Release
make
```
How to load program
1. Hold down the BOOTSEL button on your Pico-series device while plugging it into your development device using a micro USB cable to force it into USB Mass Storage Mode.
2. Copy main.uf2 in build folder into the USB Mass Storage Mode.

## Pico-CAN-B from WaveShare
Website: https://www.waveshare.com/wiki/Pico-CAN-B

# CAN Message Frame
App| msg[0] | msg[1] | msg[2] | msg[3] | msg[4~7]
---|---|---|---|---|---
frame structure | header | header | Instruction | Register | Data(uint32_t): msg[4]=L ~ msg[7]=H

## Instruction
Value | Insruction | Description
---|---|---
0x02 | Read | Instruction to read data from the Device
0x03 | Write | Instruction to write data on the Device

## Protocol
R/W | Register | Data
---|---|---
R   | 0x01 | unique board ID
R/W | 0x02 | Unit CAN ID: Standard ID(uint16_t): msg[6]=High 8 bits, msg[7]=L 3 bits
R/W | 0x03 | LED Enable/Disable
R/W | 0x04 | LED Status, On: 1, Off: 0
R/W | 0x05 | Motor Command
R/W | 0x06 | Joint 1 Command
R/W | 0x07 | Joint 2 Command
R/W | 0x08 | Joint 1 Torque Enable
R/W | 0x09 | Joint 2 Torque Enable

# Flash Register Address
Pico has a 2MB flash starting from address 0x10000000.
R/W | Register | Data
---|---|---
R/W | 0x10000000 | Unit CAN ID: High 8 bits
R/W | 0x10000001 | Unit CAN ID: Low 3 bits

# Some Tools for Development
## Serial Port Assistant
https://github.com/KangLin/SerialPortAssistant
Change port permission: 
```sh
sudo chmod 666 /dev/ttyUSB0
```

# Some Thoughts
Memory usage is different from that of a general microcontroller like Stm32.
