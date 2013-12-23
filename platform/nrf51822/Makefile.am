noinst_LIBRARIES = libnrf.a

AM_CPPFLAGS = -I$(SDK_DIR)/Nordic/nrf51822/Include			\
	      -I$(SDK_DIR)/Nordic/nrf51822/Include/gcc			\
	      -I$(SDK_DIR)/Nordic/nrf51822/Include/app_common		\
	      -I$(SDK_DIR)/Nordic/nrf51422/Include/ant/softdevice	\
	      -I$(top_builddir)/stack

ASMFLAGS = -D__HEAP_SIZE=0 -D__STACK_SIZE=1024
libnrf_a_SOURCES = system_nrf51.c		\
		   gcc_startup_nrf51.s		\
		   log.c			\
		   nrf_delay.c			\
		   app_uart.c			\
		   app_gpiote.c			\
		   app_fifo.c

MAINTAINERCLEANFILES = Makefile.in

clean-local:
	$(RM) -rf $(MAINTAINERCLEANFILES)
	$(RM) *.o
	$(RM) -rf .deps
