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

// $Id: zppinstanceregistry.h,v 1.2 2001/08/12 17:30:19 david Exp $
#ifndef __zppinstanceregistry_h__
#define __zppinstanceregistry_h__

#include "zppclass.h"
#include "zppresource.h"

#include <map>

class ZppClassEntry;
class ZppContext;

/**
 * \brief Keep control of class entries and class instances
 */
class ZppInstanceRegistry
{
	friend class ZppContext;
	
	public:
		//
		// public methods
		//

		// --------------------------------------------------
		// Class resource functions
		// --------------------------------------------------
		
		//! add instance of class to resources
		//! \return resource value
		int AddResource(ZppClass* pClass);

		//! find resource
		//! \param resource A value returned from AddResource()
		ZppClass* FindResource(int resource);
		
		// --------------------------------------------------
		// Class instance functions
		// --------------------------------------------------
		
		//! get a ZppClass instance pointer from a zval containing an object
		ZppClass* FindInstance(zval * pObject)
		{
			// wrap object value
			ZppValue object(pObject);
			return FindInstance(object);
		}
		
		//! get a ZppClass instance pointer from a zend_property_reference
		ZppClass* FindInstance(
				zend_property_reference * pPropertyReference)
		{
			return FindInstance(pPropertyReference->object);
		}

		//! get a ZppClass instance pointer from a ZppValue containing an object
		ZppClass* FindInstance(ZppValue & rObject);

	public:
		//
		// static public methods
		//

		//! get singleton instance
		static ZppInstanceRegistry& Get();

	private:
		//
		// private methods
		//

		//! Private constructor so only friends can create the class
		ZppInstanceRegistry()
			: mpClassResources(NULL)
		{}

		~ZppInstanceRegistry();
			

		//! Access mpClassResources through this structure as
		//! Zpp::GetModuleNumber() is used in ZppResourceType constructor
		ZppResourceType<ZppClass>& ClassResources()
		{
			if (!mpClassResources)
				mpClassResources = new ZppResourceType<ZppClass>;
			return *mpClassResources;
		}
	
	private:
		//
		// private attributes
		//

		//! Class resources
		ZppResourceType<ZppClass>* mpClassResources;
};

#endif // __zppinstanceregistry_h__

