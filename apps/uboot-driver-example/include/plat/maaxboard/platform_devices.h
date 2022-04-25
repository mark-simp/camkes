/*
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#define TIMER_PATH      "/soc@0/bus@30400000/timer@306a0000"
#define CCM_PATH        "/soc@0/bus@30000000/clock-controller@30380000"
#define IOMUXC_PATH     "/soc@0/bus@30000000/iomuxc@30330000"
#define OCOTP_PATH      "/soc@0/bus@30000000/ocotp-ctrl@30350000"
#define SYSCON_PATH     "/soc@0/bus@30000000/syscon@30360000"
#define USB_2_PATH      "/soc@0/usb@38200000"
#define USB_PHY_2_PATH  "/soc@0/usb-phy@382f0040"
#define MMC_PATH        "/soc@0/bus@30800000/mmc@30b40000"
#define ETH_PATH        "/soc@0/bus@30800000/ethernet@30be0000"
#define GPIO_1_PATH     "/soc@0/bus@30000000/gpio@30200000"
#define GPIO_2_PATH     "/soc@0/bus@30000000/gpio@30210000"
#define GPIO_3_PATH     "/soc@0/bus@30000000/gpio@30220000"
#define GPIO_4_PATH     "/soc@0/bus@30000000/gpio@30230000"
#define GPIO_5_PATH     "/soc@0/bus@30000000/gpio@30240000"
#define LEDS_PATH       "/leds"

#define DEVICE_PATHS {                                                          \
    USB_2_PATH,                                                                 \
    USB_PHY_2_PATH,                                                             \
    MMC_PATH,                                                                   \
    ETH_PATH,                                                                   \
    TIMER_PATH,                                                                 \
    CCM_PATH,                                                                   \
    OCOTP_PATH,                                                                 \
    SYSCON_PATH,                                                                \
    IOMUXC_PATH,                                                                \
    GPIO_1_PATH,                                                                \
    GPIO_2_PATH,                                                                \
    GPIO_3_PATH,                                                                \
    GPIO_4_PATH,                                                                \
    GPIO_5_PATH,                                                                \
    LEDS_PATH};
#define DEVICE_PATHS_LENGTH 15

#define HARDWARE_INTERFACES                                                     \
    consumes Dummy usb_2;                                                       \
    consumes Dummy usb_phy_2;                                                   \
    consumes Dummy mmc;                                                         \
    consumes Dummy eth;                                                         \
    consumes Dummy timer;                                                       \
    consumes Dummy ccm;                                                         \
    consumes Dummy iomuxc;                                                      \
    consumes Dummy ocotp;                                                       \
    consumes Dummy syscon;                                                      \
    consumes Dummy gpio_1;                                                      \
    consumes Dummy gpio_2;                                                      \
    consumes Dummy gpio_3;                                                      \
    consumes Dummy gpio_4;                                                      \
    consumes Dummy gpio_5;                                                      \
    emits Dummy dummy_source;

#define HARDWARE_COMPOSITION                                                    \
    connection seL4DTBHardware usb_2_conn(from dummy_source, to usb_2);         \
    connection seL4DTBHardware usb_phy_2_conn(from dummy_source, to usb_phy_2); \
    connection seL4DTBHardware mmc_conn(from dummy_source, to mmc);             \
    connection seL4DTBHardware eth_conn(from dummy_source, to eth);             \
    connection seL4DTBHardware timer_conn(from dummy_source, to timer);         \
    connection seL4DTBHardware ccm_conn(from dummy_source, to ccm);             \
    connection seL4DTBHardware syscon_conn(from dummy_source, to syscon);       \
    connection seL4DTBHardware ocotp_conn(from dummy_source, to ocotp);         \
    connection seL4DTBHardware iomuxc_conn(from dummy_source, to iomuxc);       \
    connection seL4DTBHardware gpio_1_conn(from dummy_source, to gpio_1);       \
    connection seL4DTBHardware gpio_2_conn(from dummy_source, to gpio_2);       \
    connection seL4DTBHardware gpio_3_conn(from dummy_source, to gpio_3);       \
    connection seL4DTBHardware gpio_4_conn(from dummy_source, to gpio_4);       \
    connection seL4DTBHardware gpio_5_conn(from dummy_source, to gpio_5);

#define HARDWARE_CONFIGURATION                                                  \
    usb_2.dtb     = dtb({ "path" : USB_2_PATH });                               \
    usb_phy_2.dtb = dtb({ "path" : USB_PHY_2_PATH });                           \
    mmc.dtb       = dtb({ "path" : MMC_PATH });                                 \
    eth.dtb       = dtb({ "path" : ETH_PATH });                                 \
    timer.dtb     = dtb({ "path" : TIMER_PATH });                               \
    ccm.dtb       = dtb({ "path" : CCM_PATH });                                 \
    syscon.dtb    = dtb({ "path" : SYSCON_PATH });                              \
    ocotp.dtb     = dtb({ "path" : OCOTP_PATH });                               \
    iomuxc.dtb    = dtb({ "path" : IOMUXC_PATH });                              \
    gpio_1.dtb    = dtb({ "path" : GPIO_1_PATH });                              \
    gpio_2.dtb    = dtb({ "path" : GPIO_2_PATH });                              \
    gpio_3.dtb    = dtb({ "path" : GPIO_3_PATH });                              \
    gpio_4.dtb    = dtb({ "path" : GPIO_4_PATH });                              \
    gpio_5.dtb    = dtb({ "path" : GPIO_5_PATH });
