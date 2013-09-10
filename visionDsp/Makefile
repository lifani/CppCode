########################################################################
# Makefile for zat of MMX platform.                                    #
########################################################################

C_SRCS = visionDsp.c ion.c
OUT_FILE = ../output/libdsp.so

 
ARMCC = arm-none-linux-gnueabi-gcc
CFLAGS = -Wall -fPIC -O3
INCLUDE = -I./
LDFLAG = -shared -lrt

.PHONY: all clean arm 

arm:
	$(ARMCC) $(CFLAGS) $(INCLUDE) $(LDFLAG) $(C_SRCS) -o $(OUT_FILE)
	
# Clean up build products.
clean:
	rm -f $(OUT_FILE)

