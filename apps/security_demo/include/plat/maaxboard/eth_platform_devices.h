/*
 * Copyright 2022, Capgemini Engineering
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

/* List the set of device tree paths that include the 'reg' entries
 * for memory regions that will need to be mapped */

#define REG_TIMER_PATH      "/soc@0/bus@30400000/timer@306a0000"
#define REG_CCM_PATH        "/soc@0/bus@30000000/clock-controller@30380000"
#define REG_IOMUXC_PATH     "/soc@0/bus@30000000/iomuxc@30330000"
#define REG_OCOTP_PATH      "/soc@0/bus@30000000/ocotp-ctrl@30350000"
#define REG_SYSCON_PATH     "/soc@0/bus@30000000/syscon@30360000"
#define REG_ETH_PATH        "/soc@0/bus@30800000/ethernet@30be0000"

#define REG_PATH_COUNT 6

#define REG_PATHS {                                                             \
    REG_ETH_PATH,                                                               \
    REG_TIMER_PATH,                                                             \
    REG_CCM_PATH,                                                               \
    REG_OCOTP_PATH,                                                             \
    REG_SYSCON_PATH,                                                            \
    REG_IOMUXC_PATH,                                                            \
    };

/* List the set of device tree paths for the devices we wish to access.
 * Note these need ot be the root nodes of each device which the
 * the library supplies a driver */

#define DEV_TIMER_PATH      REG_TIMER_PATH
#define DEV_CCM_PATH        REG_CCM_PATH
#define DEV_IOMUXC_PATH     REG_IOMUXC_PATH
#define DEV_OCOTP_PATH      REG_OCOTP_PATH
#define DEV_SYSCON_PATH     REG_SYSCON_PATH
#define DEV_ETH_PATH        REG_ETH_PATH
#define DEV_CLK_1_PATH      "/clock-ckil"
#define DEV_CLK_2_PATH      "/clock-osc-25m"
#define DEV_CLK_3_PATH      "/clock-osc-27m"
#define DEV_CLK_4_PATH      "/clock-ext1"
#define DEV_CLK_5_PATH      "/clock-ext2"
#define DEV_CLK_6_PATH      "/clock-ext3"
#define DEV_CLK_7_PATH      "/clock-ext4"

#define DEV_PATH_COUNT 13

#define DEV_PATHS {                                                             \
    DEV_ETH_PATH,                                                               \
    DEV_TIMER_PATH,                                                             \
    DEV_CCM_PATH,                                                               \
    DEV_OCOTP_PATH,                                                             \
    DEV_SYSCON_PATH,                                                            \
    DEV_IOMUXC_PATH,                                                            \
    DEV_CLK_1_PATH,                                                             \
    DEV_CLK_2_PATH,                                                             \
    DEV_CLK_3_PATH,                                                             \
    DEV_CLK_4_PATH,                                                             \
    DEV_CLK_5_PATH,                                                             \
    DEV_CLK_6_PATH,                                                             \
    DEV_CLK_7_PATH                                                              \
    };

/* Provide the hardware settings for CAmkES. Note that we only need to inform
 * CAmkES with the devices with memory mapped regions, i.e. the REG_xxx
 * devices. */

#define HARDWARE_INTERFACES                                                     \
    consumes Dummy eth;                                                         \
    consumes Dummy timer;                                                       \
    consumes Dummy ccm;                                                         \
    consumes Dummy iomuxc;                                                      \
    consumes Dummy ocotp;                                                       \
    consumes Dummy syscon;                                                      \
    emits Dummy dummy_source;

#define HARDWARE_COMPOSITION                                                    \
    connection seL4DTBHardware eth_conn(from dummy_source, to eth);             \
    connection seL4DTBHardware timer_conn(from dummy_source, to timer);         \
    connection seL4DTBHardware ccm_conn(from dummy_source, to ccm);             \
    connection seL4DTBHardware syscon_conn(from dummy_source, to syscon);       \
    connection seL4DTBHardware ocotp_conn(from dummy_source, to ocotp);         \
    connection seL4DTBHardware iomuxc_conn(from dummy_source, to iomuxc);

#define HARDWARE_CONFIGURATION                                                  \
    eth.dtb       = dtb({ "path" : REG_ETH_PATH });                             \
    timer.dtb     = dtb({ "path" : REG_TIMER_PATH });                           \
    ccm.dtb       = dtb({ "path" : REG_CCM_PATH });                             \
    syscon.dtb    = dtb({ "path" : REG_SYSCON_PATH });                          \
    ocotp.dtb     = dtb({ "path" : REG_OCOTP_PATH });                           \
    iomuxc.dtb    = dtb({ "path" : REG_IOMUXC_PATH });
