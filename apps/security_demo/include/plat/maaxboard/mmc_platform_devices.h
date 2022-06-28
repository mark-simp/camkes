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
#define REG_SYSCON_PATH     "/soc@0/bus@30000000/syscon@30360000"
#define REG_IOMUXC_PATH     "/soc@0/bus@30000000/iomuxc@30330000"
#define REG_MMC_PATH        "/soc@0/bus@30800000/mmc@30b40000"

#define REG_PATH_COUNT 5

#define REG_PATHS {                                                             \
    REG_MMC_PATH,                                                               \
    REG_TIMER_PATH,                                                             \
    REG_CCM_PATH,                                                               \
    REG_SYSCON_PATH,                                                            \
    REG_IOMUXC_PATH,                                                            \
    };

/* List the set of device tree paths for the devices we wish to access.
 * Note these need ot be the root nodes of each device which the
 * the library supplies a driver */

#define DEV_TIMER_PATH      REG_TIMER_PATH
#define DEV_CCM_PATH        REG_CCM_PATH
#define DEV_SYSCON_PATH     REG_SYSCON_PATH
#define DEV_MMC_PATH        REG_MMC_PATH
#define DEV_IOMUXC_PATH     REG_IOMUXC_PATH

#define DEV_PATH_COUNT 5

#define DEV_PATHS {                                                             \
    DEV_MMC_PATH,                                                               \
    DEV_TIMER_PATH,                                                             \
    DEV_CCM_PATH,                                                               \
    DEV_SYSCON_PATH,                                                            \
    DEV_IOMUXC_PATH,                                                            \
};

/* Provide the hardware settings for CAmkES. Note that we only need to inform
 * CAmkES with the devices with memory mapped regions, i.e. the REG_xxx
 * devices. */

#define HARDWARE_INTERFACES                                                     \
    consumes Dummy mmc;                                                         \
    consumes Dummy timer;                                                       \
    consumes Dummy ccm;                                                         \
    consumes Dummy syscon;                                                      \
    consumes Dummy iomuxc;                                                      \
    emits Dummy dummy_source;

#define HARDWARE_COMPOSITION                                                    \
    connection seL4DTBHardware mmc_conn(from dummy_source, to mmc);             \
    connection seL4DTBHardware timer_conn(from dummy_source, to timer);         \
    connection seL4DTBHardware ccm_conn(from dummy_source, to ccm);             \
    connection seL4DTBHardware syscon_conn(from dummy_source, to syscon);       \
    connection seL4DTBHardware iomuxc_conn(from dummy_source, to iomuxc);

#define HARDWARE_CONFIGURATION                                                  \
    mmc.dtb       = dtb({ "path" : REG_MMC_PATH });                             \
    timer.dtb     = dtb({ "path" : REG_TIMER_PATH });                           \
    ccm.dtb       = dtb({ "path" : REG_CCM_PATH });                             \
    syscon.dtb    = dtb({ "path" : REG_SYSCON_PATH });                          \
    iomuxc.dtb    = dtb({ "path" : REG_IOMUXC_PATH });
