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

#ifndef __zpphashtable_h__
#define __zpphashtable_h__

#include "zpp.h"

/**
 * \brief Wrap a HashTable pointer
 *
 * A more correct name for this class would be HashTable_ptr
 *
 * The Destructor type should have implemented operator dtor_func_t()
 */
template <typename T, typename Destructor>
class ZppHashTable 
{
	public:
		//
		// public types
		//

		enum Ownership
		{
			IS_NOT_OWNER = 0,
			IS_OWNER = 1
		};

		enum Persistance
		{
			IS_NOT_PERSISTANT = 0,
			IS_PERSISTANT = 1
		};
	
	public:
		//
		// public methods
		//

		//! Construct and wrap a new hashtable
		ZppHashTable(Ownership ownership) throw()
			: mOwnership(ownership)
		{
			Create();
		}

		//! Construct and wrap a new hashtable
		ZppHashTable(Persistance persistent) throw()
			: mOwnership(IS_OWNER)
		{
			Create(persistent);
		}

		//! Construct from an existing hashtable
		ZppHashTable(HashTable * pHashTable, Ownership ownership) throw()
			: mpHashTable(pHashTable), mOwnership(ownership)
		{
		}

		/**
		 * Copy constructor
		 */
		ZppHashTable(const ZppHashTable<T, Destructor>& rOther) throw ()
			: mpHashTable(rOther.mpHashTable), mOwnership(rOther.mOwnership)
		{
			rOther.mOwnership = IS_NOT_OWNER;
		}
		
		//! Assignment operator
		ZppHashTable& operator=(const ZppHashTable& rOther) throw()
		{
			if (&rOther != this)
			{
				DestroyPtr();
				mOwnership = rOther.mOwnership;
				mpHashTable = rOther.mpHashTable;
				rOther.mOwnership = IS_NOT_OWNER;
			}
		}

		//! Destructor
		virtual ~ZppHashTable() throw()
		{
			DestroyPtr();
		}

		HashTable& operator*()  const throw() { return *mpHashTable; }
		HashTable* operator->() const throw() { return mpHashTable; }
		HashTable* Get()        const throw() { return mpHashTable; }
		HashTable* Release()          throw() { mOwnership = IS_NOT_OWNER; return mpHashTable; }
		
		bool Update(char * pKey, int keySize, T * pValue) throw()
		{
			return ::zend_hash_update(mpHashTable, pKey, keySize, (void*)pValue, 
					sizeof(T), NULL) == SUCCESS;
		}

		bool Update(char * pKey, T * pValue) throw()
		{
			return Update(pKey, strlen(pKey) + 1, pValue);
		}

		bool Find(char * pKey, int keySize, T ** ppValue) const throw()
		{
			return ::zend_hash_find(mpHashTable, pKey, keySize, (void**)ppValue) 
				== SUCCESS;
		}

		bool Find(const char * pKey, T ** ppValue) const throw()
		{
			return Find(const_cast<char*>(pKey), strlen(pKey) + 1, ppValue);
		}

		bool Delete(const char * pKey, int keySize) throw()
		{
			return ::zend_hash_del(mpHashTable, const_cast<char*>(pKey), keySize) 
				== SUCCESS;
		}

		bool Delete(const char * pKey) throw()
		{
			return Delete(pKey, strlen(pKey) + 1);
		}

		bool IndexUpdate(unsigned long index, T * pValue) throw()
		{
			return ::zend_hash_index_update(mpHashTable, index, (void*)pValue, 
					sizeof(T), NULL) == SUCCESS;
		}

		bool IndexFind(unsigned long index, T ** ppValue) const throw()
		{
			return ::zend_hash_index_find(mpHashTable, index, (void**)ppValue)
				== SUCCESS;
		}

		bool IndexDelete(unsigned long index) throw()
		{
			return ::zend_hash_index_del(mpHashTable, index) == SUCCESS;
		}

		bool IndexAdd(T * pValue) throw()
		{
			return ::zend_hash_next_index_insert(mpHashTable, (void*)pValue, 
					sizeof(T), NULL) == SUCCESS;
		}

		unsigned Size() const throw()
		{
			return ::zend_hash_num_elements(mpHashTable);
		}

	private:
		//
		// private methods
		//

		void Create(Persistance persistent = IS_NOT_PERSISTANT) throw()
		{
			mpHashTable = (HashTable *)pecalloc(1, sizeof(HashTable), persistent);

			::zend_hash_init(
					mpHashTable,		/* hash table */
					0, 							/* size */
					NULL, 					/* hash function */
					Destructor(), 	/* destructor */
					persistent);	
		}

		void DestroyPtr() throw()
		{
			if ((IS_OWNER == mOwnership) && mpHashTable)
			{
				// will crash on uninitialized structure
				::zend_hash_destroy(mpHashTable);
				pefree(mpHashTable, mpHashTable->persistent);
			}
		}
		
	private:
		//
		// private attributes
		//

		HashTable * mpHashTable;
		mutable Ownership mOwnership;

};

#endif // __zpphashtable_h__
