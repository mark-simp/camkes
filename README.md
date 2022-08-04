<!--
     Copyright 2017, Data61, CSIRO (ABN 41 687 119 230)
     Copyright 2022, Capgemini Engineering

     SPDX-License-Identifier: CC-BY-SA-4.0
-->

# CAmkES

This repository contains various CAmkES applications, default configurations
and testing scripts.

```
  apps/       Example applications
  configs/    Default configurations
  test/       Testing scripts
```

This is not a standalone repository. Checkout the `camkes-manifest` repository or
go to our website for details.

The applications in this repository are:

`adder`: demonstrates the use of dataport wrapper to pass pointers via CAmkES RPC

`epit`: demonstrates the use of CAmkES interrupt connector to receive hardware interrupts

`exchangestring`: demonstrates how to pass string arguments between components

`filter`: demonstrates the use of a component to filter communication

`global-imports`:

`hierarchical-attributes`:

`hierarchical-components`:

`keyboard`: demonstrates the use of IO ports and interrupts in a CAmkES component

`lockserver`: demonstrates how to use built-in mutex

`multiassembly`:

`multiclient`: domonstrates multiple clients sharing one connection

`multiplier`: demonstrates the use of arrays as arguments

`mutex`: demonstrates the use of user-defined connectors in CAmkES

`picoserver-uboot`: demonstrates the use of the libubootdrivers driver framework for use with picoserver, using an instance of the library in place of the Ethdriver component

`rotate`: demonstrates the use of user-defined types as the arguments of an interface

`security-demo`: demonstrates the use of multiple drivers within the libubootdrivers library (Ethernet, filesystem, USB) across multiple CAmkES components, transferring data (e.g. keystrokes read by the USB keyboard driver) between a high side and a low side via a ROT13 cipher, transmitting the encrypted characters across a network via Ethernet, and logging them to a filesystem on an SD card

`swapcounter`:

`socket`: demonstrates the use of user-defined dataports

`structs`: demonstrates the use of struct and array attributes

`terminal`: a prototype for a secure terminal system

`uart`: demonstrates how to access hardware device memory

`uboot-driver-example`: tests a range of drivers from the libubootdrivers driver library including clocks, SPI, LEDs, I2C, Ethernet, SD/MMC, USB, IOMUX, GPIO, and Filesystem.
