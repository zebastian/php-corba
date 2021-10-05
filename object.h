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

#ifndef __object_h__
#define __object_h__

#include "universe.h"
#include "zppclass.h"
#include "zppclassstatic.h"
#include "implementation.h"

#include <memory>

/**
 * \brief The UniverseObject class in PHP
 */
class UniverseObject : 
		public ZppClass, 
		public ZppClassStatic<UniverseObject>,
		public UniverseImplementationCallback
{
	friend class ZppClassStatic<UniverseObject>;
	friend class std::auto_ptr<UniverseObject>;
	
	public:
		//
		// public methods
		//
		char * Ior();

	public:
		//
		// static public methods
		//

		//! use this to register this as a zend class
		static bool RegisterClass();

		static void ConvertFromCorba(CORBA::Any& rSource, zval* pResult, 
				CORBA::TypeCode_ptr pType);

		static void ConvertToCorba(ZppValue& rSource, CORBA::Any& rDestination, 
				CORBA::TypeCode_ptr pType);

		// Create an instance of this class from an IOR
		static void StringToObject(ZppValue& rIor, ZppValue& rObject);

	private:
		//
		// private types
		//
		typedef std::map<std::string, CORBA::OperationDef_var> OperationDef_map;

	private:
		//
		// private methods
		//

		UniverseObject();
		virtual ~UniverseObject();
		
		//! Create called from parent class
		virtual void Create(zval_array& rParameters);

		//! Create object from IOR
		void Create(const char * pIor);
			
		//! Create object from a CORBA::Object pointer
		void Create(CORBA::Object_ptr pObject);

		//! Create object from a PHP object and a repository ID
		void Create(ZppValue& rObject, const char * pRepositoryId);

		virtual void CallFunction(const char * pFunctionName, 
				zval_array& rParameters, ZppValue& rReturnValue);
		virtual void GetProperty(const char * pPropertyName, ZppValue& rValue);
		virtual void PutProperty(const char * pPropertyName,
				ZppValue& rValue);
		virtual void Sleep(ZppValue& rObject, ZppValue& rPropertyNames);
		virtual void Wakeup(ZppValue& rObject);
		
		virtual ZppClassEntry* ClassEntry() { return mpClassEntry; }

		//! Find operation info
		CORBA::OperationDef_ptr FindOperation(const char * pOperationName);

		//! Create exception list for operation
		void CreateExceptionList(CORBA::OperationDef_ptr operation, 
				CORBA::ExceptionList_out exceptionList);

		//! Create named value for result
		void CreateResult(CORBA::OperationDef_ptr operation,
				CORBA::NamedValue_out result);
		
		//! Create argument list for a CORBA function call
		void CreateArgumentList(CORBA::OperationDef_ptr operation,
				zval_array& rParameters, CORBA::NVList_out rArguments);
		
		//! Find attribute info
		CORBA::AttributeDef_ptr FindAttribute(const char * pAttributeName);

		//! Convert output parameters after a CORBA function call
		void ConvertOutputParameters(CORBA::NVList_ptr arguments, 
			zval_array& rParameters);

		//! Invoke a method on a PHP object 
		virtual void Invoke (CORBA::ServerRequest_ptr request);
		void Invoke_CallFunction(CORBA::ServerRequest_ptr request);

		//! Convert parameters before a Zend function call
		void ConvertZendInputParameters(CORBA::NVList_ptr source, 
				zval_array& rDestination);

		//! Convert parameters after a Zend function call
		void ConvertZendOutputParameters(zval_array& rSource, 
				CORBA::NVList_ptr pDestination);

	private:
		//
		// static private methods
		//

		//! Recursive search for operation in interface and parent interfaces
		static CORBA::OperationDef_ptr FindOperation(
				CORBA::InterfaceDef_ptr pInterface, const char * pOperationName);

		//! Get the active interface repository
		static CORBA::Repository_ptr Repository()
		{
			return UniverseContext::Get().Repository();
		}
	
		//! Get the active object request broker
		CORBA::ORB_ptr Orb()
		{
			if (CORBA::is_nil(mOrb))
				mOrb = CORBA::ORB_var::duplicate(UniverseContext::Get().Orb());
			return mOrb;
		}
	
	private:
		//
		// private attributes
		//

		//! For PHP implementations of CORBA objects
		ZppValue mObject;

		//! The ORB
		CORBA::ORB_var mOrb;

		//! The CORBA object
		CORBA::Object_var mCorbaObject;

		//! The interface info for the CORBA object
		CORBA::InterfaceDef_var mInterface;

#if 0
		//! Cached OperationDef objects
		OperationDef_map mOperationDefCache;
#endif

		UniverseImplementation * mpImplementation;

	private:
		//
		// static private attributes
		//

		//! list of static functions
		static zend_function_entry mStaticFunctions[];

};

#endif // __object_h__
