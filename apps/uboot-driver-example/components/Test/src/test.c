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

    run_uboot_command("setenv ipaddr 192.168.100.108");

    run_uboot_command("ping 192.168.100.3");

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
