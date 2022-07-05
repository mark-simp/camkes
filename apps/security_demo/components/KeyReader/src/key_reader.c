/*
 * Copyright 2022, Capgemini Engineering
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <camkes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sel4platsupport/io.h>
#include <platsupport/delay.h>

#include <camkes/io.h>
#include <uboot_drivers.h>
#include <usb_platform_devices.h>

int run(void)
{
    /* Get the IO operations (needed by the U-Boot driver library) */
    ps_io_ops_t io_ops;
    if (camkes_io_ops(&io_ops)) {
        assert("Failed to initialize io_ops");
    }

    /* Start the U-Boot driver library */
    const char *const_reg_paths[] = REG_PATHS;
    const char *const_dev_paths[] = DEV_PATHS;
    assert(!initialise_uboot_drivers(
        /* Provide the platform support IO operations */
        &io_ops,
        /* List the device tree paths that need to be memory mapped */
        const_reg_paths, REG_PATH_COUNT,
        /* List the device tree paths for the devices */
        const_dev_paths, DEV_PATH_COUNT));

    /* Set USB keyboard as input device */
    int ret = run_uboot_command("setenv stdin usbkbd");
    if (ret < 0) {
        assert("Failed to USB keyboard as the input device");
    }

    /* Start the USB subsystem */
    ret = run_uboot_command("usb start");
    if (ret < 0) {
        assert("Failed to start USB driver");
    }

    /* Loop forever reading keypresses and passing to the 'handle_character' RPC */
    while (true)
    {
        if (uboot_stdin_tstc() > 0)
            a_handle_character(uboot_stdin_getc());
        ps_mdelay(10);
    }

    return 0;
}
