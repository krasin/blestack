AM_MAKEFLAGS = --no-print-directory

bin_PROGRAMS =
libexec_PROGRAMS =

builtin_modules =
builtin_sources =
builtin_nodist =

stack_sources = stack/ll.h stack/ll.c			\
		stack/bci.h stack/bci.c

builtin_sources += examples/platform/nrf51822/helloworld/hello.c
libexec_PROGRAMS += examples/platform/nrf51822/helloworld/hello

INCLUDES = -I$(builddir)/stack

CLEANFILES =

MAINTAINERCLEANFILES = Makefile.in \
	aclocal.m4 configure config.h.in config.sub config.guess \
	ltmain.sh depcomp missing install-sh mkinstalldirs
