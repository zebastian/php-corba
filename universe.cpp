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

#include "universe.h"
//#include "zppregistry.h"
#include "object.h"
#include "struct.h"
#include "implementation.h"
#include "zppclassentry.h"

#include <memory>
using namespace std;

/* {{{ universe_run()
   Run a CORBA server */
PHP_FUNCTION(universe_run)
{
	try
	{
		Assert(ZEND_NUM_ARGS() == 0, ZppWrongParameterCount_());
		
		try
		{
			UniverseContext::Get().Orb()->run();
		}
		CATCH_CORBA_EXCEPTIONS();
	}
	CATCH_COMMON_EXCEPTIONS();
}
/*}}}*/

/* {{{ universe_object_to_string()
   Get the IOR from a UniverseObject */
PHP_FUNCTION(universe_object_to_string)
{
	try
	{
		zval **arg = NULL;
		Assert(ZEND_NUM_ARGS() == 1 && zend_get_parameters_ex(1, &arg) == SUCCESS, 
				ZppWrongParameterCount_());

		ZppValue object(*arg);

		UniverseObject * p_instance = UniverseObject::FindInstance(object);
		Assert(p_instance, ZppTypeMismatchException_());

		ZppValue result(return_value);
		
		try
		{
			CORBA::String_var str = p_instance->Ior();
			result.String(str);
		}
		CATCH_CORBA_EXCEPTIONS();
	}
	CATCH_COMMON_EXCEPTIONS();
}
/*}}}*/

/* {{{ universe_string_to_object() 
       Create a UniverseObject from an IOR */
PHP_FUNCTION(universe_string_to_object)
{
	try
	{
		// verify parameters
		zval **arg = NULL;
		Assert(ZEND_NUM_ARGS() == 1 && zend_get_parameters_ex(1, &arg) == SUCCESS, 
				ZppWrongParameterCount_());

		// prepare result
		ZppValue result(return_value);
		result.Null();
		
		// convert IOR to PHP object
		ZppValue ior(*arg);
		UniverseObject::StringToObject(ior, result);
	}
	CATCH_COMMON_EXCEPTIONS();
}
/*}}}*/

/* {{{ universe_is_valid()
   Validate a UniverseObject or UniverseStruct */
PHP_FUNCTION(universe_is_valid)
{
	try
	{
		// verify parameters
		zval **arg = NULL;
		Assert(ZEND_NUM_ARGS() == 1 && zend_get_parameters_ex(1, &arg) == SUCCESS, 
				ZppWrongParameterCount_());
		
		// find instance
		ZppValue object(*arg);
		ZppClass * p_instance = ZppInstanceRegistry::Get().FindInstance(object);

		// return boolean
		ZppValue result(return_value);
		result.Bool(p_instance != NULL);
	}
	CATCH_COMMON_EXCEPTIONS();
}
/*}}}*/

/* {{{ universe_has_interface()
   See if we have access to a certain interface in the interface repository */
PHP_FUNCTION(universe_has_interface)
{
	try
	{
		// verify parameters
		zval **arg = NULL;
		Assert(ZEND_NUM_ARGS() == 1 && zend_get_parameters_ex(1, &arg) == SUCCESS, 
				ZppWrongParameterCount_());
		
		ZppValue interface_name(*arg);

		ZppValue result(return_value);
		result.Bool(false);
		
		CORBA::Contained_var contained = UniverseContext::Get().Repository()->lookup_id(interface_name.String());
		if (!CORBA::is_nil(contained))
		{
			CORBA::InterfaceDef_var interfacedef = CORBA::InterfaceDef::_narrow(contained);
			if (!CORBA::is_nil(interfacedef))
			{
				result.Bool(true);
			}
		}
	}
	CATCH_COMMON_EXCEPTIONS();
}
/*}}}*/

/* {{{ universe_set_exception()
   Set a CORBA exception */
PHP_FUNCTION(universe_set_exception)
{
	try
	{
		// verify parameters
		zval **arg = NULL;
		Assert(ZEND_NUM_ARGS() == 1 && zend_get_parameters_ex(1, &arg) == SUCCESS, 
				ZppWrongParameterCount_());

		UniverseStruct * p_instance = UniverseStruct::FindInstance(*arg);

		try
		{
			UniverseContext::Get().Exception(p_instance->CreateException());
		}
		CATCH_CORBA_EXCEPTIONS();
	}
	CATCH_COMMON_EXCEPTIONS();
} /* }}} */
	
/* {{{ universe_initialize()
   Initialize Universe extension */
zend_bool universe_initialize()
{
	try
	{
		ZppModule::Set( new UniverseModule() );
		ZppModule::Get().Initialize();
		return true;
	}
	catch (...)
	{
		// TODO: handle error
		return false;
	}
}
/*}}}*/

void universe_shutdown()
{
	ZppModule::Destroy();
}

void universe_request_start()
{
}

void universe_request_end()
{
}

UniverseContext::~UniverseContext()
{
}

UniverseContext& UniverseContext::Get()
{
	return UniverseModule::Get().ActiveContext();
}

void UniverseModule::Initialize()
{
	ZppModule::Initialize();		// Very important to call parent class here

	UniverseObject::RegisterClass();
	UniverseStruct::RegisterClass();
}

bool UniverseContext::InitializeCorba()
{
	int argc = 1;
	char * argv[] = { "universe", 
										 NULL, NULL,	// for ORBDebugLevel
										 NULL, NULL,	// for ORBImplRepoAddr or ORBImplRepoIOR
										 NULL, NULL,	// for ORBIfaceRepoAddr or ORBIfaceRepoIOR
										 NULL, NULL,	// for ORBNamingRepoAddr or ORBNamingRepoIOR
										 NULL, NULL,	// for ORBNoIIOPServer or ORBIIOPAddr
										 NULL,				// for ORBMemTrace
										 NULL };

	try
	{
		//
		// ORBDebugLevel
		//
		char * p_ORBDebugLevel = INI_STR(INI_ORBDebugLevel);
		if (p_ORBDebugLevel)
		{
			argv[argc++] = "-ORBDebugLevel";
			argv[argc++] = p_ORBDebugLevel;
		}

		// 
		// ORBImplRepoAddr or ORBImplRepoIOR
		//
		char * p_ORBImplRepoAddr = INI_STR(INI_ORBImplRepoAddr);
		if (p_ORBImplRepoAddr && ('\0' != p_ORBImplRepoAddr[0]))
		{
			argv[argc++] = "-ORBImplRepoAddr";
			argv[argc++] = p_ORBImplRepoAddr;
		}
		else
		{
			char * p_ORBImplRepoIOR = INI_STR(INI_ORBImplRepoIOR);
			// Maybe we have the ORBImplRepoIOR option too?
			if (p_ORBImplRepoIOR && ('\0' != p_ORBImplRepoIOR[0]))
			{
				argv[argc++] = "-ORBImplRepoIOR";
				argv[argc++] = p_ORBImplRepoIOR;
			}
		}

		// 
		// ORBIfaceRepoAddr or ORBIfaceRepoIOR
		//
		char * p_ORBIfaceRepoAddr = INI_STR(INI_ORBIfaceRepoAddr);
		if (p_ORBIfaceRepoAddr && ('\0' != p_ORBIfaceRepoAddr[0]))
		{
			argv[argc++] = "-ORBIfaceRepoAddr";
			argv[argc++] = p_ORBIfaceRepoAddr;
		}
		else
		{
			char * p_ORBIfaceRepoIOR = INI_STR(INI_ORBIfaceRepoIOR);
			// Maybe we have the ORBIfaceRepoIOR option too?
			if (p_ORBIfaceRepoIOR && ('\0' != p_ORBIfaceRepoIOR[0]))
			{
				argv[argc++] = "-ORBIfaceRepoIOR";
				argv[argc++] = p_ORBIfaceRepoIOR;
			}
			else
			{
				argv[argc++] = "-ORBIfaceRepoAddr";
				argv[argc++] = DFLT_ORBIfaceRepoAddr;
			}
		}

		// 
		// ORBNamingAddr or ORBNamingIOR
		//
		char * p_ORBNamingAddr = INI_STR(INI_ORBNamingAddr);
		if (p_ORBNamingAddr && ('\0' != p_ORBNamingAddr[0]))
		{
			argv[argc++] = "-ORBNamingAddr";
			argv[argc++] = p_ORBNamingAddr;
		}
		else
		{
			char * p_ORBNamingIOR = INI_STR(INI_ORBNamingIOR);
			if (p_ORBNamingIOR && ('\0' != p_ORBNamingIOR[0]))
			{
				argv[argc++] = "-ORBNamingIOR";
				argv[argc++] = p_ORBNamingIOR;
			}
		}

		//
		// ORBNoIIOPServer or ORBIIOPAddr
		//
		if (INI_INT(INI_ORBNoIIOPServer))
		{
			argv[argc++] = "-ORBNoIIOPServer";
		}
		else
		{
			char * p_ORBIIOPAddr = INI_STR(INI_ORBIIOPAddr);
			if (p_ORBIIOPAddr && ('\0' != p_ORBIIOPAddr[0]))
			{
				argv[argc++] = "-ORBIIOPAddr";
				argv[argc++] = p_ORBIIOPAddr;
			}
		}

		// Maybe we have the ORBMemTrace option too?
		if (INI_INT(INI_ORBMemTrace))
		{
			argv[argc++] = "-ORBMemTrace";
		}

		mOrb = CORBA::ORB_init(argc, argv, "mico-local-orb");
		
		return true;
	}
	CATCH_CORBA_EXCEPTIONS()
	
	return false;
}

//! get pointer to ORB
CORBA::ORB_ptr UniverseContext::Orb()
{
	if (CORBA::is_nil(mOrb))
	{
		InitializeCorba();
		Assert(mOrb, ZppError_("No ORB available"));
	}
	return mOrb;
}

//! get pointer to interface repository
CORBA::Repository_ptr UniverseContext::Repository()
{
	if (CORBA::is_nil(mRepository) || mRepository->_non_existent())
	{
		CORBA::Object_var obj =
      Orb()->resolve_initial_references ("InterfaceRepository");
    mRepository = CORBA::Repository::_narrow (obj);

		Assert(mRepository, ZppError_("No interface repository available"));
	}
	return mRepository;
}

CORBA::Any& UniverseContext::Exception()
{
	Assert(HasException(), ZppNullPointerException_());
	return *mException;
}

void UniverseContext::Exception(CORBA::Any* pException)
{
	mException = pException;
}

