# PCA9685 README
This software is a devLib extension to [wiringPi](http://wiringpi.com/) and enables it to control the [Adafruit PCA9685 16-Channel 12-bit PWM/Servo Driver](http://www.adafruit.com/products/815) via I2C interface.

Copyright (c) 2019 Reinhard Sprung

If you have questions or improvements email me at
reinhard.sprung[at]gmail.com
如果你有问题或需要改进，给我发电子邮件。

NOTE: The software could run faster because some write function read the current register value before they write to it, but it shouldn't matter in usual setups. If you need it super fast, you probably need to program your own write functions.

注意：该软件可以运行得更快，因为一些写函数在写入之前会读取当前寄存器的值，但在通常的设置中这应该没有关系。如果你需要它非常快，你可能需要编写自己的写函数。

## REQUIREMENTS
Enable I2C on your Raspberry Pi and make sure your PCA9685 controller board can be found. A tutorial on how to do this can be found [here](https://learn.adafruit.com/adafruits-raspberry-pi-lesson-4-gpio-setup/configuring-i2c).

在你的树莓派上启用I2C，并确保可以找到你的PCA9685控制板

## INSTALL
This pca9685 library requires an installed version of wiringPi.
WiringPi comes preinstalled on standard raspbian systems so check first if it is there already. 
To do so, open a terminal and execute `gpio -v`.

这个pca9685库需要安装wiringPi版本。
WiringPi在标准的树莓派系统中是预先安装好的，所以首先检查一下它是否已经在那里了。
要执行这个操作，打开一个终端并执行“gpio -v”。

If it's not installed, the easiest way is by calling `sudo apt install wiringpi`. If you need addidtional information or want to install from sources, check out [http://wiringpi.com/download-and-install/](http://wiringpi.com/download-and-install/). 

如果它没有被安装，最简单的方法是调用“sudo apt install wiringpi”,如果你需要更多信息或想从源头上安装，可以查看这个网址。

NOTE: WiringPi is now deprecated and will not work out of the box on newer (≥Rpi4) boards, check out
[http://wiringpi.com/wiringpi-deprecated/](http://wiringpi.com/wiringpi-deprecated/)

注意：WiringPi现在已被弃用，并且在较新的（>Rpi4）开发板上不能直接使用，查看这个网址。

## USAGE
You can include __pca9685.h__ and __pca9685.c__ directly in your project or compile it and include the lib file instead.

你可以直接在你的项目中包含__pca9685.h__和__pca9685.c__，或者编译它并包含库文件。

To compile, navigate into the src folder an run

要进行编译，导航到src文件夹并运行

```console
sudo make install
```
This will install pca9685 in your __/usr/lib__, __/usr/local/lib__ and __/usr/local/include__ directories.
To include the files add the line

这将把pca9685安装到你的“/usr/lib”、“/usr/local/lib”和“/usr/local/include”目录中。要包含这些文件，添加以下行。

```cpp
#include <pca9685.h>
```
into your source code and include "__wiringPiPca9685__" in your linked files during compilation

在你的源代码中，并在编译过程中，在连接的文件中包含这个。

## EXAMPLES
There are some example files included in this repository. To compile them, cd into __examples__ directory and `make` them. 
To run, add a "__./__" before each example and execute them, e.g. `./servo`. 

这个代码库中包含一些示例文件。要编译它们……

## FUNCTIONS
Use	
```cpp
int pca9685Setup(const int pinBase, const int i2cAddress/* = 0x40*/, float freq/* = 50*/);
```
to setup a single pca9685 device at the specified i2c address and PWM frequency.

Parameters are:

	- pinBase: 		Use a pinBase > 64, eg. 300
	- i2cAddress:	The default address is 0x40
	- freq:			Frequency will be capped to range [40..1000] Hertz. Try 50 for servos

When successful, this will reserve 17 pins in wiringPi and return a file descriptor with 
which you can access advanced functions (view below).

The pca9685 pins are as follows: 

	[0...15]: The 16 individual output pins as numbered on the driver
	[16]: All pins (Note that reading from this pin returns always 0)

Use the following wiringPi functions to read and write PWM.
NOTE: Don't forget to add the pin base!


Set PWM
```cpp
void pwmWrite (int pin, int value)
```
if value <= 0, set full-off
else if value >= 4096, set full-on
else set PWM

Set full-on or full-off
```cpp
void digitalWrite (int pin, int value)
```
if value != 0, set full-on
else set full-off

Read off-register
```cpp
int digitalRead (int pin)
```
To get PWM: mask with 0xFFF
To get full-off bit: mask with 0x1000
Note: ALL_LED pin will always return 0

Read on-register
```cpp
int analogRead (int pin)
```
To get PWM: mask with 0xFFF
To get full-on bit: mask with 0x1000
Note: ALL_LED pin will always return 0



NOTE: Unfortunately wiringPi doesn't offer suitable names for pca9685's functions, so we have to work with the provided ones. 
Masking means to bitwise-AND (operator &) the return value with the mask. E.g. & 0xFFF
```cpp
int offValue = digitalRead(pinBase + 0) & 0xFFF;
```
## ADVANCED		

If you don't want to use the wiringPi functions or want to access the pca9685
directly, you can use the file descriptor returned from the setup function to access 
the following functions for each connected pca9685 individually.
(View source code for more details)

Set output frequency in a range between 40 and 1000 Hertz
```cpp
void pca9685PWMFreq(int fd, float freq);
```
Reset all PWM output of this device to default state which is full-off
```cpp
void pca9685PWMReset(int fd);
```
Write PWM on and off values to a specific pin. (View source code)
```cpp
void pca9685PWMWrite(int fd, int pin, int on, int off);
void pca9685PWMRead(int fd, int pin, int *on, int *off);
```
Write enable or disable full-on and full-off of a specific pin. (View source code)
```cpp
void pca9685FullOn(int fd, int pin, int tf);
void pca9685FullOff(int fd, int pin, int tf);
```
