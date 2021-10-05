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

#ifndef __zval_var_h__
#define __zval_var_h__

#include "zpp.h"

/**
 * Encapsulate a zval* with reference counting
 *
 * Semantics like CORBA:s _var objects
 */
class zval_var
{
	public:
		//
		// public methods
		//
		
		//! Constructor
		zval_var()
			: mpZval(nil())
		{
		}

		//! Constructor
		zval_var(zval* pZval)
			: mpZval(pZval)
		{
		}

		//! Copy constructor
		zval_var(const zval_var& rOther)
		{
			if (&rOther != this)
			{
				free();
				mpZval = duplicate(rOther.mpZval);
			}
		}

		//! Destructor
		~zval_var()
		{
			free();
		}

		//! Assign a zval* to this object
		zval_var& operator= (zval* pZval)
		{
			if (mpZval != pZval)
			{
				reset(pZval);
			}
			return *this;
		}

		zval* operator-> ()
		{
			return mpZval;
		}

		//! Operator to get encapsulated value
		operator zval* ()
		{
			return mpZval;
		}

		//! Return contents and increase reference count
		zval* _retn()
		{
			zval* p_zval_tmp = mpZval;
			mpZval = NULL;
			return p_zval_tmp;
		}

	public:
		//
		// static public methods
		//

		static void copy(zval* pSource, zval* pDest) throw();
		static zval* create() throw();
		static zval* create_copy(zval*) throw();
		static zval* duplicate(zval*) throw();
		static void release(zval*) throw();

	private:
		//
		// private attributes
		//

		zval* mpZval;

	private:
		//
		// private methods
		//
		
		//! Assign from zval_var: not implemented
		zval_var& operator= (const zval_var& other);

		//! Release the contents
		void free()
		{
			release(mpZval);
			mpZval = nil();
		}
		
		//! Change contents
		void reset(zval* pZval)
		{
			free();
			mpZval = pZval;
		}

	private:
		//
		// static private methods
		//
	
		static zval* nil()
		{
			return 0;
		}
};

#endif // __zval_var_h__

