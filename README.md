# [WIP] screw_drive_unit_pico
Code for Controlling Screw-Drive Unit by Using Raspberry Pi Pico

<img src="https://github.com/Xiaoyu0601-Wang/my_robots/blob/main/amphibious_snake_like_robot/picture/amphibious_snake_like_robot_screw_drive_unit.jpg" alt="Screw Drive Mechanism" width="400">

## Environment Configuration
Install build tools
```sh
sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential
```

Get the source with the following directory structure:
```sh
~/screw_drive_unit_pico/
  pico-sdk/
  pico-examples/
  screw_drive_unit_pico/
```

Update submodules
```sh
git submodule update --init
cd pico-sdk
git submodule update --init
```

Set up the user environment variable
```sh
cd ~
echo "export PICO_SDK_PATH=$HOME/screw_drive_unit_pico/pico-sdk" >> ~/.bashrc
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

## Pico-CAN-B from WaveShare
Website: https://www.waveshare.com/wiki/Pico-CAN-B

# CAN Message Frame
App| msg[0] | msg[1] | msg[2] | msg[3] | msg[4~7]
---|---|---|---|---|---
hello_serial | Read(0x01)/Write(0x02) | Register | |
hello_usb | The obligatory Hello World program for Pico (Output over USB version) | htt | |
blink | Blink an LED on and off. | http | |

# Some Thoughts
Memory usage is different from that of a general microcontroller like Stm32.
