SUBDIRS = ${APP_NAME_EXT}

BUILDDIR := $(BUILDDIR)/${APP_NAME_EXT}

default: apes_submake

clean : apes_clean

app_clean : 
ifneq ($(strip $(SUBDIRS)),)
	for i in $(SUBDIRS) ; do make -C $$i app_clean || exit $?; done;
endif

ifdef MAKEFILE_RULES
include $(MAKEFILE_RULES)
else
include $(APES_HOME)/support/Makefile.rules
endif

