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

#include "zppvalue.h"
#include "zppclass.h"
#include "zppclassentry.h"

extern "C"
{
#include <zend_list.h>
#include <zend_execute.h>
};

void ZppValue::Reuse()
{
	if (mpHashTable)
	{
		delete mpHashTable;
		mpHashTable = NULL;
	}

	if (mpZval)
	{
		::zval_dtor(mpZval);		// Destroy contents
	}
	else
	{
		// allocate memory for value
		// set reference count to 1
		// specify this is not a reference
		MAKE_STD_ZVAL(mpZval);
	}

	// do this to prevent leaving the object in an unsafe state
	mpZval->type = IS_NULL;
}

void ZppValue::Free()
{
	if (mpHashTable)
	{
		delete mpHashTable;
		mpHashTable = NULL;
	}

	if (mpZval)
	{
		zval_ptr_dtor(&mpZval);
		mpZval = NULL;
	}
}

zval* ZppValue::Release() throw()
{
	SafeAddRef();	
	zval* return_value = mpZval;
	Free();
	return return_value;
}


/**
 * get class name for object values
 */
const char * ZppValue::ClassName() const
{ 
	Assert(IsObject(), ZppTypeMismatchException_());
	return Z_OBJCE_P(mpZval)->name;
}

/**
 * get properties for object values
 */
ZvalHashTable& ZppValue::Properties()
{
	Assert(IsObject(), ZppTypeMismatchException_());

	if (!mpHashTable)
		mpHashTable = new ZvalHashTable(Z_OBJPROP_P(mpZval), ZvalHashTable::IS_NOT_OWNER);

	return *mpHashTable;
}

const ZvalHashTable& ZppValue::Properties() const 
{ 
	return const_cast<ZppValue*>(this)->Properties(); 
}


/**
 * get hashtable for array values
 */
ZvalHashTable& ZppValue::HashTable()
{
	Assert(IsArray(), ZppTypeMismatchException_());

	if (!mpHashTable)
		mpHashTable = new ZvalHashTable(mpZval->value.ht, ZvalHashTable::IS_NOT_OWNER);

	return *mpHashTable;
}

/**
 * get hashtable for array values
 */
const ZvalHashTable& ZppValue::HashTable() const
{
	return const_cast<ZppValue*>(this)->HashTable();
}

/**
 * get asciiz string for string values
 */
const char * ZppValue::String() const
{
	Assert(IsString(), ZppTypeMismatchException_());
	return mpZval->value.str.val;
}

bool ZppValue::Bool() const
{
	Assert(IsBool(), ZppTypeMismatchException_());
	return mpZval->value.lval != 0;
}	

long ZppValue::Long() const
{
	Assert(IsLong(), ZppTypeMismatchException_());
	return mpZval->value.lval;
}	

double ZppValue::Double() const
{
	Assert(IsDouble(), ZppTypeMismatchException_());
	return mpZval->value.dval;
}


void ZppValue::Null()
{
	Reuse();
	ZVAL_NULL(mpZval);					// assign value
}

void ZppValue::Bool(bool value)
{
	Reuse();
	ZVAL_BOOL(mpZval, value);		// assign value
}

void ZppValue::Long(long value)
{
	Reuse();
	ZVAL_LONG(mpZval, value);		// assign value
}

void ZppValue::Double(double value)
{
	Reuse();
	ZVAL_DOUBLE(mpZval, value);	// assign value
}

void ZppValue::String(const char * value)
{
	Reuse();
	if (NULL == value)
	{
		mpZval->value.str.len = 0;
		mpZval->value.str.val = empty_string;
	}
	else
	{
		mpZval->value.str.len = strlen(value);
		mpZval->value.str.val = estrndup(value, mpZval->value.str.len);
	}
	mpZval->type = IS_STRING;
}

void ZppValue::Array()
{
	Reuse();
	::array_init(mpZval);
}

void ZppValue::ToLong()
{
	Assert(mpZval, ZppNullPointerException());
	::convert_to_long(mpZval);
}

void ZppValue::ToDouble()
{
	Assert(mpZval, ZppNullPointerException());
	::convert_to_double(mpZval);
}

void ZppValue::ToBool()
{
	Assert(mpZval, ZppNullPointerException());
	::convert_to_boolean(mpZval);
}

void ZppValue::ToString()
{
	Assert(mpZval, ZppNullPointerException());
	::convert_to_string(mpZval);
}

void ZppValue::Object(ZppClass * pObject)
{
#ifdef ZTS
	TSRMLS_FETCH();
#endif
	Reuse();	// sets refcount to 1 etc
	mpZval->is_ref = 1;
	object_init_ex(mpZval, pObject->ClassEntry()->RawClassEntry());
	pObject->ConnectToValue(*this);
	// is there danger of having several zvals pointing to the same object?
}

zval * ZppValue::Duplicate(const zval * pOther)
{
	zval * p_duplicate = NULL;
	MAKE_STD_ZVAL(p_duplicate);
	*p_duplicate = *pOther;
	::zval_copy_ctor(p_duplicate);
	INIT_PZVAL(p_duplicate);
	return p_duplicate;
}

// bad semantics??
ZppValue * ZppValue::Duplicate(ZppValue * pOther)
{
	ZppValue * p_duplicate = new ZppValue();
	p_duplicate->Assign( Duplicate(pOther->mpZval) );
	return p_duplicate;
}

#if 0
void ZppValue::CopyContents(ZppValue& rOther)
{
	Reuse();
	*mpZval = *rOther.mpZval;
	::zval_copy_ctor(mpZval);
	INIT_PZVAL(mpZval);
}
#endif

#if 0
ZppValue* ZppValue::Duplicate()
{	
	if (mpZval == NULL)
		return NULL;
	
	ZppValue* p_new_value = new ZppValue();

	eLLOC_ZVAL(p_new_value->mpZval);				// allocate
	*p_new_value->mpZval = *mpZval;					// raw data copy
	zval_copy_ctor(p_new_value->mpZval);		// smart data copy
	INIT_PZVAL(p_new_value->mpZval);				// set reference count

	return p_new_value;
}
#endif

bool ZppValue::AddPropertyString(const char *key, const char *str)
{
	Assert(IsObject(), ZppTypeMismatchException_());
	return ::add_property_string(mpZval, 
			const_cast<char*>(key), const_cast<char*>(str), true) == SUCCESS;
}
		
bool ZppValue::AddPropertyValue(const char *key, ZppValue& rValue)
{
	Assert(IsObject(), ZppTypeMismatchException_());
	rValue->refcount++;
	return ::add_property_zval(mpZval, 
			const_cast<char*>(key), rValue.Get()) == SUCCESS;
}
		

bool ZppValue::AddNextIndexString(const char *str)
{
	Assert(IsArray(), ZppTypeMismatchException_());
	return ::add_next_index_string(mpZval, 
			const_cast<char*>(str), true) == SUCCESS;
}

