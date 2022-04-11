/*
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#define GPT_1_PATH  "/gpt@302d0000"
#define USB_1_PATH  "/soc@0/usb@38100000"
#define USB_2_PATH  "/soc@0/usb@38200000"
#define PHY_1_PATH  "/soc@0/usb-phy@381f0040"
#define PHY_2_PATH  "/soc@0/usb-phy@382f0040"
#define MMC_PATH    "/soc@0/bus@30800000/mmc@30b40000"
#define CCM_PATH    "/soc@0/bus@30000000/clock-controller@30380000"
#define SYSCON_PATH "/soc@0/bus@30000000/syscon@30360000"

#define DEVICE_PATHS {                                                      \
    USB_1_PATH,                                                             \
    USB_2_PATH,                                                             \
    PHY_1_PATH,                                                             \
    PHY_2_PATH,                                                             \
    MMC_PATH,                                                               \
    CCM_PATH,                                                               \
    SYSCON_PATH};
#define DEVICE_PATHS_LENGTH 7

#define HARDWARE_INTERFACES                                                 \
    consumes Dummy gpt_1;                                                   \
    consumes Dummy usb_1;                                                   \
    consumes Dummy usb_2;                                                   \
    consumes Dummy phy_1;                                                   \
    consumes Dummy phy_2;                                                   \
    consumes Dummy mmc;                                                     \
    consumes Dummy ccm;                                                     \
    consumes Dummy syscon;                                                     \
    emits Dummy dummy_source;

#define HARDWARE_COMPOSITION                                                \
    connection seL4DTBHardware gpt_1_conn(from dummy_source, to gpt_1);     \
    connection seL4DTBHardware usb_1_conn(from dummy_source, to usb_1);     \
    connection seL4DTBHardware usb_2_conn(from dummy_source, to usb_2);     \
    connection seL4DTBHardware phy_1_conn(from dummy_source, to phy_1);     \
    connection seL4DTBHardware phy_2_conn(from dummy_source, to phy_2);     \
    connection seL4DTBHardware mmc_conn(from dummy_source, to mmc);         \
    connection seL4DTBHardware ccm_conn(from dummy_source, to ccm);         \
    connection seL4DTBHardware syscon_conn(from dummy_source, to syscon);

#define HARDWARE_CONFIGURATION                                               \
    gpt_1.dtb  = dtb({ "path" : GPT_1_PATH });                               \
    gpt_1.generate_interrupts = 1;                                           \
    usb_1.dtb  = dtb({ "path" : USB_1_PATH });                               \
    usb_2.dtb  = dtb({ "path" : USB_2_PATH });                               \
    phy_1.dtb  = dtb({ "path" : PHY_1_PATH });                               \
    phy_2.dtb  = dtb({ "path" : PHY_2_PATH });                               \
    mmc.dtb    = dtb({ "path" : MMC_PATH });                                 \
    ccm.dtb    = dtb({ "path" : CCM_PATH });                                 \
    syscon.dtb = dtb({ "path" : SYSCON_PATH });
