ARCH=mipsel
SIMULATION_ARGS=100000
SOCLIB?=$(shell soclib-cc --getpath)
export SOCLIB
include $(SOCLIB)/soclib/platform/topcells/all.mk

post_test: test.out
	for i in 0 1 2 3 ; do \
		count="$$(grep -c 'Hello from cpu '$$i $(TEST_OUTPUT))" ; \
		test "$${count}" = 100 || ( \
			echo "Got $${count} greetings from cpu $${i}, expected 100" ; \
			exit 1 ) ; \
	done
