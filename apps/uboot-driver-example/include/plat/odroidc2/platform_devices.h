#pragma once

#define BUS_PATH "/soc/bus@c8100000"
#define PINCTRL1_PATH "/soc/bus@c8100000/pinctrl@14"
#define PERIPHS_PATH "/soc/periphs@c8834000"
#define PINCTRL2_PATH "/soc/periphs@c8834000/pinctrl@4b0"

#define DEV_PATHS { PINCTRL1_PATH, PINCTRL2_PATH };

#define DEV_PATH_COUNT 2

#define REG_PATHS { BUS_PATH, PERIPHS_PATH };

#define REG_PATH_COUNT 2

#define HARDWARE_INTERFACES                                                   \
    consumes Dummy bus;                                                       \
    consumes Dummy periphs;                                                   \
    emits Dummy dummy_source;

#define HARDWARE_COMPOSITION                                                \
    connection seL4DTBHardware bus_conn(from dummy_source, to bus);         \
    connection seL4DTBHardware periphs_conn(from dummy_source, to periphs);

#define HARDWARE_CONFIGURATION                                              \
    bus.dtb     = dtb({ "path" : BUS_PATH });                               \
    periphs.dtb = dtb({ "path" : PERIPHS_PATH });                           \
