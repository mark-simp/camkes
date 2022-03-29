/*
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#define GPT_1_PATH  "/gpt@302d0000"
#define USB_1_PATH  "/soc@0/usb@38100000"
#define USB_2_PATH  "/soc@0/usb@38200000"
#define PHY_1_PATH  "/soc@0/usb-phy@381f0040"
#define PHY_2_PATH  "/soc@0/usb-phy@382f0040"

#define DEVICE_PATHS {                                                      \
    USB_1_PATH,                                                             \
    USB_2_PATH,                                                             \
    PHY_1_PATH,                                                             \
    PHY_2_PATH};
#define DEVICE_PATHS_LENGTH 4

#define HARDWARE_USB_INTERFACES                                             \
    consumes Dummy gpt_1;                                                   \
    consumes Dummy usb_1;                                                   \
    consumes Dummy usb_2;                                                   \
    consumes Dummy phy_1;                                                   \
    consumes Dummy phy_2;                                                   \
    emits Dummy dummy_source;

#define HARDWARE_USB_COMPOSITION                                            \
    connection seL4DTBHardware gpt_1_conn(from dummy_source, to gpt_1);     \
    connection seL4DTBHardware usb_1_conn(from dummy_source, to usb_1);     \
    connection seL4DTBHardware usb_2_conn(from dummy_source, to usb_2);     \
    connection seL4DTBHardware phy_1_conn(from dummy_source, to phy_1);     \
    connection seL4DTBHardware phy_2_conn(from dummy_source, to phy_2);

#define HARDWARE_USB_CONFIGURATION                                          \
    gpt_1.dtb = dtb({ "path" : GPT_1_PATH });                               \
    gpt_1.generate_interrupts = 1;                                          \
    usb_1.dtb = dtb({ "path" : USB_1_PATH });                               \
    usb_1.generate_interrupts = 1;                                          \
    usb_2.dtb = dtb({ "path" : USB_2_PATH });                               \
    usb_2.generate_interrupts = 1;                                          \
    phy_1.dtb = dtb({ "path" : PHY_1_PATH });                               \
    phy_2.dtb = dtb({ "path" : PHY_2_PATH });
