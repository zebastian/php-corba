// $Id: zppmodule.cpp,v 1.3 2002/03/25 09:46:15 david Exp $
#include "zppmodule.h"
#include "zppinstanceregistry.h"
#include "zppclassentryregistry.h"

// -------------------------------------------------------------------
//
// Globals
//
// -------------------------------------------------------------------

//! Global function that is executed when the value of 
//! ZPP_INI_AbortOnException changes
ZEND_INI_MH(Zpp_AbortOnException_OnChange)
{
	try
	{
		ZppModule::AbortOnException((bool)INI_INT(ZPP_INI_AbortOnException));
		return SUCCESS;
	}
	catch (ZppException& e)
	{
		// display exception?
	}
	catch (...)
	{
	}

	return FAILURE;
}


// -------------------------------------------------------------------
//
// ZppContext
//
// -------------------------------------------------------------------

ZppContext::~ZppContext()
{
	if (mpInstanceRegistry)
	{
		delete mpInstanceRegistry;
		mpInstanceRegistry = NULL;
	}
}

//! Get active context
ZppContext& ZppModule::ActiveContext()
{
	return *Globals().mpContext;
}

ZppContext& ZppContext::Get()
{
	return ZppModule::Get().ActiveContext();
}
		
ZppInstanceRegistry& ZppContext::InstanceRegistry()
{
	if (!mpInstanceRegistry)
		mpInstanceRegistry = new ZppInstanceRegistry();
	return *mpInstanceRegistry;
}


// -------------------------------------------------------------------
//
// ZppModule
//
// -------------------------------------------------------------------

ZppModule* ZppModule::mpModule = NULL;

//! Set active module
void ZppModule::Set(ZppModule* pModule)
{
	mpModule = pModule;
}

//! Get active module
ZppModule& ZppModule::Get()
{
	Assert(NULL != mpModule, ZppNullPointerException_());
	return *mpModule;
}

//! Destroy active module
void ZppModule::Destroy()
{
	delete mpModule;
	mpModule = NULL;
}

//! Construct module
ZppModule::ZppModule()
	: mpClassEntryRegistry(NULL), 
		mAbortOnException((bool)INI_INT(ZPP_INI_AbortOnException))
#ifdef ZTS
	, mGlobalsId(-1)
#endif
{
}

/**
	If a subclass implements this function, call this implementation first!
*/
void ZppModule::Initialize()
{
#ifdef ZTS
	mGlobalsId = ts_allocate_id(&mGlobalsId, sizeof(ZppGlobals), 
			(ts_allocate_ctor)StaticConstructor, (ts_allocate_dtor)StaticDestructor);
#else
	StaticConstructor(&mGlobals);
#endif
}

//! Initialize globals
void ZppModule::StaticConstructor(ZppGlobals* pGlobals)
{
	pGlobals->mpContext = Get().CreateContext();
}

//! Destroy globals
void ZppModule::StaticDestructor(ZppGlobals* pGlobals)
{
	delete pGlobals->mpContext;
	pGlobals->mpContext = NULL;
}

//! Destruct module
ZppModule::~ZppModule()
{
#ifndef ZTS
	StaticDestructor(&mGlobals);
#endif

	if (mpClassEntryRegistry)
	{
		delete mpClassEntryRegistry;
		mpClassEntryRegistry = NULL;
	}
}

//! Get globals
ZppModule::ZppGlobals& ZppModule::Globals()
{
#ifdef ZTS
	return *static_cast<ZppGlobals*>(ts_resource(mGlobalsId));
#else
	return mGlobals;
#endif
}

ZppClassEntryRegistry& ZppModule::ClassEntryRegistry()
{
	if (!mpClassEntryRegistry)
		mpClassEntryRegistry = new ZppClassEntryRegistry();
	return *mpClassEntryRegistry;
}


