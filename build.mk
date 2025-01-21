MAKEFILE_PATH := $(abspath $(word $(shell expr $(words $(MAKEFILE_LIST)) - 1),$(MAKEFILE_LIST)))
ifeq ($(MAKEFILE_PATH),)
    MAKEFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
endif

ARCH          := $(shell uname -m)
ifeq ($(shell uname -s),Darwin)
    OS := darwin
else ifeq ($(shell uname -s),Linux)
    OS := linux
else
    OS := windows
endif
SRC_ROOT  	  := $(patsubst %/,%,$(dir $(MAKEFILE_PATH)))
BUILD_DIR 	  := $(CURDIR)
ifeq ($(shell uname), Darwin)
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


ifeq ($(shell uname),Darwin)
FRAMEWORK_PATHS = /System/Library/Frameworks /Library/Frameworks
define is_framework
$(shell for p in $(FRAMEWORK_PATHS); do \
    if [ -d "$$p/$(1).framework" ]; then \
        echo "yes"; \
        break; \
    fi; \
done)
endef

define process_libs
$(foreach dep,$(filter-out ,$(shell bash -c 'echo "$(call IMPORT_script,$(1))"')), \
$(if $(findstring .so,$(dep)), \
    $(SILVER_IMPORT)/lib/$(dep), \
    $(if $(call is_framework,$(dep)), \
        -framework $(dep), \
        -l$(dep)) \
))
endef
else
define process_libs
$(foreach dep,$(filter-out ,$(shell bash -c 'echo "$(call IMPORT_script,$(1))"')), \
$(if $(findstring .so,$(dep)), $(SILVER_IMPORT)/lib/$(dep),-l$(dep)))
endef
endif

define process_imports # ( module map )
	$(foreach dep,$(filter-out ,$(shell bash -c 'echo "$(call IMPORT_script,$(1))"')), \
		$(if $(findstring .so,$(dep)), $(SILVER_IMPORT)/lib/$(dep),$(dep)))
endef

# alone, this does not work, however when we update the stamps of ALL of the source
# rsync -a $(SRC_ROOT)/lib/* $(BUILD_DIR)/lib/;

PREP_DIRS := $(shell \
	mkdir -p $(BUILD_DIR)/app $(BUILD_DIR)/test $(BUILD_DIR)/lib && \
	if [ -d "$(SRC_ROOT)/res" ]; then \
		rsync --checksum -r $(SRC_ROOT)/res/* $(BUILD_DIR); \
	fi && \
    if [ -d "$(SRC_ROOT)/lib" ]; then \
        find $(SRC_ROOT)/lib -type f -exec sh -c ' \
            src_file="{}"; \
            dest_file="$(BUILD_DIR)/lib/$$(basename {})"; \
            cp -p "$$src_file" "$$dest_file"; \
            $(SED) -i "1i\#line 1 \"$$src_file\"" "$$dest_file"; \
        ' \; ; \
    fi && \
    if [ -d "$(SRC_ROOT)/app" ]; then \
        find $(SRC_ROOT)/app -type f -exec sh -c ' \
            src_file="{}"; \
            dest_file="$(BUILD_DIR)/app/$$(basename {})"; \
            cp -p "$$src_file" "$$dest_file"; \
        	$(SED) -i "1i\#line 1 \"$$src_file\"" "$$dest_file"; \
        ' \; ; \
    fi && \
    if [ -d "$(SRC_ROOT)/test" ]; then \
        find $(SRC_ROOT)/test -type f -exec sh -c ' \
            src_file="{}"; \
            dest_file="$(BUILD_DIR)/test/$$(basename {})"; \
            cp -p "$$src_file" "$$dest_file"; \
            $(SED) -i "1i\#line 1 \"$$src_file\"" "$$dest_file"; \
        ' \; ; \
    fi \
)

APPS_LIBS          = $(call process_libs,app)
LIB_LIBS 	       = $(call process_libs,lib)
TEST_LIBS          = $(call process_libs,test)
APPS_IMPORTS       = $(call process_imports,app)
LIB_IMPORTS        = $(call process_imports,lib)
release		      ?= 0
APP			      ?= 0
SILVER_IMPORT     ?= $(shell if [ -n "$$SRC" ]; then echo "$$SRC/silver-import"; else echo "$(BUILD_DIR)/silver-import"; fi)
CC 			       = clang # $(SILVER_IMPORT)/bin/clang
MAKEFLAGS         += --no-print-directory
LIB_INCLUDES       = -I$(BUILD_DIR)/lib  -I$(SILVER_IMPORT)/include
APP_INCLUDES       = -I$(BUILD_DIR)/app  -I$(BUILD_DIR)/lib -I$(SILVER_IMPORT)/include
TEST_INCLUDES      = -I$(BUILD_DIR)/test -I$(BUILD_DIR)/lib -I$(SILVER_IMPORT)/include
LDFLAGS 	       = -L$(BUILD_DIR) -L$(SILVER_IMPORT)/lib -Wl,-rpath,$(SILVER_IMPORT)/lib
SRCAPP_DIR 	       = $(BUILD_DIR)/app
SRCLIB_DIR 	       = $(BUILD_DIR)/lib
TEST_DIR           = $(BUILD_DIR)/test
TEST_SRCS 	       = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS 	       = $(TEST_SRCS:$(TEST_DIR)/%.c=$(BUILD_DIR)/test/%.o)
LIB_SRCS 	       = $(wildcard $(SRCLIB_DIR)/*.c)
LIB_OBJS 	       = $(LIB_SRCS:$(SRCLIB_DIR)/%.c=$(BUILD_DIR)/lib/%.o)
APP_SRCS 	       = $(wildcard $(SRCAPP_DIR)/*.c)
APP_OBJS 	       = $(APP_SRCS:$(SRCAPP_DIR)/%.c=$(BUILD_DIR)/app/%.o)
#REFLECT_TARGET    = $(BUILD_DIR)/$(PROJECT)-reflect
LIB_TARGET   	   = $(if $(strip $(LIB_OBJS)),$(BUILD_DIR)/lib$(PROJECT).so)
APP_TARGETS	 	   = $(patsubst $(SRCAPP_DIR)/%.c, $(BUILD_DIR)/%, $(wildcard $(SRCAPP_DIR)/*.c))
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
UPROJECT 	       = $(call upper,$(PROJECT))
CFLAGS 		   	   = $(if $(filter 1,$(release)),,-g) -fPIC -fno-exceptions \
	-D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS \
	-Wno-write-strings -Wno-compare-distinct-pointer-types -Wno-deprecated-declarations \
	-Wno-incompatible-pointer-types -Wfatal-errors -std=gnu11 -DMODULE="\"$(PROJECT)\"" \
	-Wno-incompatible-library-redeclaration -fvisibility=default
CFLAGS  		  := $(CFLAGS) # -fsanitize=address
LDFLAGS 		  := $(LDFLAGS) # -fsanitize=address
SRC_TRANSLATION   := $(SRC_ROOT)/translation/A-translation.c
BUILD_TRANSLATION := $(BUILD_DIR)/A-translation


ifneq ($(release),0)
	ALL_TARGETS += verify
endif

#clean_headers:
#	@rm -f $(BUILD_DIR)/lib/* $(BUILD_DIR)/app/* $(BUILD_DIR)/test/*

all: $(ALL_TARGETS)

# how do we add to ALL_TARGETS if PROJECT_HEADER exists and METHODS_HEADER does not? (might be nice to allow the user to make one)
# Generate method macros header
$(METHODS_HEADER): $(PROJECT_HEADER)
	@if [ ! -f $@ ] || [ $(PROJECT_HEADER) -nt $@ ]; then \
		rm -f $@; \
		echo "/* generated methods interface */" > $@; \
		echo "#ifndef _$(UPROJECT)_METHODS_H_" >> $@; \
		echo "#define _$(UPROJECT)_METHODS_H_" >> $@; \
		echo >> $@; \
		grep -o 'i_method[[:space:]]*([^,]*,[^,]*,[^,]*,[^,]*,[[:space:]]*[[:alnum:]_]*' $(PROJECT_HEADER) | \
		$(SED) 's/i_method[[:space:]]*([^,]*,[^,]*,[^,]*,[^,]*,[[:space:]]*\([[:alnum:]_]*\).*/\1/' | \
		while read method; do \
			if [ -n "$$method" ]; then \
				echo "#undef $$method\n#define $$method(I,...) ({ __typeof__(I) _i_ = I; ftableI(_i_)->$$method(_i_, ## __VA_ARGS__); })" >> $@; \
			fi; \
		done; \
		echo >> $@; \
		echo "#endif /* _$(UPROJECT)_METHODS_H_ */" >> $@; \
	fi

# generate wrappers (macros named same as each class found in *_schema) around new() macro
$(INIT_HEADER): $(PROJECT_HEADER)
	@if [ ! -f $@ ] || [ $(PROJECT_HEADER) -nt $@ ]; then \
		rm -f $@; \
		echo "/* generated methods interface */" > $@; \
		echo "#ifndef _$(UPROJECT)_INIT_H_" >> $@; \
		echo "#define _$(UPROJECT)_INIT_H_" >> $@; \
		echo >> $@; \
		grep -o 'declare_\(class\|mod\|meta\|vector\)[[:space:]]*([[:space:]]*[^,)]*' $(PROJECT_HEADER) | \
		$(SED) -E 's/declare_(class|mod|meta|vector)[[:space:]]*\([[:space:]]*([^,)]*)[[:space:]]*(,[[:space:]]*([^,)[:space:]]*))?/\1 \2 \4/' | \
		while read type class_name arg; do \
			if [ -z "$${class_name}" ]; then \
				continue; \
			fi; \
			echo "#ifndef NDEBUG" >> $@; \
			echo "    //#define TC_$${class_name}(MEMBER, VALUE) A_validate_type(VALUE, A_member(isa(instance), A_TYPE_PROP|A_TYPE_INTERN|A_TYPE_PRIV, #MEMBER)->type)" >> $@; \
			echo "    #define TC_$${class_name}(MEMBER, VALUE) VALUE" >> $@; \
			echo "#else" >> $@; \
			echo "    #define TC_$${class_name}(MEMBER, VALUE) VALUE" >> $@; \
			echo "#endif" >> $@; \
			echo "#define _ARG_COUNT_IMPL_$${class_name}(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, N, ...) N" >> $@; \
			echo "#define _ARG_COUNT_I_$${class_name}(...) _ARG_COUNT_IMPL_$${class_name}(__VA_ARGS__, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)" >> $@; \
			echo "#define _ARG_COUNT_$${class_name}(...)   _ARG_COUNT_I_$${class_name}(\"A-type\", ## __VA_ARGS__)" >> $@; \
			echo "#define _COMBINE_$${class_name}_(A, B)   A##B" >> $@; \
			echo "#define _COMBINE_$${class_name}(A, B)    _COMBINE_$${class_name}_(A, B)" >> $@; \
			echo "#define _N_ARGS_$${class_name}_0( TYPE)" >> $@; \
			if [ "$$type" = "meta" ] || [ "$$type" = "vector" ]; then \
				echo "#define _N_ARGS_$${class_name}_1( TYPE, a)" >> $@; \
			else \
				echo "#define _N_ARGS_$${class_name}_1( TYPE, a) _Generic((a), TYPE##_schema(TYPE, GENERICS, object) const void *: (void)0)(instance, a)" >> $@; \
			fi; \
			echo "#define _N_ARGS_$${class_name}_2( TYPE, a,b) instance->a = TC_$${class_name}(a,b);" >> $@; \
			echo "#define _N_ARGS_$${class_name}_4( TYPE, a,b, c,d) _N_ARGS_2_$${class_name}(TYPE, a,b) instance->c = TC_$${class_name}(c,d);" >> $@; \
			echo "#define _N_ARGS_$${class_name}_6( TYPE, a,b, c,d, e,f) _N_ARGS_4_$${class_name}(TYPE, a,b, c,d) instance->e = TC_$${class_name}(e,f);" >> $@; \
			echo "#define _N_ARGS_$${class_name}_8( TYPE, a,b, c,d, e,f, g,h) _N_ARGS_6_$${class_name}(TYPE, a,b, c,d, e,f) instance->g = TC_$${class_name}(g,h);" >> $@; \
			echo "#define _N_ARGS_$${class_name}_10(TYPE, a,b, c,d, e,f, g,h, i,j) _N_ARGS_8_$${class_name}(TYPE, a,b, c,d, e,f, g,h) instance->i = TC_$${class_name}(i,j);" >> $@; \
			echo "#define _N_ARGS_$${class_name}_12(TYPE, a,b, c,d, e,f, g,h, i,j, l,m) _N_ARGS_10_$${class_name}(TYPE, a,b, c,d, e,f, g,h, i,j) instance->l = TC_$${class_name}(l,m);" >> $@; \
			echo "#define _N_ARGS_$${class_name}_14(TYPE, a,b, c,d, e,f, g,h, i,j, l,m, n,o) _N_ARGS_12_$${class_name}(TYPE, a,b, c,d, e,f, g,h, i,j, l,m) instance->n = TC_$${class_name}(n,o);" >> $@; \
			echo "#define _N_ARGS_$${class_name}_16(TYPE, a,b, c,d, e,f, g,h, i,j, l,m, n,o, p,q) _N_ARGS_14_$${class_name}(TYPE, a,b, c,d, e,f, g,h, i,j, l,m, n,o) instance->p = TC_$${class_name}(p,q);" >> $@; \
			echo "#define _N_ARGS_$${class_name}_18(TYPE, a,b, c,d, e,f, g,h, i,j, l,m, n,o, p,q, r,s) _N_ARGS_16_$${class_name}(TYPE, a,b, c,d, e,f, g,h, i,j, l,m, n,o, p,q) instance->r = TC_$${class_name}(r,s);" >> $@; \
			echo "#define _N_ARGS_$${class_name}_20(TYPE, a,b, c,d, e,f, g,h, i,j, l,m, n,o, p,q, r,s, t,u) _N_ARGS_18_$${class_name}(TYPE, a,b, c,d, e,f, g,h, i,j, l,m, n,o, p,q, r,s) instance->t = TC_$${class_name}(t,u);" >> $@; \
			echo "#define _N_ARGS_$${class_name}_22(TYPE, a,b, c,d, e,f, g,h, i,j, l,m, n,o, p,q, r,s, t,u, v,w) _N_ARGS_20_$${class_name}(TYPE, a,b, c,d, e,f, g,h, i,j, l,m, n,o, p,q, r,s, t,u) instance->v = TC_$${class_name}(v,w);" >> $@; \
			echo "#define _N_ARGS_HELPER2_$${class_name}(TYPE, N, ...)  _COMBINE_$${class_name}(_N_ARGS_$${class_name}_, N)(TYPE, ## __VA_ARGS__)" >> $@; \
			echo "#define _N_ARGS_$${class_name}(TYPE,...)    _N_ARGS_HELPER2_$${class_name}(TYPE, _ARG_COUNT_$${class_name}(__VA_ARGS__), ## __VA_ARGS__)" >> $@; \
			echo "#define $${class_name}(...) ({ \\" >> $@; \
			echo "    $${class_name} instance = ($${class_name})A_alloc(typeid($${class_name}), 1, true); \\" >> $@; \
			echo "    _N_ARGS_$${class_name}($${class_name}, ## __VA_ARGS__); \\" >> $@; \
			echo "    A_initialize(instance); \\" >> $@; \
			echo "    instance; \\" >> $@; \
			echo "})" >> $@; \
		done; \
		echo >> $@; \
		echo "#endif /* _$(UPROJECT)_INIT_H_ */" >> $@; \
	fi

$(INTERN_HEADER):
	@if [ ! -f $@ ] || [ $(PROJECT_HEADER) -nt $@ ]; then \
		rm -f $@; \
		echo "/* generated methods interface */" > $@; \
		echo "#ifndef _$(UPROJECT)_INTERN_H_" >> $@; \
		echo "#define _$(UPROJECT)_INTERN_H_" >> $@; \
		echo >> $@; \
		grep -o 'declare_class[[:space:]]*([[:space:]]*[^,)]*' $(PROJECT_HEADER) | \
		$(SED) 's/declare_class[[:space:]]*([[:space:]]*\([^,)]*\).*/\1/' | \
		while read class_name; do \
			echo "#define $${class_name}_intern\t\tintern($$class_name)" >> $@; \
		done; \
		echo >> $@; \
		echo >> $@; \
		grep -o 'declare_mod[[:space:]]*([[:space:]]*[^,)]*' $(PROJECT_HEADER) | \
		$(SED) 's/declare_mod[[:space:]]*([[:space:]]*\([^,)]*\).*/\1/' | \
		while read class_name; do \
			echo "#define $${class_name}_intern\t\tintern($$class_name)" >> $@; \
		done; \
		echo >> $@; \
		grep -o 'declare_meta[[:space:]]*([[:space:]]*[^,)]*' $(PROJECT_HEADER) | \
		$(SED) 's/declare_meta[[:space:]]*([[:space:]]*\([^,)]*\).*/\1/' | \
		while read class_name; do \
			echo "#define $${class_name}_intern\t\tintern($$class_name)" >> $@; \
		done; \
		echo >> $@; \
		echo "#endif /* _$(UPROJECT)_INTERN_H_ */" >> $@; \
	fi

# A-type subprocedure syntax is a good thing it just cant happen now, and we should still attempt to say the file is c
ifeq ($(PROJECT),AAAAA) # disabled for now

$(BUILD_TRANSLATION): $(SRC_TRANSLATION) # target to compile A-translation.c
	$(CC) $(CFLAGS) -o $(BUILD_TRANSLATION) $(SRC_TRANSLATION)

build_translation: $(BUILD_TRANSLATION)

install_translation: build_translation # copy A-translation to SILVER_IMPORT/bin
	echo "installing translation <-"
	@install --directory $(SILVER_IMPORT)/bin
	@install -m 755 $(BUILD_TRANSLATION) $(SILVER_IMPORT)/bin/

all: install_translation
else

build_translation:

endif

$(BUILD_DIR)/lib/%.o: $(BUILD_DIR)/lib/%.c
	$(CC) $(CFLAGS) $(LIB_INCLUDES) -c $< -o $@

define generate_import_header
	@if [ ! -f $(1) ] || [ "$(SRC_ROOT)/import" -nt $(1) ]; then \
		rm -f $(1); \
		echo "/* generated import interface */" > $(1); \
		echo "#ifndef _$(UPROJECT)_IMPORT_$(2)_H_" >> $(1); \
		echo "#define _$(UPROJECT)_IMPORT_$(2)_H_" >> $(1); \
		echo >> $(1); \
		$(foreach import, $(3), \
			if [ "$(PROJECT)" != "$(import)" ]; then \
				if [ -f "$(SILVER_IMPORT)/include/$(import)-methods" ]; then \
				echo "#include <$(import)>" >> $(1) ; \
				echo "#include <$(import)-methods>" >> $(1) ; \
				fi; \
			fi;) \
		if [ -f "$(SRC_ROOT)/$(4)/$(PROJECT).c" ]; then \
			echo "#include <$(PROJECT)-intern>" >> $(1) ; \
		fi; \
		echo "#include <$(PROJECT)>" >> $(1); \
		echo "#include <$(PROJECT)-methods>" >> $(1); \
		echo "#include <A-reserve>" >> $(1); \
		$(foreach import, $(3), \
			if [ "$(PROJECT)" != "$(import)" ]; then \
				if [ -f "$(SILVER_IMPORT)/include/$(import)-methods" ]; then \
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
			$(SILVER_IMPORT)/bin/A-translation "$$file" "$(BUILD_DIR)/$(1)/$$(basename $$file)"; \
		fi \
	done
endef

process_c_files:
	$(call process_src,lib)
	$(call process_src,app)
	$(call process_src,test)

define RUN_IMPORT_SCRIPT
	echo "running import script"
	@bash $(SRC_ROOT)/../A/import.sh $(SILVER_IMPORT) --b $(BUILD_DIR) --i $(SRC_ROOT)/import
endef

# Add this call to the import script before building
$(LIB_TARGET): | run-import
$(APP_TARGET): | run-import

run-import:
	$(RUN_IMPORT_SCRIPT)

#$(LIB_TARGET): $(filter-out $(BUILD_DIR)/.rebuild, $(BUILD_DIR)/.rebuild)
#$(APP_TARGET): $(filter-out $(BUILD_DIR)/.rebuild, $(BUILD_DIR)/.rebuild)

#$(BUILD_DIR)/.rebuild:
#	bash $(SRC_ROOT)/../A/import.sh $(SILVER_IMPORT) --b $(BUILD_DIR) --i $(SRC_ROOT)/import

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
$(BUILD_DIR)/lib/%.o: $(BUILD_DIR)/lib/%.c $(BUILD_DIR)/lib/%
	$(CC) $(CFLAGS) $(LIB_INCLUDES) -c $< -o $@

$(BUILD_DIR)/app/%.o: $(BUILD_DIR)/app/%.c $(LIB_TARGET)
	$(CC) $(CFLAGS) $(APP_INCLUDES) -c $< -o $@

$(BUILD_DIR)/test/%.o: $(BUILD_DIR)/test/%.c $(LIB_TARGET)
	$(CC) $(CFLAGS) $(TEST_INCLUDES) -c $< -o $@

# linking
ifneq ($(strip $(LIB_TARGET)),)
$(LIB_TARGET): $(LIB_OBJS)
	$(CC) $(TARGET_FLAGS) $(LDFLAGS) -o $@ $^ $(LIB_LIBS)
endif

$(BUILD_DIR)/%: $(BUILD_DIR)/app/%.o $(LIB_TARGET)
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

	@mkdir -p $(SILVER_IMPORT)/lib
	@mkdir -p $(SILVER_IMPORT)/include
	@mkdir -p $(SILVER_IMPORT)/bin

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
	rm -rf $(BUILD_DIR)/lib$(PROJECT).so $(BUILD_DIR)/$(PROJECT) $(BUILD_DIR)/app $(BUILD_DIR)/lib $(BUILD_DIR)/test $(METHODS_HEADER) \
		   $(INIT_HEADER) $(INTERN_HEADER) $(IMPORT_LIB_HEADER) $(IMPORT_APP_HEADER) $(IMPORT_TEST_HEADER) \
		   $(REFLECT_TARGET) $(BUILD_DIR)/*-flat $(BUILD_DIR)/*-includes $(BUILD_DIR)/*.tmp.c