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

#ifndef __struct_h__
#define __struct_h__

#include "universe.h"
#include "zppvalue.h"
#include "zppclass.h"
#include "zppclassstatic.h"

#include <string>

class UniverseStruct : public ZppClass, public ZppClassStatic<UniverseStruct>
{
	friend class ZppClassStatic<UniverseStruct>;
	
	public:
		//
		// public methods
		//

		bool IsException() const throw()
		{
			return mIsException;
		}

		CORBA::Any* CreateException();
	
	public:
		//
		// static public methods
		//

		//! use this to register this as a zend class
		static bool RegisterClass();

		static void ConvertToCorba(ZppValue& rSource, CORBA::Any& rDestination, CORBA::TypeCode_ptr pType);
		static void ConvertFromCorba(CORBA::Any& rSource, ZppValue& rDestination, CORBA::TypeCode_ptr pType);

	private:
		//
		// private methods
		//

		/**
		 * constructor
		 */
		UniverseStruct();
		
		/**
		 * our own Create
		 */
		void Create(const char * pRepositoryId, bool initializeMembers = true);

		/**
		 * another Create of our own
		 */
		void Create(CORBA::TypeCode_ptr pType);

		/**
		 * initialize struct members
		 */
		void InitializeMembers();

		/**
		 * functions called from ZppClass
		 */
		virtual void Create(zval_array& rParameters);
		virtual void GetProperty(const char * pPropertyName, ZppValue& rValue);
		virtual void PutProperty(const char * pPropertyName,
				ZppValue& rValue);
		virtual void Sleep(ZppValue& rObject, ZppValue& rPropertyNames);
		virtual void Wakeup(ZppValue& rObject);
		virtual ZppClassEntry * ClassEntry()
		{
			return mpClassEntry;
		}

		void RealConvertToCorba(CORBA::Any& rDestination, CORBA::TypeCode_ptr pType);
		void RealConvertFromCorba(CORBA::Any& rSource, ZppValue& rDestination, CORBA::TypeCode_ptr pType);

		//! Get struct members hashtable
		ZvalHashTable& Members()
		{
			return mMembers.HashTable();
		}	

		CORBA::StructMemberSeq* GetStructMemberSeq();

	private:
		//
		// static private methods
		//

		static CORBA::Repository_ptr Repository()
		{
			return UniverseContext::Get().Repository();
		}

	private:
		//
		// private attributes
		//

		ZppValue mMembers;
		bool mIsException;
		CORBA::StructDef_var mStruct;
		CORBA::ExceptionDef_var mException;
		std::string mRepositoryId;

	private:
		//
		// static private attributes
		//

		//! list of static functions
		static zend_function_entry mStaticFunctions[];
};

#endif // __zppstruct_h__
