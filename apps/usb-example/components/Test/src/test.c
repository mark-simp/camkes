/*
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <camkes.h>
#include <camkes/sync.h>
#include <sel4platsupport/io.h>

#include <usb/usb.h>
#include <usb/plat/usb.h>

int run_usb_example(ps_io_ops_t *io_ops)
{
    ps_mutex_ops_t* mutex_ops = (ps_mutex_ops_t*) malloc(sizeof(*mutex_ops));
    init_camkes_mutex_ops(mutex_ops);

    usb_t* usb_host = (usb_t*) malloc(sizeof(*usb_host));

    printf("Calling usb_init...\n");

    int error = usb_init(USB_HOST_DEFAULT, io_ops, mutex_ops, usb_host);
    assert(!error);

    printf("usb_init returned success\n");

    // Loop forever

    while (1);
}

CAMKES_POST_INIT_MODULE_DEFINE(run_usb_example_, run_usb_example);
