<!--
     Copyright 2022, Capgemini Engineering

     SPDX-License-Identifier: BSD-2-Clause
-->

# PicoServer U-Boot

The PicoServer U-Boot application works much the same as the standard PicoServer application, with the key difference being that instead of utilising seL4's standard Ethdriver component, the Ethdriver component has been modifed to utilise a Ethernet driver provided by the libubootdrivers U-Boot driver framework. No modification has been made to the Echo component, but the Listener component has been removed as it was not required for the purposes of this application. The Ethdriver component has been modified to act as a wrapper around libubootdrivers ethernet driver interface and the source is inlcuded within this application as opposed to being imported like in the standard PicoServer application.

Configuration is the same as the standard application, the details of which can be found [here](https://github.com/sel4devkit/camkes/blob/master/apps/picoserver/README.md).
