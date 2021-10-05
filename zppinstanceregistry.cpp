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

// $Id: zppinstanceregistry.cpp,v 1.2 2001/08/12 17:30:19 david Exp $
#include "zppinstanceregistry.h"
#include "zppclass.h"
#include "zppmodule.h"
		
ZppInstanceRegistry& ZppInstanceRegistry::Get()
{
	return ZppContext::Get().InstanceRegistry();
}

/*
 * Resource handle is stored as a long value at property index 0
 */
ZppClass * ZppInstanceRegistry::FindInstance(ZppValue & rObject)
{
	// get handle
	zval ** pp_handle = NULL;
	bool success = rObject.Properties().IndexFind(
			0,					// index
			&pp_handle 	// pointer heaven
			);

	if (!success || NULL == pp_handle || NULL == *pp_handle)
	{
		return NULL;
	}

	ZppClass * p_instance = FindResource( Z_LVAL_PP(pp_handle) );

	// set "this" pointer
	if (p_instance)
		p_instance->ConnectToValue(rObject);

	return p_instance;
}

int ZppInstanceRegistry::AddResource(ZppClass* pClass)
{
	// add resource
	return ClassResources().Insert(pClass);
}

ZppClass* ZppInstanceRegistry::FindResource(int resource)
{
	// find resource
	return ClassResources().Find(resource);
}

ZppInstanceRegistry::~ZppInstanceRegistry()
{
	if (mpClassResources)
	{
		delete mpClassResources;
	}
}

