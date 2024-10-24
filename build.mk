
MAKEFILE_PATH := $(abspath $(word $(shell expr $(words $(MAKEFILE_LIST)) - 1),$(MAKEFILE_LIST)))
ifeq ($(MAKEFILE_PATH),)
    MAKEFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
endif
SRC_ROOT  	  := $(patsubst %/,%,$(dir $(MAKEFILE_PATH)))
BUILD_DIR 	  := $(CURDIR)

# its my wheel, i'll do it my way!
ifeq ($(strip $(SRC_ROOT)),$(strip $(BUILD_DIR)))
    $(error SRC_ROOT and BUILD_DIR cannot be the same -- invoke from build-folder with make -f ../Makefile)
endif

# thank the chatgpt for this nice thing
IMPORT := $(shell awk -v module="$(PROJECT_NAME):" \
    'BEGIN{found=0} \
     /^[^#]/ { \
        if ($$1 == module) { \
            found=1; \
            next \
        } \
        if (found == 1 && $$0 ~ /^[[:space:]]/) { \
            print $$1 \
        } else if (found == 1) { \
            exit \
        } \
     }' $(SRC_ROOT)/import)

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
LIB_TARGET 	   = $(BUILD_DIR)/lib$(PROJECT_NAME).so
REFLECT_TARGET = $(BUILD_DIR)/$(PROJECT_NAME)-reflect
CFLAGS 		   = $(if $(filter 1,$(release)),,-g) -fPIC -fno-exceptions \
	-D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS \
	-Wno-write-strings -Wno-compare-distinct-pointer-types -Wno-deprecated-declarations \
	-Wno-incompatible-pointer-types -Wfatal-errors -std=gnu11 -DMODULE="\"$(PROJECT_NAME)\""

.PHONY: all clean install import

# read dependency libs for our PROJECT_NAME (single module)
all: import $(LIB_TARGET) $(REFLECT_TARGET)

# Build object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# library
$(LIB_TARGET): $(OBJS)
	$(CC) -shared $(LDFLAGS) -o $@ $^ $(LIBS)

# A-reflect
$(REFLECT_TARGET): $(SRC_ROOT)/../A/src/meta/A-reflect.c $(LIB_TARGET)
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@ $(LDFLAGS) -L$(BUILD_DIR) -l$(PROJECT_NAME) $(LIBS)

# import
import:
	@bash $(SRC_ROOT)/../A/import.sh $(SILVER_IMPORT) --i $(SRC_ROOT)/import

# install targets and run reflect with integrity check
install: all
	@install -d $(SILVER_IMPORT)/lib
	@install -d $(SILVER_IMPORT)/include
	@install -d $(SILVER_IMPORT)/bin
	@install -m 644 $(LIB_TARGET) 			  $(SILVER_IMPORT)/lib/
	@install -m 644 $(SRC_DIR)/$(PROJECT_NAME) $(SILVER_IMPORT)/include/
	@if [ "$(PROJECT_NAME)" = "A" ]; then \
		install -m 644 $(SRC_DIR)/$(PROJECT_NAME)-type $(SILVER_IMPORT)/include/; \
	fi
	@cd $(BUILD_DIR) && ./$(PROJECT_NAME)-reflect || true
	@if [ -f $(BUILD_DIR)/lib$(PROJECT_NAME).m ]; then \
		install -m 644 $(BUILD_DIR)/lib/lib$(PROJECT_NAME).m $(SILVER_IMPORT)/lib/; \
	fi

# clean build artifacts
clean:
	rm -f $(BUILD_DIR)/*.o $(LIB_TARGET) $(REFLECT_TARGET)