CFLAGS =  $(TARGET_CFLAGS)
CFLAGS += -I$(BUILDDIR)/
CFLAGS += -I$($(TARGET_SYSTEM_KSP_OS)_INCLUDE)
CFLAGS += -I$($(TARGET_SYSTEM_KSP_TASK)_INCLUDE)
CFLAGS += -I$($(TARGET_SYSTEM_SSP_CAL)_INCLUDE)
CFLAGS += -I$($(TARGET_SYSTEM_SSP_PAL)_INCLUDE)


SRC_H=    $(wildcard *.H)
SRC_C=    $(wildcard *.C)

OBJ_c=    $(SRC_C:%.C=%.c)
OBJ_h=    $(SRC_H:%.H=%.h)

OBJ=    $(OBJ_c:%.c=$(BUILDDIR)/%.o)

default: $(OBJ_c) $(OBJ_h) apes_objects

ifdef MAKEFILE_RULES
include $(MAKEFILE_RULES)
else
include $(APES_HOME)/support/Makefile.rules
endif

app_clean :
	echo '[CLEAN...] $(APP_NAME)/$(APP_NAME_EXT) [h/c]'
	rm -f $(OBJ_c) $(OBJ_h)

%.h: %.H
	$(TARGET_M4) $(TARGET_M4FLAGS) $(MACRO_FILE) $< > $@

%.c: %.C
	$(TARGET_M4) $(TARGET_M4FLAGS) $(MACRO_FILE) $< > $@

