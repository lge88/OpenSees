include ../../Makefile.def

OBJS       = ConvergenceTest.o \
	CTestNormUnbalance.o \
	CTestNormDispIncr.o \
	CTestEnergyIncr.o \
	CTestRelativeNormUnbalance.o \
	CTestRelativeNormDispIncr.o \
	CTestRelativeEnergyIncr.o \
	CTestFixedNumIter.o \
	CTestRelativeTotalNormDispIncr.o \
	NormDispAndUnbalance.o \
	NormDispOrUnbalance.o

all:            $(OBJS) 

# Miscellaneous
tidy:	
	@$(RM) $(RMFLAGS) Makefile.bak *~ #*# core

clean: tidy
	@$(RM) $(RMFLAGS) $(OBJS) *.o

spotless: clean

wipe: spotless

# DO NOT DELETE THIS LINE -- make depend depends on it.
