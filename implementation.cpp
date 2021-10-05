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

// $Id: implementation.cpp,v 1.2 2001/12/10 18:02:47 david Exp $
#include "implementation.h"
#include "zppstream.h"

UniverseImplementation::UniverseImplementation(const char* repoid, 
		const char* name, UniverseImplementationCallback * pCallback)
	: mRepositoryId(repoid), mpCallback(pCallback)
{
	try
	{
#if 0
		CORBA::ImplementationDef_var impl = _find_impl( repoid, name );
		//assert( !CORBA::is_nil( impl ) );
		_create_ref( CORBA::BOA::ReferenceData(), 0, impl, repoid );
#endif
	}
	CATCH_CORBA_EXCEPTIONS();
}

//! Implementation of DynamicImplementation abstract interface
void UniverseImplementation::invoke (CORBA::ServerRequest_ptr request/*,
		CORBA::Environment &env*/)
{
	try
	{
		mpCallback->Invoke(request);
	}
	// XXX: catch more here?
	catch (...)
	{
		// XXX: set exception here
		cerr << "[UniverseImplementation::invoke] Exception\n";
#if 0
		CORBA::Exception * ex = 
			new CORBA::BAD_OPERATION (0, CORBA::COMPLETED_NO);
		request->set_exception (ex);
#endif
	}
}

CORBA::RepositoryId UniverseImplementation::_primary_interface (
		const PortableServer::ObjectId& oid, PortableServer::POA_ptr poa)
{
//	cerr << "[UniverseImplementation::_primary_interface]" << endl;
	return CORBA::string_dup(mRepositoryId);
}

