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

#include "zval_array.h"
#include "zval_var.h"
#include "zppexception.h"


zval** zval_array::create_empty(unsigned elementCount) throw()
{
	const size_t size = sizeof(zval*) * elementCount;
	zval ** pp_parameters = reinterpret_cast<zval**>(emalloc(size));
	memset(pp_parameters, 0, size);
	return pp_parameters;
}


zval** zval_array::create(unsigned elementCount) throw()
{
	zval ** pp_parameters = create_empty(elementCount);
	for (unsigned i = 0; i < elementCount; i++)
	{
		pp_parameters[i] = zval_var::create();
	}

	return pp_parameters;
}

void zval_array::release() throw()
{
	for (unsigned i = 0; i < mElementCount; i++)
	{
		zval_var::release(mppElements[i]);
		mppElements[i] = NULL;
	}
	mElementCount = 0;
	efree(mppElements);
	mppElements = NULL;
}

/**
 * return element at specified index
 */
zval* zval_array::operator[] (unsigned index)
{
	Assert(index >= 0 && index < mElementCount, ZppOutOfBounds_());
	return mppElements[index];
}


