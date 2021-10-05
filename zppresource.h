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

#ifndef __zppresource_h__
#define __zppresource_h__

#include "zpp.h"
#include "zppmodule.h"

/**
 * \brief Type-safe resources.
 *
 * The T objects must be allocated with the new operator.
 */
template <typename T>
class ZppResourceType
{
	public:
		//
		// public methods
		//

		// register destructor
		ZppResourceType()
		{
			// register_list_destructors is macro that need module_number defined
			int module_number = ZppModule::Get().ModuleNumber();
			mResourceHandle = register_list_destructors(ResourceDestructor, NULL);
		}
		
		// insert a resource
		int Insert(T * p)
		{
			return ::zend_list_insert(p, mResourceHandle);
		}
		
		// find a resource
		T * Find(int handle)
		{
			int type;
#ifdef ZTS
			TSRMLS_FETCH();
#endif
			void * p = ::zend_list_find(handle, &type);
			if (p != NULL || type == mResourceHandle)
				return static_cast<T *>(p);
			else
				return NULL;
		}

	private:
		//
		// static private methods
		//

		// destroy a resource
		static void ResourceDestructor(void * p)
		{
			delete static_cast<T *>(p);
		}

	private:
		//
		// private attributes
		//

		int mResourceHandle;

};

#endif //__zppresource_h__
