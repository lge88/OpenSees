include ../../../../../Makefile.def

OBJS       = Accelerator.o \
	MillerAccelerator.o naccel.o \
	RaphsonAccelerator.o PeriodicAccelerator.o \
	KrylovAccelerator.o \
	SecantAccelerator1.o SecantAccelerator2.o SecantAccelerator3.o

# MonitoredAccelerator.o \
# KrylovAccelerator2.o \
# DifferenceAccelerator.o \
# DifferenceAccelerator2.o \

# Compilation control

all:         $(OBJS)

# Miscellaneous
tidy:	
	@$(RM) $(RMFLAGS) Makefile.bak *~ #*# core

clean: tidy
	@$(RM) $(RMFLAGS) $(OBJS) *.o

spotless: clean

wipe: spotless

# DO NOT DELETE THIS LINE -- make depend depends on it.
