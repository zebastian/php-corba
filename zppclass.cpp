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

#include "zpp.h"
#include "zppclass.h"
#include "zppclassentry.h"
#include "zppexception.h"
#include "zppinstanceregistry.h"

#include <memory>

using namespace std;

extern "C"
{
#include <zend_list.h>
};

ZppClass::ZppClass()
{
	// add to class resources
	mResourceValue = ZppInstanceRegistry::Get().AddResource(this);
}

/*
 * Resource handle is stored as a long value at property index 0
 */
void ZppClass::ConnectToValue(ZppValue& rObject)
{
#if 0
	cerr << "[ZppClass::ConnectToValue] ";
	DUMP(this);
	DUMP(rObject.Get());
//	DUMP(rObject->value.obj.ce);
//	DUMP(rObject->value.obj.ce->name);
	DUMP((int)rObject->is_ref);
	DUMP(rObject->refcount);
	DUMP(rObject->value.obj.properties);
	DUMP(rObject->value.obj.properties->nNumOfElements);
#endif

#if 0
	if (mValue.IsObject());
		return;

	mValue = rObject;
#endif

	// create value with resource id
	ZppValue handle((long)mResourceValue);

	// release it as a plain zval
	zval * p_handle = handle.Release();
	
	// update object properties
	rObject.Properties().IndexUpdate(
			0,					// index
			&p_handle		// pointer to value
			);
}

