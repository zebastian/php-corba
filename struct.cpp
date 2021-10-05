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

#include "struct.h"
#include "zppclassentry.h"
#include "tocorba.h"
#include "fromcorba.h"
#include "zppstream.h"
#include "zppproxy.h"
#include "zval_array.h"

using namespace std;

const char * const gRepositoryIdPropertyKey = "RepositoryId";
const char * const gMembersPropertyKey = "Members";

ZppClassEntry * ZppClassStatic<UniverseStruct>::mpClassEntry = NULL;
#if ENABLE_SATELLITE_COMPATIBLITY
const char * const ZppClassStatic<UniverseStruct>::mpClassAlias = "OrbitStruct";
#endif

zend_function_entry UniverseStruct::mStaticFunctions[] =
{
	{"__sleep", ZppProxy::Sleep, NULL},
	{"__wakeup", ZppProxy::Wakeup, NULL},
	{NULL, NULL}
};


bool UniverseStruct::RegisterClass()
{
	mpClassEntry = new ZppClassEntry(
			"UniverseStruct",
			FactoryFunction,
			ZppClassEntry::hasPutGetProperty,
			mStaticFunctions);

#if ENABLE_SATELLITE_COMPATIBLITY
	new ZppClassEntry(
		mpClassAlias,
		mpClassEntry->FactoryFunction(),
		ZppClassEntry::hasPutGetProperty,	// mpClassEntry->...
		mStaticFunctions);								// mpClassEntry->...
#endif

	return true;
}

UniverseStruct::UniverseStruct()
	: //mMembers(true)	 /* XXX: do we need persistance here? */,
		mIsException(false)
{
}

void UniverseStruct::Create(const char * pRepositoryId, bool initializeMembers)
{
	try
	{
		mRepositoryId = pRepositoryId;

		mStruct = CORBA::StructDef::_narrow(
			Repository()->lookup_id(pRepositoryId));

		if (mStruct)
		{
			mIsException = false;
		}
		else
		{
			mIsException = true;
			mException = CORBA::ExceptionDef::_narrow(
				Repository()->lookup_id(pRepositoryId));
		}
		
		Assert(mStruct || mException, ZppError_("Could not find struct or exception"));

		if (initializeMembers)
		{
			InitializeMembers();
		}
	}
	CATCH_CORBA_EXCEPTIONS();
}
		
CORBA::StructMemberSeq* UniverseStruct::GetStructMemberSeq()
{
	// Get member sequence
	CORBA::StructMemberSeq_var members;
	
	if (!CORBA::is_nil(mStruct))
	{
		members = mStruct->members();
	}
	else if (!CORBA::is_nil(mException))
	{
		members = mException->members();
	}

	// CORBA::is_nil doesn't work here
	Assert(members, ZppNullPointerException_());	

	return members._retn();
}

void UniverseStruct::InitializeMembers()
{
	mMembers.Array();

	// Get member sequence
	CORBA::StructMemberSeq_var members = GetStructMemberSeq();
	
	// Iterate through sequence
	for (unsigned i = 0; i < members->length(); i++)
	{
		ZppValue value;
		value.Null();

		zval * p_value = value.Release();
		
		// Add members to hashtable
#ifdef PHP_WIN32
		// Work-around for MSVC++
		Members().Update(members->operator[](i).name, &p_value);
#else
		Members().Update(members[i].name, &p_value);
#endif
	}
}

void UniverseStruct::Create(zval_array& rParameters)
{
	Assert (rParameters.count() == 1, ZppWrongParameterCount_());

	::convert_to_string(rParameters[0]);
	Create(rParameters[0]->value.str.val);
}

void UniverseStruct::GetProperty(const char * pPropertyName, ZppValue& rValue)
{
	// XXX: care about case sensitive match?
	zval * p_value = Members()[pPropertyName];

	if (p_value)
	{
		rValue.Assign( ZppValue::Duplicate(p_value) );
	}
	else
	{
		rValue.Null();
		return;
	}

}

void UniverseStruct::PutProperty(const char * pPropertyName,
		ZppValue& rValue)
{
//	cerr << "UniverseStruct::PutProperty(\"" << pPropertyName << "\" , ...)" << endl;

	zval * p_value = ZppValue::Duplicate(rValue.Get()); 
	
	// XXX: care about case sensitive match?
	Members().Update(const_cast<char*>(pPropertyName), &p_value);
}

void UniverseStruct::Sleep(ZppValue& rObject, ZppValue& rPropertyNames)
{
	// Save repository id
	rObject.AddPropertyString(gRepositoryIdPropertyKey, mRepositoryId.c_str());
	rPropertyNames.AddNextIndexString(gRepositoryIdPropertyKey);

	// Save members
	rObject.AddPropertyValue(gMembersPropertyKey, mMembers);
	rPropertyNames.AddNextIndexString(gMembersPropertyKey);

#if 0
	// Each member should be saved
	CORBA::StructMemberSeq* p_members = GetStructMemberSeq();
	for (unsigned i = 0; i < p_members->length(); i++)
	{
		rPropertyNames.AddNextIndexString((*p_members)[i].name);
	}
#endif
}

void UniverseStruct::Wakeup(ZppValue& rObject)
{
	ZppValue repository_id( rObject.Properties()[gRepositoryIdPropertyKey] );

	// Create, but do not initialize members
	Create(repository_id.String(),false);

	mMembers.Put( rObject.Properties()[gMembersPropertyKey] );
	
	// Delete properties
	rObject.Properties().Delete(gRepositoryIdPropertyKey);
	rObject.Properties().Delete(gMembersPropertyKey);
}

void UniverseStruct::RealConvertToCorba(
		CORBA::Any& rDestination, CORBA::TypeCode_ptr pType)
{
	// Verify repository id:s
	Assert(mRepositoryId.compare(pType->id()) == 0,
		ZppTypeMismatchException_());

	// Start of struct/exception
	if (mIsException)
	{
		CORBA::Boolean r = rDestination.except_put_begin(mRepositoryId.c_str());
		assert(r);
	}
	else
	{
		/*CORBA::Boolean r = */rDestination.struct_put_begin();
	}

	// Iterate through sequence
	for (unsigned i = 0; i < pType->member_count(); i++)
	{
		// convert this memeber
		to_corba(Members()[pType->member_name(i)], rDestination, pType->member_type(i));
	}

	// End of struct/exception
	if (mIsException)
	{
		CORBA::Boolean r = rDestination.except_put_end();
		assert(r);
	}
	else
	{
		/*CORBA::Boolean r = */rDestination.struct_put_end();
	}

}

/**
	Convert the contents of a struct to a CORBA::Any value
*/
void UniverseStruct::ConvertToCorba(ZppValue& rSource, 
		CORBA::Any& rDestination, CORBA::TypeCode_ptr pType)
{
	// Call instance method
	FindInstance(rSource)->RealConvertToCorba(rDestination, pType);
}

void UniverseStruct::RealConvertFromCorba(CORBA::Any& rSource, 
		ZppValue& rDestination, CORBA::TypeCode_ptr pType)
{
	// Verify repository id:s
	Assert(mRepositoryId.compare(pType->id()) == 0,
		ZppTypeMismatchException_());

	// Start of struct/exception
	if (mIsException)
	{
		CORBA::String_var repoid;
		/*CORBA::Boolean r = */rSource.except_get_begin(repoid.out());
	}
	else
	{
		/*CORBA::Boolean r = */rSource.struct_get_begin();
	}

	// Iterate through sequence
	for (unsigned i = 0; i < pType->member_count(); i++)
	{
		// convert this member
		from_corba(rSource, Members()[pType->member_name(i)], pType->member_type(i));
	}

	// End of struct/exception
	if (mIsException)
	{
		/*CORBA::Boolean r = */rSource.except_get_end();
	}
	else
	{
		/*CORBA::Boolean r = */rSource.struct_get_end();
	}

}


void UniverseStruct::Create(CORBA::TypeCode_ptr pType)
{
	Create(pType->id());
}

/**
	Convert the contents of a struct from a CORBA::Any value
*/
void UniverseStruct::ConvertFromCorba(CORBA::Any& rSource, 
		ZppValue& rDestination, CORBA::TypeCode_ptr pType)
{
	// create instance of UniverseStruct
	UniverseStruct * p_instance = new UniverseStruct();
	// store instance in ZppValue
	rDestination.Object(p_instance);

	// late construction
	p_instance->Create(pType);

	// Call instance method
	p_instance->RealConvertFromCorba(rSource, rDestination, pType);
}

CORBA::Any* UniverseStruct::CreateException()
{
	Assert(mIsException, ZppTypeMismatchException_());

	CORBA::TypeCode_var typecode = UniverseContext::Get().Orb()->create_exception_tc(
			mRepositoryId.c_str(),
			"dummy",
			*mException->members());

	CORBA::Any_var any = new CORBA::Any();
	any->set_type(typecode); // XXX: mico extension?

	RealConvertToCorba(*any, typecode);

	return any._retn();

}

