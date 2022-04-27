/*
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <camkes.h>
#include <platsupport/delay.h>

#include <uboot_drivers.h>
#include <platform_devices.h>

/** If eth frames have been received by the driver, copy a single frame from
 * the driver's buffer (rx_bufs), into the dataport of the caller of this
 * function.
 *
 * @param[out] len The size in bytes of the eth frame.
 * @return  If there are no frames available to be consumed, returns negative.
 *          If there was an error or the component hasn't been initialized yet,
 *          returns negative.
 *          If there was only one frame available to be consumed, returns 0.
 *          If there are other frames to be consumed even after the one that
 *          will be returned by the current invocation, returns 1 (i.e, "there
 *          is more data.").
 */
int client_rx(int *len)
{
    printf("client_rx called");

    return 0;
}

int client_tx(int len)
{
    printf("client_tx called");

    return 0;
}

void client_mac(uint8_t *b1, uint8_t *b2, uint8_t *b3, uint8_t *b4, uint8_t *b5, uint8_t *b6)
{
    *b1 = 0;
    *b2 = 0;
    *b3 = 0;
    *b4 = 0;
    *b5 = 0;
    *b6 = 0;

    printf("client_mac called");
}

int server_init(ps_io_ops_t *io_ops)
{
    printf("Starting U-Boot driver library\n");

    /* Start the U-Boot driver library containing the ethernet driver */
    const char *device_paths[] = DEVICE_PATHS
    int error = initialise_uboot_drivers(io_ops, device_paths, DEVICE_PATHS_LENGTH);
    if (error)
        ZF_LOGF("Unable to find an ethernet device");

    /* Initialise ethernet */
    error = uboot_eth_init();
    if (error)
        ZF_LOGF("Unable to initialise ethernet");

    /* Wait for incoming data */
    int ret;
    while(1) {
        // printf("Testing...\n");
        ret = uboot_eth_receive();
        if (ret > 0)
            continue;
        else if (ret < 0)
            ZF_LOGE("eth_rx returned code %i", ret);

        /* Add a small delay to prevent busy looping */
        ps_mdelay(1);
    }

    return 0;
}

CAMKES_POST_INIT_MODULE_DEFINE(ethdriver_run, server_init);