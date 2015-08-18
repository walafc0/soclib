ARCH=mips32el
SIMULATION_ARGS=100000
SOCLIB?=$(shell soclib-cc --getpath)
export SOCLIB
N_INITS?=1
SOFT=soft/bin$(N_INITS)proc.soft
include $(SOCLIB)/soclib/platform/topcells/all.mk
