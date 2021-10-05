dnl $Id: config.m4,v 1.10 2002/03/12 01:52:52 david Exp $
dnl config.m4 for extension universe
dnl vim: syntax=config

dnl This is the entry used by configure
PHP_ARG_WITH(universe, whether to enable CORBA support via Universe,
[  --with-universe[=DIR]   Enable CORBA support via Universe (EXPERIMENTIAL).
                          DIR is the base directory for MICO.])

if test "$PHP_UNIVERSE" != "no"; then

  dnl So, we have an extension called universe
	ifdef([PHP_NEW_EXTENSION],[
		PHP_NEW_EXTENSION(universe, \
			php_universe.c \
			fromcorba.cpp \
			implementation.cpp \
			object.cpp \
			struct.cpp \
			tocorba.cpp \
			universe.cpp \
			zpp.cpp \
			zppclass.cpp \
			zppclassentry.cpp \
			zppclassentryregistry.cpp \
			zppexception.cpp \
			zppinstanceregistry.cpp \
			zppmodule.cpp \
			zppproxy.cpp \
			zppvalue.cpp \
			zval_array.cpp \
			zval_var.cpp \
			,$ext_shared,,[-g -Wall -pedantic]) dnl -ansi
	],[
  	PHP_EXTENSION(universe, $ext_shared)
	])

  dnl We want to use these variables in Makefile.in
  PHP_SUBST(UNIVERSE_SHARED_LIBADD)
  PHP_SUBST(UNIVERSE_OS_DEFINES)

  dnl And we use C++
  PHP_REQUIRE_CXX

  dnl Now, where is MICO?
  AC_MSG_CHECKING(for MICO)

  dnl These directories look reasonable
  for i in $PHP_UNIVERSE /usr/local/mico /usr/local /usr; do

    dnl Is there a subdirectory called mico in the include directory?
    if test -d $i/include/mico; then

      dnl Save directory and report result
      MICO_PATH=$i
      AC_MSG_RESULT(found in $i)
			break
    fi
  done

  dnl Abort if the loop above failed
  if test -z "$MICO_PATH"; then
    AC_MSG_RESULT(not found)
    AC_MSG_ERROR(Please (re)install MICO)
  fi

  dnl MICO version check ripped from mico-setup.sh in the libmico2.3.5 Debian package
  AC_MSG_CHECKING(MICO version)
  MICO_VERSION=` sed -n '/MICO_VERSION/ { y/b/./; s#^[^"]*"\([^"]*\)".*$#\1#p; }' \
		"$MICO_PATH/include/mico/version.h" `

	if test -z "$MICO_VERSION"; then
    AC_MSG_RESULT(not found)
    AC_MSG_ERROR(Please (re)install MICO)
	else
	  AC_MSG_RESULT($MICO_VERSION)
  fi

  dnl Figure out OS specific definitions
  ac_uname_s=`uname -s 2>/dev/null`
  case ${ac_uname_s} in
    SunOS)
      UNIVERSE_OS_DEFINES='-D__EXTENSIONS__';;
  esac

  dnl Specify MICO include path and library
  PHP_ADD_INCLUDE($MICO_PATH/include)
  PHP_ADD_LIBRARY_WITH_PATH(mico$MICO_VERSION, $MICO_PATH/lib, UNIVERSE_SHARED_LIBADD)
  PHP_ADD_LIBRARY(stdc++)

  AC_DEFINE(HAVE_UNIVERSE,1,[ ])

fi

