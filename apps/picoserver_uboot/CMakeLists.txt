#
# SPDX-License-Identifier: BSD-2-Clause
#

cmake_minimum_required(VERSION 3.7.2)

project(picoserver_uboot C)

includeGlobalComponents()

CAmkESAddCPPInclude("${CMAKE_CURRENT_LIST_DIR}/include/plat/${KernelPlatform}/")

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

DeclareCAmkESComponent(
    EthdriverUboot
    SOURCES
    components/EthdriverUboot/src/ethdriver.c
    INCLUDES
    include/plat/${KernelPlatform}
    LIBS
    ubootdrivers
)

DeclareCAmkESRootserver(
    picoserver_uboot.camkes
    CPP_FLAGS
    ${cpp_define}
    -DPICOSERVER_IP_ADDR=\"${PICOSERVER_IP_ADDR}\"
)