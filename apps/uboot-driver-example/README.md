<!--
  SPDX-License-Identifier: BSD-2-Clause
-->

# U-Boot Driver Framework Test Application

This CaMKeS application utilises the libubootdrivers driver framework provided from project_libs in order to perform tests across a number of devices. Currently the applications main supported platform is the Avnet MaaXBoard, with tests for USB, MMC, Filesystem, Ethernet, GPIO, I2C, SPI IOMUX, Timers and LEDs. Also supported is the Odroid C2, with support for testing only the on-board LEDs.

## MaaXBoard Tests

When the application is first loaded, the text "Starting U-Boot driver example" is printed to the screen. The application then uses the `initialise_uboot_drivers` method provided by the libubootdrivers API in order to ready the devices listed in the platform specific configuration file for this application. These files are stored under camkes/apps/uboot-driver-example/include/plat/platform_name/platform_devices.h . The application then uses the `run_uboot_command` method provded by libubootdrivers API in order to run the `dm_tree` U-Boot command. This command displays a tree of all the currently initialized devies. The `clk_dump` command will then be run, displaying a list of all the currently enabled clocks.

SPI is the next driver which is tested, altought this requires a BMP280 to be connected to the SPI pins on the MaaXBoards GPIO header, if the BMP280 is connected, it will be initialized and raw temprature data will be read from it and printed to the screen 10 times. For more information about the BMP280 see [here](https://github.com/sel4devkit/seL4-DevKit-Doc/blob/main/seL4-doc/src/appendices/spi_bmp280.md).

After the SPI driver has been tested, the clock driver will be tested. This is done by running the U-Boot command `clocks`. This command lists the names and frequencies of all currently enabled clocks. Following that, the application will display a list of the on-board LEDs using the `led list` U-Boot command. It will then rapidly turn on and off both the MaaXBoard's LEDs in a rolling wave type pattern.

Upon compltetion of the LED test, the I2C driver is tested. This is done by communication with the on-board power management IC, which is connected to the SOC via a I2C bus. First the app runs the U-Boot command `i2c dev 0` which probes I2C bus zero for devices, then `i2c probe` is run, which should return the chip identifier of the PMIC, which is `0x4b.

Once I2C has been tested, the application will test the ethernet driver, firtsly the `setenv ipaddr` command is run which sets an IP address for the device (this should be changed from the test.c file as required). The application will then attempt to ping the IP address specified in the test.c using the `ping` command.

The USB driver is tested next. First, the `stdin` enviroment variable is set to `usbkbd` in order to test a USB keyboard as an input device. Next the application with start the USB driver with the `usb start` command, and display a list of file system partitions stored to any currently connected USB mass storage device, using the `part list usb 0` command. The `fatls` command will then be run, listing files and directories on any FAT partition on a connected mass storage device. **Note: due to only one USB port functioning on the MaaXBoard, the keyboard is tested after the MMC, PINMUX and GPIO tests.**

The MMC driver (SD Card) will now be tested. First the `mmc info` command will be run, which displays information such as storage size for any connected MMC devices. In most cases this will just be any SD card inserted into the SD card slot. Then, like the USB test, `part list` and `fatls` will be run for the MMC device 0 (SD Card slot), listing partition information and then listing files and directories on any FAT partition.

PINMUX and GPIO are tested next, with their associated status commands run which will display information about their current configuration.

Finally, the USB keyboard driver is tested. First, "Echoing input from the USB keyboard:" is printed to the screen, the application then loops 1000 times, wating 10ms every loop, attempting to read a character from a key press and print it to the screen. The characters read will be displated alongside the text "Received character: "

Finally, the application executes the `usb stop` command to shutdown the USB controllers, and calls the `shutdown_uboot_drivers` from the libubootdrivers API to shutdown all drivers. The test program is now complete.
