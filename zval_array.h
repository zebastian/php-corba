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

#ifndef __zval_array_h__
#define __zval_array_h__

#include "zpp.h"


/**
 * Store a zval** and an element count together
 */
class zval_array
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

		enum Allocation
		{
			ALLOCATE_ARRAY = 0,
			ALLOCATE_ELEMENTS = 1
		};
	
	public:
		//
		// public methods
		//
	
		//! Construct from existing array
		zval_array(zval** ppElements, unsigned elementCount, 
				Ownership ownership = IS_NOT_OWNER)
			: mppElements(ppElements), mElementCount(elementCount),
		    mOwnership(ownership)
		{
		}

		//! Construct a new array
		zval_array(unsigned elementCount, Ownership ownership = IS_OWNER,
				Allocation allocation = ALLOCATE_ELEMENTS)
			: mppElements(NULL), mElementCount(elementCount), 
		    mOwnership(ownership)
		{
			switch (allocation)
			{
				case ALLOCATE_ARRAY:
					mppElements = create_empty(elementCount);
					break;
				case ALLOCATE_ELEMENTS:
					mppElements = create(elementCount);
					break;
			}
		}

		//! Destructor
		virtual ~zval_array()
		{
			if (IS_OWNER == mOwnership)
			{
				release();
			}
		}

		//! return element at specified index
		zval* operator[] (unsigned index);
		zval* operator[] (int index) { return operator[]((unsigned)index); }

		operator zval**()
		{
			return mppElements;
		}

		unsigned count() throw()
		{
			return mElementCount;
		}
		
		void release() throw();

	public:
		//
		// static public methods
		//

		static zval** create_empty(unsigned elementCount) throw();
		static zval** create(unsigned elementCount) throw();

	private:
		//
		// private attributes
		//
		
		zval** mppElements;
		unsigned mElementCount;
		Ownership mOwnership;
};


#endif // __zval_array_h__

