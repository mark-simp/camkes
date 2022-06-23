/*
 * Copyright 2022, Capgemini Engineering
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <camkes.h>
#include <stdio.h>
#include <stdlib.h>

int run(void) {

    while(1) {
        /* Wait for a notification that data has been written to the buffer */
        e_wait();

        /* Retrieve data from the circular buffer held in the dataport. As the dataport
         * is shared with another component we ensure that only one component accesses it
         * at the time through use of a mutex */

        l_lock(); // Start of critical section

        d_acquire();
        while(d->head != d->tail) {
            // Buffer is not empty, read the next character from it
            char encrypted_char = d->data[d->tail];
            d->tail += 1;
            printf("Transmitter: Read '%c' from dataport\n", encrypted_char);
        }
        d_release();

        l_unlock(); // End of critical section
    }

    return 0;
}
