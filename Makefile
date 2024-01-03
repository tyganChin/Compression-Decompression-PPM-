 #
 # Assignment: arith
 # Name: Makefile
 # Authors: Tygan Chin (tchin02) & Emily Ye (eye03)
 # Date: 10/18/23
 # Summary: Builds the ppm compression/decompression program along with test 	
 #	    files for the program.
 #

############## Variables ###############

CC = gcc # The compiler being used

# Updating include path to use Comp 40 .h files and CII interfaces
IFLAGS = -I/comp/40/build/include -I/usr/sup/cii40/include/cii

# Compile flags
CFLAGS = -g -std=gnu99 -Wall -Wextra -Werror -Wfatal-errors -pedantic $(IFLAGS)

# Linking flags
LDFLAGS = -g -L/comp/40/build/lib -L/usr/sup/cii40/lib64

# Libraries needed for linking
LDLIBS = -l40locality -larith40 -lnetpbm -lcii40 -lm -lrt

# Collect all .h files in your directory.
INCLUDES = $(shell echo *.h)

############### Rules ###############

all: 40image-6

## Compile step (.c files -> .o files)
%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) -c $< -o $@

## Linking step (.o -> executable program)
40image-6: 40image.o compress40.o pack.o quantize.o RGBcompvConvert.o \
	bitpack.o uarray2.o a2plain.o 
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)
	
clean:
	rm -f $(EXECUTABLES) *.o