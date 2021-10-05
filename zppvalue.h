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

#ifndef __zppvalue_h__
#define __zppvalue_h__

#include "zpp.h"
#include "zpphashtable.h"
#include "zppexception.h"

class ZppClass;
class ZvalHashTable;

/**
 * \brief zval wrapper class
 *
 * This class will wrap a pointer to a zval: A copy of a ZppValue object will
 * contain a copy of the pointer to the zval.
 * 
 * A more correct name for this class would be zval_ptr.
 */
class ZppValue
{
	public:
		//
		// public methods
		//

		//
		// This part is a mimic of auto_ptr's interface
		//
	
		/**
		 *  Construct from a zval pointer
		 */
		explicit ZppValue(zval * pZval) throw() 
			: mpZval(pZval), mpHashTable(NULL)
		{
			SafeAddRef();
		}

		/**
		 *  Copy constructor
		 */
		ZppValue(ZppValue& other) throw() 
			: mpZval(other.mpZval), mpHashTable(NULL)
		{
			SafeAddRef();
		}

		/**
		 * Assignment operator
		 */
		ZppValue& operator=(ZppValue& rOther) throw()
		{
			if (&rOther != this)
			{
				Free();
				mpZval = rOther.mpZval;
				SafeAddRef();
			}
			return *this;
		}

		/**
		 * Destructor, you know
		 */
		~ZppValue()
		{
			Free();
		}

		/**
		 * Release contents of object
		 */
		void Free();

		zval& operator*()  const throw() { return *mpZval; }
		zval* operator->() const throw() { return mpZval; }
		zval* Get()        const throw() { return mpZval; }
		zval* Release()          throw();
		void Put(zval* pZval)    throw() { Free(); mpZval = pZval; SafeAddRef(); }
		void Assign(zval* pZval) throw() { Free(); mpZval = pZval; }

		//
		// Here comes zval-specific stuff
		// 
		
		/**
		 * Default constructor
		 */
		ZppValue() throw() 
			: mpZval(NULL), mpHashTable(NULL)
		{}
		
#if 0
		/**
		 * Construct a boolean value
		 *
		 * XXX: Not typesafe
		 */
		ZppValue(bool value) 
			: mpZval(NULL), mpHashTable(NULL)
		{ Bool(value); }
#endif

		/**
		 * Construct a long value
		 */
		ZppValue(long value) throw() 
			: mpZval(NULL), mpHashTable(NULL)
		{ Long(value); }
	
		/**
		 * Construct a double value
		 */
		ZppValue(double value) throw() 
			: mpZval(NULL), mpHashTable(NULL)
		{ Double(value); }
	
		/**
		 * Construct a string value
		 */
		ZppValue(const char * value) throw() 
			: mpZval(NULL), mpHashTable(NULL)
		{ String(value); }

		/**
		 * Construct a new object value
		 */
		ZppValue(ZppClass * value)
			: mpZval(NULL), mpHashTable(NULL)
		{ Object(value); }

		//
		// Type functions
		//

		int Type() const throw() { return mpZval ? mpZval->type : IS_NULL; }
		
		bool IsNull() const throw() { return mpZval && mpZval->type == IS_NULL; }
		bool IsDouble() const throw() { return mpZval && mpZval->type == IS_DOUBLE; }
		bool IsLong()   const throw() { return mpZval && mpZval->type == IS_LONG; }
		bool IsObject() const throw() { return mpZval && mpZval->type == IS_OBJECT; }
		bool IsString() const throw() { return mpZval && mpZval->type == IS_STRING; }
		bool IsArray()  const throw() { return mpZval && mpZval->type == IS_ARRAY; }
		bool IsBool()   const throw() { return mpZval && mpZval->type == IS_BOOL; }
		
		//
		// Functions to get the contents of value
		//
		
		const char * ClassName() const;						// for an object
		ZvalHashTable& Properties();							// for an object
		const ZvalHashTable& Properties() const;	// for an object
		const char * String() const;
		long Long() const;
		bool Bool() const;
		double Double() const;
		ZvalHashTable& HashTable();								// for an array
		const ZvalHashTable& HashTable() const;		// for an array

		//
		// Functions to set the contents of a value
		//

		void Null();
		void Bool(bool value);
		void Long(long value);
		void Double(double value);
		void String(const char * value);
		void Array();
		void Object(ZppClass * pObject);
		
		//
		// Data type conversion functions
		//
		
		void ToLong();
		void ToDouble();
		void ToBool();
		void ToString();

		//
		// Object property functions
		//

		bool AddPropertyString(const char *key, const char *str);
		bool AddPropertyValue(const char *key, ZppValue& rValue);

		//
		// Array functions
		//

		bool AddNextIndexString(const char *str);
		
		//
		// Reference value functions
		//
		bool IsReference() const throw() { return 0 != mpZval->is_ref; }
		void SetReference(bool ref = true) throw() { mpZval->is_ref = ref; }
		unsigned ReferenceCount() const throw() { return mpZval->refcount; }

	public:
		//
		// static public methods
		//

		/**
		 * duplicate the zval and create a new ZppValue to wrap it
		 */
		static zval * Duplicate(const zval * pOther);
		static ZppValue * Duplicate(ZppValue * pOther);

	private:
		//
		// private methods
		//

		/**
		 * This will destroy the contents of the contained value
		 * or create a new contained value if needed.
		 */
		void Reuse();
		
		/**
		 * Increase reference counting if value is available
		 */
		void SafeAddRef()
		{
			if (mpZval)
				mpZval->refcount++;
		}
		
	private:
		//
		// private attributes
		//

		zval* mpZval;
		
		// only create this once
		mutable ZvalHashTable* mpHashTable;

};

#if 0
/**
 * wrap a hashtable containing zvals
 *
 * currently never owns a hashtable
 */
class ZppValueHashTable
{
	private:
		ZvalHashTable * mpHashTale;

	public:
		explicit ZppValueHashTable(ZvalHashTable * pHashTable)
			: mpHashTale(pHashTable)
		{
		}

		explicit ZppValueHashTable(HashTable * pRawHashTable)
			: mpHashTale( new ZvalHash(pRawHashTable, false) )
		{
		}

		
};
#endif

/**
 * Destructor class for ZppHashTable
 */
struct ZvalPtrDestructor
{
	operator dtor_func_t()
	{
		return reinterpret_cast<dtor_func_t>(_zval_ptr_dtor);
	}
};


/**
 * The base type for a HashTable of zval* elements
 */
typedef ZppHashTable<zval*, ZvalPtrDestructor> _ZvalHashTable;

/**
 * A HashTable of zval* elements + extra sauce
 */
class ZvalHashTable : public _ZvalHashTable
{
	public:
		ZvalHashTable(HashTable * pHashTable, Ownership ownership)
			: _ZvalHashTable(pHashTable, ownership)
		{
		}
		
		/**
		 * Nice and simple array operator
		 */
		zval* operator[] (unsigned long index) const throw()
		{
			zval** pp_value = NULL;
			if (IndexFind(index, &pp_value) && pp_value)
				return *pp_value;
			return NULL;
		}
		
		zval* operator[] (const char * pKey) const throw()
		{
			zval** pp_value = NULL;
			if (Find(pKey, &pp_value) && pp_value)
				return *pp_value;
			return NULL;
		}
		
		bool IndexAdd(zval* pValue) throw()
		{
			zval* tmp = pValue;
			return _ZvalHashTable::IndexAdd(&tmp);
		}

};


#endif //__zppvalue_h__
