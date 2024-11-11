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
		printf "%s\n" "$$(cat $(SRC_ROOT)/import)" | { \
		found=0; \
		while IFS= read -r line; do \
			new_found=0; \
			if echo "$$line" | grep -q "^$(1):"; then \
				found=1; \
				new_found=1; \
				line=$$(echo "$$line" | sed -E 's/^$(1):[[:space:]]*(.*)/ \1/'); \
			elif echo "$$line" | grep -q "^[a-zA-Z][a-zA-Z0-9_]*:"; then \
				found=0; \
			fi; \
			if [ $$found -eq 1 ]; then \
				if [ -n "$$line" ] && echo "$$line" | grep -q "^[[:space:]]"; then \
					for lib in $$line; do \
						echo "$$lib"; \
					done; \
				elif [ $$new_found -eq 0 ]; then \
					break; \
				fi; \
			fi; \
		done
	})
endef

define process_libs # ( module map )
	$(foreach dep,$(filter-out ,$(shell bash -c 'echo "$(call IMPORT_script,$(1))"')), \
		$(if $(findstring .so,$(dep)), $(SILVER_IMPORT)/lib/$(dep),-l$(dep)))
endef

define process_imports # ( module map )
	$(foreach dep,$(filter-out ,$(shell bash -c 'echo "$(call IMPORT_script,$(1))"')), \
		$(if $(findstring .so,$(dep)), $(SILVER_IMPORT)/lib/$(dep),$(dep)))
endef

APPS_LIBS          = $(call process_libs,app)
LIB_LIBS 	       = $(call process_libs,lib)
TEST_LIBS          = $(call process_libs,test)
APPS_IMPORTS       = $(call process_imports,app)
LIB_IMPORTS        = $(call process_imports,lib)
release		      ?= 0
APP			      ?= 0
SILVER_IMPORT     ?= $(shell if [ -n "$$SRC" ]; then echo "$$SRC/silver-import"; else echo "$(BUILD_DIR)/silver-import"; fi)
CC 			       = clang-18 # $(SILVER_IMPORT)/bin/clang
MAKEFLAGS         += --no-print-directory --silent
LIB_INCLUDES       = -I$(BUILD_DIR)/lib  -I$(SRC_ROOT)/lib -I$(SILVER_IMPORT)/include
APP_INCLUDES       = -I$(BUILD_DIR)/app  -I$(SRC_ROOT)/app -I$(SRC_ROOT)/lib -I$(SILVER_IMPORT)/include
TEST_INCLUDES      = -I$(BUILD_DIR)/test -I$(SRC_ROOT)/test -I$(SRC_ROOT)/lib -I$(SILVER_IMPORT)/include
LDFLAGS 	       = -L$(SILVER_IMPORT)/lib -Wl,-rpath,$(SILVER_IMPORT)/lib
SRCAPP_DIR 	       = $(SRC_ROOT)/app
SRCLIB_DIR 	       = $(SRC_ROOT)/lib
TEST_DIR           = $(SRC_ROOT)/test
TEST_SRCS 	       = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS 	       = $(TEST_SRCS:$(TEST_DIR)/%.c=$(BUILD_DIR)/test/%.o)
LIB_SRCS 	       = $(wildcard $(SRCLIB_DIR)/*.c)
LIB_OBJS 	       = $(LIB_SRCS:$(SRCLIB_DIR)/%.c=$(BUILD_DIR)/lib/%.o)
APP_SRCS 	       = $(wildcard $(SRCAPP_DIR)/*.c)
APP_OBJS 	       = $(APP_SRCS:$(SRCAPP_DIR)/%.c=$(BUILD_DIR)/app/%.o)
#REFLECT_TARGET    = $(BUILD_DIR)/$(PROJECT)-reflect
LIB_TARGET   	   = $(if $(strip $(LIB_OBJS)),$(BUILD_DIR)/lib/lib$(PROJECT).so)
APP_TARGETS	 	   = $(patsubst $(SRCAPP_DIR)/%.c, $(BUILD_DIR)/app/%, $(wildcard $(SRCAPP_DIR)/*.c))
TARGET_FLAGS 	   = -shared
ALL_TARGETS  	   = prepare_dirs import methods $(LIB_TARGET) $(APP_TARGETS) $(BUILD_DIR)/$(PROJECT)-flat $(BUILD_DIR)/$(PROJECT)-includes tests
APP_FLAGS    	   = 
PROJECT_HEADER_R   = $(if $(wildcard $(SRC_ROOT)/lib/$(PROJECT)),lib/$(PROJECT),$(if $(wildcard $(SRC_ROOT)/app/$(PROJECT)),app/$(PROJECT),))
PROJECT_HEADER     = $(SRC_ROOT)/$(PROJECT_HEADER_R)
IMPORT_APP_HEADER  = $(BUILD_DIR)/app/import
IMPORT_LIB_HEADER  = $(BUILD_DIR)/lib/import
IMPORT_TEST_HEADER = $(BUILD_DIR)/test/import
METHODS_HEADER 	   = $(BUILD_DIR)/$(PROJECT_HEADER_R)-methods
INIT_HEADER        = $(BUILD_DIR)/$(PROJECT_HEADER_R)-init
INTERN_HEADER      = $(BUILD_DIR)/$(PROJECT_HEADER_R)-intern
upper 		       = $(shell echo $(1) | tr '[:lower:]' '[:upper:]')
UPROJECT 	       = $(call upper,$(PROJECT))
CFLAGS 		   	   = $(if $(filter 1,$(release)),,-g) -fPIC -fno-exceptions \
	-D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS \
	-Wno-write-strings -Wno-compare-distinct-pointer-types -Wno-deprecated-declarations \
	-Wno-incompatible-pointer-types -Wfatal-errors -std=gnu11 -DMODULE="\"$(PROJECT)\"" \
	-Wno-incompatible-library-redeclaration
CFLAGS  		  := $(CFLAGS) -fsanitize=address
LDFLAGS 		  := $(LDFLAGS) -fsanitize=address

ifneq ($(release),0)
	ALL_TARGETS += verify
endif

all: $(ALL_TARGETS)

# how do we add to ALL_TARGETS if PROJECT_HEADER exists and METHODS_HEADER does not? (might be nice to allow the user to make one)
# Generate method macros header
$(METHODS_HEADER): $(PROJECT_HEADER)
	@rm -f $@  # delete the METHODS_HEADER before generating it
	@echo "/* generated methods interface */" > $@
	@echo "#ifndef _$(UPROJECT)_METHODS_H_" >> $@
	@echo "#define _$(UPROJECT)_METHODS_H_" >> $@
	@# python-like flat methods
	@echo >> $@
	@grep -o 'i_method[[:space:]]*([^,]*,[^,]*,[^,]*,[^,]*,[[:space:]]*[[:alnum:]_]*' $(PROJECT_HEADER) | \
	sed 's/i_method[[:space:]]*([^,]*,[^,]*,[^,]*,[^,]*,[[:space:]]*\([[:alnum:]_]*\).*/\1/' | \
	while read method; do \
		echo "#undef $$method\n#define $$method(I,...) ftableI(I) -> $$method(I, ##__VA_ARGS__)" >> $@; \
	done
	@echo >> $@
	@echo "#endif /* _$(UPROJECT)_METHODS_H_ */" >> $@

# generate wrappers (macros named same as each class found in *_schema) around new() macro
$(INIT_HEADER): $(PROJECT_HEADER)
	@rm -f $@  # delete the INIT_HEADER before generating it
	@echo "/* generated methods interface */" > $@
	@echo "#ifndef _$(UPROJECT)_INIT_H_" >> $@
	@echo "#define _$(UPROJECT)_INIT_H_" >> $@
	@# new wrappers; these allow you to use the type of the same name in C source, but not as tokens in macros
	@echo >> $@
	@grep -o 'declare_class[[:space:]]*([[:space:]]*[^,)]*' $(PROJECT_HEADER) | \
	sed 's/declare_class[[:space:]]*([[:space:]]*\([^,)]*\).*/\1/' | \
	while read class_name; do \
		echo "#define $$class_name(...)\t\tnew($$class_name, __VA_ARGS__)" >> $@; \
	done
	@echo >> $@
	@echo >> $@
	@grep -o 'declare_mod[[:space:]]*([[:space:]]*[^,)]*' $(PROJECT_HEADER) | \
	sed 's/declare_mod[[:space:]]*([[:space:]]*\([^,)]*\).*/\1/' | \
	while read class_name; do \
		echo "#define $$class_name(...)\t\tnew($$class_name, __VA_ARGS__)" >> $@; \
	done
	@echo >> $@
	@echo "#endif /* _$(UPROJECT)_INIT_H_ */" >> $@

$(INTERN_HEADER):
	@rm -f $@  # delete the INTERN_HEADER before generating it
	@echo "/* generated methods interface */" > $@
	@echo "#ifndef _$(UPROJECT)_INTERN_H_" >> $@
	@echo "#define _$(UPROJECT)_INTERN_H_" >> $@
	@# new wrappers; these allow you to use the type of the same name in C source, but not as tokens in macros
	@echo >> $@
	@grep -o 'declare_class[[:space:]]*([[:space:]]*[^,)]*' $(PROJECT_HEADER) | \
	sed 's/declare_class[[:space:]]*([[:space:]]*\([^,)]*\).*/\1/' | \
	while read class_name; do \
		echo "#define $${class_name}_intern\t\tintern($$class_name)" >> $@; \
	done
	@echo >> $@
	@echo >> $@
	@grep -o 'declare_mod[[:space:]]*([[:space:]]*[^,)]*' $(PROJECT_HEADER) | \
	sed 's/declare_mod[[:space:]]*([[:space:]]*\([^,)]*\).*/\1/' | \
	while read class_name; do \
		echo "#define $${class_name}_intern\t\tintern($$class_name)" >> $@; \
	done
	@echo >> $@
	@echo "#endif /* _$(UPROJECT)_INTERN_H_ */" >> $@

define generate_import_header
	@rm -f $(1)
	@echo "/* generated import interface */" > $(1)
	@echo "#ifndef _$(UPROJECT)_IMPORT_$(2)_H_" >> $(1)
	@echo "#define _$(UPROJECT)_IMPORT_$(2)_H_" >> $(1)
	@echo >> $(1)
	@$(foreach import, $(3), \
		if [ -f "$(SILVER_IMPORT)/include/$(import)-methods" ]; then \
		echo "#include <$(import)>" >> $(1) ; \
		echo "#include <$(import)-methods>" >> $(1) ; \
		fi;)
	@echo "#include <$(PROJECT)-intern>" >> $(1)
	@echo "#include <$(PROJECT)>" >> $(1)
	@echo "#include <$(PROJECT)-methods>" >> $(1)
	@echo "#include <A-reserve>" >> $(1)
	@$(foreach import, $(3), \
		if [ -f "$(SILVER_IMPORT)/include/$(import)-methods" ]; then \
		echo "#include <$(import)-init>" >> $(1) ; \
		fi;)
	@echo "#include <$(PROJECT)-init>" >> $(1)
	@echo >> $(1)
	@echo "#endif" >> $(1)
endef

$(IMPORT_LIB_HEADER):
	$(call generate_import_header,$@,LIB,$(LIB_IMPORTS))

$(IMPORT_APP_HEADER):
	$(call generate_import_header,$@,APP,$(APPS_IMPORTS))

$(IMPORT_TEST_HEADER):
	$(call generate_import_header,$@,TEST,$(TEST_IMPORTS))

.PRECIOUS: *.o
.SUFFIXES:
.PHONY: all clean install import tests verify prepare_dirs methods clean $(METHODS_HEADER) $(INIT_HEADER) $(INTERN_HEADER) $(IMPORT_LIB_HEADER) $(IMPORT_APP_HEADER) $(IMPORT_TEST_HEADER)

methods: $(METHODS_HEADER) $(INIT_HEADER) $(INTERN_HEADER) $(IMPORT_LIB_HEADER) $(IMPORT_APP_HEADER) $(IMPORT_TEST_HEADER)

prepare_dirs:
	@mkdir -p $(BUILD_DIR)/app $(BUILD_DIR)/test $(BUILD_DIR)/lib
	@if [ -d "$(SRC_ROOT)/res" ]; then \
		cp -r $(SRC_ROOT)/res/* $(BUILD_DIR); \
	fi
	@if [ -d "$(SRC_ROOT)/lib" ]; then \
		find $(SRC_ROOT)/lib -maxdepth 1 -type f ! -name '*.c' -exec cp -r {} $(BUILD_DIR)/lib \; ; \
	fi

import:
	@bash $(SRC_ROOT)/../A/import.sh $(SILVER_IMPORT) --i $(SRC_ROOT)/import

$(BUILD_DIR)/$(PROJECT)-includes:
	@echo "#include <$(PROJECT)>" > $@.tmp.c
	@$(CC) $(CFLAGS) $(LIB_INCLUDES) $(APP_INCLUDES) -E -dD -C -P $@.tmp.c | \
		awk '/\/\/\/ start-trim/ {in_trim=1; next} /\/\/\/ end-trim/ {in_trim=0; next} in_trim {print}' > $@
	@rm $@.tmp.c

$(BUILD_DIR)/$(PROJECT)-flat:
	@echo "#include <$(PROJECT)>" > $@.tmp.c
	@$(CC) $(CFLAGS) $(LIB_INCLUDES) $(APP_INCLUDES) -E -dD -C -P $@.tmp.c | \
	awk '/\/\/\/ start-trim/ {print; in_trim=1; next} /\/\/\/ end-trim/ {in_trim=0; print; next} !in_trim {print}' > $@
	@rm $@.tmp.c

# compiling
$(BUILD_DIR)/lib/%.o: $(SRCLIB_DIR)/%.c
	$(CC) $(CFLAGS) $(LIB_INCLUDES) -c $< -o $@

$(BUILD_DIR)/app/%.o: $(SRCAPP_DIR)/%.c $(LIB_TARGET)
	$(CC) $(CFLAGS) $(APP_INCLUDES) -c $< -o $@

$(BUILD_DIR)/test/%.o: $(TEST_DIR)/%.c $(LIB_TARGET)
	$(CC) $(CFLAGS) $(TEST_INCLUDES) -c $< -o $@

# linking
ifneq ($(strip $(LIB_TARGET)),)
$(LIB_TARGET): $(LIB_OBJS)
	$(CC) $(TARGET_FLAGS) $(LDFLAGS) -o $@ $^ $(LIB_LIBS)
endif

$(BUILD_DIR)/app/%: $(BUILD_DIR)/app/%.o $(LIB_TARGET)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIB_TARGET) $(APPS_LIBS)

$(BUILD_DIR)/test/%: $(BUILD_DIR)/test/%.o $(LIB_TARGET)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIB_TARGET) $(TEST_LIBS)

#ifneq ($(strip $(LIB_TARGET)),)
#$(REFLECT_TARGET): $(SRC_ROOT)/../A/meta/A-reflect.c $(LIB_TARGET)
#	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@ -L$(BUILD_DIR)/lib $(LDFLAGS) -l$(PROJECT) $(LIB_LIBS)
#endif

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
	@install --directory $(SILVER_IMPORT)/lib
	@install --directory $(SILVER_IMPORT)/include
	@install --directory $(SILVER_IMPORT)/bin
	@if [ -n "$(LIB_TARGET)" ]; then \
		if [ -f $(BUILD_DIR)/$(PROJECT)-includes ]; then \
			install -m 644 $(LIB_TARGET)  $(SILVER_IMPORT)/lib/; \
		fi \
	fi

	@if [ -n "$(APP_TARGETS)" ]; then \
		install -m 644 $(APP_TARGETS) $(SILVER_IMPORT)/bin/; \
	fi

	@if [ -f "$(SRCLIB_DIR)/$(PROJECT)" ]; then \
		install -m 644 $(SRCLIB_DIR)/$(PROJECT) $(SILVER_IMPORT)/include/; \
	fi

	@if [ -f "$(METHODS_HEADER)" ]; then \
		install -m 644 $(METHODS_HEADER) $(SILVER_IMPORT)/include/; \
	fi

	@if [ -f "$(INIT_HEADER)" ]; then \
		install -m 644 $(INIT_HEADER) $(SILVER_IMPORT)/include/; \
	fi

	@if [ -f "$(INTERN_HEADER)" ]; then \
		install -m 644 $(INTERN_HEADER) $(SILVER_IMPORT)/include/; \
	fi

	@if [ -f $(SRCLIB_DIR)/A-reserve ]; then \
		install -m 644 $(SRCLIB_DIR)/A-reserve $(SILVER_IMPORT)/include/; \
	fi

	@if [ -f $(BUILD_DIR)/$(PROJECT)-includes ]; then \
		install -m 644 $(BUILD_DIR)/$(PROJECT)-includes $(SILVER_IMPORT)/include/; \
		install -m 644 $(BUILD_DIR)/$(PROJECT)-flat $(SILVER_IMPORT)/include/; \
	fi

	@cd $(BUILD_DIR) # && ./$(PROJECT)-reflect || true
	@if [ -f $(BUILD_DIR)/lib$(PROJECT).m ]; then \
		install -m 644 $(BUILD_DIR)/lib$(PROJECT).m $(SILVER_IMPORT)/lib/; \
	fi

clean:
	rm -rf $(BUILD_DIR)/app $(BUILD_DIR)/lib $(BUILD_DIR)/test $(METHODS_HEADER) \
		   $(INIT_HEADER) $(INTERN_HEADER) $(IMPORT_LIB_HEADER) $(IMPORT_APP_HEADER) $(IMPORT_TEST_HEADER) \
		   $(REFLECT_TARGET) $(BUILD_DIR)/*-flat $(BUILD_DIR)/*-includes $(BUILD_DIR)/*.tmp.c