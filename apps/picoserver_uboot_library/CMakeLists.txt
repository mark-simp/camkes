#
# Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
#
# SPDX-License-Identifier: BSD-2-Clause
#

cmake_minimum_required(VERSION 3.7.2)

project(picoserver C)

includeGlobalComponents()

set(CAmkESCPP ON CACHE BOOL "" FORCE)
if("${KernelArch}" STREQUAL "arm")
    set(cpp_define -DKernelArchArm)
endif()

set(PICOSERVER_IP_ADDR "" CACHE STRING "IP address for the Picoserver component")

DeclareCAmkESComponent(
    Echo
    SOURCES
    components/Echo/src/echo.c
    LIBS
    picotcp
    PicoServer-client
)

DeclareCAmkESRootserver(
    picoserver.camkes
    CPP_FLAGS
    ${cpp_define}
    -DPICOSERVER_IP_ADDR=\"${PICOSERVER_IP_ADDR}\"
)
