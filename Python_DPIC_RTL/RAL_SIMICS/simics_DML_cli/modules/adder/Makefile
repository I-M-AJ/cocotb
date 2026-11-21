# -*- Makefile ; coding: utf-8 -*-

# © 2026 Intel Corporation

#
# See Simics Model Builder User's Guide for more details on what to put here.
#

MODULE_CLASSES = adder
SRC_FILES = adder.dml
PYTHON_FILES = module_load.py

SIMICS_API := 7
THREAD_SAFE := yes

ifeq ($(MODULE_MAKEFILE),)
$(error Make sure you compile your module from the project directory)
else
include $(MODULE_MAKEFILE)
endif
