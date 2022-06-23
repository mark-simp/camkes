/*
 * Copyright 2022, Capgemini Engineering
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <camkes.h>
#include <string.h>

/* Encryption routine. For the purposes of the demo we use "rot 13" */
char rot_13(char src)
{
    char result;

    // Only handle alphabet characters
    if((src >= 97 && src <= 122) || (src >= 65 && src <= 90)) {
        if(src > 109 || (src > 77 && src < 91)) {
            // Characters that wrap around to the start of the alphabet
            result = src - 13;
        } else {
            // Characters that can be incremented
            result = src + 13;
        }
    } else {
        // Not an alphabet character, leave it unchanged
        result = src;
    }

    return result;
}

void l_lock(void)
{
    int error = m_lock();
    if (error != 0)
        printf("Crypto: Error when locking mutex\n");
}

void l_unlock(void)
{
    int error = m_unlock();
    if (error != 0)
        printf("Crypto: Error when unlocking mutex\n");
}

void b_handle_character(char c)
{
    char encrypted_char = rot_13(c);

    /* Add data to the circular buffer held in the dataport. As the dataport is
     * shared with another component we ensure that only one component accesses it
     * at the time through use of a mutex */

    l_lock(); // Start of critical section

    d_acquire();
    if ((char)(d->head + 1) == d->tail) {
        /* Buffer is full, discard the character */
        printf("Crypto: Dataport buffer is full, discarding character\n");
    } else {
        d->data[d->head] = encrypted_char;
        d->head += 1;
    }
    d_release();

    l_unlock(); // End of critical section

    /* Emit a notification that data has been written to the buffer */
    e_emit();
}
