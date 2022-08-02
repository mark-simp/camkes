<!--
  Copyright 2022 Capgemini Engineering
  SPDX-License-Identifier: BSD-2-Clause
-->

# Security Demo Test Application

This CAmkES application runs on the Avnet MaaXBoard and uses the U-Boot Driver Library (projects_libs/libubootdrivers). It is nominally labelled as a 'security domain' demonstrator (named security_demo) but this is mainly in the context of being a keyboard-based encryption device. It is intentionally simple and its main purpose is to show data separation and to provide worked examples of inter-component communications using different seL4 mechanisms.

## Basic Description

An operator types a plaintext message using a USB-connected keyboard. The application encrypts the message and transmits the resulting ciphertext via Ethernet to a client that has connected to a predetermined port. The application also records the ciphertext messages in a logfile on the SD card of the device. Because the emphasis of the application is to demonstrate seL4 mechanisms rather than cryptography, the encryption used is intentionally simple (ROT13).

## Architecture Overview

The application's CAmkES components include:

- KeyReader
- Crypto
- Transmitter
- PicoServer
- EthDriverUboot

Inter-component communications are demonstrated using core seL4 connector types, including:

- `seL4RPCCall`
- `seL4SharedData`
- `seL4Notification`

Three hardware devices are involved in the operation of the application:

- The KeyReader component requires access to the USB device to allow for plaintext characters to be input from a USB keyboard.
- The Transmitter component requires access to the SD/MMC device to allow for the ciphertext message to be output to a log file.
- The EthDriverUboot component requires access to the Ethernet device to allow for ciphertext message to be output to the network.

Device drivers for the required hardware access are supplied by the U-Boot Driver Library.
