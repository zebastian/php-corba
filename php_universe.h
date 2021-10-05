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

#ifndef PHP_UNIVERSE_H
#define PHP_UNIVERSE_H

extern zend_module_entry universe_module_entry;
#define phpext_universe_ptr &universe_module_entry

#ifdef PHP_WIN32
#define PHP_UNIVERSE_API __declspec(dllexport)
#else
#define PHP_UNIVERSE_API
#endif

PHP_MINIT_FUNCTION(universe);
PHP_MSHUTDOWN_FUNCTION(universe);
#if 1
PHP_RINIT_FUNCTION(universe);
PHP_RSHUTDOWN_FUNCTION(universe);
#endif
PHP_MINFO_FUNCTION(universe);

#if 0
PHP_FUNCTION(confirm_universe_compiled);	/* For testing, remove later. */
#endif

PHP_FUNCTION(universe_run);
PHP_FUNCTION(universe_object_to_string);
PHP_FUNCTION(universe_string_to_object);
PHP_FUNCTION(universe_is_valid);
PHP_FUNCTION(universe_has_interface);
PHP_FUNCTION(universe_set_exception);

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(universe)
ZEND_END_MODULE_GLOBALS(universe);
*/

/* In every function that needs to use variables in php_universe_globals,
   do call UNIVERSELS_FETCH(); after declaring other variables used by
   that function, and always refer to them as UNIVERSEG(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define UNIVERSEG(v) (universe_globals->v)
#define UNIVERSELS_FETCH() php_universe_globals *universe_globals = ts_resource(universe_globals_id)
#else
#define UNIVERSEG(v) (universe_globals.v)
#define UNIVERSELS_FETCH()
#endif

#endif	/* PHP_UNIVERSE_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
