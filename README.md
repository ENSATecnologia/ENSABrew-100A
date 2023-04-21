# ENSABrew-100A
The ENSABrew-100A is an electronic device for automation in the craft beer brewing process! Based on the ATmega 2561 microcontroller, it includes analog inputs (NTC), digital inputs (water level and volume control) and modulated outputs (PWM/SSR). Connection via Wi-Fi or Bluetooth app. All these facilities in a single product.

This project is based on the excellent project: :+1: [**@MCUdude/MegaCore**](https://github.com/MCUdude/MegaCore)

# MegaCore
[![Build Status](https://travis-ci.com/MCUdude/MegaCore.svg?branch=master)](https://travis-ci.com/MCUdude/MegaCore) [![MegaCore forum thread](https://img.shields.io/badge/support-forum-blue.svg)](https://forum.arduino.cc/index.php?topic=386733.0)

## How to install
#### Boards Manager Installation
This installation method requires Arduino IDE version 1.6.4 or greater;
* Open the Arduino IDE;
* Open the **File > Preferences** menu item;
* Enter the following URL in **Additional Boards Manager URLs**: `https://mcudude.github.io/MegaCore/package_MCUdude_MegaCore_index.json`
* Open the **Tools > Board > Boards Manager...** menu item;
* Wait for the platform indexes to finish downloading;
* Scroll down until you see the **MegaCore** entry and click on it;
* Click **Install (version 2.1.1)**;
* After installation is complete close the **Boards Manager** window.

## Getting started with MegaCore
Ok, so you're downloaded and installed MegaCore, but how to get started? Here's a quick start guide:
* Hook up your microcontroller as shown in the [pinout diagram](#pinout);
  1. If you're not planning to use the bootloader (uploading code using a USB to serial adapter), the FTDI header and the 100 nF capacitor on the reset pin can be omitted;
* Open the **Tools > Board** menu item, select **MegaCore** and select our preferred target;
* Select your preferred clock frequency. **16 MHz** is standard on most Arduino boards;
* Select what kind of programmer you're using under the **Programmers** menu;
* Hit **Burn Bootloader**. If an LED is connected to pin PB5/PB7, it should flash twice every second;
* Now that the correct fuse settings is sat and the bootloader burnt, you can upload your code in two ways:
  1. Disconnect your programmer tool, and connect a USB to serial adapter to the microcontroller, like shown in the [pinout diagram](#pinout). Then select the correct serial port under the **Tools** menu, and click the **Upload** button. If you're getting some kind of timeout error, it means your RX and TX pins are swapped, or your auto reset circuity isn't working properly (the 100 nF capacitor on the reset line).
  2. Keep your programmer connected, and hold down the `shift` button while clicking **Upload**. This will erase the bootloader and upload your code using the programmer tool.

Your code should now be running on the microcontroller!

## Pinout

### 64-pin chips
Since there are no standardized Arduino pinout for this chip family, **MCUdude** created one tried to make it as simple and logical as possible. The standard LED pin is assigned to Arduino pin 13 (PB5), and will blink twice if you hit the reset button.

| ATmega165/169/325/329/645/649                           |
|---------------------------------------------------------|
| <img src="https://i.imgur.com/YJ3ojm1.png" width="280"> |

## Programmers
Select your microcontroller in the boards menu, then select the clock frequency. You'll have to hit "Burn bootloader" in order to set the correct fuses and upload the correct bootloader. <br/>
Make sure you connect an ISP programmer, and select the correct one in the "Programmers" menu. For time critical operations an external oscillator is recommended.

## Write to own flash
MegaCore uses Optiboot Flash, a bootloader that supports flash writing within the running application, thanks to the work of [@majekw](https://github.com/majekw).
This means that content from e.g. a sensor can be stored in the flash memory directly without the need of external memory. Flash memory is much faster than EEPROM, and can handle at least 10 000 write cycles before wear becomes an issue.
For more information on how it works and how you can use this in you own application, check out the [Serial_read_write](https://github.com/MCUdude/MegaCore/blob/master/avr/libraries/Optiboot_flasher/examples/Serial_read_write/Serial_read_write.ino) for a simple proof-of-concept demo, and
[Flash_put_get](https://github.com/MCUdude/MegaCore/blob/master/avr/libraries/Optiboot_flasher/examples/Flash_put_get/Flash_put_get.ino) + [Flash_iterate](https://github.com/MCUdude/MegaCore/blob/master/avr/libraries/Optiboot_flasher/examples/Flash_iterate/Flash_iterate.ino) for useful examples on how you can store strings, structs and variables to flash and retrieve then afterwards.
The [Read_write_without_buffer](https://github.com/MCUdude/MegaCore/blob/master/avr/libraries/Optiboot_flasher/examples/Read_write_without_buffer/Read_write_without_buffer.ino) example demonstrate how you can read and write to the flash memory on a lower level without using a RAM buffer.