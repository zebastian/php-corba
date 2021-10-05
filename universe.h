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

#ifndef __universe_h__
#define __universe_h__

#define ENABLE_SATELLITE_COMPATIBLITY 1

#include "zpp.h"
#include <php.h>
BEGIN_EXTERN_C()
#include "php_universe.h"
END_EXTERN_C()

#define INI_ORBNoIIOPServer   "universe.ORBNoIIOPServer"
#define INI_ORBImplRepoAddr   "universe.ORBImplRepoAddr"
#define INI_ORBImplRepoIOR    "universe.ORBImplRepoIOR"
#define INI_ORBIfaceRepoAddr	"universe.ORBIfaceRepoAddr"
#define INI_ORBIfaceRepoIOR		"universe.ORBIfaceRepoIOR"
#define INI_ORBNamingAddr     "universe.ORBNamingAddr"
#define INI_ORBNamingIOR      "universe.ORBNamingIOR"
#define INI_ORBIIOPAddr 			"universe.ORBIIOPAddr"
#define INI_ORBDebugLevel 		"universe.ORBDebugLevel"
#define INI_ORBMemTrace       "universe.ORBMemTrace"

#define DFLT_ORBIfaceRepoAddr "inet:localhost:2222"

#ifdef __cplusplus

#include "zppmodule.h"
#include "zppstream.h"

#include <CORBA.h>

class UniverseContext : public ZppContext
{
	public:
		//
		// public methods
		//
		virtual ~UniverseContext();

		//! get pointer to ORB
		CORBA::ORB_ptr Orb();

		//! get pointer to interface repository
		CORBA::Repository_ptr Repository();

		CORBA::Any& Exception();
		void Exception(CORBA::Any* pException);
		bool HasException() throw() { return mException != NULL; }

	public:
		//
		// static public methods
		//

		//! Get active context
		static UniverseContext& Get();

	private:
		//
		// private methods
		//

		//! Initialize CORBA
		bool InitializeCorba();

	private:
		//
		// private attributes
		//

		//! Object Request Broker
		CORBA::ORB_var mOrb;

		//! Interface Repository
		CORBA::Repository_var mRepository;

		CORBA::Any_var mException;
};

class UniverseModule : public ZppModule
{
	public:
		//
		// public methods
		//

		//! Initialize module
		virtual void Initialize();

		//! create context
		ZppContext* CreateContext()
		{
			return new UniverseContext();
		}

		//! Get the active UniverseContext object
		UniverseContext& ActiveContext()
		{
			return static_cast<UniverseContext&>(ZppModule::ActiveContext());
		}

	public:
		//
		// static public methods
		//

		//! Get the active UniverseModule object
		static UniverseModule& Get()
		{
			return static_cast<UniverseModule&>(ZppModule::Get());
		}
};

#endif


BEGIN_EXTERN_C()

/**
 * Should be called on module initialization
 */
zend_bool universe_initialize();
void universe_shutdown();

void universe_request_start();
void universe_request_end();

END_EXTERN_C()

/**
 * Macro to catch corba exceptions and rethrow them as ZppError
 */
#define CATCH_CORBA_EXCEPTIONS() \
	catch (CORBA::COMM_FAILURE & ) \
	{ \
		throw ZppError_("CORBA::COMM_FAILURE"); \
	} \
	catch (CORBA::SystemException & e) \
	{ \
		ostrstream_zpp buffer; \
		buffer << "CORBA::SystemException: "; \
		e._print(buffer); \
		buffer << ends; \
		throw ZppError_(buffer.str()); \
	} \
	catch (CORBA::Exception & e) \
	{ \
		ostrstream_zpp buffer; \
		buffer << "CORBA::Exception: "; \
		e._print(buffer); \
		buffer << ends; \
		throw ZppError_(buffer.str()); \
	}



#endif
//__universe_h__

