MAKEFILE_PATH := $(abspath $(word $(shell expr $(words $(MAKEFILE_LIST)) - 1),$(MAKEFILE_LIST)))
ifeq ($(MAKEFILE_PATH),)
    MAKEFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
endif

UNAME 		   = $(shell uname)
ARCH          := $(shell uname -m)
SYS           := $(shell uname -s)
ifeq ($(SYS),Darwin)
    OS := darwin
else ifeq ($(SYS),Linux)
    OS := linux
else
    OS := windows
endif
AI_DICTATION  	   := rune-gen
SRC_ROOT  	  	   := $(patsubst %/,%,$(dir $(MAKEFILE_PATH)))
BUILD_DIR 	  	   := $(CURDIR)
MAKEFILE_PATH_ROOT := $(abspath $(lastword $(MAKEFILE_LIST)))
ATYPE_ROOT 		   := $(patsubst %/,%,$(dir $(MAKEFILE_PATH_ROOT)))

ifeq ($(UNAME), Darwin)
    SED = gsed
	NPROC = gnproc
else
    SED = sed
	NPROC = nproc
endif

# its my wheel, i'll do it my way! /Oval
ifeq ($(strip $(SRC_ROOT)),$(strip $(BUILD_DIR)))
    $(error SRC_ROOT and BUILD_DIR cannot be the same -- invoke from build-folder with make -f ../Makefile)
endif

# read the libraries from the target-group arg at $(1)
define IMPORT_script
	$(shell \
		printf "%s\n" "$$(cat $(SRC_ROOT)/import)" | { \
		found=0; \
		found_arch=1; \
		found_os=1; \
		arch_list="x86_64 arm64"; \
		os_list="darwin windows linux"; \
		while IFS= read -r line; do \
			new_found=0; \
			if echo "$$line" | grep -q "^#"; then \
				continue; \
			fi; \
			for arch in $$arch_list; do \
				if echo "$$line" | grep -q "[[:space:]]*$$arch:"; then \
					if [ "$(ARCH)" = "$$arch" ]; then \
						found_arch=1; \
					else \
						found_arch=0; \
					fi; \
					continue 2; \
				fi; \
			done; \
			for os in $$os_list; do \
				if echo "$$line" | grep -q "[[:space:]]*$$os:"; then \
					if [ "$(OS)" = "$$os" ]; then \
						found_os=1; \
					else \
						found_os=0; \
					fi; \
					continue 2; \
				fi; \
			done; \
			if echo "$$line" | grep -q "^$(1):"; then \
				found=1; \
				new_found=1; \
				found_arch=1; \
				found_os=1; \
				line=$$(echo "$$line" | $(SED) -E 's/^$(1):[[:space:]]*(.*)/ \1/'); \
			elif echo "$$line" | grep -q "^[a-zA-Z][a-zA-Z0-9_]*:"; then \
				found=0; \
			fi; \
			if [ $$found -eq 1 ] && [ $$found_arch -eq 1 ] && [ $$found_os -eq 1 ]; then \
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

ifeq ($(UNAME),Darwin)
FRAMEWORK_PATHS = /System/Library/Frameworks /Library/Frameworks
define is_framework
$(shell for p in $(FRAMEWORK_PATHS); do \
    if [ -d "$$p/$(1).framework" ]; then \
        echo "yes"; \
        break; \
    fi; \
done)
endef
endif

comma = ,
define process_libs
$(foreach dep,$(filter-out ,$(shell bash -c 'echo "$(call IMPORT_script,$(1))"')), \
	$(if $(filter -%,$(strip $(dep))),, \
		$(if $(findstring .so,$(dep)), \
			$(IMPORT)/lib/$(strip $(dep)), \
			$(if $(and $(findstring Darwin,$(UNAME)), $(call is_framework,$(strip $(dep)))), \
				-framework $(strip $(dep)), \
				$(if $(filter @%,$(strip $(dep))), \
					-Wl$(comma)-Bstatic -l$(subst @,,$(dep)) -Wl$(comma)-Bdynamic -l$(subst @,,$(dep)), \
					-l$(dep)\
				)\
			)\
		)\
	)\
)
endef

define process_imports
$(foreach dep,$(filter-out ,$(shell bash -c 'echo "$(call IMPORT_script,$(1))"')), \
	$(if $(filter -%,$(strip $(dep))),, \
		$(if $(findstring .so,$(dep)), $(IMPORT)/lib/$(strip $(dep)),$(strip $(dep)))
	)\
)
endef

define process_cflags
$(foreach dep,$(filter-out ,$(shell bash -c 'echo "$(call IMPORT_script,$(1))"')), \
	$(if $(filter -%,$(strip $(dep))),$(strip $(dep)),)\
)
endef

PREP_DIRS := $(shell \
	mkdir -p $(BUILD_DIR)/app $(BUILD_DIR)/test $(BUILD_DIR)/lib && \
	if [ -d "$(SRC_ROOT)/res" ]; then \
		rsync --checksum -r $(SRC_ROOT)/res/* $(BUILD_DIR); \
	fi && \
    if [ -d "$(SRC_ROOT)/lib" ]; then \
        find $(SRC_ROOT)/lib -type f -exec sh -c ' \
            src_file="{}"; \
            dest_file="$(BUILD_DIR)/lib/$$(basename {})"; \
			if [ ! -f "$$dest_file" ] || [ "$$src_file" -nt "$$dest_file" ]; then \
				echo "copying $$src_file -> $$dest_file"; \
				cp -p "$$src_file" "$$dest_file"; \
				$(SED) -i "1i\#line 1 \"$$src_file\"" "$$dest_file"; \
			fi \
        ' \; ; \
    fi && \
    if [ -d "$(SRC_ROOT)/app" ]; then \
        find $(SRC_ROOT)/app -type f -exec sh -c ' \
            src_file="{}"; \
            dest_file="$(BUILD_DIR)/app/$$(basename {})"; \
			if [ ! -f "$$dest_file" ] || [ "$$src_file" -nt "$$dest_file" ]; then \
				cp -p "$$src_file" "$$dest_file"; \
				$(SED) -i "1i\#line 1 \"$$src_file\"" "$$dest_file"; \
			fi \
        ' \; ; \
    fi && \
    if [ -d "$(SRC_ROOT)/test" ]; then \
        find $(SRC_ROOT)/test -type f -exec sh -c ' \
            src_file="{}"; \
            dest_file="$(BUILD_DIR)/test/$$(basename {})"; \
			if [ ! -f "$$dest_file" ] || [ "$$src_file" -nt "$$dest_file" ]; then \
				cp -p "$$src_file" "$$dest_file"; \
				$(SED) -i "1i\#line 1 \"$$src_file\"" "$$dest_file"; \
			fi \
        ' \; ; \
    fi \
)

APPS_LIBS          = $(call process_libs,app)
LIB_LIBS 	       = $(call process_libs,lib)
TEST_LIBS          = $(call process_libs,test)
LIB_CFLAGS         = $(call process_cflags,lib)
APPS_CFLAGS        = $(call process_cflags,app)
TEST_CFLAGS        = $(call process_cflags,test)
APPS_IMPORTS       = $(call process_imports,app)
LIB_IMPORTS        = $(call process_imports,lib)
release		      ?= 0
APP			      ?= 0
CC 			       = $(IMPORT)/bin/clang
CXX			       = $(IMPORT)/bin/clang++
MAKEFLAGS         += --no-print-directory
LIB_INCLUDES       = -I$(BUILD_DIR)/lib  -I$(IMPORT)/include
APP_INCLUDES       = -I$(BUILD_DIR)/app  -I$(BUILD_DIR)/lib -I$(IMPORT)/include
TEST_INCLUDES      = -I$(BUILD_DIR)/test -I$(BUILD_DIR)/lib -I$(IMPORT)/include
# default behavior of -l cannot be set back once you do -Wl,-Bdynamic or -Wl,-Bstatic -- there is no -Wl,-Bdynamic-first
# to create sanity lets establish -Wl,-Bdynamic first
LDFLAGS 	       = -L$(BUILD_DIR) -L$(IMPORT)/lib -Wl,-rpath,$(IMPORT)/lib -Wl,-Bdynamic
SRCAPP_DIR 	       = $(BUILD_DIR)/app
SRCLIB_DIR 	       = $(BUILD_DIR)/lib
TEST_DIR           = $(BUILD_DIR)/test
TEST_SRCS 	       = $(wildcard $(TEST_DIR)/*.c)    $(wildcard $(TEST_DIR)/*.cc)
TEST_OBJS 		   = $(patsubst $(TEST_DIR)/%.c,    $(BUILD_DIR)/test/%.o, $(wildcard $(TEST_DIR)/*.c)) \
           			 $(patsubst $(TEST_DIR)/%.cc,   $(BUILD_DIR)/test/%.o, $(wildcard $(TEST_DIR)/*.cc))
LIB_SRCS 	       = $(wildcard $(SRCLIB_DIR)/*.c)  $(wildcard $(SRCLIB_DIR)/*.cc)
LIB_OBJS 		   = $(patsubst $(SRCLIB_DIR)/%.c,  $(BUILD_DIR)/lib/%.o, $(wildcard $(SRCLIB_DIR)/*.c)) \
           			 $(patsubst $(SRCLIB_DIR)/%.cc, $(BUILD_DIR)/lib/%.o, $(wildcard $(SRCLIB_DIR)/*.cc))
APP_SRCS 	       = $(wildcard $(SRCAPP_DIR)/*.c)  $(wildcard $(SRCAPP_DIR)/*.cc)
APP_OBJS 		   = $(patsubst $(SRCAPP_DIR)/%.c,  $(BUILD_DIR)/app/%.o, $(wildcard $(SRCAPP_DIR)/*.c)) \
           			 $(patsubst $(SRCAPP_DIR)/%.cc, $(BUILD_DIR)/app/%.o, $(wildcard $(SRCAPP_DIR)/*.cc))
#REFLECT_TARGET    = $(BUILD_DIR)/$(PROJECT)-reflect
LIB_TARGET   	   = $(if $(strip $(LIB_OBJS)),$(BUILD_DIR)/lib$(PROJECT).so)
APP_TARGETS	 	   = $(patsubst $(SRCAPP_DIR)/%.c, $(BUILD_DIR)/%, $(wildcard $(SRCAPP_DIR)/*.c)) \
					 $(patsubst $(SRCAPP_DIR)/%.cc, $(BUILD_DIR)/%, $(wildcard $(SRCAPP_DIR)/*.cc))
TARGET_FLAGS 	   = -shared
ALL_TARGETS  	   = run-import methods $(LIB_TARGET) $(APP_TARGETS) $(BUILD_DIR)/$(PROJECT)-flat $(BUILD_DIR)/$(PROJECT)-includes tests
APP_FLAGS    	   = 
PROJECT_HEADER_R   = $(if $(wildcard $(BUILD_DIR)/lib/$(PROJECT)),lib/$(PROJECT),$(if $(wildcard $(BUILD_DIR)/app/$(PROJECT)),app/$(PROJECT),))
PROJECT_HEADER     = $(BUILD_DIR)/$(PROJECT_HEADER_R)
IMPORT_APP_HEADER  = $(BUILD_DIR)/app/import
IMPORT_LIB_HEADER  = $(BUILD_DIR)/lib/import
IMPORT_TEST_HEADER = $(BUILD_DIR)/test/import
METHODS_HEADER 	   = $(BUILD_DIR)/$(PROJECT_HEADER_R)-methods
INIT_HEADER        = $(BUILD_DIR)/$(PROJECT_HEADER_R)-init
INTERN_HEADER      = $(BUILD_DIR)/$(PROJECT_HEADER_R)-intern
upper 		       = $(shell echo $(1) | tr '[:lower:]' '[:upper:]')
LINKER 			   = $(if $(filter %.cc %.cpp,$(LIB_SRCS)), $(CXX), $(CC))
UPROJECT 	       = $(call upper,$(PROJECT))
CXXFLAGS 		   = $(if $(filter 1,$(release)),,-g) -fPIC -DMODULE="\"$(PROJECT)\"" -std=c++17 -Wno-writable-strings
CFLAGS 		   	   = $(if $(filter 1,$(release)),,-g) -fPIC -fno-exceptions \
	-D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS \
	-Wno-write-strings -Wno-compare-distinct-pointer-types -Wno-deprecated-declarations \
	-Wno-incompatible-pointer-types -Wfatal-errors -std=gnu11 -DMODULE="\"$(PROJECT)\"" \
	-Wno-incompatible-library-redeclaration -fvisibility=default
ifeq ($(ASAN),1)
CFLAGS := $(CFLAGS) -fsanitize=address
LDFLAGS := $(LDFLAGS) -fsanitize=address
endif
SRC_TRANSLATION   := $(SRC_ROOT)/translation/A-translation.c
BUILD_TRANSLATION := $(BUILD_DIR)/A-translation


ifneq ($(release),0)
	ALL_TARGETS += verify
endif

#clean_headers:
#	@rm -f $(BUILD_DIR)/lib/* $(BUILD_DIR)/app/* $(BUILD_DIR)/test/*

all: $(ALL_TARGETS)

# for both headers:
# we make unique macros for each so we may expand other calls within
# otherwise we are limited by 1 expansion policy
# we also facilitate different expansion for class vs struct
# this makes for more intuitive, more readable c code.
# it costs the pre-processor nothing to parse this, even for hundreds of methods

# generate method macros header (PROJECT-method)
$(METHODS_HEADER): $(PROJECT_HEADER)
	@bash $(ATYPE_ROOT)/method.sh $(METHODS_HEADER) $(PROJECT_HEADER) $(UPROJECT)

# generate init macros header (PROJECT-init)
$(INIT_HEADER): $(PROJECT_HEADER)
	@bash $(ATYPE_ROOT)/init.sh $(INIT_HEADER) $(PROJECT_HEADER) $(UPROJECT)

$(INTERN_HEADER): $(PROJECT_HEADER)
	@bash $(ATYPE_ROOT)/intern.sh $(INTERN_HEADER) $(PROJECT_HEADER) $(UPROJECT)

# A-type subprocedure syntax is a good thing it just cant happen now, and we should still attempt to say the file is c
ifeq ($(PROJECT),AAAAA) # disabled for now

$(BUILD_TRANSLATION): $(SRC_TRANSLATION) # target to compile A-translation.c
	$(CC) $(CFLAGS) -o $(BUILD_TRANSLATION) $(SRC_TRANSLATION)

build_translation: $(BUILD_TRANSLATION)

install_translation: build_translation # copy A-translation to IMPORT/bin
	echo "installing translation <-"
	@install --directory $(IMPORT)/bin
	@install -m 755 $(BUILD_TRANSLATION) $(IMPORT)/bin/

all: install_translation
else

build_translation:

endif

# this code is less ugly than maintaining headers.
# here is where we may include generated headers, too.
# A-type projects provide extra defs in -methods and -init
# Collect all .c and .cc files
# Get only files with NO extension (user modules)
LIB_MODULES := $(foreach f,$(wildcard $(SRC_ROOT)/lib/*),$(if $(suffix $f),,$(notdir $f)))

define generate_import_header
	@if [ ! -f $(1) ] || [ "$(SRC_ROOT)/import" -nt $(1) ]; then \
		rm -f $(1); \
		echo "/* generated import interface */" > $(1); \
		echo "#ifndef _$(UPROJECT)_IMPORT_$(2)_H_" >> $(1); \
		echo "#define _$(UPROJECT)_IMPORT_$(2)_H_" >> $(1); \
		$(foreach import, $(3), \
			if [ "$(PROJECT)" != "$(import)" ]; then \
				if [ -f "$(IMPORT)/include/$(import)-methods" ]; then \
				echo "#include <$(import)>" >> $(1) ; \
				echo "#include <$(import)-methods>" >> $(1) ; \
				fi; \
			fi;) \
		echo "#include <$(PROJECT)-intern>" >> $(1) ; \
		echo "#include <$(PROJECT)>" >> $(1); \
		$(foreach name, $(LIB_MODULES), \
			echo "#include <$(basename $(notdir $(name)))>" >> $(1) ; \
		) \
		echo "#include <$(PROJECT)-methods>" >> $(1); \
		echo "#include <A-reserve>" >> $(1); \
		$(foreach import, $(3), \
			if [ "$(PROJECT)" != "$(import)" ]; then \
				if [ -f "$(IMPORT)/include/$(import)-methods" ]; then \
					echo "#include <$(import)-init>" >> $(1) ; \
				fi; \
			fi;) \
		echo "#include <$(PROJECT)-init>" >> $(1); \
		echo >> $(1); \
		echo "#endif" >> $(1); \
	fi
endef

$(IMPORT_LIB_HEADER):
	$(call generate_import_header,$@,LIB,$(LIB_IMPORTS),lib)

$(IMPORT_APP_HEADER):
	$(call generate_import_header,$@,APP,$(APPS_IMPORTS),app)

$(IMPORT_TEST_HEADER):
	$(call generate_import_header,$@,TEST,$(TEST_IMPORTS),test)

.PRECIOUS: *.o
.SUFFIXES:
.PHONY: all clean install run-import tests verify process_c_files methods clean $(METHODS_HEADER) $(INIT_HEADER) $(INTERN_HEADER) $(IMPORT_LIB_HEADER) $(IMPORT_APP_HEADER) $(IMPORT_TEST_HEADER)

methods: $(METHODS_HEADER) $(INIT_HEADER) $(INTERN_HEADER) $(IMPORT_LIB_HEADER) $(IMPORT_APP_HEADER) $(IMPORT_TEST_HEADER)

define process_src
	@for file in $(SRC_ROOT)/$(1)/*.c; do \
		if [ -f "$$file" ]; then \
			echo "A-translation '$$file' '$(BUILD_DIR)/$(1)/$$(basename $$file)'"; \
			$(IMPORT)/bin/A-translation "$$file" "$(BUILD_DIR)/$(1)/$$(basename $$file)"; \
		fi \
	done; \
	for file in $(SRC_ROOT)/$(1)/*.ai; do \
		if [ -f "$$file" ]; then \
			base_name=$$(basename "$$file" .ai); \
			if [[ $$base_name =~ ^.*\..*$$ ]]; then \
				target_ext=$${base_name##*.}; \
				target_base=$${base_name%.*}; \
				target_file="$(BUILD_DIR)/$(1)/$$target_base.$$target_ext"; \
				echo "Processing AI file '$$file' -> '$$target_file'"; \
				export LD_LIBRARY_PATH=$$IMPORT; \
				$(BUILD_DIR)/A-generate -i "$$file" -o "$$target_file" || exit 1; \
				unset LD_LIBRARY_PATH; \
				if [ $$? -eq 0 ] && [ -f "$$target_file" ]; then \
					echo "Successfully generated $$target_file"; \
				else \
					echo "Error: Failed to process $$file"; \
					exit 1; \
				fi \
			fi \
		fi \
	done
endef

process_c_files:
	$(call process_src,lib)
	$(call process_src,app)
	$(call process_src,test)

# Fix Makefile behavior with this; otherwise its On Error Resume Next -- style functionality.  not exactly what most people want
.NOTPARALLEL:

define RUN_IMPORT_SCRIPT
	@echo "running import script"
	@bash $(ATYPE_ROOT)/import.sh --b $(BUILD_DIR) --i $(SRC_ROOT)/import || { echo "import script failed"; exit 1; }
endef

# Add this call to the import script before building
$(LIB_TARGET): | run-import
$(APP_TARGET): | run-import

run-import:
	$(RUN_IMPORT_SCRIPT)

#$(LIB_TARGET): $(filter-out $(BUILD_DIR)/.rebuild, $(BUILD_DIR)/.rebuild)
#$(APP_TARGET): $(filter-out $(BUILD_DIR)/.rebuild, $(BUILD_DIR)/.rebuild)

#$(BUILD_DIR)/.rebuild:
#	bash $(SRC_ROOT)/../A/import.sh $(IMPORT) --b $(BUILD_DIR) --i $(SRC_ROOT)/import

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
# ---------------------------------------------
# lib (todo: allow -flags to be placed in import)
$(BUILD_DIR)/lib/%.o: $(BUILD_DIR)/lib/%.c
	$(CC) $(CFLAGS) $(LIB_INCLUDES) $(LIB_CFLAGS) -c $< -o $@
$(BUILD_DIR)/lib/%.o: $(BUILD_DIR)/lib/%.cc
	$(CXX) $(CXXFLAGS) $(LIB_INCLUDES) $(LIB_CFLAGS) -c $< -o $@

# app
$(BUILD_DIR)/app/%.o: $(BUILD_DIR)/app/%.c
	$(CC) $(CFLAGS) $(APP_INCLUDES) -c $< -o $@
$(BUILD_DIR)/app/%.o: $(BUILD_DIR)/app/%.cc
	$(CXX) $(CXXFLAGS) $(APP_INCLUDES) -c $< -o $@

# test
$(BUILD_DIR)/test/%.o: $(BUILD_DIR)/test/%.c
	$(CC) $(CFLAGS) $(TEST_INCLUDES) -c $< -o $@
$(BUILD_DIR)/test/%.o: $(BUILD_DIR)/test/%.cc
	$(CXX) $(CXXFLAGS) $(TEST_INCLUDES) -c $< -o $@

# linking
# ---------------------------------------------
ifneq ($(strip $(LIB_TARGET)),)
$(LIB_TARGET): $(LIB_OBJS)
	$(LINKER) $(TARGET_FLAGS) $(LDFLAGS) -o $@ $^ $(LIB_LIBS)
	@echo "built lib: $@"
endif

$(BUILD_DIR)/%: $(BUILD_DIR)/app/%.o $(LIB_TARGET)
	$(LINKER) $(LDFLAGS) -o $@ $^ $(LIB_TARGET) $(APPS_LIBS);
	@echo "built app: $@"

$(BUILD_DIR)/test/%: $(BUILD_DIR)/test/%.o $(LIB_TARGET)
	$(LINKER) $(LDFLAGS) -o $@ $^ $(LIB_TARGET) $(TEST_LIBS)
	@echo "built test: $@"

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

	@mkdir -p $(IMPORT)/lib
	@mkdir -p $(IMPORT)/include
	@mkdir -p $(IMPORT)/bin

	@if [ -n "$(strip $(LIB_TARGET))" ]; then \
		if [ -f $(BUILD_DIR)/$(PROJECT)-includes ]; then \
			install -m 644 $(LIB_TARGET)  $(IMPORT)/lib/; \
		fi \
	fi

	@if [ -n "$(strip $(APP_TARGETS))" ]; then \
		install -m 755 $(APP_TARGETS) $(IMPORT)/bin/; \
	fi

	@if [ -f "$(SRCLIB_DIR)/$(PROJECT)" ]; then \
		install -m 644 $(SRCLIB_DIR)/$(PROJECT) $(IMPORT)/include/; \
	fi

	@if [ -f "$(METHODS_HEADER)" ]; then \
		install -m 644 $(METHODS_HEADER) $(IMPORT)/include/; \
	fi

	@if [ -f "$(INIT_HEADER)" ]; then \
		install -m 644 $(INIT_HEADER) $(IMPORT)/include/; \
	fi

	@if [ -f "$(INTERN_HEADER)" ]; then \
		install -m 644 $(INTERN_HEADER) $(IMPORT)/include/; \
	fi

	@if [ -f $(SRCLIB_DIR)/A-reserve ]; then \
		install -m 644 $(SRCLIB_DIR)/A-reserve $(IMPORT)/include/; \
	fi

	@if [ -f $(BUILD_DIR)/$(PROJECT)-includes ]; then \
		install -m 644 $(BUILD_DIR)/$(PROJECT)-includes $(IMPORT)/include/; \
		install -m 644 $(BUILD_DIR)/$(PROJECT)-flat $(IMPORT)/include/; \
	fi

	@cd $(BUILD_DIR) # && ./$(PROJECT)-reflect || true
	@if [ -f $(BUILD_DIR)/lib$(PROJECT).m ]; then \
		install -m 644 $(BUILD_DIR)/lib$(PROJECT).m $(IMPORT)/lib/; \
	fi

clean:
	rm -rf $(BUILD_DIR)/lib$(PROJECT).so $(BUILD_DIR)/$(PROJECT) $(BUILD_DIR)/app $(BUILD_DIR)/lib $(BUILD_DIR)/test $(METHODS_HEADER) \
		   $(INIT_HEADER) $(INTERN_HEADER) $(IMPORT_LIB_HEADER) $(IMPORT_APP_HEADER) $(IMPORT_TEST_HEADER) \
		   $(REFLECT_TARGET) $(BUILD_DIR)/*-flat $(BUILD_DIR)/*-includes $(BUILD_DIR)/*.tmp.c