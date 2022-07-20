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
 * 'tail' index of the data array denoting the oldest character in the buffer.
 * 'head' index of the data array denoting the next unused character in the buffer.
 *
 * When 'head' == 'tail' the buffer is empty (the initial state).
 *
 * When ('head' + 1) mod 256 == 'tail' the buffer is full and no more data can be added
 * to the buffer (any further data will be discarded).
 *
 * 'data':  .............***************.............
 *          ^            ^              ^           ^
 *          |            |              |           |
 *          index 0      'tail'         'head'      index 255
 *
 * It should be noted that a buffer size of 256, and the types of the 'head' and 'tail'
 * indexes, have been deliberately chosen to allow the 'head' and 'tail' indexes to be
 * incremented by users of this buffer through use of a simple "head++"; we rely upon
 * type wrap-around of the indexes.
 *
 */

typedef struct {
    char data[256];
    char head;
    char tail;
} dataport_buffer_t;

#endif
