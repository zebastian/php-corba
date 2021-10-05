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

// $Id: implementation.h,v 1.1 2001/08/05 15:27:01 david Exp $
#ifndef __implementation_h__
#define __implementation_h__

#include "universe.h"
#include "zppclass.h"
#include "zppclassstatic.h"

/**
 * interface for callback for UniverseImplementation
 */
class UniverseImplementationCallback
{
	public:
		//
		// public methods
		//

		virtual void Invoke (CORBA::ServerRequest_ptr request) = 0;
};

class UniverseImplementation :
		public PortableServer::DynamicImplementation
{
	public:
		//
		// public methods
		//

		UniverseImplementation(const char* repoid, 
				const char* name, UniverseImplementationCallback * pCallback);
			
		//! Implementation of DynamicImplementation abstract interface
		virtual void invoke (CORBA::ServerRequest_ptr request/*,
				CORBA::Environment &env*/);

		//! PortableServer::DynamicImplementation::_primary_interface
	 	virtual CORBA::RepositoryId _primary_interface (
				const PortableServer::ObjectId& oid, PortableServer::POA_ptr poa);

	private:
		//
		// private attributes
		//

		CORBA::RepositoryId_var mRepositoryId;
		UniverseImplementationCallback * mpCallback;
};


#endif // __implementation_h__

