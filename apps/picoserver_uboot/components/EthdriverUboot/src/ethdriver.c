/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <camkes.h>
#include <platsupport/delay.h>

#include <uboot_drivers.h>
#include <platform_devices.h>

#define RX_BUFS 256

#define CLIENT_RX_BUFS 128
#define CLIENT_TX_BUFS 128

#define BUF_SIZE 2048


typedef struct eth_buf {
    void *buf;
} eth_buf_t;

typedef struct rx_frame {
    eth_buf_t *buf; // Clients share a pool of RX frames
    int len;
    int client;
} rx_frame_t;

typedef struct tx_frame {
    eth_buf_t buf; // Each client has a pool of TX frames
    int len;
    int client;
} tx_frame_t;

typedef struct client {
    /* this flag indicates whether we or not we need to notify the client
     * if new data is received. We only notify once the client observes
     * the last packet */
    int should_notify;

    /* keeps track of the head of the queue */
    int pending_rx_head;
    /* keeps track of the tail of the queue */
    int pending_rx_tail;
    /*
     * this is a cyclic queue of RX buffers pending to be read by a client,
     * the head represents the first buffer in the queue, and the tail the last
     */
    rx_frame_t pending_rx[CLIENT_RX_BUFS];

    /* keeps track of how many TX buffers are in use */
    int num_tx;
    /* the allocated TX buffers for the client */
    tx_frame_t tx_mem[CLIENT_TX_BUFS];
    /*
     * this represents the pool of buffers that can be used for TX,
     * this array is a sliding array in that num_tx acts a pointer to
     * separate between buffers that are in use and buffers that are
     * not in use. E.g. 'o' = free, 'x' = in use
     *  -------------------------------------
     *  | o | o | o | o | o | o | x | x | x |
     *  -------------------------------------
     *                          ^
     *                        num_tx
     */
    tx_frame_t *pending_tx[CLIENT_TX_BUFS];

    /* mac address for this client */
    uint8_t mac[6];

    /* Badge for this client */
    seL4_Word client_id;

    /* dataport for this client */
    void *dataport;
} client_t;

static int num_clients = 0;
static client_t *clients = NULL;

static int num_rx_bufs;
static eth_buf_t rx_bufs[RX_BUFS];
static eth_buf_t *rx_buf_pool[RX_BUFS];

static int done_init = 0;

/* Functions provided by the Ethdriver template */
void client_emit(unsigned int client_id);
unsigned int client_get_sender_id(void);
unsigned int client_num_badges(void);
seL4_Word client_enumerate_badge(unsigned int i);
void *client_buf(seL4_Word client_id);
bool client_has_mac(unsigned int client_id);
void client_get_mac(unsigned int client_id, uint8_t *mac);

static void eth_tx_complete(void *iface, void *cookie)
{
    tx_frame_t *buf = (tx_frame_t *)cookie;
    client_t *client = &clients[buf->client];
    client->pending_tx[client->num_tx] = buf;
    client->num_tx++;
}

static int eth_allocate_rx_buf(void *iface, size_t buf_size, void **cookie)
{
    if (buf_size > BUF_SIZE) {
        return 0;
    }
    if (num_rx_bufs == 0) {
        return 0;
    }
    num_rx_bufs--;
    *cookie = rx_buf_pool[num_rx_bufs];
    return 1;
}

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
    // the dest address is in the IP header (16 bytes in), which is located after the
    // ethernet header. the dest address itself is a standard 4byte IP address
    const int eth_header_len = 14;
    const int ip_hdr_dest_offset = 16;
    if (len < eth_header_len + ip_hdr_dest_offset + 4) {
        return 0;
    }
    // read out a copy of the IP address so that it is correctly aligned
    uint32_t addr;
    // TODO Find out why ARM memcpy faults on unaligned addresses
    //memcpy(&addr, ((uintptr_t)buf) + eth_header_len + ip_hdr_dest_offset, 4);
    for (int i = 0; i < 4; i++) {
        ((char *)&addr)[i] = ((char *)(buf + eth_header_len + ip_hdr_dest_offset))[i];
    }
    // multicast addresses start with bit pattern 1110, which after accounting for
    // network byte ordering is 0xe0
    if ((addr & 0xf0) == 0xe0) {
        return 1;
    }
    return 0;
}

static void give_client_buf(client_t *client, eth_buf_t *buf, unsigned int len)
{
    client->pending_rx[client->pending_rx_head] = (rx_frame_t) {
        buf, len, 0
    };
    client->pending_rx_head = (client->pending_rx_head + 1) % CLIENT_RX_BUFS;
    if (client->should_notify) {
        client_emit(client->client_id);
        client->should_notify = 0;
    }
}

static void eth_rx_complete(void *iface, unsigned int num_bufs, void **cookies, unsigned int *lens)
{
    /* insert filtering here. currently everything just goes to one client */
    if (num_bufs != 1) {
        goto error;
    }
    eth_buf_t *curr_buf = cookies[0];
    client_t *client = detect_client(curr_buf->buf, lens[0]);
    if (!client) {
        if (is_broadcast(curr_buf->buf, lens[0]) || is_multicast(curr_buf->buf, lens[0])) {
            /* in a broadcast duplicate this buffer for every other client, we will fallthrough
             * to give the buffer to client 0 at the end */
            for (int i = 1; i < num_clients; i++) {
                client = &clients[i];
                if ((client->pending_rx_head + 1) % CLIENT_RX_BUFS != client->pending_rx_tail) {
                    void *cookie;
                    int ret = eth_allocate_rx_buf(iface, lens[0], &cookie);
                    eth_buf_t *new_buf = cookie;
                    if (ret) {
                        memcpy(new_buf->buf, curr_buf->buf, lens[0]);
                        give_client_buf(client, new_buf, lens[0]);
                    }
                }
            }
        } else {
            goto error;
        }
        client = &clients[0];
    }
    if ((client->pending_rx_head + 1) % CLIENT_RX_BUFS == client->pending_rx_tail) {
        goto error;
    }
    give_client_buf(client, curr_buf, lens[0]);
    return;
error:
    /* abort and put all the bufs back */
    for (int i = 0; i < num_bufs; i++) {
        eth_buf_t *returned_buf = cookies[i];
        rx_buf_pool[num_rx_bufs] = returned_buf;
        num_rx_bufs++;
    }
}


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
    printf("client_mac called");

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
    const char *device_paths[] = DEVICE_PATHS
    int error = initialise_uboot_drivers(io_ops, device_paths, DEVICE_PATHS_LENGTH);
    if (error)
        ZF_LOGF("Unable to find an ethernet device");

    /* Initialise ethernet */
    error = uboot_eth_init();
    if (error)
        ZF_LOGF("Unable to initialise ethernet");

    // /* Wait for incoming data */
    // int ret;
    // unsigned char *packet;
    // while(1) {
    //     ret = uboot_eth_receive(&packet);
    //     if (ret > 0) {
    //         printf("Received packet @ %p: ", packet);
    //         for (int i = 0; i < ret; i++) {
    //             printf("%02x ", packet[i]);
    //         }
    //         printf("\n");

    //         uboot_eth_free_packet(&packet);
    //         continue;
    //     }
    //     else if (ret < 0)
    //         ZF_LOGE("eth_rx returned code %i", ret);

    //     /* Add a small delay to prevent busy looping */
    //     ps_mdelay(1);
    // }

    /* preallocate buffers */
    for (int i = 0; i < RX_BUFS; i++) {
        void *buf = malloc(BUF_SIZE);
        assert(buf);
        memset(buf, 0, BUF_SIZE);
        rx_bufs[num_rx_bufs] = (eth_buf_t) {
            .buf = buf
        };
        rx_buf_pool[num_rx_bufs] = &(rx_bufs[num_rx_bufs]);
        num_rx_bufs++;
    }
    num_clients = client_num_badges();
    clients = calloc(num_clients, sizeof(client_t));
    for (int client = 0; client < num_clients; client++) {
        clients[client].should_notify = 1;
        clients[client].client_id = client_enumerate_badge(client);
        clients[client].dataport = client_buf(clients[client].client_id);
        for (int i = 0; i < CLIENT_TX_BUFS; i++) {
            void *buf = malloc(BUF_SIZE);
            assert(buf);
            memset(buf, 0, BUF_SIZE);
            tx_frame_t *tx_buf = &clients[client].tx_mem[clients[client].num_tx];
            *tx_buf = (tx_frame_t) {
                .len = BUF_SIZE, .client = client
            };
            tx_buf->buf = (eth_buf_t) {
                .buf = buf
            };
            clients[client].pending_tx[clients[client].num_tx] = tx_buf;
            clients[client].num_tx++;
        }
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

    return 0;
}

CAMKES_POST_INIT_MODULE_DEFINE(ethdriver_run, server_init);