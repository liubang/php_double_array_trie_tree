PHP_ARG_ENABLE(linger_TrieTree, whether to enable linger_TrieTree support,
[  --enable-linger_TrieTree           Enable linger_TrieTree support])

if test "$PHP_LINGER_TRIETREE" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-linger_TrieTree -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/linger_TrieTree.h"  # you most likely want to change this
  dnl if test -r $PHP_LINGER_TRIETREE/$SEARCH_FOR; then # path given as parameter
  dnl   LINGER_TRIETREE_DIR=$PHP_LINGER_TRIETREE
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for linger_TrieTree files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       LINGER_TRIETREE_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$LINGER_TRIETREE_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the linger_TrieTree distribution])
  dnl fi

  dnl # --with-linger_TrieTree -> add include path
  dnl PHP_ADD_INCLUDE($LINGER_TRIETREE_DIR/include)

  dnl # --with-linger_TrieTree -> check for lib and symbol presence
  dnl LIBNAME=linger_TrieTree # you may want to change this
  dnl LIBSYMBOL=linger_TrieTree # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $LINGER_TRIETREE_DIR/$PHP_LIBDIR, LINGER_TRIETREE_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_LINGER_TRIETREELIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong linger_TrieTree lib version or lib not found])
  dnl ],[
  dnl   -L$LINGER_TRIETREE_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  PHP_SUBST(LINGER_TRIETREE_SHARED_LIBADD)
  source_files="src/datrie/alpha-map.c \
                 src/datrie/darray.c \
                 src/datrie/fileutils.c \
                 src/datrie/tail.c \
                 src/datrie/trie.c \
                 linger_TrieTree.c"
  PHP_NEW_EXTENSION(linger_TrieTree, $source_files, $ext_shared)
fi
