PROJECT_NAME := A
REL := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
include $(REL)../A/build.mk