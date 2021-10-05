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

// $Id: zppproxy.cpp,v 1.7 2002/02/04 23:35:52 david Exp $
#include "zppproxy.h"
#include "zppclass.h"
#include "zppinstanceregistry.h"
#include "zppclassentryregistry.h"
#include "zppvalue.h"
#include "zppexception.h"
#include "zval_array.h"

#include <memory>
using namespace std;

void ZppProxy::CallFunction(INTERNAL_FUNCTION_PARAMETERS,
		zend_property_reference * pPropertyReference)
{
	// get function name
	::zend_overloaded_element * p_name_element =
		(zend_overloaded_element *)pPropertyReference->elements_list->tail->data;

	try
	{
		Assert(
				pPropertyReference->elements_list->head ==
				pPropertyReference->elements_list->tail,
				ZppError_("Cannot handle nested function calls"));
		
		char * p_name = p_name_element->element.value.str.val;

		// get object 
		ZppValue object(this_ptr);

		// XXX: is pPropertyReference->object == this_ptr (internal parameter) ?

		// verify class name
		Assert(
			ZppClassEntryRegistry::Get().IsClass(object.ClassName()), 
			ZppError_("Invalid class name"));

		// handle parameters
		zval_array arguments(ZEND_NUM_ARGS(), 
				zval_array::IS_OWNER, zval_array::ALLOCATE_ARRAY);
			
		int rv = ::zend_get_parameters_array(ht, ZEND_NUM_ARGS(), arguments);
		Assert(rv == SUCCESS, ZppError_("Failed to get parameters"));

		for (int i = 0; i < ZEND_NUM_ARGS(); i++)
		{
			ZVAL_ADDREF(arguments[i]);
		}

		// constructor or normal function?
		if (::zend_llist_count(pPropertyReference->elements_list) == 1
				&& !strcasecmp(p_name, object.ClassName()))
		{
			//
			// Constructor
			//
			
			// allocate object
			ZppClass * p_instance = 
				ZppClassEntryRegistry::Get().CreateInstance(object.ClassName());
			Assert(p_instance, ZppError_("Object allocation failed"));

			// save instance pointer
			p_instance->ConnectToValue(object);

			// construct object
			p_instance->Create(arguments);
		}
		else 
		{
			//
			// Normal function
			//
			
			// get class instance
			ZppClass * p_instance = ZppInstanceRegistry::Get().FindInstance(object);
			Assert(p_instance, ZppError_("Unable to get class instance"));
			
			// Wrap return value
			ZppValue result(return_value);
			result.Null();
			
			// call function
			p_instance->CallFunction(p_name, arguments, result);
		}
	}
	CATCH_COMMON_EXCEPTIONS();

	// seems to be required!
	::zval_dtor(&p_name_element->element);
}

zval ZppProxy::GetProperty(zend_property_reference * pPropertyReference)
{
	ZppValue value;

	try
	{
		zend_llist_element * element = NULL;
		
		// property value starting with the object for conveniance
		value.Assign(ZppValue::Duplicate(pPropertyReference->object) );

		// loop through all elements in the property reference list
		for (element = pPropertyReference->elements_list->head; 
			element; element = element->next)
		{
			// verify we are dealing with objects here
			if (!value.IsObject())
			{
				throw ZppError_("Not an object");
				break;
			}

			// get pointer to zend_overloaded_element struct
			zend_overloaded_element * overloaded_property = 
				reinterpret_cast<zend_overloaded_element *>(element->data);
	
			// look at the property type	
			switch (overloaded_property->type)
			{
				case OE_IS_ARRAY:
					throw ZppError_("Cannot handle arrays");
					break;

				case OE_IS_OBJECT:
					{
						// get class instance
						ZppClass * p_instance = ZppInstanceRegistry::Get().FindInstance(value);
						Assert(p_instance, ZppError_("Unable to get class instance"));

						char * p_name = overloaded_property->element.value.str.val;
						
						// get property
						p_instance->GetProperty(p_name, value);
					}
					break;

				case OE_IS_METHOD:
					throw ZppError_("Cannot handle methods");
					break;

				default:
					throw ZppError_("Unknown property type");
					break;
			
			} // switch(overloaded_property->type)
					
			::zval_dtor(&overloaded_property->element);
		
		} // for(all elements)

		if (element)
		{
			// for loop not finished, reset return value
			value.Null();
		}
	}
	CATCH_COMMON_EXCEPTIONS();

	// make sure we have a valid value
	if (NULL == value.Get())
	{
		value.Null();
	}
	
	// Ugly piece of code here... :-(
	zval* p_result = value.Release();
	zval result = *p_result;
	FREE_ZVAL(p_result);
	return result;
}

int ZppProxy::PutProperty(
				zend_property_reference * pPropertyReference, 
				zval * pValue)
{
	int result = FAILURE;

	try
	{
		zend_llist_element * element = NULL;
		
		// property value, starting with the object for conveniance
		ZppValue object;
		
		object.Assign(ZppValue::Duplicate(pPropertyReference->object) );

		// loop through all elements in the property reference list
		for (element = pPropertyReference->elements_list->head; 
			element; element = element->next)
		{
			// verify we are dealing with objects here
			if (!object.IsObject())
			{
				throw ZppError_("Not an object");
				break;
			}

			// get pointer to zend_overloaded_element struct
			zend_overloaded_element * overloaded_property = 
				reinterpret_cast<zend_overloaded_element *>(element->data);
	
			// look at the property type	
			switch (overloaded_property->type)
			{
				case OE_IS_ARRAY:
					throw ZppError_("Cannot handle arrays");
					break;

				case OE_IS_OBJECT:
					{
						// get class instance
						ZppClass * p_instance = ZppInstanceRegistry::Get().FindInstance(object);
						Assert(p_instance, ZppError_("Unable to get class instance"));

						char * p_name = overloaded_property->element.value.str.val;
						
						if (element == pPropertyReference->elements_list->tail)
						{
							// Put property if this is the last element in the list
							ZppValue value(pValue);
							// put property
							p_instance->PutProperty(p_name, value);
						}
						else
						{
							// get property
							p_instance->GetProperty(p_name, object);
						}
					}
					break;

				case OE_IS_METHOD:
					throw ZppError_("Cannot handle methods");
					break;

				default:
					throw ZppError_("Unknown property type");
					break;
			
			} // switch(overloaded_property->type)
					
			::zval_dtor(&overloaded_property->element);
		
		} // for(all elements)

	}
	CATCH_COMMON_EXCEPTIONS();

	return result;
}

void ZppProxy::Sleep(INTERNAL_FUNCTION_PARAMETERS)
{
	try
	{
		ZppValue object(this_ptr);

#if 0
		// get class info
		ZppClassEntry * p_info = ZppClassEntry::FindClassInfo(object.ClassName());
		Assert(p_info, ZppError_("Class info not found"));
#endif
		// get class instance
		ZppClass * p_instance = ZppInstanceRegistry::Get().FindInstance(object);
		Assert(p_instance, ZppError_("Unable to get class instance"));

		// create array of property names in return value
		ZppValue property_names(return_value);
		property_names.Array();

#if 0
		// save instance
		p_instance->ConnectToValue(object);
#endif

		// run sleep to know what property names to save
		p_instance->Sleep(object, property_names);
	}
	CATCH_COMMON_EXCEPTIONS();
}

void ZppProxy::Wakeup(INTERNAL_FUNCTION_PARAMETERS)
{
	try
	{
		ZppValue object(this_ptr);

#if 0
		// get class info
		ZppClassEntry * p_info = ZppClassEntry::FindClassInfo(object.ClassName());
		Assert(p_info, ZppError_("Class info not found"));
#endif

		// allocate object
		ZppClass * p_instance = 
			ZppClassEntryRegistry::Get().CreateInstance(object.ClassName());
		Assert(p_instance, ZppError_("Object allocation failed"));

		// save instance
		p_instance->ConnectToValue(object);

		// wakeup object
		p_instance->Wakeup(object);
	}
	CATCH_COMMON_EXCEPTIONS();
}

