/*
 * Copyright 2022, Capgemini Engineering
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <camkes.h>
#include <autoconf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <picoserver.h>
#include <assert.h>

#include <sel4platsupport/io.h>
#include <platsupport/delay.h>

#include <uboot_drivers.h>
#include <mmc_platform_devices.h>

#define ETH_PORT 1234

extern void *eth_send_buf;

/* A buffer of encrypted characters to transmit over ethernet */
#define ETH_TX_BUF_LEN 4096
char eth_pending_tx_buf[ETH_TX_BUF_LEN];
uint eth_pending_length = 0;

/* A buffer of encrypted characters to log to the SD/MMC card */
#define MMC_TX_BUF_LEN 4096
char mmc_pending_tx_buf[ETH_TX_BUF_LEN];
uint mmc_pending_length = 0;

/* File descriptor of socket to transmit to. A value of -1 indicates no socket is connected. */
int eth_socket = -1;


void listen_for_socket(void)
{
    printf("%s instance starting up, going to be listening on %s:%d\n",
           get_instance_name(), ip_addr, ETH_PORT);

    int socket_in = eth_control_open(false);
    if (socket_in == -1) {
        assert(!"Failed to open a socket for listening!");
    }

    int ret = eth_control_bind(socket_in, PICOSERVER_ANY_ADDR_IPV4, ETH_PORT);
    if (ret) {
        assert(!"Failed to bind a socket for listening!");
    }

    ret = eth_control_listen(socket_in, 1);
    if (ret) {
        assert(!"Failed to listen for incoming connections!");
    }
}


void handle_picoserver_notification(void)
{
    picoserver_event_t server_event = eth_control_event_poll();
    int ret = 0;
    int socket = 0;
    uint16_t events = 0;
    char ip_string[16] = {0};

    while (server_event.num_events_left > 0 || server_event.events) {
        socket = server_event.socket_fd;
        events = server_event.events;
        if (events & PICOSERVER_CONN) {
            if (socket != 0) {
                picoserver_peer_t peer = eth_control_accept(socket);
                if (peer.result == -1) {
                    assert(!"Failed to accept a peer");
                }
                pico_ipv4_to_string(ip_string, peer.peer_addr);
                printf("%s: Connection established with %s on socket %d\n", get_instance_name(), ip_string, socket);
                /* Store the file descriptor of connected socket */
                eth_socket = peer.socket;
            }
        }
        if (events & PICOSERVER_CLOSE) {
            ret = eth_control_shutdown(socket, PICOSERVER_SHUT_RDWR);
            printf("%s: Connection closing on socket %d\n", get_instance_name(), socket);
            /* Socket no longer connected, clear the stored file descriptor */
            eth_socket = -1;
        }
        if (events & PICOSERVER_FIN) {
            printf("%s: Connection closed on socket %d\n", get_instance_name(), socket);
            /* Socket no longer connected, clear the stored file descriptor */
            eth_socket = -1;
        }
        if (events & PICOSERVER_ERR) {
            printf("%s: Error with socket %d, going to die\n", get_instance_name(), socket);
            assert(0);
        }
        server_event = eth_control_event_poll();
    }
}


void receive_data_from_crypto_component(void)
{
    /* Retrieve data from the circular buffer held in the dataport 'd'. As the dataport
     * is shared with another component we ensure only one component accesses it
     * at a time through use of a mutex accessed via the 'l_xxx' RPC calls. */

    l_lock(); // Start of critical section

    d_acquire();
    while(d->head != d->tail) {
        /* Buffer is not empty, read the next character from it */
        char encrypted_char = d->data[d->tail];
        d->tail += 1;
        /* Store the read character in the buffer of pending data to send over ethernet. */
        /* If the buffer is full then discard the character */
        if (eth_pending_length < ETH_TX_BUF_LEN) {
            eth_pending_tx_buf[eth_pending_length] = encrypted_char;
            eth_pending_length += 1;
        }
        /* Store the read character in the buffer of pending data to log to SD/MMC. */
        /* If the buffer is full then discard the character */
        if (mmc_pending_length < MMC_TX_BUF_LEN) {
            mmc_pending_tx_buf[mmc_pending_length] = encrypted_char;
            mmc_pending_length += 1;
        }
    }
    d_release();

    l_unlock(); // End of critical section
}


void transmit_pending_eth_buffer(void)
{
    /* Copy all keypresses stored in the 'eth_pending_tx_buf' buffer to
     * the ethernet send buffer ('eth_send_buf') then instruct the
     * PicoServer component holding the PicoTCP stack to transmit the
     * data */

    strncpy(eth_send_buf, eth_pending_tx_buf, eth_pending_length);
    eth_send_send(eth_socket, eth_pending_length, 0);
    /* All pending characters have now been sent. Clear the buffer */
    memset(eth_send_buf, 0, eth_pending_length);
    memset(eth_pending_tx_buf, 0, eth_pending_length);
    eth_pending_length = 0;
}


int transmitter_run(ps_io_ops_t *io_ops)
{
    /* Listen for connections on the ethernet socket we wish to transmit to */
    listen_for_socket();

    /* Start the U-Boot driver library */
    const char *const_reg_paths[] = REG_PATHS;
    const char *const_dev_paths[] = DEV_PATHS;
    assert(!initialise_uboot_drivers(
        /* Provide the platform support IO operations */
        io_ops,
        /* List the device tree paths that need to be memory mapped */
        const_reg_paths, REG_PATH_COUNT,
        /* List the device tree paths for the devices */
        const_dev_paths, DEV_PATH_COUNT));

    run_uboot_command("mmc info");
    run_uboot_command("part list mmc 0");
    run_uboot_command("fatls mmc 0");

    /* Now poll for events and handle them */
    bool idle_cycle;
    seL4_Word badge;

    while(1) {
        idle_cycle = true;

        /* Process notification of receipt of encrypted characters */
        if (e_poll()) {
            idle_cycle = false;
            receive_data_from_crypto_component();
        }

        /* Send any received encrypted characters to the socket (if connected) */
        if (eth_socket >= 0 && eth_pending_length > 0) {
            idle_cycle = false;
            transmit_pending_eth_buffer();
        }

        /* Process notification of any events from the PicoTCP stack */
        seL4_Poll(eth_control_notification(), &badge);
        if (badge) {
            idle_cycle = false;
            handle_picoserver_notification();
        }

        /* Sleep on idle cycles to prevent busy looping */
        if (idle_cycle) {
            ps_mdelay(10);
        }
    }

    return 0;
}

CAMKES_POST_INIT_MODULE_DEFINE(transmitter_run_, transmitter_run);
