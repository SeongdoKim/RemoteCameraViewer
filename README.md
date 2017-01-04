# RemoteCameraViewer
This repository is a Linux application to receive image streaming from an Android smartphone, which is seen by the camera of the connected Android smartphone. This application is tested on Ubuntu 16.04 with OpenCV 2.4.11 version.

# Prerequisites
- Either OpenCV 2.X or OpenCV 3.X 
- JSON library for C/C++

### OpenCV
To install OpenCV 2.X, you can type the following commands on your Linux shell.
```
sudo apt-get install build-essential
sudo apt-get install cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev
sudo apt-get install libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libjasper-dev libdc1394-22-dev
sudo apt-get install libopencv-dev
```
If a feature of OpenCV 3.X is required, please follow the instruction from the official OpenCV 3.X documentation.

### JSON Library
In the application, JSON objects are used to communicate with an Android application. JSON library can be installed by executing the following commands on your Linux shell.
```
sudo apt-get install libjson0 libjson0-dev
```

# Complie Instruction
To compile the source codes, first download the repository and move to the folder where you downloaded the repository. To compile the code, please type the following commands:
```
cmake .
make
```
If the compilation succeeds, you can find an executable file named 'RemoteCameraViewer'. If you run this program, you will be able to see some messages that the program is ready to receive a connection. Now it is time to connect your Android application to this program. For the Android side application, please refer the [Android side application repository](https://github.com/SeongdoKim/AndroidRemoteCamera).
