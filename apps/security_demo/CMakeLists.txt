#
# Copyright 2022, Capgemini Engineering
#
# SPDX-License-Identifier: BSD-2-Clause
#

cmake_minimum_required(VERSION 3.7.2)

project(security_demo C)

DeclareCAmkESComponent(Crypto SOURCES components/Crypto/src/crypto.c)
DeclareCAmkESComponent(Key_Reader SOURCES components/Key_Reader/src/key_reader.c)

DeclareCAmkESRootserver(security_demo.camkes)
