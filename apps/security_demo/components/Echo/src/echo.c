/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <camkes.h>
#include <autoconf.h>
#include <stdio.h>
#include <string.h>
#include <picoserver.h>
#include <assert.h>

#include <platsupport/delay.h>

#define ECHO_PORT 1234

extern void *echo_recv_buf;
extern void *echo_send_buf;

/* Hold the file descriptor of socket to transmit to. A value of -1 indicates no socket is connected. */
int eth_socket = -1;

seL4_CPtr echo_control_notification();

void listen_for_socket(void)
{
    printf("%s instance starting up, going to be listening on %s:%d\n",
           get_instance_name(), ip_addr, ECHO_PORT);

    int socket_in = echo_control_open(false);
    if (socket_in == -1) {
        assert(!"Failed to open a socket for listening!");
    }

    int ret = echo_control_bind(socket_in, PICOSERVER_ANY_ADDR_IPV4, ECHO_PORT);
    if (ret) {
        assert(!"Failed to bind a socket for listening!");
    }

    ret = echo_control_listen(socket_in, 1);
    if (ret) {
        assert(!"Failed to listen for incoming connections!");
    }
}

void handle_picoserver_notification(void)
{
    picoserver_event_t server_event = echo_control_event_poll();
    int ret = 0;
    int socket = 0;
    uint16_t events = 0;
    char ip_string[16] = {0};

    printf("Echo handling notification\n");

    while (server_event.num_events_left > 0 || server_event.events) {
        socket = server_event.socket_fd;
        events = server_event.events;
        if (events & PICOSERVER_CONN) {
            if (socket != 0) {
                picoserver_peer_t peer = echo_control_accept(socket);
                if (peer.result == -1) {
                    assert(!"Failed to accept a peer");
                }
                pico_ipv4_to_string(ip_string, peer.peer_addr);
                printf("%s: Connection established with %s on socket %d\n", get_instance_name(), ip_string, socket);
                /* Store the FD of connected socket */
                eth_socket = peer.socket;
            }
        }
        if (events & PICOSERVER_READ) {
            ret = echo_recv_recv(socket, 4096, 0);
            printf("%s: Received message of length %d --> %s\n", get_instance_name(), strlen(echo_recv_buf), echo_recv_buf);
            memset(echo_recv_buf, 0, 4096);
        }
        if (events & PICOSERVER_CLOSE) {
            ret = echo_control_shutdown(socket, PICOSERVER_SHUT_RDWR);
            printf("%s: Connection closing on socket %d\n", get_instance_name(), socket);
            /* Socket no longer connected, clear the FD */
            eth_socket = -1;
        }
        if (events & PICOSERVER_FIN) {
            printf("%s: Connection closed on socket %d\n", get_instance_name(), socket);
            /* Socket no longer connected, clear the FD */
            eth_socket = -1;
        }
        if (events & PICOSERVER_ERR) {
            printf("%s: Error with socket %d, going to die\n", get_instance_name(), socket);
            assert(0);
        }
        server_event = echo_control_event_poll();
    }
}

int run(void)
{
    /* Listen for connections on the docket we wish to transmit to */
    listen_for_socket();

    /* Now poll for events and handle them */

    seL4_Word badge;

    while (1) {
        seL4_Poll(echo_control_notification(), &badge);
        if (badge) {
            handle_picoserver_notification();
        } else {
            if (eth_socket >= 0) {
                strncpy(echo_send_buf, "Nothing to do, sleeping\n", 25);
                echo_send_send(eth_socket, 25, 0);
                memset(echo_send_buf, 0, 4096);
            }
            ps_mdelay(50);
        }
    }
}
