/*
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#define GPT_1_PATH  "/gpt@302d0000"
#define USB_1_PATH  "/soc@0/usb@38100000"
#define USB_2_PATH  "/soc@0/usb@38200000"

#define DEVICE_PATHS {                                                      \
    USB_1_PATH,                                                             \
    USB_2_PATH};
#define DEVICE_PATHS_LENGTH 2

#define HARDWARE_USB_INTERFACES                                             \
    consumes Dummy gpt_1;                                                   \
    consumes Dummy usb_1;                                                   \
    consumes Dummy usb_2;                                                   \
    emits Dummy dummy_source;

#define HARDWARE_USB_COMPOSITION                                            \
    connection seL4DTBHardware gpt_1_conn(from dummy_source, to gpt_1);     \
    connection seL4DTBHardware usb_1_conn(from dummy_source, to usb_1);     \
    connection seL4DTBHardware usb_2_conn(from dummy_source, to usb_2);

#define HARDWARE_USB_CONFIGURATION                                          \
    gpt_1.dtb = dtb({ "path" : GPT_1_PATH });                               \
    gpt_1.generate_interrupts = 1;                                          \
    usb_1.dtb = dtb({ "path" :USB_1_PATH });                                \
    usb_1.generate_interrupts = 1;                                          \
    usb_2.dtb = dtb({ "path" : USB_2_PATH });                               \
    usb_2.generate_interrupts = 1;
