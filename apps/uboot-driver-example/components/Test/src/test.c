/*
 * Copyright 2022 Capgemini Engineering
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <camkes.h>
#include <sel4platsupport/io.h>
#include <platsupport/delay.h>

#include <uboot_drivers.h>
#include <platform_devices.h>

/* Determine which functionality to test based upon the platform */
#if defined(CONFIG_PLAT_MAAXBOARD)
    #define TEST_CLK
    #define TEST_CLOCKS
    #define TEST_SPI
    #define TEST_LED
    #define TEST_LED_NAME_1 "usr_led"
    #define TEST_LED_NAME_2 "sys_led"
    #define TEST_I2C
    #define TEST_ETHERNET
    #define TEST_USB
    #define TEST_MMC
    #define TEST_PINMUX
    #define TEST_GPIO
    #define TEST_FILESYSTEM
    #define TEST_FILESYSTEM_PARTITION "mmc 0:1"  // Partition 1 on mmc device 0
    #define TEST_FILESYSTEM_FILENAME  "test_file.txt"

#elif defined(CONFIG_PLAT_ODROIDC2)
    #define TEST_PINMUX
    #define TEST_GPIO
    #define TEST_LED
    #define TEST_LED_NAME_1 "c2:blue:alive"
    #define TEST_LED_NAME_2 "c2:blue:alive"

#else
    /* Enable all tests for unrecognised platform */
    #define TEST_CLK
    #define TEST_CLOCKS
    #define TEST_SPI
    #define TEST_LED
    #define TEST_LED_NAME_1 "dummy_led"
    #define TEST_LED_NAME_2 "dummy_led"
    #define TEST_I2C
    #define TEST_ETHERNET
    #define TEST_USB
    #define TEST_MMC
    #define TEST_PINMUX
    #define TEST_GPIO
    #define TEST_FILESYSTEM
    #define TEST_FILESYSTEM_PARTITION "dummy_partition"
    #define TEST_FILESYSTEM_FILENAME  "test_file.txt"
#endif

int run_uboot_driver_example(ps_io_ops_t *io_ops)
{
    printf("Starting U-Boot driver example\n");

    const char *const_reg_paths[] = REG_PATHS;
    const char *const_dev_paths[] = DEV_PATHS;
    assert(!initialise_uboot_drivers(
        /* Provide the platform support IO operations */
        io_ops,
        /* List the device tree paths that need to be memory mapped */
        const_reg_paths, REG_PATH_COUNT,
        /* List the device tree paths for the devices */
        const_dev_paths, DEV_PATH_COUNT));

    run_uboot_command("dm tree");

    #ifdef TEST_CLK
    run_uboot_command("clk dump");
    #endif

    #ifdef TEST_SPI
    printf("Initialising BMP280 sensor on SPI bus (if connected):\n");
    // Read id register of device on SPI bus 0
    // For the BMP280 sensor, expect to return id of 0x58
    run_uboot_command("sspi 0:0.3@1000000 16 D000");

    // Initialise BMP280 prior to reading temperature register
    run_uboot_command("sspi 0:0.3@1000000 16 7508");
    run_uboot_command("sspi 0:0.3@1000000 16 7437");

    // The raw data does not correspond to meaningful temperatures without further processing
    printf("Raw temperature data from BMP280 sensor:\n");
    for (int x=0; x<=10; x++) {
        run_uboot_command("sspi 0:0.3@1000000 24 FA0000");
    }
    #endif

    #ifdef TEST_CLOCKS
    run_uboot_command("clocks");
    #endif

    #ifdef TEST_LED
    run_uboot_command("led list");

    // Flash the LEDs
    for (int x=0; x<4; x++) {
        ps_mdelay(125);
        run_uboot_command("led "TEST_LED_NAME_1" off");
        ps_mdelay(125);
        run_uboot_command("led "TEST_LED_NAME_2" on");
        ps_mdelay(125);
        run_uboot_command("led "TEST_LED_NAME_1" on");
        ps_mdelay(125);
        run_uboot_command("led "TEST_LED_NAME_2" off");
    }
    #endif

    #ifdef TEST_I2C
    // Probe and read device already present on MaaXBoard I2C bus
    run_uboot_command("i2c dev 0");
    // Probing I2C bus 0 should return chip address of 0x4b for BD71837MWV Power Management IC
    run_uboot_command("i2c probe");
    run_uboot_command("i2c md 0x4b 0x0.1 0x20");
    #endif

    #ifdef TEST_ETHERNET
    /* Edit the following for your IP addresses:
     * run_uboot_command("setenv ipaddr xxx.xxx.xxx.xxx"); // IP address to allocate to MaaXBoard
     * run_uboot_command("ping yyy.yyy.yyy.yyy"); // IP address of host machine
     */
    run_uboot_command("setenv ipaddr 192.168.1.106");
    run_uboot_command("ping 192.168.1.194");

    /* Optionally, to ping to an address beyond the local network:
     * run_uboot_command("setenv gatewayip zzz.zzz.zzz.zzz"); // IP address of router
     * run_uboot_command("setenv netmask 255.255.255.0");
     * run_uboot_command("ping 8.8.8.8"); // An example internet IP address (Google DNS)
     */
    #endif

    #ifdef TEST_USB
    // USB operations
    run_uboot_command("setenv stdin usbkbd"); // Use a USB keyboard as the input device

    run_uboot_command("usb start");

    run_uboot_command("part list usb 0");

    run_uboot_command("fatls usb 0");
    #endif

    #ifdef TEST_MMC
    // SD/MMC operations
    run_uboot_command("mmc info");

    run_uboot_command("part list mmc 0");

    run_uboot_command("fatls mmc 0");
    #endif

    #ifdef TEST_PINMUX
    run_uboot_command("pinmux status -a");
    #endif

    #ifdef TEST_GPIO
    run_uboot_command("gpio status -a");
    #endif

    run_uboot_command("dm tree");

    #ifdef TEST_FILESYSTEM
    /* Example of filesystem handling. Writes a file to a FAT partition before reading
     * the contents back and deleting the file. */

    // String to build the U-Boot command in
    char uboot_cmd[64];

    // Test string to write to the file
    const char test_string[] = "Hello file!";

    // Test string to read the file into
    #define MAX_BYTES_TO_READ 32
    char read_string[MAX_BYTES_TO_READ];

    // Build command to write the test string to a file and execute command
    sprintf(uboot_cmd, "fatwrite %s 0x%x %s %x %x",
        TEST_FILESYSTEM_PARTITION,   // The U-Boot partition designation
        &test_string,                // Address of the data to write
        TEST_FILESYSTEM_FILENAME,    // Filename to write to (or create)
        strlen(test_string),         // The number of bytes to write
        0);                          // The offset in the file to start writing from
    run_uboot_command(uboot_cmd);

    // Read then output contents of the file
    sprintf(uboot_cmd, "fatload %s 0x%x %s %x %x",
        TEST_FILESYSTEM_PARTITION,   // The U-Boot partition designation
        &read_string,                // Address to read the data into
        TEST_FILESYSTEM_FILENAME,    // Filename to read from
        MAX_BYTES_TO_READ,           // Max number of bytes to read (0 = to end of file)
        0);                          // The offset in the file to start read from
    run_uboot_command(uboot_cmd);
    printf("String read from file %s: %s\n", TEST_FILESYSTEM_FILENAME, read_string);

    // Now delete the temporary file used for the example
    sprintf(uboot_cmd, "fatrm %s %s", TEST_FILESYSTEM_PARTITION, TEST_FILESYSTEM_FILENAME);
    run_uboot_command(uboot_cmd);
    #endif

    #ifdef TEST_USB
    run_uboot_command("usb tree");

    run_uboot_command("usb info");

    // Loop for a while reading keypresses and echoing to screen
    printf("Echoing input from the USB keyboard:\n");
    for (int x=0; x<=1000; x++) {
        while (uboot_stdin_tstc() > 0)
            printf("Received character: %c\n", uboot_stdin_getc(), stdout);
        ps_mdelay(10);
    }

    run_uboot_command("usb stop");
    #endif

    shutdown_uboot_drivers();

    printf("Completed U-Boot driver example\n");

    // Loop forever
    while (1);
}

CAMKES_POST_INIT_MODULE_DEFINE(run_uboot_driver_example_, run_uboot_driver_example);
