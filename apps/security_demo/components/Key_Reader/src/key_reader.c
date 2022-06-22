/*
 * Copyright 2022, Capgemini Engineering
 *
ß * SPDX-License-Identifier: BSD-2-Clause
 */

#include <camkes.h>
#include <stdio.h>
#include <stdlib.h>

int run()
{
    printf("Starting Key_Reader\n");

    a_handle_character('a');
    a_handle_character('b');
    a_handle_character('c');

    printf("Stopping Key_Reader\n");
    return 0;
}
