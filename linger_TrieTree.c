/*
  +----------------------------------------------------------------------+
  | linger_TrieTree                                                      |
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
  | Author: liubang <it.liubang@gmail.com>                               |
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

static int le_linger_TrieTree;

zend_class_entry *linger_TrieTree_ce;
static zend_object_handlers linger_TrieTree_object_handlers;

typedef struct _TrieObject {
    zend_object std;
    Trie trie;
} TrieObject;


static void linger_TrieTree_free_object_storage_handler(TrieObject *intern TSRMLS_DC)
{
    zend_object_std_dtor(&intern->std TSRMLS_CC);
    trie_free(intern->trie);
    linger_efree(intern);
}

zend_object_value linger_TrieTree_create_object_handler(zend_class_entry *class_type TSRMLS_DC)
{
    zend_object_value retval;
    TrieObject intern = emalloc(sizeof(TrieObject)); 
    memset(intern, 0, sizeof(TrieObject));
    Trie *trie;
    AlphaMap *alpha_map;
    alpha_map = alpha_map_new();
    if (!alpha_map) {
        return NULL;
    }
    if (alpha_map_add_range(alpha_map, 0x00, 0xff) != 0) {
        alpha_map_free(alpha_map);
        return NULL;
    }
    trie = trie_new(alpha_map);
    alpha_map_free(alpha_map);
    if (!trie) {
        return NULL;
    }
    intern->trie = trie;
    zend_object_std_init(&intern->std, class_type TSRMLS_CC);
    retval.handle = zend_object_store_put(
            intern,
            (zend_objects_store_dtor_t) zend_objects_destroy_object,
            (zend_objects_free_object_storage_t) linger_TrieTree_free_object_storage_handler,
            NULL
            TSRMLS_CC
            );
    retval.handlers = &linger_TrieTree_object_handlers;
    return retval;
}


const zend_function_entry linger_TrieTree_methods[] = {
	PHP_FE_END	
};

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(linger_TrieTree)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Linger\\TrieTree", linger_TrieTree_methods);
    linger_TrieTree_ce = zend_register_internal_class(&ce TSRMLS_CC);
    linger_TrieTree_ce->create_object = linger_TrieTree_create_object_handler;
    memcpy(&linger_TrieTree_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(linger_TrieTree)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(linger_TrieTree)
{
	return SUCCESS;
}
/* }}} */

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
}
/* }}} */

/* {{{ linger_TrieTree_module_entry
 */
zend_module_entry linger_TrieTree_module_entry = {
	STANDARD_MODULE_HEADER,
	"linger_TrieTree",
	linger_TrieTree_functions,
	PHP_MINIT(linger_TrieTree),
	PHP_MSHUTDOWN(linger_TrieTree),
	PHP_RINIT(linger_TrieTree),		
	PHP_RSHUTDOWN(linger_TrieTree),	
	PHP_MINFO(linger_TrieTree),
	PHP_LINGER_TRIETREE_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_LINGER_TRIETREE
ZEND_GET_MODULE(linger_TrieTree)
#endif

