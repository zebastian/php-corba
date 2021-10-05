/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: David Eriksson <eriksson@php.net>                            |
   +----------------------------------------------------------------------+
 */

#ifndef __zpp_h__
#define __zpp_h__

#ifdef PHP_WIN32

/* get rid of VC++ warning about truncation of long debug identifiers */
#pragma warning ( disable : 4786 )

#ifdef __cplusplus
#include <iostream>
#include <cmath>
#include <strstream>
#define istream std::istream
#define _INC_IOSTREAM
#endif

#define __PRETTY_FUNCTION__ __FILE__

#endif
// PHP_WIN32

#ifdef __cplusplus
extern "C"
{
#endif
	
/*
 * C only code below
 */

/*
 * Needed for Zend headers for compiling with -ansi
 */
#define _GNU_SOURCE 1

/**
	PHP headers (for strcasecmp etc on win32)
*/
#include <php.h>

/**
 * Zend headers
 */
#include <zend.h>
#include <zend_API.h>
#include <zend_ini.h>

/*
 * And remove it
 */
#undef _GNU_SOURCE

#if 0 /* these may be needed for some systems */
#ifndef strcasecmp
int      strcasecmp(const char *, const char *);
#endif
#ifndef strdup
char    *strdup(const char *);
#endif
#endif

/**
 * php.ini entry
 */
#define ZPP_INI_AbortOnException "zpp.AbortOnException"

/**
 * function that handles modifications of ZPP_INI_AbortOnException
 */
ZEND_INI_MH(Zpp_AbortOnException_OnChange);


#ifdef __cplusplus
};
#endif
// __cplusplus

#define DUMP(x) cerr << "[" __FILE__ ":" << __LINE__ << "] " << #x << " = " << x << endl;


#endif
//__zpp_h__

