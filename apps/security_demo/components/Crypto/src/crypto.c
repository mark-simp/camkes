/*
 * Copyright 2022, Capgemini Engineering
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <camkes.h>
#include <string.h>

void b__init(void)
{
}

void b_handle_character(char c)
{
    printf("Crypto received char '%c'\n", c);
}
