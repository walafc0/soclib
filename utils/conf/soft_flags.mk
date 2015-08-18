mipseb_CC_PREFIX=mipsel-unknown-elf-
mipseb_CFLAGS=-mips2 -mno-branch-likely -gstabs+ -DSOCLIB_MIPS_R3000 -EB
mipseb_LDFLAGS=-EB

mipsel_CC_PREFIX=mipsel-unknown-elf-
mipsel_CFLAGS=-mips2 -mno-branch-likely -gstabs+ -DSOCLIB_MIPS_R3000 -EL
mipsel_LDFLAGS=-EL

mips32eb_CC_PREFIX=mipsel-unknown-elf-
mips32eb_CFLAGS=-mips32 -gstabs+ -DSOCLIB_MIPS32 -EB
mips32eb_LDFLAGS=-EB

mips32el_CC_PREFIX=mipsel-unknown-elf-
mips32el_CFLAGS=-mips32 -gstabs+ -DSOCLIB_MIPS32 -EL
mips32el_LDFLAGS=-EL

powerpc_CC_PREFIX=powerpc-unknown-elf-
powerpc_CFLAGS=-mcpu=405 -mstrict-align -gstabs+

mpc7447a_CC_PREFIX=powerpc-7450-linux-gnu-
mpc7447a_CFLAGS=-nostdinc -gstabs+
mpc7447a_LDFLAGS=-nostdlib

microblaze_CC_PREFIX=mb-
microblaze_CFLAGS=-mno-xl-soft-div -mno-xl-soft-mul -gstabs+
microblaze_LDFLAGS=-nostdlib

sparc_CC_PREFIX=sparc-unknown-elf-
sparc_CFLAGS= -mcpu=v8
sparc_LDFLAGS= 

nios2_CC_PREFIX=nios2-unknown-elf-
nios2_CFLAGS=-mhw-mul -mhw-div

arm7tdmi_CC_PREFIX = arm-unknown-linux-gnu-
arm7tdmi_CFLAGS = -mcpu=arm7tdmi -mlittle-endian -nostdinc -gstabs+
arm7tdmi_LDFLAGS = -A armv4t -EL -nostdlib

arm966_CC_PREFIX = arm-unknown-linux-gnu-
arm966_CFLAGS = -mcpu=arm9tdmi -mlittle-endian -nostdinc -gstabs+
arm966_LDFLAGS = -A armv5te -EL -nostdlib

arm_CC_PREFIX = arm-unknown-elf-
arm_CFLAGS = -mlittle-endian -march=armv6k -msoft-float -fsigned-char -mfloat-abi=softfp -mfpu=vfp
arm_LDFLAGS = -EL

lm32_CC_PREFIX 	= 	lm32-elf-
lm32_CFLAGS 	+=  -gstabs+ -mmultiply-enabled -mdivide-enabled -msign-extend-enabled -mbarrel-shift-enabled
lm32_LDFLAGS 	+=  -nostdlib 

-include $(HOME)/.soclib/soft_compilers.conf
-include $(PLATFORM_DIR)/soft_flags.conf
