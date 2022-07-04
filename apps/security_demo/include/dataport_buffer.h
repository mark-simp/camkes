/*
 * Copyright 2022, Capgemini Engineering
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef _DATAPORT_BUFFER_H_
#define _DATAPORT_BUFFER_H_

/* The following structure defines a circular buffer of 256 characters held within a
 * dataport shared between two components.
 *
 * 'data' is an array of characters.
 * 'tail' points to the oldest character in the buffer.
 * 'head' points to the next unused character in the buffer.
 *
 * When 'head' == 'tail' the buffer is empty (the initial state).
 *
 * When 'head' + 1 == 'tail' the buffer is full and no more data can be added to
 * the buffer (any further data will be discarded).
 *
 * 'data':  .............***************.............
 *          ^            ^              ^           ^
 *          |            |              |           |
 *          index 0      'tail'         'head'      index 255
 */

typedef struct {
    char data[256];
    char head;
    char tail;
} dataport_buffer_t;

#endif
