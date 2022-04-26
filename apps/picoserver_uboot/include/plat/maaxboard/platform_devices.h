/*
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#define TIMER_PATH      "/soc@0/bus@30400000/timer@306a0000"
#define CCM_PATH        "/soc@0/bus@30000000/clock-controller@30380000"
#define IOMUXC_PATH     "/soc@0/bus@30000000/iomuxc@30330000"
#define OCOTP_PATH      "/soc@0/bus@30000000/ocotp-ctrl@30350000"
#define SYSCON_PATH     "/soc@0/bus@30000000/syscon@30360000"
#define ETH_PATH        "/soc@0/bus@30800000/ethernet@30be0000"
#define GPIO_PATH       "/soc@0/bus@30000000/gpio@30200000"

#define DEVICE_PATHS {                                                          \
    ETH_PATH,                                                                   \
    TIMER_PATH,                                                                 \
    CCM_PATH,                                                                   \
    OCOTP_PATH,                                                                 \
    SYSCON_PATH,                                                                \
    IOMUXC_PATH,                                                                \
    GPIO_PATH};
#define DEVICE_PATHS_LENGTH 7

#define HARDWARE_INTERFACES                                                     \
    consumes Dummy eth;                                                         \
    consumes Dummy timer;                                                       \
    consumes Dummy ccm;                                                         \
    consumes Dummy iomuxc;                                                      \
    consumes Dummy ocotp;                                                       \
    consumes Dummy syscon;                                                      \
    consumes Dummy gpio;                                                        \
    emits Dummy dummy_source;

#define HARDWARE_COMPOSITION                                                    \
    connection seL4DTBHardware eth_conn(from dummy_source, to eth);             \
    connection seL4DTBHardware timer_conn(from dummy_source, to timer);         \
    connection seL4DTBHardware ccm_conn(from dummy_source, to ccm);             \
    connection seL4DTBHardware syscon_conn(from dummy_source, to syscon);       \
    connection seL4DTBHardware ocotp_conn(from dummy_source, to ocotp);         \
    connection seL4DTBHardware iomuxc_conn(from dummy_source, to iomuxc);       \
    connection seL4DTBHardware gpio_conn(from dummy_source, to gpio);

#define HARDWARE_CONFIGURATION                                                  \
    eth.dtb       = dtb({ "path" : ETH_PATH });                                 \
    timer.dtb     = dtb({ "path" : TIMER_PATH });                               \
    ccm.dtb       = dtb({ "path" : CCM_PATH });                                 \
    syscon.dtb    = dtb({ "path" : SYSCON_PATH });                              \
    ocotp.dtb     = dtb({ "path" : OCOTP_PATH });                               \
    iomuxc.dtb    = dtb({ "path" : IOMUXC_PATH });                              \
    gpio.dtb      = dtb({ "path" : GPIO_PATH });
