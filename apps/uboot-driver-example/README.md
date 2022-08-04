<!--
     Copyright 2022, Capgemini Engineering

     SPDX-License-Identifier: BSD-2-Clause
-->

# U-Boot Driver Framework Test Application

This CAmkES application exercises the libubootdrivers driver library from projects_libs, performing tests across a number of devices. The application's primary supported platform is the Avnet MaaXBoard, with tests for USB, SD/MMC, Filesystem, Ethernet, GPIO, I2C, SPI, IOMUX, Timers, and LEDs. There is also minimal support for the Odroid-C2 platform, demonstrating control of the on-board LED.

## MaaXBoard Tests

Some of the key features of the application are as follows:

- The `initialise_uboot_drivers` method provided by the libubootdrivers API prepares the devices listed in the platform-specific configuration file for this application (i.e. `camkes/apps/uboot-driver-example/include/plat/<platform_name>/platform_devices.h`).
- The application uses the `run_uboot_command` method provided by the libubootdrivers API in order to run U-Boot commands. Early examples in the application are `dm_tree` (displays a tree of all the currently initialised devices) and `clk_dump` (displaying a list of all the currently enabled clocks).
- The application tests the library's SPI driver, if a BMP280 pressure sensor has been connected to the SPI bus on the MaaXBoard's GPIO header; the sensor will be initialised and raw temperature data will be read from it and printed to the screen. For more information about connecting a BMP280, see the seL4 Developer Kit [here](https://github.com/sel4devkit/seL4-DevKit-Doc/blob/main/seL4-doc/src/appendices/spi_bmp280.md). If no sensor is connected, the application still executes the operations, but they do not return valid data.
- The application tests the clock driver by running the U-Boot command `clocks`, which lists the names and frequencies of all currently enabled clocks.
- The application displays a list of the on-board LEDs using the U-Boot command `led list`. It will then flash the MaaXBoard's two LEDs in a cyclic pattern.
- The application tests the I2C driver by communicating with the on-board power management IC, which is connected to the SoC via an I2C bus. The application probes the bus and reads the chip identifier of the power management IC, which is `0x4b`.
- The application tests the Ethernet driver by attempting to ping an IP address. Note that IP addresses are hard-coded into `components/Test/src/test.c` and these need to be edited appropriately by the user before compilation.
- The application initialises the USB driver with the U-Boot `usb start` command (there is a corresponding `usb stop` command later). It displays a list of file system partitions on any currently connected USB mass storage device (using the `part list usb 0` command) and lists files and directories on any FAT partition (using the `fatls` command).
- The application also includes a USB keyboard test, reading and echoing keypresses during a defined period.
- The application accesses the SD card using the SD/MMC driver. It displays information such as storage size for any connected MMC devices (using the `mmc info` command). Like the USB mass storage test, the `part list` and `fatls` commands list partition information and files and directories on any FAT partition on the SD card.
- Filesystem operations write a test file to a FAT partition on the SD card before reading the contents back and deleting the file.
- The application tests IOMUX/PINMUX and GPIO drivers, using status commands to display information about their current configuration.
- The application shuts down all drivers using the `shutdown_uboot_drivers` method provided by the libubootdrivers API.

## Odroid-C2 Tests
The application's test suite for the Odroid-C2 platform is currently much reduced compared with that of the Avnet MaaXBoard. It displays a list of the on-board LEDs using the `led list` U-Boot command (there is only one LED on this board). It then rapidly flashes the Odroid-C2's LED.

## Application configuration
Details for additional configuration of tests can be found within the comments of `components/Test/src/test.c`.
