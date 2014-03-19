########################################################################
# Makefile for zat of MMX platform.                                    #
########################################################################

C_SRCS		= system_properties.c properties.c
OUT_FILE        = libproperty.so


ARMCC		= arm-none-linux-gnueabi-gcc
CFLAGS          = -Wall -g -shared -DHAVE_LIBC_SYSTEM_PROPERTIES -D__ARM_EABI__

.PHONY:         all clean arm 

arm:
	$(ARMCC) $(CFLAGS) $(C_SRCS) -o $(OUT_FILE)
	
# Clean up build products.
clean:
	@rm -f $(OUT_FILE)

