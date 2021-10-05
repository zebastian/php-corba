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

// $Id: zppclassentryregistry.h,v 1.2 2001/08/12 17:30:19 david Exp $
#ifndef __zppclassentryregistry_h__
#define __zppclassentryregistry_h__

#include "zppclass.h"
#include "zppresource.h"

#include <map>
#include <string>

class ZppClassEntry;
class ZppContext;

/**
 * \brief Keep control of class entries and class instances
 */
class ZppClassEntryRegistry
{
	friend class ZppModule;
	
	public:
		//
		// public methods
		//

		//! add a class entry to registry
		void AddClassEntry(ZppClassEntry* pClassEntry);

		//!	find a class entry
		ZppClassEntry* FindClassEntry(const char* pClassName);

		//! verify that a class exists
		bool IsClass(const char* pClassName);

		//! create an instance of a class
		ZppClass* CreateInstance(const char* pClassName);

	public:
		//
		// static public methods
		//

		//! get singleton instance
		static ZppClassEntryRegistry& Get();

	private:
		//
		// private types
		//

		//! Short name for long type
		typedef std::map<std::string, ZppClassEntry*> ZppNameToEntryMap;

	private:
		//
		// private methods
		//

		//! Private constructor so only friends can create the class
		ZppClassEntryRegistry()
		{}

		virtual ~ZppClassEntryRegistry();

	private:
		//
		// private attributes
		//

		//! Map class names to class entries
		ZppNameToEntryMap mNameToEntry;
};

#endif // __zppclassentryregistry_h__

