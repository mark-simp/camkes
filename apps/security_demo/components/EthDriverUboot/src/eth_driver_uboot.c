/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2022, Capgemini Engineering
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <camkes.h>
#include <platsupport/delay.h>

#include <uboot_drivers.h>
#include <eth_platform_devices.h>

#define ETHIF_TX_FAILED -1
#define ETHIF_TX_COMPLETE 1

#define CLIENT_RX_BUFS 128

#define BUF_SIZE 2048

typedef struct rx_frame {
    unsigned char buf[BUF_SIZE];
    int len;
} rx_frame_t;

typedef struct client {
    /* Keeps track of the head of the queue */
    int pending_rx_head;
    /* Keeps track of the tail of the queue */
    int pending_rx_tail;
    /*
     * This is a cyclic queue of RX buffers pending to be read by a client,
     * the head represents the first buffer in the queue, and the tail the last
     */
    rx_frame_t pending_rx[CLIENT_RX_BUFS];

    /* MAC address for this client */
    uint8_t mac[6];

    /* Badge for this client */
    seL4_Word client_id;

    /* Dataport for this client */
    unsigned char *dataport;
} client_t;

static int num_clients = 0;
static client_t *clients = NULL;

static int done_init = 0;

/* Functions provided by the Ethdriver template */
void client_emit(unsigned int client_id);
unsigned int client_get_sender_id(void);
unsigned int client_num_badges(void);
seL4_Word client_enumerate_badge(unsigned int i);
void *client_buf(seL4_Word client_id);
bool client_has_mac(unsigned int client_id);
void client_get_mac(unsigned int client_id, uint8_t *mac);

static client_t *detect_client(void *buf, unsigned int len)
{
    if (len < 6) {
        return NULL;
    }
    for (int i = 0; i < num_clients; i++) {
        if (memcmp(clients[i].mac, buf, 6) == 0) {
            return &clients[i];
        }
    }
    return NULL;
}

static int is_broadcast(void *buf, unsigned int len)
{
    static uint8_t broadcast[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    if (len < 6) {
        return 0;
    }
    if (memcmp(buf, broadcast, 6) == 0) {
        return 1;
    }
    return 0;
}

static int is_multicast(void *buf, unsigned int len)
{
    /* the dest address is in the IP header (16 bytes in), which is located after the
            ethernet header. the dest address itself is a standard 4byte IP address */
    const int eth_header_len = 14;
    const int ip_hdr_dest_offset = 16;
    if (len < eth_header_len + ip_hdr_dest_offset + 4) {
        return 0;
    }
    // Read out a copy of the IP address so that it is correctly aligned
    uint32_t addr;
    // TODO Find out why ARM memcpy faults on unaligned addresses
    //memcpy(&addr, ((uintptr_t)buf) + eth_header_len + ip_hdr_dest_offset, 4);
    for (int i = 0; i < 4; i++) {
        ((char *)&addr)[i] = ((char *)(buf + eth_header_len + ip_hdr_dest_offset))[i];
    }
    /* Multicast addresses start with bit pattern 1110, which after accounting for
       network byte ordering is 0xe0 */
    if ((addr & 0xf0) == 0xe0) {
        return 1;
    }
    return 0;
}

/** If eth frames have been received by the driver, copy the frames into the
 * associated client(s) buffer (pending_rx). Then copy a single frame (if any
 * are available) into the dataport of the caller of this function.
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
    if (!done_init) {
        *len = 0;
        return -1;
    }

    unsigned char *packet;

    /* Read all received frames from the driver and copy them into the
     * client->pending_rx buffer for the relevant client(s) */
    while (1) {
        /* Read the next frame */
        int ret = uboot_eth_receive(&packet);

        /* Exit the loop if there's no frame */
        if (ret <= 0) {
            break;
        }

        /* Discard any packets that are too long */
        if (ret > BUF_SIZE) {
            uboot_eth_free_packet(&packet);
            break;
        }

        /* Copy the received frame into the client buffers. Silently drop
         * frames if the buffers are full */
        client_t *client = detect_client(packet, ret);
        if (!client) {
            if (is_broadcast(packet, ret) || is_multicast(packet, ret)) {
                /* in a broadcast duplicate this buffer for every client */
                for (int i = 0; i < num_clients; i++) {
                    client = &clients[i];
                    if ((client->pending_rx_head + 1) % CLIENT_RX_BUFS != client->pending_rx_tail) {
                        memcpy(client->pending_rx[client->pending_rx_head].buf, packet, ret);
                        client->pending_rx[client->pending_rx_head].len = ret;
                        client->pending_rx_head = (client->pending_rx_head + 1) % CLIENT_RX_BUFS;
                    }
                }
            }
        } else {
            if ((client->pending_rx_head + 1) % CLIENT_RX_BUFS != client->pending_rx_tail) {
                memcpy(client->pending_rx[client->pending_rx_head].buf, packet, ret);
                client->pending_rx[client->pending_rx_head].len = ret;
                client->pending_rx_head = (client->pending_rx_head + 1) % CLIENT_RX_BUFS;
            }
        }

        /* Free the received packet from the driver's buffer */
        uboot_eth_free_packet(&packet);
    }

    /* Send the next frame (if any) for this client */
    int id = client_get_sender_id();
    client_t *client = NULL;
    for (int i = 0; i < num_clients; i++) {
        if (clients[i].client_id == id) {
            client = &clients[i];
        }
    }
    assert(client);

    /* Return a negative result if there are no frames available for this client */
    if (client->pending_rx_head == client->pending_rx_tail) {
        client_emit(client->client_id);
        *len = 0;
        return -1;
    }

    rx_frame_t rx = client->pending_rx[client->pending_rx_tail];
    memcpy(client->dataport, rx.buf, rx.len);
    *len = rx.len;
    client->pending_rx_tail = (client->pending_rx_tail + 1) % CLIENT_RX_BUFS;
    if (client->pending_rx_tail == client->pending_rx_head) {
        client_emit(client->client_id);
        return 0;
    } else {
        return 1;
    }
}

int client_tx(int len)
{
    if (!done_init) {
        return -1;
    }
    if (len > BUF_SIZE) {
        len = BUF_SIZE;
    }
    if (len < 12) {
        return -1;
    }

    int err = ETHIF_TX_COMPLETE;
    int id = client_get_sender_id();
    client_t *client = NULL;
    for (int i = 0; i < num_clients; i++) {
        if (clients[i].client_id == id) {
            client = &clients[i];
        }
    }
    assert(client);

    /* transmit */
    if (0 != uboot_eth_send(client->dataport, len)) {
        return ETHIF_TX_FAILED;
    } else {
        return ETHIF_TX_COMPLETE;
    }
}

void client_mac(uint8_t *b1, uint8_t *b2, uint8_t *b3, uint8_t *b4, uint8_t *b5, uint8_t *b6)
{
    int id = client_get_sender_id();
    client_t *client = NULL;
    for (int i = 0; i < num_clients; i++) {
        if (clients[i].client_id == id) {
            client = &clients[i];
        }
    }
    assert(client);
    assert(done_init);
    *b1 = client->mac[0];
    *b2 = client->mac[1];
    *b3 = client->mac[2];
    *b4 = client->mac[3];
    *b5 = client->mac[4];
    *b6 = client->mac[5];
}

int server_init(ps_io_ops_t *io_ops)
{
    printf("Starting U-Boot driver library\n");

    /* Start the U-Boot driver library containing the ethernet driver */
    const char *const_reg_paths[] = REG_PATHS;
    const char *const_dev_paths[] = DEV_PATHS;
    int error = initialise_uboot_drivers(
        /* Provide the platform support IO operations */
        io_ops,
        /* List the device tree paths that need to be memory mapped */
        const_reg_paths, REG_PATH_COUNT,
        /* List the device tree paths for the devices */
        const_dev_paths, DEV_PATH_COUNT);
    if (error) {
        ZF_LOGF("Unable to find an ethernet device");
    }

    /* Initialise ethernet */
    error = uboot_eth_init();
    if (error) {
        ZF_LOGF("Unable to initialise ethernet");
    }

    /* Preallocate buffers */
    num_clients = client_num_badges();
    clients = calloc(num_clients, sizeof(client_t));
    for (int client = 0; client < num_clients; client++) {
        clients[client].client_id = client_enumerate_badge(client);
        clients[client].dataport = client_buf(clients[client].client_id);
        clients[client].pending_rx_head = 0;
        clients[client].pending_rx_tail = 0;
    }

    uint8_t hw_mac[6];
    memcpy(hw_mac, uboot_eth_get_ethaddr(), 6);

    int num_defaults = 0;
    for (int client = 0; client < num_clients; client++) {
        if (client_has_mac(clients[client].client_id)) {
            client_get_mac(clients[client].client_id, clients[client].mac);
        } else {
            ++num_defaults;
            memcpy(clients[client].mac, hw_mac, 6);
            ZF_LOGF_IF((num_defaults > 1), "Should not have 2 clients with the same MAC address");
        }
    }

    done_init = 1;

    for (int client = 0; client < num_clients; client++) {
        client_emit(clients[client].client_id);
    }

    return 0;
}

CAMKES_POST_INIT_MODULE_DEFINE(ethdriver_run, server_init);
