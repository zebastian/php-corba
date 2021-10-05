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

// $Id: zppclassentryregistry.cpp,v 1.2 2001/08/12 17:30:19 david Exp $
#include "zppclassentryregistry.h"
#include "zppclass.h"
#include "zppclassentry.h"
#include "zppmodule.h"
		
ZppClassEntryRegistry& ZppClassEntryRegistry::Get()
{
	return ZppModule::Get().ClassEntryRegistry();
}

void ZppClassEntryRegistry::AddClassEntry(ZppClassEntry* pClassEntry)
{
	// convert name to lowercase
	char * p_name_lowercase = ::estrdup(pClassEntry->Name());
	zend_str_tolower(p_name_lowercase, strlen(p_name_lowercase));
	
	// store class info in map
	mNameToEntry[p_name_lowercase] = pClassEntry;
	
	::efree(p_name_lowercase);

	pClassEntry->Register();
}

#if 0
void ZppClassEntryRegistry::RegisterClassEntries()
{
	for (ZppNameToEntryMap::iterator item = mNameToEntry.begin(); 
			item != mNameToEntry.end(); item++)
	{
		item->second->Register();
	}
}
#endif

ZppClassEntry* ZppClassEntryRegistry::FindClassEntry(const char* pClassName)
{
	// convert name to lowercase
	char * p_name_lowercase = estrdup(pClassName);
	::zend_str_tolower(p_name_lowercase, strlen(p_name_lowercase));
		
	// find class info in map
	ZppClassEntry * p_entry = NULL;
	ZppNameToEntryMap::iterator item = mNameToEntry.find(p_name_lowercase);
	
	bool success = (item != mNameToEntry.end());
	if (success)
		p_entry = item->second;

	::efree(p_name_lowercase);
	
	if (!success)
		return NULL;

	return p_entry; 
}

bool ZppClassEntryRegistry::IsClass(const char* pClassName)
{
	return NULL != FindClassEntry(pClassName);
}

ZppClass* ZppClassEntryRegistry::CreateInstance(const char* pClassName)
{
	ZppClassEntry* p_entry = FindClassEntry(pClassName);
	if (!p_entry)
		return NULL;

	return (*p_entry->FactoryFunction())();
}
		
ZppClassEntryRegistry::~ZppClassEntryRegistry()
{
#if 0
	for (ZppNameToEntryMap::iterator item = mNameToEntry.begin();
			item != mNameToEntry.end(); item++)
	{
		ZppClassEntry * p = item->second;
		delete p;
	}
#endif
}

