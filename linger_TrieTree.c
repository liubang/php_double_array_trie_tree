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
#include "ext/standard/php_var.h"
#include "php_linger_TrieTree.h"

static int le_linger_TrieTree;

zend_class_entry *linger_TrieTree_ce;
static zend_object_handlers linger_TrieTree_object_handlers;

#if PHP_MAJOR_VERSION >= 5 && PHP_MAJOR_VERSION < 7
static void linger_TrieTree_free_object_storage_handler(zend_object *internal TSRMLS_DC)
#elif PHP_MAJOR_VERSION >= 7
static void linger_TrieTree_free_object_storage_handler(zend_object *internal)
#endif
{
    // TrieObject *trieObject = ZOBJ_GET_TRIE_OBJECT(internal);
    TrieObject *trieObject = (TrieObject *) internal;
    trie_free(trieObject->trie);
#if PHP_MAJOR_VERSION >= 5 && PHP_MAJOR_VERSION < 7
    zend_object_std_dtor(internal TSRMLS_CC);
#elif PHP_MAJOR_VERSION >= 7
    zend_object_std_dtor(internal);
#endif
    linger_efree(internal);
}

static void php_error_func(const char *docref, int type, const char *format) {
#if PHP_MAJOR_VERSION >= 5 && PHP_MAJOR_VERSION < 7
    php_error_docref(docref TSRMLS_CC, type, format);
#elif PHP_MAJOR_VERSION >= 7
    php_error_docref(docref, type, format);
#endif
}

static void php_throw_exception_func(zend_class_entry *exception_ce, const char *message, int64_t code) {
#if PHP_MAJOR_VERSION >= 5 && PHP_MAJOR_VERSION < 7
    zend_throw_exception(exception_ce, message, (zend_ulong) code TSRMLS_CC);
#elif PHP_MAJOR_VERSION >= 7
    zend_throw_exception(exception_ce, message, (zend_long) code);
#endif
}


#define TRIE_NEW(trie, alpha_map)                                       \
    do {                                                                \
        alpha_map = alpha_map_new();                                    \
        if (!alpha_map) {                                               \
            php_error_func(NULL, E_ERROR, "init error");    \
        }                                                               \
        if (alpha_map_add_range(alpha_map, 0x00, 0xff) != 0) {          \
            alpha_map_free(alpha_map);                                  \
            php_error_func(NULL, E_ERROR, "init error");    \
        }                                                               \
        trie = trie_new(alpha_map);                                     \
        alpha_map_free(alpha_map);                                      \
        if (!trie) {                                                    \
            php_error_func(NULL, E_ERROR, "init error");    \
        }                                                               \
    } while(0)

#if PHP_MAJOR_VERSION >= 5 && PHP_MAJOR_VERSION < 7
zend_object_value linger_TrieTree_create_object_handler(zend_class_entry *class_type TSRMLS_DC)
{
    zend_object_value retval;
    TrieObject *intern = emalloc(sizeof(TrieObject));
    memset(intern, 0, sizeof(TrieObject));
    intern->trie = NULL;
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
#elif PHP_MAJOR_VERSION >= 7
zend_object *linger_TrieTree_create_object_handler(zend_class_entry *class_type) {
    TrieObject *internal;
    internal = ecalloc(1, sizeof(TrieObject) + zend_object_properties_size(class_type)); 
    internal->trie = NULL;

    zend_object_std_init(&internal->std, class_type);
    object_properties_init(&internal->std, class_type);
    internal->std.handlers = &linger_TrieTree_object_handlers;

    return &internal->std;
}
#endif


/* linger/TrieTree methods */

PHP_METHOD(linger_TrieTree, __construct)
{
    char *path;
    long path_len;
    Trie *trie;
    TrieObject *intern;
#if PHP_MAJOR_VERSION >= 5 && PHP_MAJOR_VERSION < 7
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &path, &path_len) == FAILURE) {
#elif PHP_MAJOR_VERSION >= 7
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &path, &path_len) == FAILURE) {
#endif
        RETURN_NULL();
    }
    // intern = zend_object_store_get_object(getThis() TSRMLS_CC);
    intern = GET_TRIE_OBJECT(getThis());
    intern->trie = trie_new_from_file(path);
    if (!intern->trie) {
        php_throw_exception_func(NULL, "Unable to load file", 0);
    }
}

#define MAKE_ALPHA_TEXT(alpha_text, text, text_len)                 \
    do {                                                            \
        alpha_text = emalloc(sizeof(AlphaChar) * (text_len + 1));   \
        int i;                                                      \
        for (i = 0; i < text_len; i++) {                            \
            alpha_text[i] = (AlphaChar) text[i];                    \
        }                                                           \
        alpha_text[text_len] = TRIE_CHAR_TERM;                      \
    } while(0)


static int trie_search_one(Trie *trie, unsigned char *org_text, int org_text_len, zval **data)
{
    TrieState *s;
    const AlphaChar *p;
    const AlphaChar *base;
    AlphaChar *text;
    MAKE_ALPHA_TEXT(text, org_text, org_text_len);
    base = text;

    if (!(s = trie_root(trie))) {
        linger_efree(base);
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
            trie_state_free(s);
            int str_len = p - text;
            unsigned char *str = emalloc(sizeof(unsigned char) * (str_len + 1));
            strncpy(str, org_text + (text - base), str_len);
            str[str_len] = '\0';
            MY_ZVAL_STRING(*data, str, 1);
            linger_efree(str);
            linger_efree(base);
            return 1;
        }

        trie_state_rewind(s);
        text++;
    }

    trie_state_free(s);
    linger_efree(base);
    return 0;
}


static int trie_search_all(Trie *trie, unsigned char *org_text, int org_text_len, zval **data)
{
    TrieState *s;
    const AlphaChar *p;
    AlphaChar *base;
    AlphaChar *text;
    MAKE_ALPHA_TEXT(text, org_text, org_text_len);
    zval *word = NULL;
    base = text;
    if (! (s = trie_root(trie))) {
        linger_efree(base);
        return -1;
    }

    while (*text) {
        p = text;
        if(! trie_state_is_walkable(s, *p)) {
            trie_state_rewind(s);
            text++;
            continue;
        }
        while(*p && trie_state_is_walkable(s, *p) && ! trie_state_is_leaf(s)) {
            trie_state_walk(s, *p++);
            if (trie_state_is_terminal(s)) {
                MY_MAKE_STD_ZVAL(word);
                int str_len = p - text;
                unsigned char *str = emalloc(sizeof(unsigned char) * (str_len + 1));
                strncpy(str, org_text + (text - base), str_len);
                str[str_len] = '\0';
                MY_ZVAL_STRING(word, str, 1);
                linger_efree(str);
                add_next_index_zval(*data, word);
            }
        }
        trie_state_rewind(s);
        text++;
    }
    trie_state_free(s);
    linger_efree(base);
    return 0;
}


PHP_METHOD(linger_TrieTree, searchOne)
{
    char *text;
    int text_len;

#if PHP_MAJOR_VERSION >= 5 && PHP_MAJOR_VERSION < 7
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &text, &text_len) == FAILURE) {
#elif PHP_MAJOR_VERSION >= 7
    zend_string *zs_text;
    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "S", &zs_text) == FAILURE) {
#endif
        return;
    }

#if PHP_MAJOR_VERSION >= 7
    text = ZSTR_VAL(zs_text);
    text_len = ZSTR_LEN(zs_text);
#endif

    if (!text_len) {
        php_throw_exception_func(NULL, "empty str!", 0);
    }

    TrieObject *intern = GET_TRIE_OBJECT(getThis());
    int offset = -1, ret;
    ret = trie_search_one(intern->trie, (unsigned char *) text, text_len, &return_value);
    if (ret == 0) {
        RETURN_NULL();
    } else if (ret == -1) {
        RETURN_FALSE;
    }
}

PHP_METHOD(linger_TrieTree, searchAll)
{
    char *text;
    int text_len;

#if PHP_MAJOR_VERSION >= 5 && PHP_MAJOR_VERSION < 7
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &text, &text_len) == FAILURE) {
#else
    zend_string *zs_text;
    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "S", &zs_text) == FAILURE) {
#endif
        return;
    }

#if PHP_MAJOR_VERSION >= 7
    text = ZSTR_VAL(zs_text);
    text_len = ZSTR_LEN(zs_text);
#endif

    if (!text_len) {
        php_throw_exception_func(NULL, "empty str!", 0);
        return;
    }
    array_init(return_value);
    int i, ret;
    //TrieObject *intern = zend_object_store_get_object(getThis() TSRMLS_CC);
    TrieObject *intern = GET_TRIE_OBJECT(getThis());
    ret = trie_search_all(intern->trie, text, text_len, &return_value);
    if (ret == -1) {
        RETURN_FALSE;
    }
}

#define KEYWORD_MAX_LEN 1024

PHP_METHOD(linger_TrieTree, build)
{
    zval *keyword_arr = NULL;
    AlphaChar alpha_key[KEYWORD_MAX_LEN + 1];
    char *file;
    int file_len;
#if PHP_MAJOR_VERSION >= 5 && PHP_MAJOR_VERSION < 7
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zs", &keyword_arr, &file, &file_len) == FAILURE) {
#elif PHP_MAJOR_VERSION >= 7
    zend_string *file_path;
    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "zS", &keyword_arr, &file_path) == FAILURE) {
#endif
        return;
    }

#if PHP_MAJOR_VERSION >= 7
    file = ZSTR_VAL(file_path);
    file_len = ZSTR_LEN(file_path);
#endif

    if (Z_TYPE_P(keyword_arr) != IS_ARRAY) {
        php_throw_exception_func(NULL, "the first parameter must be array", 0);
        return;
    }

    zval **current;
    HashTable *ht_keys = HASH_OF(keyword_arr);
    if (!ht_keys) {
        php_throw_exception_func(NULL, "Couldn't get HashTable in parameter one", 0);
        return;
    }

    Trie *trie;
    AlphaMap *alpha_map;
    TRIE_NEW(trie, alpha_map);

#define TRIE_STORE(p, alpha_key, trie)              \
    do {                                            \
        int i = 0;                                  \
        while (*p && *p != '\n' && *p != '\r') {    \
            alpha_key[i++] = (AlphaChar)*p;         \
            p++;                                    \
        }                                           \
        alpha_key[i] = TRIE_CHAR_TERM;              \
        if (!trie_store(trie, alpha_key, -1)) {     \
            RETURN_FALSE;                           \
        }                                           \
    } while(0)

#if PHP_MAJOR_VERSION >=5 && PHP_MAJOR_VERSION < 7
    for (zend_hash_internal_pointer_reset(ht_keys); 
            zend_hash_get_current_data(ht_keys, (void **) &current) == SUCCESS; 
            zend_hash_move_forward(ht_keys)) {
        unsigned char *word = emalloc(Z_STRLEN_P((*current)) + 1);
        memcpy(word, Z_STRVAL_P((*current)), Z_STRLEN_P((*current)) + 1);
        TRIE_STORE(word, alpha_key, trie);
    }
#elif PHP_MAJOR_VERSION >= 7
    zval *zv;
    ZEND_HASH_FOREACH_VAL(ht_keys, zv) {
        unsigned char *word = emalloc(Z_STRLEN_P(zv) + 1);
        memcpy(word, Z_STRVAL_P(zv), Z_STRLEN_P(zv) + 1);
        TRIE_STORE(word, alpha_key, trie);
    } ZEND_HASH_FOREACH_END();
#endif

    trie_save(trie, file);
    trie_free(trie);

    RETURN_TRUE;
}

const zend_function_entry linger_TrieTree_methods[] = {
    PHP_ME(linger_TrieTree, __construct, arginfo_class_linger_trietree___construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(linger_TrieTree, searchOne, arginfo_class_linger_trietree_searchone, ZEND_ACC_PUBLIC)
    PHP_ME(linger_TrieTree, searchAll, arginfo_class_linger_trietree_searchall, ZEND_ACC_PUBLIC)
    PHP_ME(linger_TrieTree, build, arginfo_class_linger_trietree_build, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_FE_END
};

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(linger_TrieTree)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Linger\\TrieTree", linger_TrieTree_methods);
#if PHP_MAJOR_VERSION >=5 && PHP_MAJOR_VERSION < 7
    linger_TrieTree_ce = zend_register_internal_class(&ce TSRMLS_CC);
#elif PHP_MAJOR_VERSION >= 7
    linger_TrieTree_ce = zend_register_internal_class(&ce);
#endif
    linger_TrieTree_ce->create_object = linger_TrieTree_create_object_handler;
    memcpy(&linger_TrieTree_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

#if PHP_MAJOR_VERSION >= 7
    linger_TrieTree_object_handlers.offset = XtOffsetOf(TrieObject, std);
    linger_TrieTree_object_handlers.free_obj = linger_TrieTree_free_object_storage_handler;
#endif

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

