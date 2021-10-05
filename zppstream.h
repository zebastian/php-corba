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

// $Id: zppstream.h,v 1.2 2001/08/12 13:24:34 david Exp $
#ifndef __zppstream_h__
#define __zppstream_h__

#include <strstream>

class strstream_zpp : public std::strstream
{
	public:
		virtual ~strstream_zpp()
		{
#ifdef PHP_WIN32
			rdbuf()->freeze(0);
#else
			freeze(0);
#endif
		}
};

class ostrstream_zpp : public std::ostrstream
{
	public:
		virtual ~ostrstream_zpp()
		{
#ifdef PHP_WIN32
			rdbuf()->freeze(0);
#else
			freeze(0);
#endif
		}
};

#endif // __zppstream_h__

