#!/bin/bash

if [ -z $APES_HOME ] ; then
	echo "[ERROR  ] The APES environment has not been installed"
	echo "[ERROR  ] Please execute the install.sh script in the APES directory"
else

	#
	# Cleaning up things a bit...
	#

	unset MAKEFILE_RULES

	unset TARGET_CAL
	unset TARGET_SYSTEM_KSP_OS
	unset TARGET_SYSTEM_KSP_TASK
	unset TARGET_SYSTEM_ASP_C
	unset TARGET_SYSTEM_ASP_M
	unset TARGET_SYSTEM_ASP_COM
	unset TARGET_SYSTEM_SSP_CAL
	unset TARGET_SYSTEM_SSP_PAL
	unset TARGET_SYSTEM_LIBS

	unset DNA_COMPONENTS
	unset DNA_CFLAGS
	unset DNA_MODULES
	unset DNA_CORE_SERVICES
	unset DNA_DRIVERS
	unset DNA_FILESYSTEMS

	unset TARGET_CC
	unset TARGET_CFLAGS
	unset TARGET_LD
	unset TARGET_LDFLAGS
	unset TARGET_ARFLAGS
	unset TARGET_AR
	unset TARGET_RANLIB
	unset TARGET_LDSCRIPT
	unset TARGET_LIBGCC

	#
	# Including the configuration file
	#

	if [ -e "platform_configurations/$1" ] ; then
		source platform_configurations/$1
		#
		# Print out the configuration
		#

		echo
		echo "[PLATFORM  settings]"
		echo "	| NoC                    : ${TARGET_NOC}"
		echo "	| Processor Number       : ${TARGET_NBP}"
		echo "	| distribution           : ${TARGET_MEM}"
		echo "	| Number of cache lines  : ${TARGET_NCL}"
		echo "	| log2 line size (bytes) : ${TARGET_CLS}"
	else
		echo "[ERROR  ] The configuration file does not exist"
		echo "[ERROR  ] Please create one and restart the installation"
	fi

	if [ -e "app_configurations/$2" ] ; then
		source app_configurations/$2
		
		export TARGET_SYSTEM_LIBS="${TARGET_SYSTEM_SSP_CAL} ${TARGET_SYSTEM_KSP_OS} ${TARGET_SYSTEM_ASP_COM} ${TARGET_SYSTEM_KSP_TASK} ${TARGET_SYSTEM_ASP_C} ${TARGET_SYSTEM_ASP_M}"  

		#
		# Print out the configuration
		#

		echo
		echo "[APP settings]"
		echo "	| Target prefix          : ${TARGET_PREFIX}"
		echo "	| Application Name       : ${APP_NAME}"
		echo "	| Benchmark classe       : ${BENCH_CLASS}"
		echo "	| Application Extention  : ${APP_NAME_EXT}"
		echo "	| Macro_File             : ${MACRO_FILE}"
		echo "	| Ldscript               : ${TARGET_LDSCRIPT}"
		echo
		echo "[APES settings]"
		echo "	| Operating system       : ${TARGET_SYSTEM_KSP_OS}"
		echo "	| Task library           : ${TARGET_SYSTEM_KSP_TASK}"
		echo "	| Communication library  : ${TARGET_SYSTEM_ASP_COM}"
		echo "	| CAL                    : ${TARGET_SYSTEM_SSP_CAL}"
		echo "	| PAL                    : ${TARGET_SYSTEM_SSP_PAL}"
		echo
		echo "[Tool Chain settings]"
		echo "	| M4       : ${TARGET_M4}"
		echo "	| M4FLAGS  : ${TARGET_M4FLAGS}"
		echo "	| CC       : ${TARGET_CC}"
		echo "	| CFLAGS   : ${TARGET_CFLAGS}"
		echo "	| LD       : ${TARGET_LD}"
		echo "	| LDFLAGS  : ${TARGET_LDFLAGS}"
		echo "	| AR       : ${TARGET_AR}"
		echo "	| ARFLAGS  : ${TARGET_ARFLAGS}"
		echo "	| RANLIB   : ${TARGET_RANLIB}"

	else
		echo "[ERROR  ] The configuration file does not exist"
		echo "[ERROR  ] Please create one and restart the installation"
	fi
fi
