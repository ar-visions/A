PROJECT_NAME := $(LIB)

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

# read the libraries from the target-group arg at $(1)
define IMPORT_script
$(shell \
    found=0; \
    while IFS= read -r line; do \
        if echo "$$line" | grep -q "^$(1):"; then \
            found=1; \
            continue; \
        elif echo "$$line" | grep -q "^[a-zA-Z][a-zA-Z0-9_]*:"; then \
            found=0; \
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
endef

IMPORT_LIB    := $(shell bash -c 'echo "$(call IMPORT_script,lib)"')
LIB_LIBS       = $(foreach dep,$(IMPORT_LIB),$(if $(findstring .so,$(dep)),$(SILVER_IMPORT)/lib/$(dep),-l$(dep)))
IMPORT_APP    := $(shell bash -c 'echo "$(call IMPORT_script,app)"')
APPS_LIBS      = $(foreach dep,$(IMPORT_APP),$(if $(findstring .so,$(dep)),$(SILVER_IMPORT)/app/$(dep),-l$(dep)))
IMPORT_TEST   := $(shell bash -c 'echo "$(call IMPORT_script,test)"')
TEST_LIBS      = $(foreach dep,$(IMPORT_TEST),$(if $(findstring .so,$(dep)),$(SILVER_IMPORT)/test/$(dep),-l$(dep)))
release		  ?= 0
APP			  ?= 0
SILVER_IMPORT ?= $(shell if [ -n "$$SRC" ]; then echo "$$SRC/silver-import"; else echo "$(BUILD_DIR)/silver-import"; fi)
CC 			   = clang
MAKEFLAGS     += --no-print-directory
INCLUDES 	   = -I$(SRC_ROOT)/lib -I$(SRC_ROOT)/app -I$(SILVER_IMPORT)/include
LDFLAGS 	   = -L$(SILVER_IMPORT)/lib -Wl,-rpath,$(SILVER_IMPORT)/lib
SRCAPP_DIR 	   = $(SRC_ROOT)/app
SRCLIB_DIR 	   = $(SRC_ROOT)/lib
TEST_DIR       = $(SRC_ROOT)/test
TEST_SRCS 	   = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS 	   = $(TEST_SRCS:$(TEST_DIR)/%.c=$(BUILD_DIR)/test/%.o)
LIB_SRCS 	   = $(wildcard $(SRCLIB_DIR)/*.c)
LIB_OBJS 	   = $(LIB_SRCS:$(SRCLIB_DIR)/%.c=$(BUILD_DIR)/lib/%.o)
APP_SRCS 	   = $(wildcard $(SRCAPP_DIR)/*.c)
APP_OBJS 	   = $(APP_SRCS:$(SRCAPP_DIR)/%.c=$(BUILD_DIR)/app/%.o)
REFLECT_TARGET = $(BUILD_DIR)/$(PROJECT_NAME)-reflect

#$(error "$(APP_OBJS)")

# should be in same stream as dependencies; anything with a minus would be a cflag
CFLAGS 		   = $(if $(filter 1,$(release)),,-g) -fPIC -fno-exceptions \
	-D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS \
	-Wno-write-strings -Wno-compare-distinct-pointer-types -Wno-deprecated-declarations \
	-Wno-incompatible-pointer-types -Wfatal-errors -std=gnu11 -DMODULE="\"$(PROJECT_NAME)\"" \
	-Wno-incompatible-library-redeclaration

LIB_TARGET 	 = $(BUILD_DIR)/lib$(PROJECT_NAME).so
APP_TARGETS	 = $(patsubst $(SRCAPP_DIR)/%.c, $(BUILD_DIR)/app/%, $(wildcard $(SRCAPP_DIR)/*.c))
TARGET_FLAGS = -shared
ALL_TARGETS  = import $(LIB_TARGET) $(APP_TARGETS) $(REFLECT_TARGET) $(BUILD_DIR)/$(PROJECT_NAME)-flat $(BUILD_DIR)/$(PROJECT_NAME)-includes tests
APP_FLAGS    = 

ifneq ($(release),0)
	ALL_TARGETS += verify
endif

.PHONY: all clean install import tests verify prepare_dirs
.PRECIOUS: *.o

all: prepare_dirs $(ALL_TARGETS)

prepare_dirs:
	mkdir -p $(BUILD_DIR)/app $(BUILD_DIR)/test $(BUILD_DIR)/lib

import:
	@bash $(SRC_ROOT)/../A/import.sh $(SILVER_IMPORT) --i $(SRC_ROOT)/import

$(BUILD_DIR)/$(PROJECT_NAME)-includes:
	@echo "#include <$(PROJECT_NAME)>" > $@.tmp.c
	@$(CC) $(CFLAGS) $(INCLUDES) -E -dD -C -P $@.tmp.c | \
		awk '/\/\/\/ start-trim/ {in_trim=1; next} /\/\/\/ end-trim/ {in_trim=0; next} in_trim {print}' > $@
	@rm $@.tmp.c

$(BUILD_DIR)/$(PROJECT_NAME)-flat:
	@echo "#include <$(PROJECT_NAME)>" > $@.tmp.c
	@$(CC) $(CFLAGS) $(INCLUDES) -E -dD -C -P $@.tmp.c | \
	awk '/\/\/\/ start-trim/ {print; in_trim=1; next} /\/\/\/ end-trim/ {in_trim=0; print; next} !in_trim {print}' > $@
	@rm $@.tmp.c

# compiling
$(BUILD_DIR)/lib/%.o: $(SRCLIB_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/app/%.o: $(SRCAPP_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/test/%.o: $(TEST_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# linking
$(LIB_TARGET): $(LIB_OBJS)
	$(CC) $(TARGET_FLAGS) $(LDFLAGS) -o $@ $^ $(LIB_LIBS)

$(BUILD_DIR)/app/%: $(BUILD_DIR)/app/%.o $(LIB_TARGET)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIB_LIBS)

$(BUILD_DIR)/test/%: $(BUILD_DIR)/test/%.o $(LIB_TARGET)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIB_LIBS)


$(REFLECT_TARGET): $(SRC_ROOT)/../A/meta/A-reflect.c $(LIB_TARGET)
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@ -L$(BUILD_DIR)/lib $(LDFLAGS) -l$(PROJECT_NAME) $(LIB_LIBS)

TEST_TARGETS := $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/test/%,$(TEST_SRCS))

# build all tests individually
tests: $(TEST_TARGETS)

verify: $(TEST_TARGETS)
	@for test_exec in $(TEST_TARGETS); do \
		echo "test: $$test_exec..."; \
		$$test_exec; \
		if [ $$? -ne 0 ]; then \
			echo "$$test_exec failed"; \
			exit 1; \
		fi; \
	done
	@echo "all tests passed."

# install targets and run reflect with integrity check
install: all
	install --directory $(SILVER_IMPORT)/lib
	install --directory $(SILVER_IMPORT)/include
	install --directory $(SILVER_IMPORT)/bin

	@if [ -f $(BUILD_DIR)/$(PROJECT_NAME)-includes ]; then \
		install -m 644 $(LIB_TARGET)  $(SILVER_IMPORT)/lib/; \
	fi

	@if [ -n $(APP_TARGETS) ]; then \
		install -m 644 $(APP_TARGETS) $(SILVER_IMPORT)/bin/; \
	fi

	@if [ -n $(SRCLIB_DIR)/$(PROJECT_NAME) ]; then \
		install -m 644 $(SRCLIB_DIR)/$(PROJECT_NAME) $(SILVER_IMPORT)/include/; \
	fi

	@if [ -f $(BUILD_DIR)/$(PROJECT_NAME)-includes ]; then \
		install -m 644 $(BUILD_DIR)/$(PROJECT_NAME)-includes $(SILVER_IMPORT)/include/; \
		install -m 644 $(BUILD_DIR)/$(PROJECT_NAME)-flat $(SILVER_IMPORT)/include/; \
	fi

	@cd $(BUILD_DIR) && ./$(PROJECT_NAME)-reflect || true
	if [ -f $(BUILD_DIR)/lib$(PROJECT_NAME).m ]; then \
		install -m 644 $(BUILD_DIR)/lib$(PROJECT_NAME).m $(SILVER_IMPORT)/lib/; \
	fi

# clean build artifacts
clean:
	rm -rf $(BUILD_DIR)/app $(BUILD_DIR)/lib $(BUILD_DIR)/test $(REFLECT_TARGET) $(BUILD_DIR)/*-flat $(BUILD_DIR)/*.tmp.c

ifeq ($(VERBOSE),1)
	$(info $(PROJECT_NAME) : $(IMPORT) -- $(SRC_ROOT)/../A/import.sh $(SILVER_IMPORT) --i $(SRC_ROOT)/import)
endif