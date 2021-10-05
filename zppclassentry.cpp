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

// $Id: zppclassentry.cpp,v 1.3 2002/03/25 09:46:15 david Exp $
#include "zppclassentry.h"
#include "zppclassentryregistry.h"
#include "zppproxy.h"

ZppClassEntry::ZppClassEntry(const char * pName, 
		FactoryFunctionPtr pFactoryFunction, int flags, 
		zend_function_entry * pStaticFunctions)
	: mpFactoryFunction(pFactoryFunction),
		mFlags(flags)
{
	::memset(&mClassEntry, 0, sizeof(zend_class_entry));
	
	// not using INIT_OVERLOADED_CLASS_ENTRY macro because name is not a literal
	mClassEntry.name = ::strdup(pName);
	mClassEntry.name_length = ::strlen(pName);
	mClassEntry.builtin_functions = pStaticFunctions;
	mClassEntry.handle_function_call =
		/*(flags & hasCallFunction) ?*/ ZppProxy::CallFunction /*: NULL*/;
	mClassEntry.handle_property_get =
		(flags & hasGetProperty) ? ZppProxy::GetProperty : NULL;
	mClassEntry.handle_property_set = 
		(flags & hasPutProperty) ? ZppProxy::PutProperty : NULL;
	
	// add to map
	ZppClassEntryRegistry::Get().AddClassEntry(this);
}

ZppClassEntry::~ZppClassEntry()
{
	::free(mClassEntry.name);
}

void ZppClassEntry::Register()
{
	// register with Zend
#ifdef ZTS
	TSRMLS_FETCH();
	::zend_register_internal_class(&mClassEntry TSRMLS_CC);
#else
	::zend_register_internal_class(&mClassEntry);
#endif
}
