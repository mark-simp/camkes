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

void circular_buffer_lock_lock(void)
{
    int error = circular_buffer_mutex_lock();
    if (error != 0)
        printf("Crypto: Error when locking mutex\n");
}

void circular_buffer_lock_unlock(void)
{
    int error = circular_buffer_mutex_unlock();
    if (error != 0)
        printf("Crypto: Error when unlocking mutex\n");
}

void clear_text_handle_character(char c)
{
    char encrypted_char = rot_13(c);

    /* Add data to the circular buffer held in the dataport. As the dataport is
     * shared with another component we ensure that only one component accesses it
     * at the time through use of a mutex */

    circular_buffer_lock_lock(); // Start of critical section

    circular_buffer_data_acquire();
    if ((char)(circular_buffer_data->head + 1) == circular_buffer_data->tail) {
        /* Buffer is full, discard the character */
        printf("Crypto: Dataport buffer is full, discarding character\n");
    } else {
        circular_buffer_data->data[circular_buffer_data->head] = encrypted_char;
        circular_buffer_data->head += 1;
    }
    circular_buffer_data_release();

    circular_buffer_lock_unlock(); // End of critical section

    /* Emit a notification that data has been written to the buffer */
    circular_buffer_notify_emit();
}
