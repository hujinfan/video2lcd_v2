CROSS_COMPILE =
AS            = $(CROSS_COMPILE)as
LD            = $(CROSS_COMPILE)ld
CC            = $(CROSS_COMPILE)gcc
CPP           = $(CC) -E
AR            = $(CROSS_COMPILE)ar
NM            = $(CROSS_COMPILE)nm
STRIP         = $(CROSS_COMPILE)strip
OBJCOPY       = $(CROSS_COMPILE)objcopy
OBJDUMP       = $(CROSS_COMPILE)objdump

export AS LD CC CPP AR NM
export STRIP OBJCOPY OBJDUMP

#CFLAGS := -Wall -Werror -O2 -g
CFLAGS += -I $(shell pwd)/include
LDFLAGS :=

TOPDIR := $(shell pwd)

TARGET := video2lcd

obj-y += main.o
obj-y += display/

all :
	make -C ./ -f $(TOPDIR)/Makefile.build
	$(CC) $(LDFLAGS) -o $(TARGET) built-in.o

clean:
	rm -f $(shell find -name "*.o")
	rm -f $(TARGET)

distclean:
	rm -f $(shell find -name "*.o")
	rm -f $(shell find -name "*.d")
	rm -f $(TARGET)

export CFLAGS LDFLAGS
export TOPDIR
