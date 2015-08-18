# 
# $Id$
# 
# [ Description ]
# 
# Makefile
# 

#-----[ directory ]----------------------------------------
SOCLIB				?= $(shell soclib-cc --getpath)
DIR_DOCUMENTATION		= $(SOCLIB)/doc

#-----[ include ]------------------------------------------

all				:
				@\
				$(MAKE) all_documentation

local_clean			:

include				$(DIR_DOCUMENTATION)/Makefile.Documentation
