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

#include "tocorba.h"
#include "zppvalue.h"
#include "zppexception.h"
#include "object.h"
#include "struct.h"
#include "array_var.h"
#include <mico/fixed.h>


/**
	Convert a sequence
*/
static void to_corba_sequence(zval* pSource, CORBA::Any& rDestination, CORBA::TypeCode_ptr pType)
{
	CORBA::TypeCode_var content_type = pType->content_type();

	if (IS_STRING == pSource->type && content_type->kind() == CORBA::tk_octet)
	{
		// Convert string to array of octets

		const size_t size = pSource->value.str.len;
		rDestination.seq_put_begin(size);

		for (size_t i = 0; i < size; i++)
		{
			rDestination <<= CORBA::Any::from_octet(pSource->value.str.val[i]);
		}

		rDestination.seq_put_end();
	}
	else
	{
		Assert(IS_ARRAY == pSource->type, ZppTypeMismatchException_());
		array_var source(zval_var::duplicate(pSource));
		
		const unsigned size = source.count();
		rDestination.seq_put_begin(size);

		for (unsigned i = 0; i < size; i++)
		{
			to_corba(source[i], rDestination, content_type);
		}

		rDestination.seq_put_end();
	}
}

void to_corba(zval* pSource, CORBA::Any& rDestination, CORBA::TypeCode_ptr pType)
{
	if (NULL == pType)
		pType = rDestination.type(); 

	switch (pType->kind())
	{
		case CORBA::tk_short:	
			convert_to_long(pSource); 
			rDestination <<= (CORBA::Short)pSource->value.lval;
			break;
		case CORBA::tk_ushort:
			convert_to_long(pSource); 
			rDestination <<= (CORBA::UShort)pSource->value.lval;
			break;
		case CORBA::tk_long: 
			convert_to_long(pSource); 
			rDestination <<= (CORBA::Long)pSource->value.lval;
			break;
		case CORBA::tk_ulong:	
			convert_to_long(pSource); 
			rDestination <<= (CORBA::ULong)pSource->value.lval;
			break;
													
		case CORBA::tk_float:
			convert_to_double(pSource); 
			rDestination <<= (CORBA::Float)pSource->value.dval;
			break;
			
		case CORBA::tk_double: 	
			convert_to_double(pSource);
			rDestination <<= (CORBA::Double)pSource->value.dval;
			break;

		case CORBA::tk_boolean:
			convert_to_boolean(pSource);
			rDestination <<= CORBA::Any::from_boolean(pSource->value.lval != 0);
			break;
														
		case CORBA::tk_char:
			if (IS_STRING == pSource->type)
			{
				// to allow assignment like this: $x = 'X';
				rDestination <<= CORBA::Any::from_char(pSource->value.str.val[0]);
			}
			else
			{
				convert_to_long(pSource);
				rDestination <<= CORBA::Any::from_char(pSource->value.lval);
			}
			break;
		case CORBA::tk_octet: 
			convert_to_long(pSource);
			rDestination <<= CORBA::Any::from_octet(pSource->value.lval);
			break;
		
		case CORBA::tk_string: 
			if (IS_NULL == pSource->type)
			{
				rDestination <<= CORBA::Any::from_string("", pType->length());
			}
			else
			{
				convert_to_string(pSource);
				rDestination <<= CORBA::Any::from_string(pSource->value.str.val, pType->length());
			}
			break;

		case CORBA::tk_enum: 
			// Uses mico-extension TypeCode::enum_put
			if (IS_STRING == pSource->type)
			{
				rDestination.enum_put(pType->member_index(pSource->value.str.val));	
			}
			else
			{
				convert_to_long(pSource);
				// TODO: validate range
				rDestination.enum_put(pSource->value.lval);
			}
			break;
		
		case CORBA::tk_struct:
		case CORBA::tk_except: 
			{
				ZppValue source(pSource);
				UniverseStruct::ConvertToCorba(source, rDestination, pType);
			}
			break;
			
		case CORBA::tk_objref: 
			{
				ZppValue source(pSource);
				UniverseObject::ConvertToCorba(source, rDestination, pType);
			}
			break;
			
		case CORBA::tk_sequence: 
			to_corba_sequence(pSource, rDestination, pType);
			break;

		case CORBA::tk_alias: 
			{
				CORBA::TypeCode_var subtype = pType->content_type();
				to_corba(pSource, rDestination, subtype);
			}
			break;
			
		case CORBA::tk_fixed:
#if 0
			{
				rSource.ToString();
				
				//
				// XXX: This _should_ work, but does not with MICO.
				// (see p. 679 in Advanced CORBA Programming with C++)
				//
				
				/*CORBA::*/Fixed value(rSource.String());
				rDestination <<= CORBA::Any::from_fixed(value, 
						value.fixed_digits(), value.fixed_scale());
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
		case CORBA::tk_null:
		case CORBA::tk_void: 
		default:
			{
				ostrstream_zpp msg;
				msg << "Conversion to this CORBA data type (" << pType->kind() << ") is not yet supported" << ends;
				throw ZppError_(msg.str());
			}
	}
	
}

