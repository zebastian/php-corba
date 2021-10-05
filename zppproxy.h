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

// $Id: zppproxy.h,v 1.2 2002/02/04 20:34:16 david Exp $
#ifndef __zppproxy_h__
#define __zppproxy_h__

#include "zpp.h"
#include "zppclassentryregistry.h"

class ZppValue;

class ZppProxy
{
	public:
		//
		// public types
		//

	public:
		//
		// static public methods 
		//

		/**
		 * used by Zend to call a function
		 */
		static void CallFunction(INTERNAL_FUNCTION_PARAMETERS,
				zend_property_reference * pPropertyReference);

		/**
		 * used by Zend to get a property
		 */
		static zval GetProperty(
				zend_property_reference * pPropertyReference);

		/**
		 * used by Zend to put a property
		 */
		static int PutProperty(
				zend_property_reference * pPropertyReference, 
				zval * pValue);

		/**
		 * Called to put an object to sleep
		 */
		static void Sleep(INTERNAL_FUNCTION_PARAMETERS);

		/**
		 * called to wakeup an object
		 */
		static void Wakeup(INTERNAL_FUNCTION_PARAMETERS);

#if 0
		/**
		 * handle a static function
		 */
		template <typename T>
		static void CallStaticFunction(void (T::*pFunction)(ZppValueArray &, ZppValue &), 
				INTERNAL_FUNCTION_PARAMETERS)
		{
			// get object 
			ZppValue object(this_ptr);

			try
			{
				// verify class name
				Assert(
					ZppClassEntryRegistry::Get().IsClass(object.ClassName()), 
					ZppError_("Invalid class name"));

				// handle parameters
				zval ** pp_arguments = new zval *[ZEND_NUM_ARGS()];
		//				auto_ptr<zval *> arguments_cleanup(pp_arguments);
					
				int rv = ::zend_get_parameters_array(ht, ZEND_NUM_ARGS(), pp_arguments);
				Assert(rv == SUCCESS, ZppError_("Failed to get parameters"));

				// wrap arguments in nice array
				ZppValueArray arguments(ZEND_NUM_ARGS(), pp_arguments);

				// get class instance
				T * p_instance = static_cast<T*>(ZppClassEntryRegistry::Get().FindInstance(object));
				Assert(p_instance, ZppError_("Unable to get class instance"));
				
				// Wrap return value
				ZppValue result(return_value);
				
				// Call function
				(p_instance->*pFunction)(arguments, result);
			}
			CATCH_COMMON_EXCEPTIONS();
		}
#endif

	private:
		//
		// private methods 
		//

		//! Private constructor to prevent instances of this class
		ZppProxy() {}
	
};

#endif // __zppproxy_h__
