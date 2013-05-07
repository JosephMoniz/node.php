#ifndef nodephp_events_h
#define nodephp_events_h

#include "php.h"

typedef struct _event_emitter_t event_emitter_t;

struct _event_emitter_t {
  zend_object obj;
  zval *listeners;
  zval *once;
};

// object ctors and dtros
zend_object_value event_emitter_new(zend_class_entry *class_type TSRMLS_DC);
void event_emitter_free(void *object TSRMLS_DC);

// node_event_emitter object methods
PHP_METHOD(node_event_emitter, addListener);
PHP_METHOD(node_event_emitter, on);
PHP_METHOD(node_event_emitter, once);
PHP_METHOD(node_event_emitter, addListener);
PHP_METHOD(node_event_emitter, removeListener);
PHP_METHOD(node_event_emitter, removeAllListeners);
PHP_METHOD(node_event_emitter, listeners);
PHP_METHOD(node_event_emitter, emit);

extern zend_function_entry event_emitter_methods[];

#endif
