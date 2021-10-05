
#include "zval_var.h"

void zval_var::copy(zval* pSource, zval* pDest) throw()
{
	*pDest = *pSource;
	::zval_copy_ctor(pDest);
	INIT_PZVAL(pDest);
}

zval* zval_var::create() throw()
{
	zval* p_value;

	// allocate memory for value
	// set reference count to 1
	// specify this is not a reference
	MAKE_STD_ZVAL(p_value);

	// do this to prevent leaving the object in an unsafe state
	p_value->type = IS_NULL;

	return p_value;
}

zval* zval_var::create_copy(zval* pOther) throw()
{
	zval * p_duplicate = NULL;
	MAKE_STD_ZVAL(p_duplicate);
	zval_var::copy(pOther, p_duplicate);
	return p_duplicate;
}

zval* zval_var::duplicate(zval* pOther) throw()
{
	if (NULL != pOther)
	{
		pOther->refcount++;
	}
	return pOther;
}

void zval_var::release(zval* pZval) throw()
{
	if (pZval)
	{
		zval_ptr_dtor(&pZval);
	}
}

