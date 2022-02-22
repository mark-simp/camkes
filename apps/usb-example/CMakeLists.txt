#
# SPDX-License-Identifier: BSD-2-Clause
#

cmake_minimum_required(VERSION 3.8.2)

project(usb-example C)

CAmkESAddCPPInclude("${CMAKE_CURRENT_LIST_DIR}/include/plat/${KernelPlatform}/")

includeGlobalComponents()

DeclareCAmkESComponent(
    Test
    SOURCES
    components/Test/src/test.c
    LIBS
    usbdrivers
    muslc
    utils
    sel4muslcsys
    sel4platsupport
    sel4utils
    sel4debug
    sel4allocman
)

DeclareCAmkESRootserver(usb-example.camkes)
