# Here you can tweak come configuration options about the kernel that
# will be built.

# You first have to specify where MutekH is on your filesystem, but
# you'd rather use an environment variable.
# Try export MUTEKH_DIR=/path/to/mutekh

#MUTEKH_DIR=/home/t0043357/sources/soclib/tools/mutekh

# You may choose which application to build with MutekH. Here the
# directory is relative to MutekH directory, but this is not mandatory
# at all. You may specify another external directory.
APP_DIR=$(shell pwd)/../common/decoder_h264_clone/decoder_soclib

# Then you may choose to build for mips, arm or ppc.
CPU=mips32el
FORMAT=qcif
NB_PROC=3
CACHE_LINE_SIZE=16
FRAMEBUFFER=YES
DBF=YES

GDB_SERVER=NO
MEM_CHECKER=NO
TIMER=NO

# Inside the $APP directory above must exist a MutekH build
# configuration file. This variable is the name of this very file.
CONFIG=config


# We'll assume your configuration file is conditional like explained
# in https://www.mutekh.org/trac/mutekh/wiki/BuildSystem#Advancedsyntax

# Standard configuration files provided in examples expect a ARCH-CPU
# couple, plus a platform name (to compile hardware layout definition
# in the kernel)
BUILD=soclib-$(CPU):pf_decoder_h264:pf_caba

# Now we can define the expected kernel file
KERNEL=mutekh/kernel-soclib-$(CPU).out
