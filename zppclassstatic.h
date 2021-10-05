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

// $Id: zppclassstatic.h,v 1.2 2002/03/07 13:14:55 david Exp $
#ifndef __zppclassstatic_h__
#define __zppclassstatic_h__

#include "zppinstanceregistry.h"

template <typename T> 
class ZppClassStatic
{
	public:
		//
		// static public methods
		//

		//! Find an instance of this class
		static T* FindInstance(zval* pZppValue)
		{
			ZppValue value(pZppValue);
			return FindInstance(value);
		}

		//! Find an instance of this class
		static T* FindInstance(ZppValue& rZppValue)
		{
			// verify we got an object
			Assert(rZppValue.IsObject(), ZppTypeMismatchException_());

#if ENABLE_SATELLITE_COMPATIBLITY
			if (::strcasecmp(rZppValue.ClassName(), mpClassAlias) != 0)
#endif
				// verify the object has the expected class name
			Assert(::strcasecmp(rZppValue.ClassName(), mpClassEntry->Name()) == 0,
				ZppTypeMismatchException_());
		
			// find instance of UniverseObject
			T * p_instance = 
				static_cast<T*>(ZppInstanceRegistry::Get().FindInstance(rZppValue));
			Assert(NULL != p_instance, ZppNullPointerException_());

			return p_instance;
		}

	protected:
		//
		// static protected methods
		//

		/**
		 * required for creating objects
		 */
		static ZppClass * FactoryFunction()
		{
			return new T();
		}

	protected:
		//
		// static protected attributes
		//

		/**
		 * class information
		 */
		static ZppClassEntry * mpClassEntry;

#if ENABLE_SATELLITE_COMPATIBLITY
		//! Another name for this class
		static const char * const mpClassAlias;
#endif
};


#endif // __zppclassstatic_h__

