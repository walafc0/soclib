bin_PROGRAMS = your_exe_name

your_exe_name_SOURCES = \
	source1.c \
	source2.c \
	main.c

INCLUDES = -I ../common/src

myplayh264_LDADD = ../common/src/libcommonh264.a -lpthread -lm

AM_CFLAGS = -g -O2 -Wall -DBUILD_TESTS -D__USE_GNU -fsigned-char #-pg
