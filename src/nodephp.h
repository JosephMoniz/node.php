#ifndef nodephp_node_h
#define nodephp_node_h

typedef unsigned long int ulong;
typedef unsigned short int ushort;
typedef unsigned int uint;

// php deps
#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"

#define NODEPHP_VERSION "1.0"
#define NODEPHP_EXTNAME "nodephp"

/* Shamelessly nicked from mongo-php-driver */
#if ZEND_MODULE_API_NO >= 20100525
#define init_properties(obj, class_type)     \
  object_properties_init((obj), class_type)
#else
#define init_properties(obj, class_type)     \
  do {                                       \
  zval *tmp;                                 \
  zend_hash_copy((obj)->properties,          \
    &class_type->default_properties,         \
    (copy_ctor_func_t) zval_add_ref,         \
    (void *) &tmp,                           \
    sizeof(zval*));                          \
  }                                          \
  while (0)
#endif

#ifdef ZTS
#include "TSRM.h"
# define TSRMLS_SET(o)    (o)->TSRMLS_C = TSRMLS_C
# define TSRMLS_GET(o)    TSRMLS_C = (o)->TSRMLS_C
# define TSRMLS_D_GET(o)  TSRMLS_D = (o)->TSRMLS_C

#else /* ZTS not defined */
# define TSRMLS_SET(o)    /* empty */
# define TSRMLS_GET(o)    /* empty */
# define TSRMLS_D_GET(o)  /* empty */

#endif /* ZTS not defined */

// module initialization, shutdown and info functions
PHP_MINIT_FUNCTION(nodephp);
PHP_MSHUTDOWN_FUNCTION(nodephp);
PHP_MINFO_FUNCTION(nodephp);

// global php functions
PHP_FUNCTION(nodephp_run);

// php class entries
extern zend_class_entry *http_server_ce;
extern zend_class_entry *http_server_response_ce;
extern zend_class_entry *event_emitter_ce;

extern zend_module_entry nodephp_module_entry;
#define phpext_nodephp_ptr &nodephp_module_entry

#endif // nodephp_node_h
