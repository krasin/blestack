noinst_LIBRARIES = libnrf.a

AM_CPPFLAGS =-I$(SDK_DIR)/Nordic/nrf51822/Include
AM_CPPFLAGS += -I$(SDK_DIR)/Nordic/nrf51822/Include/gcc

libnrf_a_SOURCES = system_nrf51.c gcc_startup_nrf51.s

MAINTAINERCLEANFILES = Makefile.in

clean-local:
	$(RM) -rf $(MAINTAINERCLEANFILES)
	$(RM) *.o
	$(RM) -rf .deps
