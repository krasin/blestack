AM_MAKEFLAGS = --no-print-directory
AM_CPPFLAGS =

STACK_SOURCE_FILES = stack/ll.c stack/bci.c

STACK_OBJECT_FILES = $(STACK_SOURCE_FILES:.c=.o)

AM_CPPFLAGS += -Istack

if PLATFORM_NRF
include Makefile.nrf
endif

if PLATFORM_UBERTOOTH
include Makefile.ubertooth
endif
