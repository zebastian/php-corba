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

// $Id: zppclassentry.h,v 1.2 2001/08/30 16:55:54 david Exp $
#ifndef __zppclassentry_h_
#define __zppclassentry_h__

#include "zpp.h"

#include <string>
#include <map>

class ZppClass;
class ZppClassEntry;
class ZppValue;

class ZppClassEntry
{
	public:
		//
		// public types
		//

		/**
		 * class flags
		 */
		enum ClassFlags
		{
			hasCallFunction = 1,
			hasGetProperty = 2,
			hasPutProperty = 4,
			hasPutGetProperty = hasPutProperty | hasGetProperty,
			hasEverything = hasCallFunction | hasPutGetProperty,
			hasSerialization = 8
		};

		/** 
		 * Functions of this type are used to create new objects that
		 * are subtypes of ZppClass
		 */
		typedef ZppClass * (*FactoryFunctionPtr)();

	public:
		//
		// public methods
		//

		/**
		 * constructor
		 */
		ZppClassEntry(const char * pName, 
				FactoryFunctionPtr mpFactoryFunction, int flags = hasEverything,
				zend_function_entry * pStaticFunctions = NULL);

		/**
		 * destructor
		 */
		~ZppClassEntry();

		/**
		 * get name attribute
		 */
		const char * Name()
		{
			return mClassEntry.name;
		}

		/**
		 * get the factory function pointer
		 */
		FactoryFunctionPtr FactoryFunction()
		{
			return mpFactoryFunction;
		}

		/**
		 * Get a pointer to the aggregated zend_class_entry structure
		 */
		zend_class_entry * RawClassEntry() { return &mClassEntry; }

		//! Register this class entry with Zend
		void Register();

	private:
		//
		// private attributes
		//

		/** 
		 * pointer to the factory function for this class
		 */
		FactoryFunctionPtr mpFactoryFunction;

		/**
		 * class flags
		 */
		int mFlags;

		/**
		 * Zend's class entry structure
		 */
		zend_class_entry mClassEntry;

};


#endif // __zppclassentry_h__

