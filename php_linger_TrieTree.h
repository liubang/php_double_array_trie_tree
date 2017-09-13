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


#ifndef PHP_LINGER_TRIETREE_H
#define PHP_LINGER_TRIETREE_H

extern zend_module_entry linger_TrieTree_module_entry;
#define phpext_linger_TrieTree_ptr &linger_TrieTree_module_entry

#define PHP_LINGER_TRIETREE_VERSION "1.0.0" 

#ifdef PHP_WIN32
#	define PHP_LINGER_TRIETREE_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_LINGER_TRIETREE_API __attribute__ ((visibility("default")))
#else
#	define PHP_LINGER_TRIETREE_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#ifdef ZTS
#define LINGER_TRIETREE_G(v) TSRMG(linger_TrieTree_globals_id, zend_linger_TrieTree_globals *, v)
#else
#define LINGER_TRIETREE_G(v) (linger_TrieTree_globals.v)
#endif

#define linger_efree(ptr) if(ptr) efree(ptr)

#endif	/* PHP_LINGER_TRIETREE_H */

