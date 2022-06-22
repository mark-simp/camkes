/*
 * Copyright 2022, Capgemini Engineering
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <camkes.h>
#include <string.h>

#include <dataport_buffer.h>

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

void b__init(void)
{
}

void b_handle_character(char c)
{
    char encrypted_char = rot_13(c);

    printf("Crypto received char '%c', encrypted to '%c'\n", c, encrypted_char);

    printf("Dataport head = %i, tail = %i\n", d->head, d->tail);

    /* Add data to the circular buffer held in the dataport */

    if ((char)(d->head + 1) == d->tail) {
        /* Buffer is full, discard the character */
        printf("Crypto: Dataport buffer is full, discarding character\n");
    } else {
        d->data[d->head] = encrypted_char;
        d->head += 1;
    }

}
