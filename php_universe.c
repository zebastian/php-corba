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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_universe.h"

// for php_info_print_table_start
#include "ext/standard/info.h"

#include "zpp.h"
#include "universe.h"

/* If you declare any globals in php_universe.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(universe);
*/

#if 0
/* True global resources - no need for thread safety here */
static int le_universe;
#endif

/* Every user visible function must have an entry in universe_functions[].
*/
function_entry universe_functions[] = {
	PHP_FE(universe_run, NULL)
	PHP_FE(universe_object_to_string, NULL)
	PHP_FE(universe_string_to_object, NULL)
	PHP_FE(universe_is_valid, NULL)
	PHP_FE(universe_has_interface, NULL)
	PHP_FE(universe_set_exception, NULL)
	{NULL, NULL, NULL}	/* Must be the last line in universe_functions[] */
};

zend_module_entry universe_module_entry = {
#ifdef STANDARD_MODULE_HEADER
	STANDARD_MODULE_HEADER,
#endif
	"universe",
	universe_functions,
	PHP_MINIT(universe),
	PHP_MSHUTDOWN(universe),
#if 1
	PHP_RINIT(universe),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(universe),	/* Replace with NULL if there's nothing to do at request end */
#else
	NULL, NULL,
#endif
	PHP_MINFO(universe),
#ifdef NO_VERSION_YET
	NO_VERSION_YET,
#endif
	STANDARD_MODULE_PROPERTIES
};

/* see php4/README.SELF-CONTAINED-EXTENSIONS */
#ifdef COMPILE_DL_UNIVERSE
ZEND_GET_MODULE(universe)
#endif

ZEND_INI_BEGIN()
			ZEND_INI_ENTRY(INI_ORBNoIIOPServer,      "0", ZEND_INI_SYSTEM, NULL)
      ZEND_INI_ENTRY(INI_ORBImplRepoAddr,      NULL, ZEND_INI_SYSTEM, NULL)
      ZEND_INI_ENTRY(INI_ORBImplRepoIOR,       NULL, ZEND_INI_SYSTEM, NULL)
      ZEND_INI_ENTRY(INI_ORBIfaceRepoAddr,     NULL, ZEND_INI_SYSTEM, NULL)
      ZEND_INI_ENTRY(INI_ORBIfaceRepoIOR,      NULL, ZEND_INI_SYSTEM, NULL)
			ZEND_INI_ENTRY(INI_ORBNamingAddr,        NULL, ZEND_INI_SYSTEM, NULL)
			ZEND_INI_ENTRY(INI_ORBNamingIOR,         NULL, ZEND_INI_SYSTEM, NULL)
      ZEND_INI_ENTRY(INI_ORBIIOPAddr,          NULL,  ZEND_INI_SYSTEM, NULL)
      ZEND_INI_ENTRY(INI_ORBDebugLevel,        "0", ZEND_INI_SYSTEM, NULL)
      ZEND_INI_ENTRY(INI_ORBMemTrace,          "0", ZEND_INI_SYSTEM, NULL)
      ZEND_INI_ENTRY(ZPP_INI_AbortOnException, "0", ZEND_INI_SYSTEM, Zpp_AbortOnException_OnChange)
ZEND_INI_END()

PHP_MINIT_FUNCTION(universe)
{
	REGISTER_INI_ENTRIES();
	
	if (!universe_initialize())
		return FAILURE;

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(universe)
{
	universe_shutdown();
	
	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}

#if 1
/* Remove if there's nothing to do at request start */
PHP_RINIT_FUNCTION(universe)
{
	universe_request_start();
	return SUCCESS;
}

/* Remove if there's nothing to do at request end */
PHP_RSHUTDOWN_FUNCTION(universe)
{
	universe_request_end();
	return SUCCESS;
}
#endif

PHP_MINFO_FUNCTION(universe)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "universe support", "enabled");
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}

#if 0
/* Remove the following function when you have succesfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_universe_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_universe_compiled)
{
	zval **arg;
	int len;
	char string[256];

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(arg);

	len = sprintf(string, "Congratulations, you have successfully modified ext/universe/config.m4, module %s is compiled into PHP", Z_STRVAL_PP(arg));
	RETURN_STRINGL(string, len, 1);
}
/* }}} */
/* The previous line is meant for emacs, so it can correctly fold and unfold
   functions in source code. See the corresponding marks just before function
   definition, where the functions purpose is also documented. Please follow
   this convention for the convenience of others editing your code.
*/
#endif


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
