#include <stdlib.h>
#include <string.h>

#include "node_function.h"

zval* node_function_call_zval(zval *func, int argc, ...) {
  zend_fcall_info fci = empty_fcall_info;
  zend_fcall_info_cache fci_cache = empty_fcall_info_cache;
  char *is_callable_error = NULL;
  zval* result;
  va_list argv;

  int call_info = zend_fcall_info_init( func
                                      , 0
                                      , &fci
                                      , &fci_cache
                                      , NULL
                                      , &is_callable_error TSRMLS_CC
                                      );
  if (call_info == SUCCESS) {
    // set pointer to the return zval
    fci.retval_ptr_ptr = &result;
    // gather the argument info
    va_start(argv, argc);
    zend_fcall_info_argv(&fci, argc, &argv);
    va_end(argv);
    // call the function
    zend_call_function(&fci, &fci_cache TSRMLS_CC);
  } else {
    ALLOC_INIT_ZVAL(result);
    ZVAL_BOOL(result, 0);
  }

  zend_fcall_info_args_clear(&fci, 1);
  return result;
}

zend_bool node_function_is_zval_callable(zval *func) {
  char *error;
  zend_bool is_callable;

  is_callable = zend_is_callable_ex( func
                                   , NULL
                                   , 0
                                   , NULL
                                   , NULL
                                   , NULL
                                   , &error TSRMLS_CC
                                   );

  if (error) { efree(error); }

  return is_callable;
}
