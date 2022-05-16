/*
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <camkes.h>
#include <sel4platsupport/io.h>
#include <platsupport/delay.h>

#include <uboot_drivers.h>
#include <platform_devices.h>

int run_uboot_driver_example(ps_io_ops_t *io_ops)
{
    printf("Starting U-Boot driver example\n");

    const char *device_paths[] = DEVICE_PATHS
    assert(!initialise_uboot_drivers(io_ops, device_paths, DEVICE_PATHS_LENGTH));

    run_uboot_command("dm tree");

    run_uboot_command("clk dump");

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

    run_uboot_command("clocks");

    run_uboot_command("led list");

    // Flash the LEDs
    for (int x=0; x<4; x++) {
        ps_mdelay(125);
        run_uboot_command("led usr_led off");
        ps_mdelay(125);
        run_uboot_command("led sys_led on");
        ps_mdelay(125);
        run_uboot_command("led usr_led on");
        ps_mdelay(125);
        run_uboot_command("led sys_led off");
    }

    // Probe and read device already present on MaaXBoard I2C bus
    run_uboot_command("i2c dev 0");
    // Probing I2C bus 0 should return chip address of 0x4b for BD71837MWV Power Management IC
    run_uboot_command("i2c probe");
    run_uboot_command("i2c md 0x4b 0x0.1 0x20");

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

    // USB and SD/MMC operations
    
    run_uboot_command("setenv stdin usbkbd"); // Use a USB keyboard as the input device

    run_uboot_command("usb start");

    run_uboot_command("part list usb 0");

    run_uboot_command("fatls usb 0");

    run_uboot_command("mmc info");

    run_uboot_command("part list mmc 0");

    run_uboot_command("fatls mmc 0");

    run_uboot_command("dm tree");

    run_uboot_command("usb tree");

    run_uboot_command("usb info");

    // Loop for a while reading keypresses and echoing to screen
    printf("Echoing input from the USB keyboard:\n");
    for (int x=0; x<=1000; x++) {
        while (uboot_stdin_tstc())
            printf("Received character: %c\n", uboot_stdin_getc(), stdout);
        ps_mdelay(10);
    }

    run_uboot_command("usb stop");

    shutdown_uboot_drivers();

    printf("Completed U-Boot driver example\n");

    // Loop forever
    while (1);
}

CAMKES_POST_INIT_MODULE_DEFINE(run_uboot_driver_example_, run_uboot_driver_example);
