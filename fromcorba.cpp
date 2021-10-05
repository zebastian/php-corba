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

#include "fromcorba.h"
#include "zppvalue.h"
#include "object.h"
#include "struct.h"
#include "zval_var.h"
#include "array_var.h"
#include <mico/fixed.h>
#include <iostream>
using namespace std;

void from_corba(CORBA::NamedValue_ptr pSource, zval* pResult)
{
	from_corba(*pSource->value(), pResult);
}

void from_corba_sequence(CORBA::Any& rSource, zval* pDestination, CORBA::TypeCode_ptr pType)
{
	CORBA::TypeCode_var content_type = pType->content_type();

	Assert(IS_NULL == pDestination->type, ZppTypeMismatchException_());

	if (content_type->kind() == CORBA::tk_octet)
	{
		CORBA::ULong size = 0;
		rSource.seq_get_begin(size);

		char* buffer = (char*)emalloc(size+1);
		buffer[size] = 0;

		for (unsigned i = 0; i < size; i++)
		{
			CORBA::Octet value;
			if (rSource >>= CORBA::Any::to_octet(value))
				buffer[i] = value;
		}
		ZVAL_STRINGL(pDestination, buffer, size, false);

		rSource.seq_get_end();
	}
	else
	{
		::array_init(pDestination);
		array_var destination(zval_var::duplicate(pDestination));
		
		CORBA::ULong size = 0;
		rSource.seq_get_begin(size);

		for (unsigned i = 0; i < size; i++)
		{
			zval_var element = zval_var::create();
			from_corba(rSource, element, content_type);
			destination.next_index_insert(element._retn());
		}

		rSource.seq_get_end();
	}
}

void from_corba(CORBA::Any& rSource, zval* pResult, CORBA::TypeCode_ptr pType)
{
	if (NULL == pType)
		pType = rSource.type(); 

	// properly nuke any previous value
	::zval_dtor(pResult);
	ZVAL_NULL(pResult);
	
	switch (pType->kind())
	{
		case CORBA::tk_short:
			{
				CORBA::Short value;
				if (rSource >>= value)
					ZVAL_LONG(pResult, value);
			}
			break;
		case CORBA::tk_ushort:	
			{
				CORBA::UShort value;
				if (rSource >>= value)
					ZVAL_LONG(pResult, value);
			}
			break;

		case CORBA::tk_long: 		
			{
				CORBA::Long value;
				if (rSource >>= value)
					ZVAL_LONG(pResult,  value ); 
			}
			break;

		case CORBA::tk_ulong:		
			{
				CORBA::ULong value;
				if (rSource >>= value)
					ZVAL_LONG(pResult, value);
			}
			break;
													
		case CORBA::tk_float: 
			{
				CORBA::Float value;
				if (rSource >>= value)
					ZVAL_DOUBLE(pResult, value);
			}
			break;

		case CORBA::tk_double: 	
			{
				CORBA::Double value;
				if (rSource >>= value)
					ZVAL_DOUBLE(pResult, value);
			}
			break;

		case CORBA::tk_boolean:
			{
				CORBA::Boolean value;
				if (rSource >>= CORBA::Any::to_boolean(value))
					ZVAL_BOOL(pResult,  value != 0 );
			}
			break;
														
		case CORBA::tk_char:
			{
				CORBA::Char value;
				if (rSource >>= CORBA::Any::to_char(value))
					ZVAL_LONG(pResult,  value ); 
			}
			break;

		case CORBA::tk_octet: 
			{
				CORBA::Octet value;
				if (rSource >>= CORBA::Any::to_octet(value))
					ZVAL_LONG(pResult, value);
			}
			break;
		
		case CORBA::tk_objref: 
			UniverseObject::ConvertFromCorba(rSource, pResult, pType);
			break;
		
		case CORBA::tk_struct:
		case CORBA::tk_except: 
			{
				ZppValue result(pResult);
				UniverseStruct::ConvertFromCorba(rSource, result, pType);
			}
			break;

		case CORBA::tk_string: 
			{
				// who owns the memory pointed to by value? not me, I suppose
				const char * value;
				if (rSource >>= CORBA::Any::to_string(value, pType->length()))
				{
					ZVAL_STRING(pResult, const_cast<char*>(value), true);
				}
			}
			break;
		
		case CORBA::tk_enum:
			{
				CORBA::ULong value;
				if (rSource.enum_get(value))
				{
					ZVAL_STRING(pResult, const_cast<char*>(pType->member_name(value)), true);
//					rResult.Long(value);
				}
			}
			break;

		case CORBA::tk_alias: 
			{
				CORBA::TypeCode_var subtype = pType->content_type();
				from_corba(rSource, pResult, subtype);
			}
			break;
			
		case CORBA::tk_sequence: 
			from_corba_sequence(rSource, pResult, pType);
			break;
		
		case CORBA::tk_fixed: 
#if 0
			{
				//
				// XXX: This code has not been tested because the corresponding code in 
				// to_corba.cpp does not work with MICO.
				//

				/*CORBA::*/Fixed value;
				rSource >>= CORBA::Any::to_fixed(value,
						pType->fixed_digits(), pType->fixed_scale());
	
				ostrstream_zpp buffer;
				buffer << value;
				rResult.String(buffer.str());
			}
			break;
#endif

		case CORBA::tk_any: 
		case CORBA::tk_TypeCode: 
		case CORBA::tk_Principal: 
		case CORBA::tk_union: 
		case CORBA::tk_array: 
		case CORBA::tk_longlong: 
		case CORBA::tk_ulonglong: 
		case CORBA::tk_longdouble: 
		case CORBA::tk_wchar: 
		case CORBA::tk_wstring: 
		case CORBA::tk_value: 
		case CORBA::tk_value_box: 
		case CORBA::tk_native: 
		case CORBA::tk_abstract_interface: 
#if MICO_BIN_VERSION >= 0x020306
		case CORBA::tk_local_interface:
#endif
			// XXX: error unhandled type
			cerr << "Conversion from this CORBA data type (" << pType->kind() << ") is not yet supported" << ends;
			// fall through for now
	
		case CORBA::tk_null:
			// fall through
		case CORBA::tk_void: 
			break;
	}
}

