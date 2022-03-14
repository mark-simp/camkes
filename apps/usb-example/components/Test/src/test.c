/*
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <camkes.h>
#include <sel4platsupport/io.h>

#include <sel4_usb.h>
#include <usb_devices.h>

#include <libfdt.h>

int run_usb_example(ps_io_ops_t *io_ops)
{

    printf("Calling sel4_usb_init\n");

    const char *device_paths[] = DEVICE_PATHS
    int error = sel4_usb_init(io_ops, device_paths, DEVICE_PATHS_LENGTH);
    assert(!error);

    printf("Returned from sel4_usb_init\n");

    // Loop forever
    while (1);
}

CAMKES_POST_INIT_MODULE_DEFINE(run_usb_example_, run_usb_example);
