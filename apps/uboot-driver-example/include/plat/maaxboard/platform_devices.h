/*
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#define TIMER_PATH      "/soc@0/bus@30400000/timer@306a0000"
#define CCM_PATH        "/soc@0/bus@30000000/clock-controller@30380000"
#define SYSCON_PATH     "/soc@0/bus@30000000/syscon@30360000"
#define IOMUXC_PATH     "/soc@0/bus@30000000/iomuxc@30330000"
#define USB_2_PATH      "/soc@0/usb@38200000"
#define USB_PHY_2_PATH  "/soc@0/usb-phy@382f0040"
#define MMC_PATH        "/soc@0/bus@30800000/mmc@30b40000"

#define DEVICE_PATHS {                                                          \
    USB_2_PATH,                                                                 \
    USB_PHY_2_PATH,                                                             \
    MMC_PATH,                                                                   \
    TIMER_PATH,                                                                 \
    CCM_PATH,                                                                   \
    SYSCON_PATH,                                                                \
    IOMUXC_PATH};
#define DEVICE_PATHS_LENGTH 7

#define HARDWARE_INTERFACES                                                     \
    consumes Dummy usb_2;                                                       \
    consumes Dummy usb_phy_2;                                                   \
    consumes Dummy mmc;                                                         \
    consumes Dummy timer;                                                       \
    consumes Dummy ccm;                                                         \
    consumes Dummy syscon;                                                      \
    consumes Dummy iomuxc;                                                      \
    emits Dummy dummy_source;

#define HARDWARE_COMPOSITION                                                    \
    connection seL4DTBHardware usb_2_conn(from dummy_source, to usb_2);         \
    connection seL4DTBHardware usb_phy_2_conn(from dummy_source, to usb_phy_2); \
    connection seL4DTBHardware mmc_conn(from dummy_source, to mmc);             \
    connection seL4DTBHardware timer_conn(from dummy_source, to timer);         \
    connection seL4DTBHardware ccm_conn(from dummy_source, to ccm);             \
    connection seL4DTBHardware syscon_conn(from dummy_source, to syscon);       \
    connection seL4DTBHardware iomuxc_conn(from dummy_source, to iomuxc);

#define HARDWARE_CONFIGURATION                                                  \
    usb_2.dtb     = dtb({ "path" : USB_2_PATH });                               \
    usb_phy_2.dtb = dtb({ "path" : USB_PHY_2_PATH });                           \
    mmc.dtb       = dtb({ "path" : MMC_PATH });                                 \
    timer.dtb     = dtb({ "path" : TIMER_PATH });                               \
    ccm.dtb       = dtb({ "path" : CCM_PATH });                                 \
    syscon.dtb    = dtb({ "path" : SYSCON_PATH });                              \
    iomuxc.dtb    = dtb({ "path" : IOMUXC_PATH });
