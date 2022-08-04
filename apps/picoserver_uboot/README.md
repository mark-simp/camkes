<!--
     Copyright 2022, Capgemini Engineering

     SPDX-License-Identifier: BSD-2-Clause
-->

# PicoServer U-Boot

The PicoServer U-Boot application runs on the Avnet MaaXBoard. It is similar to the standard PicoServer application, with the key difference being that the Ethdriver component has been modifed to use an Ethernet driver provided by the libubootdrivers U-Boot driver framework. No modification has been made to the Echo component, but the Listener component has been removed as it was not required for this application.

Configuration is the same as for the standard application, the details of which can be found [here](https://github.com/sel4devkit/camkes/blob/master/apps/picoserver/README.md).

Details of how to build and run this application are covered by the relevant section of [this seL4 Developer Kit](https://github.com/sel4devkit/seL4-DevKit-Doc/blob/main/seL4-doc/src/uboot_driver_usage.md)
