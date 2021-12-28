![truck](https://github.com/joaorcosme/mining-var/blob/master/etc/truck.png?raw=true)

### An Augmented Reality Application

This repository contains an Augmented Reality application to aid mining vehicle operators.

Author: João Cosme (joaorcosme@gmail.com)

#### Installation

This project depends on:

- [Nana 1.6.1](http://nanapro.org/en-us/)

- [OpenCV 2.4](https://opencv.org/)

- [Driver and CAN Layer 2 API for the Softing CANpro USB interface](https://industrial.softing.com/support/downloads.html?tx_softingdownloadcenter_downloadarea%5Baction%5D=index&tx_softingdownloadcenter_downloadarea%5Bcontroller%5D=DownloadArea)

If these libraries are correctly installed, you just need to run `make` on the root directory to compile the application.

#### Running

After compiling, there will be 2 executables in the project directory:

- `./can/can_test`: this will launch a window that displays real-time data from the BS-9000 Radar sensor.

- `./augreality/ar_app`: this will launch an AR window that displays video from the default camera + sensor data translated into graphical elements.

#### License

The GNU General Public License v3.0
