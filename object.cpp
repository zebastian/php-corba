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

// $Id: object.cpp,v 1.40 2002/07/16 22:59:31 david Exp $
#include "object.h"
#include "zppclassentry.h"
#include "zppproxy.h"
#include "zppexception.h"
#include "zppstream.h"
#include "fromcorba.h"
#include "tocorba.h"
#include "zval_array.h"

#include <memory>
using namespace std;

#if 0
static void IorProxy(INTERNAL_FUNCTION_PARAMETERS)
{
	ZppProxy::CallStaticFunction(&UniverseObject::Ior, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
#endif

const char * const gIorPropertyKey = "IOR";
const char * const gGetPrefix = "_get_";
const char * const gSetPrefix = "_set_";


ZppClassEntry * ZppClassStatic<UniverseObject>::mpClassEntry = NULL;
#if ENABLE_SATELLITE_COMPATIBLITY
const char * const ZppClassStatic<UniverseObject>::mpClassAlias = "OrbitObject";
#endif

zend_function_entry UniverseObject::mStaticFunctions[] =
{
	{"__sleep", ZppProxy::Sleep, NULL},
	{"__wakeup", ZppProxy::Wakeup, NULL},
	{NULL, NULL}
};

/* {{{ UniverseObject::RegisterClass()
   Register the UniverseObject class */
bool UniverseObject::RegisterClass()
{
	mpClassEntry = new ZppClassEntry(
		"UniverseObject",
		FactoryFunction,
		ZppClassEntry::hasEverything,
		mStaticFunctions);
	
#if ENABLE_SATELLITE_COMPATIBLITY
	new ZppClassEntry(
		mpClassAlias,
		mpClassEntry->FactoryFunction(),
		ZppClassEntry::hasEverything,		// mpClassEntry->...
		mStaticFunctions);							// mpClassEntry->...
#endif

	return true;
}
/*}}}*/

/* {{{ UniverseObject::UniverseObject()
   UniverseObject constructor */
UniverseObject::UniverseObject() 
	: mpImplementation(NULL) 
{
}
/*}}}*/

UniverseObject::~UniverseObject()/*{{{*/
{
	if (mpImplementation)
		delete mpImplementation;
}/*}}}*/

void UniverseObject::Create(zval_array& rParameters)/*{{{*/
{
	if (rParameters.count() == 1)
	{
		// assuming IOR or path to IOR
		Assert(IS_STRING == rParameters[0]->type, ZppTypeMismatchException_());
		Create(rParameters[0]->value.str.val);
	}
	else if (rParameters.count() == 2)
	{
		// assuming object and repository id
		Assert(IS_STRING == rParameters[1]->type, ZppTypeMismatchException_());
		ZppValue object(rParameters[0]);
		Create(object, rParameters[1]->value.str.val);
	}
	else
	{
		throw ZppWrongParameterCount_();
	}
}/*}}}*/

void UniverseObject::Create(ZppValue& rObject, const char * pRepositoryId)/*{{{*/
{
	try
	{
		// verify we have an object
		mObject = rObject;
		Assert(mObject.IsObject(), ZppTypeMismatchException_());
		
		// validate interface
		CORBA::Contained_var contained = Repository()->lookup_id(pRepositoryId); 
		mInterface = CORBA::InterfaceDef::_narrow( contained );
		Assert(mInterface, ZppError_("Could not find interface"));

		// create CORBA object
		mpImplementation = new UniverseImplementation(
				mInterface->id(),
				mInterface->name(),
				this);

		mCorbaObject = mpImplementation->_this();
		//DUMP(mCorbaObject->_refcnt());
//		mCorbaObject->_deref();

		CORBA::Object_var poaobj = Orb()->resolve_initial_references ("RootPOA");
		PortableServer::POA_var poa = PortableServer::POA::_narrow (poaobj);
		PortableServer::POAManager_var mgr = poa->the_POAManager();

		mgr->activate ();
	}
	CATCH_CORBA_EXCEPTIONS();
}/*}}}*/

void UniverseObject::Create(const char * pIor)/*{{{*/
{
	try
	{
		CORBA::Object_var obj = Orb()->string_to_object(pIor);
		Assert(!CORBA::is_nil(obj), ZppError_("Cannot create Universe object from invalid IOR"));
		Create(obj);
	}
	CATCH_CORBA_EXCEPTIONS();
}/*}}}*/

void UniverseObject::Create(CORBA::Object_ptr pObject)/*{{{*/
{
	try
	{
		Assert(!CORBA::is_nil(pObject), ZppError_("Cannot create Universe object from invalid CORBA object"));
		mCorbaObject = CORBA::Object::_duplicate(pObject);
		
		const char * p_id = mCorbaObject->_repoid();

		CORBA::Contained_var contained = Repository()->lookup_id(p_id);
		mInterface = CORBA::InterfaceDef::_narrow(contained);	
		Assert(mInterface, ZppError_("Could not find interface"));

	}
	CATCH_CORBA_EXCEPTIONS();
}/*}}}*/

/* {{{ UniverseObject::FindOperation*/
CORBA::OperationDef_ptr UniverseObject::FindOperation(
		CORBA::InterfaceDef_ptr pInterface, const char * pOperationName)
{
	// XXX: This is a depth-first search, maybe rewrite to breadth-first search?
	
	CORBA::Contained_var contained = pInterface->lookup(pOperationName);
	CORBA::OperationDef_var p_operation = CORBA::OperationDef::_narrow(contained);
	
	if (!CORBA::is_nil(p_operation))
		return p_operation._retn(); // remove ownership and return pointer
	
	CORBA::InterfaceDefSeq_var p_base_interfaces = pInterface->base_interfaces();

	if (p_base_interfaces) // XXX: CORBA::is_nil() doesn't work here?!
	{
		// loop until all parents are searched or an operation is found
		for (unsigned i = 0; i < p_base_interfaces->length() && CORBA::is_nil(p_operation); i++)
		{
#ifdef PHP_WIN32
			// Work-around for MSVC++
			p_operation = FindOperation( p_base_interfaces->operator[](i), pOperationName );
#else
			p_operation = FindOperation( p_base_interfaces[i], pOperationName );
#endif
		}
	}

	return p_operation._retn(); // remove ownership and return pointer
}/*}}}*/

CORBA::OperationDef_ptr UniverseObject::FindOperation(const char * pOperationName)/*{{{*/
{
	// Prevent crash
	Assert(mInterface, ZppNullPointerException_());

#if 0
	OperationDef_map::iterator item = mOperationDefCache.find(pOperationName);
	if (mOperationDefCache.end() == item)
	{
#endif
		CORBA::OperationDef_var p_operation = 
			FindOperation(mInterface, pOperationName);
		if (CORBA::is_nil(p_operation))
		{
			strstream_zpp msg;
			msg << "Could not find operation \"" <<pOperationName << '"' << ends; 
			throw ZppError_(msg.str());
		}
//		mOperationDefCache[pOperationName] = p_operation;
		return p_operation._retn(); // remove ownership and return pointer
#if 0
	}
	else
	{
		item->second->_ref();
		return item->second;
	}
#endif
}/*}}}*/

/*{{{ UniverseObject::CreateExceptionList*/
void UniverseObject::CreateExceptionList(CORBA::OperationDef_ptr operation,
		CORBA::ExceptionList_out exceptionList)
{
	Orb()->create_exception_list(exceptionList);
		
	CORBA::ExceptionDefSeq_var exceptions( operation->exceptions() );

#if MICO_BIN_VERSION < 0x020307
	if (exceptions->length())
	{
		::zend_error(E_NOTICE, 
				"This method call may throw an exception. "
				"You need MICO 2.3.7 or later for proper exception handling.");
	}
#endif

	for (MICO_ULong i = 0; i < exceptions->length(); i++)
	{
		exceptionList->add( exceptions[i]->type() );
	}
}/*}}}*/

void UniverseObject::CreateResult(CORBA::OperationDef_ptr operation,/*{{{*/
		CORBA::NamedValue_out result)
{
	CORBA::TypeCode_var result_type = operation->result();
	if (result_type)
	{
		Orb()->create_named_value(result);
		result->value()->set_type(result_type);
		result_type->_deref();
	}
}/*}}}*/

void UniverseObject::CreateArgumentList(CORBA::OperationDef_ptr operation,/*{{{*/
		zval_array& rParameters, CORBA::NVList_out rArguments)
{
	CORBA::ParDescriptionSeq_var params = operation->params();
	Assert(rParameters.count() == params->length(),
			ZppWrongParameterCount_());

	// This will create a list with flags and types set correctly
	Orb()->create_operation_list(operation, rArguments);

	Assert(rParameters.count() == rArguments->count(),
			ZppWrongParameterCount_());

	for (unsigned i = 0; i < rParameters.count(); i++)
	{
		CORBA::NamedValue_ptr namedvalue = rArguments->item(i);
		
		// Convert input parameters only
		CORBA::Flags flags = namedvalue->flags();
		if ( (flags & CORBA::ARG_IN) || (flags & CORBA::ARG_INOUT) )
		{
			to_corba(rParameters[i], *namedvalue->value());
		}
	}
}/*}}}*/

void UniverseObject::ConvertOutputParameters(CORBA::NVList_ptr arguments, /*{{{*/
		zval_array& rParameters)
{
	for (unsigned i = 0; i < rParameters.count(); i++)
	{
		CORBA::NamedValue_ptr namedvalue = arguments->item(i);
		
		// Convert output parameters only
		CORBA::Flags flags = namedvalue->flags();
		if ( (flags & CORBA::ARG_OUT) || (flags & CORBA::ARG_INOUT) )
		{
			from_corba(namedvalue, rParameters[i]);
		}
	}
}/*}}}*/

void UniverseObject::CallFunction(const char * pFunctionName, /*{{{*/
	zval_array& rParameters, ZppValue & rReturnValue)
{
	try 
	{
		CORBA::OperationDef_var operation = FindOperation(pFunctionName);
		
		//
		// Get exception list from operation info
		//
		CORBA::ExceptionList_var exception_list;
		CreateExceptionList(operation, exception_list.out());
			
		//
		// Create result value
		//
		CORBA::NamedValue_var result;
		CreateResult(operation, result.out());
		
		//
		// Add arguments
		//
		CORBA::NVList_var arguments;
		CreateArgumentList(operation, rParameters, arguments.out());

		//
		// Create request
		//
		CORBA::Request_var request;
		CORBA::String_var name = operation->name();

		mCorbaObject->_create_request(
				NULL,	/* context */
				name,
				arguments, /* argument list */
				result,
				exception_list,
				NULL, /* context list */
				request.out(),
				0 		/* flags */	
				);
		
		//
		// Perform request
		//
		request->invoke();

		//
		// Check for exception
		//
		if (request->env()->exception())
		{
			CORBA::UnknownUserException* exception = 
				CORBA::UnknownUserException::_downcast(request->env()->exception());
			ostrstream_zpp buffer;
			buffer << "User exception: " << exception->_except_repoid() << ends;
			throw ZppError(buffer.str());
		}
		else
		{
			//
			// TODO: Take care of output arguments
			//
			ConvertOutputParameters(arguments, rParameters);
			
			//
			// Take care of return value
			//
			if (result)
			{
				from_corba(result, rReturnValue.Get());
			}
			else
			{
				rReturnValue.Null();
			}
		}
	}
	CATCH_CORBA_EXCEPTIONS();
}/*}}}*/

CORBA::AttributeDef_ptr UniverseObject::FindAttribute(const char * pAttributeName)/*{{{*/
{
	Assert(pAttributeName, ZppNullPointerException_());
	
	CORBA::Contained_var contained = mInterface->lookup(pAttributeName);
	CORBA::AttributeDef_var p_attribute = CORBA::AttributeDef::_narrow(contained);
	
	if (CORBA::is_nil(p_attribute))
	{
		strstream_zpp message;
		message << "Could not find attribute \"" << pAttributeName << '"' << ends;
		throw ZppError_(message.str());
	}

	return p_attribute._retn();
}/*}}}*/

void UniverseObject::GetProperty(const char * pPropertyName, ZppValue& rValue)/*{{{*/
{
	try	
	{
		//
		// Get attribute info
		//
		CORBA::AttributeDef_var attribute( FindAttribute(pPropertyName) );

		//
		// create name for request
		//
		strstream_zpp name;
		name << gGetPrefix << attribute->name() << ends;

		//
		// create result value
		//
		CORBA::NamedValue_var result;
		Orb()->create_named_value(result);
		result->value()->set_type(attribute->type());

		//
		// Create request
		//
		CORBA::Request_var request;

		mCorbaObject->_create_request(
				NULL,	/* context */
				name.str(),
				NULL, /* argument list */
				result,
				request,
				0 		/* flags */	
				);

		//
		// Perform request
		//
		request->invoke();
		
		//
		// Handle return value
		//
		from_corba(result, rValue.Get());
	}
	CATCH_CORBA_EXCEPTIONS();
}/*}}}*/

void UniverseObject::PutProperty(const char * pPropertyName, ZppValue& rValue)/*{{{*/
{
	try
	{
		//
		// Get attribute info
		//
		CORBA::AttributeDef_var attribute( FindAttribute(pPropertyName) );

		// Verify it's a writable attribute
		if (attribute->mode() == CORBA::ATTR_READONLY)
		{
			throw ZppError_("Property is read-only");
		}
		
		//
		// create name for request
		//
		strstream_zpp name;
		name << gSetPrefix << attribute->name() << ends;
	
		//
		// create argument list with one element
		// 
		CORBA::NVList_var arguments;
		Orb()->create_list(0, arguments);		// XXX: what's the first param?

		//
		// Add a namedvalue to list and set value
		//
		CORBA::NamedValue_ptr namedvalue = arguments->add(CORBA::ARG_IN);
		namedvalue->value()->set_type(attribute->type());
		to_corba(rValue.Get(), *namedvalue->value());

		//
		// Create request
		//
		CORBA::Request_var request;

		mCorbaObject->_create_request(
				NULL,	/* context */
				name.str(),
				arguments, /* argument list */
				NULL, /* result */
				request,
				0 		/* flags */	
				);

		//
		// Perform request
		//
		request->invoke();
	}
	CATCH_CORBA_EXCEPTIONS();
}/*}}}*/

void UniverseObject::Sleep(ZppValue& rObject, ZppValue& rPropertyNames)/*{{{*/
{
	// get IOR from object
	char* p_ior = Orb()->object_to_string(mCorbaObject);
	Assert(p_ior, ZppNullPointerException_());
		
	// add property to our object
	rObject.AddPropertyString(gIorPropertyKey, p_ior);

	// add property name to array
	rPropertyNames.AddNextIndexString(gIorPropertyKey);

	// XXX: leaking p_ior?
}/*}}}*/

void UniverseObject::Wakeup(ZppValue& rObject)/*{{{*/
{
#if 0
	// get IOR
	zval ** pp_ior = NULL;
	bool success = Value().Properties().Find(
			gIorPropertyKey,	// key
			&pp_ior						// pointer heaven
			);

	// verify things
	Assert(success && pp_ior && *pp_ior, ZppNullPointerException_());
#endif
	// call Create method with IOR
	ZppValue ior( rObject.Properties()[gIorPropertyKey] );
	Create(ior.String());
}/*}}}*/

void UniverseObject::ConvertFromCorba(CORBA::Any& rSource, zval* pResult, /*{{{*/
		CORBA::TypeCode_ptr pType)
{
	CORBA::Object_var value;
	rSource >>= CORBA::Any::to_object(value);
	
	UniverseObject * p_object = new UniverseObject;
	p_object->Create(value);
	
	ZppValue result(pResult);
	result.Object(p_object);
}/*}}}*/
		
void UniverseObject::ConvertToCorba(ZppValue& rSource, CORBA::Any& rDestination, /*{{{*/
		CORBA::TypeCode_ptr pType)
{
	// XXX: mico extension
	rDestination <<= CORBA::Any::from_object( 
			FindInstance(rSource)->mCorbaObject, pType->name() );
}/*}}}*/

void UniverseObject::Invoke (CORBA::ServerRequest_ptr request)/*{{{*/
{
	if (strncmp(request->op_name(), gGetPrefix, strlen(gGetPrefix)) == 0)
	{
		cerr << "Getting properties from PHP is not implemented yet." << endl;
	}
	else if (strncmp(request->op_name(), gSetPrefix, strlen(gSetPrefix)) == 0)
	{
		cerr << "Setting properties in PHP is not implemented yet." << endl;
	}
	else
	{
		Invoke_CallFunction(request);
	}
}/*}}}*/

void UniverseObject::Invoke_CallFunction(CORBA::ServerRequest_ptr request)/*{{{*/
{
#ifdef ZTS
	TSRMLS_FETCH();
#endif
	UniverseContext::Get().Exception(NULL);
	
	// ZEND_API int call_user_function(HashTable *function_table, 
	// 		zval **object_pp, zval *function_name, zval *retval_ptr, 
	//		int param_count, zval *params[]);
	// ZEND_API int call_user_function_ex(HashTable *function_table, 
	//		zval **object_pp, zval *function_name, zval **retval_ptr_ptr, 
	//		int param_count, zval **params[], int no_separation, HashTable *symbol_table);

	//
	// Get operation info
	//
	CORBA::OperationDef_var operation( /*request->op_def()*/ FindOperation(request->op_name()) );

	CORBA::ParDescriptionSeq_var descriptions = operation->params();

	
	// MEMORY: ServerRequest takes ownership of the params object
	//         (see comments in include/mico/dsi.h)
	CORBA::NVList_ptr named_values = NULL;
	Orb()->create_operation_list(operation, named_values);
	request->arguments(named_values);

	//
	// Prepare zval_array for parameters
	//
	int parameter_count = descriptions->length();
	zval_array zvalues(parameter_count);
	
	//
	// Convert input parameters
	//
	ConvertZendInputParameters(named_values, zvalues);

	//
	// Prepare some parameters to call_user_function()
	//
	zval * p_object = mObject.Get();
	ZppValue return_value; return_value.Null();
	CORBA::String_var name( operation->name() );
	ZppValue function_name( name );

	//
	// Make function call
	//
#ifdef ZTS
	int rv = ::call_user_function(NULL, &p_object, function_name.Get(), 
			return_value.Get(), parameter_count, zvalues TSRMLS_CC);
#else
	int rv = ::call_user_function(NULL, &p_object, function_name.Get(),
			return_value.Get(), parameter_count, zvalues);
#endif

	if (FAILURE == rv)
	{
		// TODO: set exception
		cerr << "[UniverseObject::Invoke] call_user_function failed" << endl;
		return;
	}
	
	if (UniverseContext::Get().HasException())
	{
		request->set_exception(UniverseContext::Get().Exception());
		UniverseContext::Get().Exception(NULL);
	}
	else
	{
		//
		// Convert output parameters
		//
		ConvertZendOutputParameters(zvalues, named_values);

		//
		// Convert return value
		//
		CORBA::TypeCode_var result_type = operation->result();
		if (result_type && result_type->kind() != CORBA::tk_void)
		{
			CORBA::Any result;
			result.set_type(result_type);
			to_corba(return_value.Get(), result);
			request->set_result(result);
		}
	}
}/*}}}*/

void UniverseObject::ConvertZendInputParameters(CORBA::NVList_ptr source, /*{{{*/
		zval_array& rDestination)
{
	Assert(
			(NULL == source && rDestination.count() == 0) ||
			(NULL != source && rDestination.count() == source->count()),
			ZppWrongParameterCount_());

	for (unsigned i = 0; i < rDestination.count(); i++)
	{
		CORBA::NamedValue_ptr namedvalue = source->item(i);
		CORBA::Flags flags = namedvalue->flags();

		// Convert input parameters only
		if ( (flags & CORBA::ARG_IN) || (flags & CORBA::ARG_INOUT) )
		{
			from_corba(namedvalue, rDestination[i]);
		}

		// Make output values references
		if ( (flags & CORBA::ARG_OUT) || (flags & CORBA::ARG_INOUT) )
		{
			rDestination[i]->is_ref = true;
		}
	}
}/*}}}*/

void UniverseObject::ConvertZendOutputParameters(zval_array& rSource, /*{{{*/
		CORBA::NVList_ptr pDestination)
{
	for (unsigned i = 0; i < rSource.count(); i++)
	{
		CORBA::NamedValue_ptr namedvalue = pDestination->item(i);
		
		// Convert output parameters only
		CORBA::Flags flags = namedvalue->flags();
		if ( (flags & CORBA::ARG_OUT) || (flags & CORBA::ARG_INOUT) )
		{
			to_corba(rSource[i], *namedvalue->value());
		}
	}
}/*}}}*/

char * UniverseObject::Ior()/*{{{*/
{
	// XXX: the caller of this function has to destroy the returned string!
	return Orb()->object_to_string(mCorbaObject);
}/*}}}*/
		
void UniverseObject::StringToObject(ZppValue& rIor, ZppValue& rObject)/*{{{*/
{
	try
	{
		// create object
		auto_ptr<UniverseObject> p_instance( new UniverseObject() );

		// initialize with IOR
		p_instance->Create(rIor.String());
		
		// return object
		rObject.Object(p_instance.release());
	}
	CATCH_CORBA_EXCEPTIONS();
}/*}}}*/

