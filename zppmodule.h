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

// $Id: zppmodule.h,v 1.2 2001/08/12 17:30:19 david Exp $
#ifndef __zppmodule_h__
#define __zppmodule_h__

#include "zpp.h"

class ZppInstanceRegistry;
class ZppClassEntryRegistry;

/**
 * Base class for contexts
 */
class ZppContext
{
	public:
		//
		// public methods
		//

		ZppContext()
			: mpInstanceRegistry(NULL)
		{}

		virtual ~ZppContext();

		ZppInstanceRegistry& InstanceRegistry();

	public:
		//
		// static public methods
		//

		static ZppContext& Get();

	private:
		//
		// private attributes
		//

		ZppInstanceRegistry* mpInstanceRegistry;
};

/**
	Base class for modules
*/
class ZppModule
{
	private:
		//
		// private types
		//

		struct ZppGlobals
		{
			ZppContext * mpContext;
		};
	
	public:
		//
		// public methods
		//

		//! Construct module
		ZppModule();
		
		//! Destruct module
		virtual ~ZppModule();

		//! Get active context
		ZppContext& ActiveContext();
		
		//! Initialize module
		virtual void Initialize();
		
		//! Implement in subclass to create context
		virtual ZppContext* CreateContext() = 0;
		
		//! Get class entry registry
		ZppClassEntryRegistry& ClassEntryRegistry();

		//! Get module number
		int ModuleNumber()
		{
			// XXX: return real module number
			return 0;
		}

	public:
		//
		// static public methods
		//

		//! Set active module
		static void Set(ZppModule* pModule);

		//! Get active module
		static ZppModule& Get();

		//! Destroy active module
		static void Destroy();
	
			
		static void AbortOnException(bool abortOnException) throw() 
		{ 
			if (mpModule)
				mpModule->mAbortOnException = abortOnException; 
		}

		static bool AbortOnException() throw() 
		{ 
			if (mpModule)
				return mpModule->mAbortOnException; 
			else
				return false;
		}
		
	private:
		//
		// private methods
		//

		//! Get globals
		ZppGlobals& Globals();

	private:
		//
		// static private methods
		//

		//! Initialize globals
		static void StaticConstructor(ZppGlobals* pGlobals);

		//! Destroy globals
		static void StaticDestructor(ZppGlobals* pGlobals);

	private:
		//
		// private attributes
		//

		ZppClassEntryRegistry* mpClassEntryRegistry;
		bool mAbortOnException;

#ifdef ZTS
		ts_rsrc_id mGlobalsId;
#else
		ZppGlobals mGlobals;
#endif
		
	private:
		//
		// static private attributes
		//
		static ZppModule* mpModule;
		
};

#endif // __zppmodule_h__
