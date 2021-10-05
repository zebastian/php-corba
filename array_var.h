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

#ifndef __array_var_h__
#define __array_var_h__

#include "zpphashtable.h"
#include "zppvalue.h"
#include "zval_var.h"

/**
 * Wrap an array zval
 */
class array_var
{
	public:
		array_var(zval* pZval)
			: mZval(pZval), 
				mHashTable(pZval->value.ht, pzval_hashtable::IS_NOT_OWNER)
		{
			// Note: pZval has to be an array!
		}
		
		/**
		 * Nice and simple array operator
		 */
		zval* operator[] (unsigned long index) const throw()
		{
			zval** pp_value = NULL;
			if (mHashTable.IndexFind(index, &pp_value) && pp_value)
				return *pp_value;
			return NULL;
		}
		
		zval* operator[] (const char * pKey) const throw()
		{
			zval** pp_value = NULL;
			if (mHashTable.Find(pKey, &pp_value) && pp_value)
				return *pp_value;
			return NULL;
		}
		
		bool next_index_insert(zval* pValue) throw()
		{
			zval* tmp = pValue;
			return mHashTable.IndexAdd(&tmp);
		}

		unsigned count() throw()
		{
			return mHashTable.Size();
		}

	private:
		typedef ZppHashTable<zval*, ZvalPtrDestructor> pzval_hashtable;
	
		zval_var mZval;
		pzval_hashtable mHashTable;
};

#endif // __array_var_h__

