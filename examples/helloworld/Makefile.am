AM_CFLAGS=-mcpu=cortex-m0 -mthumb -mfloat-abi=soft -c
AM_LDFLAGS=--specs=nano.specs -Xlinker -Map=helloworld.map		\
	   -T$(top_builddir)/platform/nrf51822/gcc_nrf51_blank_xxaa.ld	\
	   -L$(SDK_DIR)/Nordic/nrf51822/Source/templates/gcc

PLATFORM_LIBS = $(top_builddir)/stack/libblestack.a			\
		$(top_builddir)/platform/nrf51822/libnrf.a

AM_CPPFLAGS=-I$(top_builddir)/platform/nrf51822

HELLO_SOURCE_FILES = main.c
HELLO_OBJECT_FILES = $(HELLO_SOURCE_FILES:.c=.o)

all: helloworld.bin

helloworld.bin: helloworld.out
	$(OBJCOPY) -Obinary helloworld.out $@

helloworld.out: $(HELLO_OBJECT_FILES)

	$(CC) $(AM_LDFLAGS) $(HELLO_OBJECT_FILES) $(PLATFORM_LIBS) -o $@

	$(OBJDUMP) -h helloworld.out

# Build object files from C source files
%.o: %.c
	$(CC) -c $(CFLAGS) $(AM_CPPFLAGS) -o $@ $<

# Build object files from ASM source files
%.o: %.s
	$(CC) -c $(ASMFLAGS) $(AM_CPPFLAGS) -o $@ $<

MAINTAINERCLEANFILES = Makefile.in		\
		       helloworld.bin		\
		       helloworld.out		\
		       helloworld.map

clean-local:
	$(RM) -rf $(MAINTAINERCLEANFILES)
	$(RM) *.o
