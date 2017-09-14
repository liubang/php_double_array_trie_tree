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

#include "src/datrie/triedefs.h"
#include "src/datrie/typedefs.h"
#include "src/datrie/trie.h"

static int le_linger_TrieTree;

zend_class_entry *linger_TrieTree_ce;
static zend_object_handlers linger_TrieTree_object_handlers;

typedef struct _TrieObject {
    zend_object std;
    Trie *trie;
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
    TrieObject *intern = emalloc(sizeof(TrieObject)); 
    memset(intern, 0, sizeof(TrieObject));
    Trie *trie;
    AlphaMap *alpha_map;
    alpha_map = alpha_map_new();
    if (!alpha_map) {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "init error");
    }
    if (alpha_map_add_range(alpha_map, 0x00, 0xff) != 0) {
        alpha_map_free(alpha_map);
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "init error");
    }
    trie = trie_new(alpha_map);
    alpha_map_free(alpha_map);
    if (!trie) {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "init error");
    }
    intern->trie = trie;
    zend_object_std_init(&intern->std, class_type TSRMLS_CC);
    retval.handle = zend_objects_store_put(
            intern,
            (zend_objects_store_dtor_t) zend_objects_destroy_object,
            (zend_objects_free_object_storage_t) linger_TrieTree_free_object_storage_handler,
            NULL
            TSRMLS_CC
            );
    retval.handlers = &linger_TrieTree_object_handlers;
    return retval;
}


/* linger/TrieTree methods */

PHP_METHOD(linger_TrieTree, __construct)
{
    char *path;
    long path_len;
    Trie *trie;
    TrieObject *intern;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &path, &path_len) == FAILURE) {
        RETURN_NULL();
    }
    intern = zend_object_store_get_object(getThis() TSRMLS_CC);
    intern->trie = trie_new_from_file(path);
    if (!intern->trie) {
        zend_throw_exception(NULL, "Unable to load file", 0 TSRMLS_CC);
    }
}

static int trie_search_one(Trie *trie, const AlphaChar *text, long *offset, TrieData *length)
{
    TrieState *s;
    const AlphaChar *p;
    const AlphaChar *base;

    if (!(s = trie_root(trie))) {
        return -1;
    }

    while (*text) {
        p = text;
        if (!trie_state_is_walkable(s, *p)) {
            trie_state_rewind(s);
            text++;
            continue;
        } else {
            trie_state_walk(s, *p++);
        }

        while (trie_state_is_walkable(s, *p) && !trie_state_is_terminal(s)) {
            trie_state_walk(s, *p++);
        }

        if (trie_state_is_terminal(s)) {
            *offset = text - base;
            *length = p - text;
            trie_state_free(s);
            return 1;
        }
        trie_state_rewind(s);
        text++;
    }
    trie_state_free(s);
    return 0;
}

static int trie_search_all(Trie *trie, const AlphaChar *text, zval *data)
{
    TrieState *s;    
    const AlphaChar *p;
    const AlphaChar *base;
    zval *word = NULL;
    base = text;
    if (!(s = trie_root(trie))) {
        return -1;
    }
    while (*text) {
        p = text;
        if (!trie_state_is_walkable(s, *p)) {
            trie_state_rewind(s);
            text++;
            continue;
        }
        while (*p && trie_state_is_walkable(s, *p) && !trie_state_is_leaf(s)) {
            trie_state_walk(s, *p++);
            if (trie_state_is_terminal(s)) {
                MAKE_STD_ZVAL(word);
                array_init_size(word, 3);
                add_next_index_long(word, text - base);
                add_next_index_long(word, p - text);
                add_next_index_zval(data, word);
            }
        }
        trie_state_rewind(s);
        text++;
    }
    trie_state_free(s);
    return 0;
}

PHP_METHOD(linger_TrieTree, searchOne)
{
    char *text;
    int text_len;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &text, &text_len) == FAILURE) {
        RETURN_FALSE;
    }

    if (text_len < 1 || strlen(text) != text_len) {
        zend_throw_exception(NULL, "empty str!", 0 TSRMLS_CC);
    }

    array_init(return_value);
    AlphaChar *alpha_text;
    TrieObject *intern = zend_object_store_get_object(getThis() TSRMLS_CC);
    alpha_text = emalloc(sizeof(AlphaChar) * (text_len + 1));
    for (int i = 0; i < text_len; i++) {
        alpha_text[i] = (AlphaChar) text[i]; 
    }
    alpha_text[text_len] = TRIE_CHAR_TERM;
    long offset = -1, ret;
    TrieData length = 0;
    ret = trie_search_one(intern->trie, alpha_text, &offset, &length);
    linger_efree(alpha_text);
    if (ret == 0) {
        return;
    } else if (ret == 1) {
        add_next_index_long(return_value, offset);
        add_next_index_long(return_value, length);
    } else {
        RETURN_FALSE;
    }
}

PHP_METHOD(linger_TrieTree, searchAll)
{
    char *text;
    int text_len;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &text, &text_len) == FAILURE) {
        RETURN_FALSE;
    }
    if (text_len < 1 || strlen(text) != text_len) {
        zend_throw_exception(NULL, "empty str", 0 TSRMLS_CC);
        return;
    }
    array_init(return_value);
    AlphaChar *alpha_text;
    int i, ret;
    alpha_text = emalloc(sizeof(AlphaChar) * (text_len + 1));
    for (i = 0; i < text_len; i++) {
        alpha_text[i] = (AlphaChar)text[i];
    }
    alpha_text[text_len] = TRIE_CHAR_TERM;
    TrieObject *intern = zend_object_store_get_object(getThis() TSRMLS_CC);
    ret = trie_search_all(intern->trie, alpha_text, return_value);
    efree(alpha_text);
    if (ret == 0) {
        return;     
    } else {
        RETURN_FALSE;
    }
}

const zend_function_entry linger_TrieTree_methods[] = {
    PHP_ME(linger_TrieTree, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(linger_TrieTree, searchOne, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_TrieTree, searchAll, NULL, ZEND_ACC_PUBLIC)
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
    NULL,
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

