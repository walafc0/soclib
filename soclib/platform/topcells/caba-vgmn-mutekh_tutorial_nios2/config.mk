## Here you can tweak come configuration options about the kernel that
# will be built.

# You first have to specify where MutekH is on your filesystem, but
# you'd rather use an environment variable.
# Try export MUTEKH_DIR=/path/to/mutekh

MUTEKH_DIR=/Users/charot/soft/mutekh

# You may choose which application to build with MutekH. Here the
# directory is relative to MutekH directory, but this is not mandatory
# at all. You may specify another external directory.
APP=$(MUTEKH_DIR)/examples/hello

# Inside the $APP directory above must exist a MutekH build
# configuration file. This variable is the name of this very file.
CONFIG=config

# Then you may choose to build for mips32el, arm or ppc.
CPU=nios

# We'll assume your configuration file is conditional like explained
# in https://www.mutekh.org/trac/mutekh/wiki/BuildSystem#Advancedsyntax

# Standard configuration files provided in examples expect a ARCH-CPU
# couple, plus a platform name (to compile hardware layout definition
# in the kernel)
BUILD=soclib-$(CPU):pf-tutorial

# Now we can define the expected kernel file
KERNEL=mutekh/hello-soclib-$(CPU).out
