#
# Copyright 2022, Capgemini Engineering
#
# SPDX-License-Identifier: BSD-2-Clause
#

cmake_minimum_required(VERSION 3.8.2)

project(security_demo C)

CAmkESAddCPPInclude("${CMAKE_CURRENT_LIST_DIR}/include/plat/${KernelPlatform}/")

includeGlobalComponents()

# Setting for the PicoTCP library
set(PICOSERVER_IP_ADDR "" CACHE STRING "IP address for the Picoserver component")
set(LibPicotcp ON CACHE BOOL "" FORCE)
set(LibPicotcpBsd OFF CACHE BOOL "" FORCE)
set(LibEthdriverNumPreallocatedBuffers 32 CACHE STRING "" FORCE)

DeclareCAmkESComponent(
    Crypto
    SOURCES
    components/Crypto/src/crypto.c
    INCLUDES
    include
)

DeclareCAmkESComponent(
    KeyReader
    SOURCES
    components/KeyReader/src/key_reader.c
    INCLUDES
    include/plat/${KernelPlatform}
    LIBS
    ubootdrivers
)

DeclareCAmkESComponent(
    Transmitter
    SOURCES
    components/Transmitter/src/transmitter.c
    INCLUDES
    include
)

DeclareCAmkESComponent(
    Echo
    SOURCES
    components/Echo/src/echo.c
    LIBS
    picotcp
    PicoServer-client
)

DeclareCAmkESComponent(
    EthDriverUboot
    SOURCES
    components/EthDriverUboot/src/eth_driver_uboot.c
    INCLUDES
    include/plat/${KernelPlatform}
    LIBS
    ubootdrivers
)

DeclareCAmkESRootserver(
    security_demo.camkes
    CPP_FLAGS
    -DPICOSERVER_IP_ADDR=\"${PICOSERVER_IP_ADDR}\" # Make IP address available in CAmkES file
)
