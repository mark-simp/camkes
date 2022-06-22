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

void b__init(void)
{
}

void b_handle_character(char c)
{
    printf("Crypto received char '%c', encrypted to '%c'\n", c, rot_13(c));
}
