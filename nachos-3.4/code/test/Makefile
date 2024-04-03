# use normal make for this Makefile
#
# Makefile for building user programs to run on top of Nachos
#
# Several things to be aware of:
#
#    Nachos assumes that the location of the program startup routine (the
# 	location the kernel jumps to when the program initially starts up)
#       is at location 0.  This means: start.o must be the first .o passed 
# 	to ld, in order for the routine "Start" to be loaded at location 0
#

# if you are cross-compiling, you need to point to the right executables
# and change the flags to ld and the build procedure for as

GCCDIR = ../../../gnu-decstation-ultrix/decstation-ultrix/2.95.3/

LDFLAGS = -T script -N
ASFLAGS = -mips2
CPPFLAGS = $(INCDIR)

# if you aren't cross-compiling:
# GCCDIR =
# LDFLAGS = -N -T 0
# ASFLAGS =
# CPPFLAGS = -P $(INCDIR)

PATH = $(GCCDIR):/lib:/usr/bin:/bin

CC = $(GCCDIR)gcc -B../../../gnu-decstation-ultrix/
AS = $(GCCDIR)as
LD = $(GCCDIR)ld

CPP = gcc -E
INCDIR =-I../userprog -I../threads
CFLAGS = -G 0 -c $(INCDIR)

<<<<<<< HEAD
all: halt shell matmult sort createFile readchar
=======
all: halt shell matmult sort createFile readint readchar help ascii quicksort mergesort
>>>>>>> 8a78f601b5ee69d8893ef0636c205fb601ad8081

start.o: start.s ../userprog/syscall.h
	$(CPP) $(CPPFLAGS) start.c > strt.s
	$(AS) $(ASFLAGS) -o start.o strt.s
	rm strt.s

halt.o: halt.c
	$(CC) $(CFLAGS) -c halt.c
halt: halt.o start.o
	$(LD) $(LDFLAGS) start.o halt.o -o halt.coff
	../bin/coff2noff halt.coff halt

shell.o: shell.c
	$(CC) $(CFLAGS) -c shell.c
shell: shell.o start.o
	$(LD) $(LDFLAGS) start.o shell.o -o shell.coff
	../bin/coff2noff shell.coff shell

sort.o: sort.c
	$(CC) $(CFLAGS) -c sort.c
sort: sort.o start.o
	$(LD) $(LDFLAGS) start.o sort.o -o sort.coff
	../bin/coff2noff sort.coff sort

matmult.o: matmult.c
	$(CC) $(CFLAGS) -c matmult.c
matmult: matmult.o start.o
	$(LD) $(LDFLAGS) start.o matmult.o -o matmult.coff
	../bin/coff2noff matmult.coff matmult

createFile.o: createFile.c
	$(CC) $(CFLAGS) -c createFile.c
createFile: createFile.o start.o
	$(LD) $(LDFLAGS) start.o createFile.o -o createFile.coff
	../bin/coff2noff createFile.coff createFile
readchar.o: readchar.c
	$(CC) $(CFLAGS) -c readchar.c
readchar: readchar.o start.o
	$(LD) $(LDFLAGS) start.o readchar.o -o readchar.coff
	../bin/coff2noff readchar.coff readchar

createFile.o: createFile.c
	$(CC) $(CFLAGS) -c createFile.c
createFile: createFile.o start.o
	$(LD) $(LDFLAGS) start.o createFile.o -o createFile.coff
	../bin/coff2noff createFile.coff createFile
<<<<<<< HEAD
=======
readint.o: readint.c
	$(CC) $(CFLAGS) -c readint.c
readint: readint.o start.o
	$(LD) $(LDFLAGS) start.o readint.o -o readint.coff
	../bin/coff2noff readint.coff readint

>>>>>>> 8a78f601b5ee69d8893ef0636c205fb601ad8081
readchar.o: readchar.c
	$(CC) $(CFLAGS) -c readchar.c
readchar: readchar.o start.o
	$(LD) $(LDFLAGS) start.o readchar.o -o readchar.coff
<<<<<<< HEAD
	../bin/coff2noff readchar.coff readchar
=======
	../bin/coff2noff readchar.coff readchar

help.o: help.c
	$(CC) $(CFLAGS) -c help.c
help: help.o start.o
	$(LD) $(LDFLAGS) start.o help.o -o help.coff
	../bin/coff2noff help.coff help

ascii.o: ascii.c
	$(CC) $(CFLAGS) -c ascii.c
ascii: ascii.o start.o
	$(LD) $(LDFLAGS) start.o ascii.o -o ascii.coff
	../bin/coff2noff ascii.coff ascii


quicksort.o: quicksort.c
	$(CC) $(CFLAGS) -c quicksort.c
quicksort: quicksort.o start.o
	$(LD) $(LDFLAGS) start.o quicksort.o -o quicksort.coff
	../bin/coff2noff quicksort.coff quicksort

mergesort.o: mergesort.c
	$(CC) $(CFLAGS) -c mergesort.c
mergesort: mergesort.o start.o
	$(LD) $(LDFLAGS) start.o mergesort.o -o mergesort.coff
	../bin/coff2noff mergesort.coff mergesort
>>>>>>> 8a78f601b5ee69d8893ef0636c205fb601ad8081
