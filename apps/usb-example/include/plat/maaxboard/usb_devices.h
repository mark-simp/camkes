/*
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#define HARDWARE_USB_INTERFACES                                             \
    consumes Dummy usb_1;                                                   \
    consumes Dummy usb_2;                                                   \
    emits Dummy dummy_source;

#define HARDWARE_USB_COMPOSITION                                            \
    connection seL4DTBHardware usb_1_conn(from dummy_source, to usb_1);     \
    connection seL4DTBHardware usb_2_conn(from dummy_source, to usb_2);

#define HARDWARE_USB_CONFIGURATION                                          \
    usb_1.dtb = dtb({ "path" : "/soc@0/usb@38100000" });                    \
    usb_1.generate_interrupts = 1;                                          \
    usb_2.dtb = dtb({ "path" : "/soc@0/usb@38200000" });                    \
    usb_2.generate_interrupts = 1;
