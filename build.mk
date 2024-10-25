
ifeq ($(strip $(APP)),)
    ifeq ($(strip $(LIB)),)
        $(error Neither APP nor LIB is defined)
    else
        PROJECT_NAME := $(LIB)
		BUILD_TYPE := LIB
    endif
else
    PROJECT_NAME := $(APP)
	BUILD_TYPE := APP
endif

MAKEFILE_PATH := $(abspath $(word $(shell expr $(words $(MAKEFILE_LIST)) - 1),$(MAKEFILE_LIST)))
ifeq ($(MAKEFILE_PATH),)
    MAKEFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
endif
SRC_ROOT  	  := $(patsubst %/,%,$(dir $(MAKEFILE_PATH)))
BUILD_DIR 	  := $(CURDIR)

# its my wheel, i'll do it my way! /Oval
ifeq ($(strip $(SRC_ROOT)),$(strip $(BUILD_DIR)))
    $(error SRC_ROOT and BUILD_DIR cannot be the same -- invoke from build-folder with make -f ../Makefile)
endif

IMPORT := $(shell \
    found=0; \
    while IFS= read -r line; do \
        if echo "$$line" | grep -q "^$(PROJECT_NAME):"; then \
            found=1; \
            continue; \
        fi; \
        if [ $$found -eq 1 ]; then \
            if [ -n "$$line" ] && echo "$$line" | grep -q "^[[:space:]]"; then \
                for lib in $$line; do \
                    echo "$$lib"; \
                done; \
            else \
                break; \
            fi; \
        fi; \
    done < $(SRC_ROOT)/import)

release		  ?= 0
LIBS           = $(foreach dep,$(IMPORT),$(if $(findstring .so,$(dep)),$(SILVER_IMPORT)/lib/$(dep),-l$(dep)))
APP			  ?= 0
SILVER_IMPORT ?= $(shell if [ -n "$$SRC" ]; then echo "$$SRC/silver-import"; else echo "$(BUILD_DIR)/silver-import"; fi)
CC 			   = clang-18
MAKEFLAGS     += --no-print-directory
INCLUDES 	   = -I$(SRC_ROOT)/src -I$(SILVER_IMPORT)/include
LDFLAGS 	   = -L$(SILVER_IMPORT)/lib -Wl,-rpath,$(SILVER_IMPORT)/lib
SRC_DIR 	   = $(SRC_ROOT)/src
SRCS 		   = $(wildcard $(SRC_DIR)/*.c)
OBJS 		   = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
REFLECT_TARGET = $(BUILD_DIR)/$(PROJECT_NAME)-reflect
CFLAGS 		   = $(if $(filter 1,$(release)),,-g) -fPIC -fno-exceptions \
	-D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS \
	-Wno-write-strings -Wno-compare-distinct-pointer-types -Wno-deprecated-declarations \
	-Wno-incompatible-pointer-types -Wfatal-errors -std=gnu11 -DMODULE="\"$(PROJECT_NAME)\""

ifeq ($(BUILD_TYPE),LIB)
	TARGET 	     = $(BUILD_DIR)/lib$(PROJECT_NAME).so
	TARGET_FLAGS = -shared
	ALL_TARGETS  = import $(TARGET) $(REFLECT_TARGET) $(BUILD_DIR)/$(PROJECT_NAME)-flat
endif

ifeq ($(BUILD_TYPE),APP)
	TARGET 	     = $(BUILD_DIR)/$(PROJECT_NAME)
	TARGET_FLAGS = 
	ALL_TARGETS  = import $(TARGET)
endif

$(info $(PROJECT_NAME) $(BUILD_TYPE): $(IMPORT) -- $(SRC_ROOT)/../A/import.sh $(SILVER_IMPORT) --i $(SRC_ROOT)/import)

.PHONY: all clean install import

all: $(ALL_TARGETS)

import:
	@bash $(SRC_ROOT)/../A/import.sh $(SILVER_IMPORT) --i $(SRC_ROOT)/import

$(BUILD_DIR)/$(PROJECT_NAME)-flat:
	@echo "#include <$(PROJECT_NAME)>" > $@.tmp.c
	@$(CC) $(CFLAGS) $(INCLUDES) -E -dD -C $@.tmp.c | \
	awk '/\/\/\/ start-trim/ {print; in_trim=1; next} /\/\/\/ end-trim/ {in_trim=0; print; next} !in_trim {print}' > $@
	@rm $@.tmp.c

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(TARGET_FLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(REFLECT_TARGET): $(SRC_ROOT)/../A/src/meta/A-reflect.c $(TARGET)
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@ -L$(BUILD_DIR) $(LDFLAGS) -l$(PROJECT_NAME) $(LIBS)

# install targets and run reflect with integrity check
install: all
	install -d $(SILVER_IMPORT)/lib
	install -d $(SILVER_IMPORT)/include
	install -d $(SILVER_IMPORT)/bin
	install -m 644 $(TARGET) $(SILVER_IMPORT)/lib/
	install -m 644 $(SRC_DIR)/$(PROJECT_NAME) $(SILVER_IMPORT)/include/
	@if [ "$(BUILD_TYPE)" = "LIB" ]; then \
		install -m 644 $(BUILD_DIR)/$(PROJECT_NAME)-flat $(SILVER_IMPORT)/include/; \
	fi
	@cd $(BUILD_DIR) && ./$(PROJECT_NAME)-reflect || true
	if [ -f $(BUILD_DIR)/lib$(PROJECT_NAME).m ]; then \
		install -m 644 $(BUILD_DIR)/lib$(PROJECT_NAME).m $(SILVER_IMPORT)/lib/; \
	fi

# clean build artifacts
clean:
	rm -f $(BUILD_DIR)/*.o $(TARGET) $(REFLECT_TARGET) $(BUILD_DIR)/*-flat $(BUILD_DIR)/*.tmp.c