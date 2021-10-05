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

#ifndef __zppclass_h__
#define __zppclass_h__

#include "zpp.h"
#include "zpphashtable.h"
#include "zppvalue.h"
#include "zppresource.h"

class zval_array;

class ZppClassEntry;

class ZppClass
{
	friend class ZppResourceType<ZppClass>;
	
	public:
		//
		// public methods
		//

		/**
		 *  Implemented in subtypes and called on object creation 
		 */
		virtual void Create(zval_array& rParameters)
		{
			// do nothing by default
		}

		/**
		 *  Implemented in subtypes and used for dynamic function calls
		 */
		virtual void CallFunction(const char * pFunctionName, 
				zval_array& rParameters, ZppValue& rReturnValue)
		{}
			
		/**
		 *  Implemented in subtypes and used for dynamic property gets
		 */
		virtual void GetProperty(const char * pPropertyName, ZppValue& rValue)
		{}

		/**
		 *  Implemented in subtypes and used for dynamic property puts
		 */
		virtual void PutProperty(const char * pPropertyName,
				ZppValue& rValue)
		{}

		/**
		 *  Implemented in subtypes and used for serialization of objects
		 */
		virtual void Sleep(ZppValue& rObject, ZppValue& rPropertyNames)
		{}
		
		/**
		 *  Implemented in subtypes and used for deserialization of objects
		 */
		virtual void Wakeup(ZppValue& rObject)
		{}

		/**
		 * Implemented in subtypes for type information
		 */
		virtual ZppClassEntry * ClassEntry() = 0;

		//! Connect this object to a value
		void ConnectToValue(ZppValue& rObject);

	protected:
		//
		// protected methods
		//

		/** 
		 * constructor
		 */
		ZppClass();
		
		/** 
		 * destructor, needed?
		 */
		virtual ~ZppClass()
		{
		}

	private:
		//
		// private attributes
		//

		int mResourceValue;
};

#endif // __zppclass_h__

