#
# SPDX-License-Identifier: BSD-2-Clause
#

cmake_minimum_required(VERSION 3.8.2)

project(uboot-driver-example C)

CAmkESAddCPPInclude("${CMAKE_CURRENT_LIST_DIR}/include/plat/${KernelPlatform}/")

includeGlobalComponents()

DeclareCAmkESComponent(
    Test
    SOURCES
    components/Test/src/test.c
    INCLUDES
    include/plat/${KernelPlatform}
    LIBS
    ubootdrivers
)

DeclareCAmkESRootserver(uboot-driver-example.camkes)
