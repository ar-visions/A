CC = gcc
PROJECT = tapestry
CFLAGS := $(CFLAGS) -I. -I./include -I../lib -std=c11 -fPIC -fno-exceptions \
	-D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS \
	-Wno-write-strings -Wno-compare-distinct-pointer-types -Wno-deprecated-declarations \
	-Wno-incompatible-pointer-types -Wfatal-errors -std=gnu11 -DMODULE="\"$(PROJECT)\"" \
	-Wno-incompatible-library-redeclaration -fvisibility=default

OBJS = tapestry.o A.o
TARGET = tapestry

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o tapestry $^ -lm 

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) *.o

install:
	sudo install -m 755 $(TARGET) /usr/local/bin/$(TARGET)