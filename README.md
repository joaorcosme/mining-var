![truck](https://bitbucket.org/joaorcosme/pfc/raw/bda0d4c984e351e6121811aea09a06206f71a724/etc/truck.png)

### An Augmented Reality Application

This repository contains an Augmented Reality application to aid mining vehicle operators.

Author: João Cosme (joaorcosme@gmail.com)

On Bitbucket: https://bitbucket.org/joaorcosme/pfc/

#### Installation

This project depends on:

- [Nana 1.6.1](http://nanapro.org/en-us/)

- [OpenCV 2.4](https://opencv.org/)

- [Driver and CAN Layer 2 API for the Softing CANpro USB interface](https://industrial.softing.com/en/products/interface-cards-gateways/usb-interface-cards/can/can-single-channel-usb-interface-card.html)

If these libraries are correctly installed, you just need to run `make` on the root directory to compile the application.

#### Running

After compiling, there will be 2 executables in the project directory:

- `./can/can_test`: this will launch a window that displays real-time data from the BS-9000 Radar sensor.

- `./augreality/ar_app`: this will launch an AR window that displays video from the default camera + sensor data translated into graphical elements.

#### License

The GNU General Public License v3.0
