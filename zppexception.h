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

#ifndef __zppexception_h__
#define __zppexception_h__

#include "zpp.h"
#include <string>
#include <stdexcept>

/**
 * Parent class for all Zpp exceptions
 */
class ZppException : public std::exception
{
	protected:
		ZppException(const char * pInfo, const char * pLocation = empty_string, 
				unsigned line = 0);

	public:
		virtual ~ZppException() throw ()
		{
		}

		void Print() const;

		const char * Info() const throw() { return mInfo.c_str(); }
		const char * Location() const throw() { return mLocation.c_str(); }
		unsigned Line() const throw() { return mLine; }

		//! Inherited from std::exception
		virtual const char * what() const throw() { return Info(); }

	private:
		std::string mInfo;
		std::string mLocation;
		unsigned mLine;
};

/**
 * This class and its subclasses represent errors 
 * that should propagate to the user via zend_error
 */
class ZppError : public ZppException
{
	public:
		ZppError(const char * pInfo, 
				const char * pLocation = empty_string, unsigned line = 0)
			: ZppException(pInfo, pLocation, line)
		{ }
};

#define ZppError_(info) \
	ZppError(info, __PRETTY_FUNCTION__, __LINE__)

class ZppTypeMismatchException : public ZppException 
{
	public:
		ZppTypeMismatchException(
				const char * pLocation = empty_string, unsigned line = 0)
			: ZppException("Type mismatch", pLocation, line)
		{ }
};

#define ZppTypeMismatchException_() \
	ZppTypeMismatchException(__PRETTY_FUNCTION__, __LINE__)

class ZppNullPointerException : public ZppException 
{
	public:
		ZppNullPointerException(
				const char * pLocation = empty_string, unsigned line = 0)
			: ZppException("Null pointer", pLocation, line)
		{ }
};

#define ZppNullPointerException_() \
	ZppNullPointerException(__PRETTY_FUNCTION__, __LINE__)

class ZppOutOfBounds : public ZppException
{
	public:
		ZppOutOfBounds( 
				const char * pLocation = empty_string, unsigned line = 0)
			: ZppException("Out of bounds", pLocation, line)
		{ }
};

#define ZppOutOfBounds_() \
	ZppOutOfBounds(__PRETTY_FUNCTION__, __LINE__)

class ZppNotImplemented : public ZppError
{
	public:
		ZppNotImplemented( 
				const char * pLocation = empty_string, unsigned line = 0)
			: ZppError("Not implemented", pLocation, line)
		{ }
};

#define ZppNotImplemented_() \
	ZppNotImplemented(__PRETTY_FUNCTION__, __LINE__)

class ZppWrongParameterCount : public ZppError
{
	public:
		ZppWrongParameterCount( 
				const char * pLocation = empty_string, unsigned line = 0)
			: ZppError("Wrong parameter count", pLocation, line)
		{ }
};

#define ZppWrongParameterCount_() \
	ZppWrongParameterCount(__PRETTY_FUNCTION__, __LINE__)


#define Assert(condition, exception) \
	if (!(condition)) throw exception;
	
#if 0
/**
 * throw exception if value is false
 *
 * implementation taken from:
 * "A UML Process - the Astrakan Approach", page 105:
 * "Arbetssteg: Välj implementation för preconditions"
 *
 * XXX: this _always_ instanciates the exception!
 */
template <typename C, typename E> inline void Assert (C condition, E exception)
{
	if (!condition) 
		throw exception;
}
#endif

/**
 * macro with catch statements for common exceptions
 */
#define CATCH_COMMON_EXCEPTIONS() \
	catch (ZppWrongParameterCount & ) \
	{ \
		::zend_error(E_WARNING,"Wrong parameter count for member function"); \
	} \
	catch (ZppError & e) \
	{ \
		::zend_error(E_WARNING, e.Info()); \
	} \
	catch (ZppException & e) \
	{ \
		::zend_error(E_WARNING, "[%s:%i] ZPP Exception: \"%s\"", \
				e.Location(), e.Line(), e.Info()); \
	} \
	catch (std::exception& e) \
	{ \
		::zend_error(E_WARNING, "Exception: \"%s\"", e.what()); \
	} \
	catch (...) \
	{ \
		::zend_error(E_WARNING, "[%s:%i] Unknown exception caught", __FILE__, __LINE__); \
	}


#endif // __zppexception_h__

