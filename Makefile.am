AM_MAKEFLAGS = --no-print-directory
MAINTAINERCLEANFILES =

if PLATFORM_NRF
SUBDIRS = stack platform/nrf51822 examples/helloworld
endif

if PLATFORM_UBERTOOTH
include Makefile.ubertooth
endif

MAINTAINERCLEANFILES += Makefile.in		\
			aclocal.m4		\
			configure		\
			config.h		\
			ltmain.sh		\
			autom4te.cache		\
			stamp-h1

clean-local:
	$(RM) -rf $(MAINTAINERCLEANFILES)
