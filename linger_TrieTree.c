/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_linger_TrieTree.h"

/* If you declare any globals in php_linger_TrieTree.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(linger_TrieTree)
*/

/* True global resources - no need for thread safety here */
static int le_linger_TrieTree;

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("linger_TrieTree.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_linger_TrieTree_globals, linger_TrieTree_globals)
    STD_PHP_INI_ENTRY("linger_TrieTree.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_linger_TrieTree_globals, linger_TrieTree_globals)
PHP_INI_END()
*/
/* }}} */

/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_linger_TrieTree_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_linger_TrieTree_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "linger_TrieTree", arg);
	RETURN_STRINGL(strg, len, 0);
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/


/* {{{ php_linger_TrieTree_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_linger_TrieTree_init_globals(zend_linger_TrieTree_globals *linger_TrieTree_globals)
{
	linger_TrieTree_globals->global_value = 0;
	linger_TrieTree_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(linger_TrieTree)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(linger_TrieTree)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(linger_TrieTree)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(linger_TrieTree)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(linger_TrieTree)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "linger_TrieTree support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* {{{ linger_TrieTree_functions[]
 *
 * Every user visible function must have an entry in linger_TrieTree_functions[].
 */
const zend_function_entry linger_TrieTree_functions[] = {
	PHP_FE(confirm_linger_TrieTree_compiled,	NULL)		/* For testing, remove later. */
	PHP_FE_END	/* Must be the last line in linger_TrieTree_functions[] */
};
/* }}} */

/* {{{ linger_TrieTree_module_entry
 */
zend_module_entry linger_TrieTree_module_entry = {
	STANDARD_MODULE_HEADER,
	"linger_TrieTree",
	linger_TrieTree_functions,
	PHP_MINIT(linger_TrieTree),
	PHP_MSHUTDOWN(linger_TrieTree),
	PHP_RINIT(linger_TrieTree),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(linger_TrieTree),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(linger_TrieTree),
	PHP_LINGER_TRIETREE_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_LINGER_TRIETREE
ZEND_GET_MODULE(linger_TrieTree)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
