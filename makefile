# File: Makefile (unix version)
# (C)opyright 1987 InfoTaskforce.

CC = gcc

OBJECTS =	file.o funcs.o infocom.o init.o input.o\
		interp.o io.o jump.o object.o options.o page.o\
		print.o property.o support.o variable.o term.o \
		printf.o mem.o string.o c_getc.o c_putc.o

# Compile options are described below:
#
# ALLOCALL : As originally coded, when the program allocated
#           memory it always made sure that there was always
#           25k left free. This is not a requirement in most
#           system. If this isn't a requirement on your system
#           then use this option.
# DEBUG    : Compile a debug version of the interpreter. Compare
#           output produced with this option against a working
#           debug version when you suspect problems in the
#           virtual machine [ this commonly produces error 21 ].
# TERMCAP  : Compile a version that uses termcap features for
#           terminal output. You may also have to include a
#           termcap library as well.
# TIMESEED : Seed the random number generator with time (). Use
#           this option if time () is available on you system.
#
# DO_FILE :  actually do file operations
#            otherwise use binutils to get an object of a game file.
#            no saving.
CFLAGS=-I. -O0 #-DO_FILE -DALLOCALL -DTIMESEED -DTERMCAP # -DDEBUG

# If TERMCAP option defined include the termcap library here
# -ltermcap not required for Sys V.
LDFLAGS=	#-lcurses -ltermcap

default: infocom

crt0.o:	crt0.s
	a32k -o crt0.o crt0.s
planetfa.o: planetfa.dat
	xld  -b binary -o planetfa.o planetfa.dat
#	d:/MinGW/bin/ld  -b binary -o planetfa.o planetfa.dat

infocom: $(OBJECTS) planetfa.o
	gcc -o infocom.exe planetfa.o $(OBJECTS)

#ns32k stand alone
ns32infocom: $(OBJECTS) crt0.o planetfa.o
	xld -v -nostdlib --strip-all --oformat=binary --script=stand.ldscript -Ttext 0x0 -M -Map infocom.map  -o infocom.bin crt0.o $(OBJECTS) planetfa.o
	32016 infocom.bin


$(OBJECTS): infocom.h
