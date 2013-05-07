#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "nodephp.h"
#include "node_events.h"
#include "node_function.h"

// pivate protocols
int _node_event_emitter_on(zend_object *self, zval *event, zval *handler);
int _node_event_emitter_once(zend_object *self, zval *event, zval *handler);
int _node_event_emitter_add_to_array(zval *ht, zval *event, zval *handler);
int _node_event_emitter_on(zend_object *self, zval *event, zval *handler);
int _node_event_emitter_emit(zend_object *self, zval *event, zval *data);

// register methods for the event_emitter object
zend_function_entry event_emitter_methods[] = {
  PHP_ME(node_event_emitter, addListener,        NULL, ZEND_ACC_PUBLIC)
  PHP_ME(node_event_emitter, on,                 NULL, ZEND_ACC_PUBLIC)
  PHP_ME(node_event_emitter, once,               NULL, ZEND_ACC_PUBLIC)
  PHP_ME(node_event_emitter, removeListener,     NULL, ZEND_ACC_PUBLIC)
  PHP_ME(node_event_emitter, removeAllListeners, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(node_event_emitter, listeners,          NULL, ZEND_ACC_PUBLIC)
  PHP_ME(node_event_emitter, emit,               NULL, ZEND_ACC_PUBLIC)
  NODEPHP_END_FUNCTIONS
};

// object ctors and dtros
zend_object_value event_emitter_new(zend_class_entry *class_type TSRMLS_DC) {
  zend_object_value instance;
  event_emitter_t *emitter = emalloc(sizeof(event_emitter_t));

  ALLOC_INIT_ZVAL(emitter->listeners);
  ALLOC_INIT_ZVAL(emitter->once);
  array_init(emitter->listeners);
  array_init(emitter->once);

  zend_object_std_init(&emitter->obj, class_type TSRMLS_CC);
  init_properties(&emitter->obj, class_type);

  TSRMLS_SET(emitter);

  instance.handle = zend_objects_store_put((void*) emitter,
                                           (zend_objects_store_dtor_t) zend_objects_destroy_object,
                                           event_emitter_free,
                                           NULL
                                           TSRMLS_CC);
  instance.handlers = zend_get_std_object_handlers();

  return instance;
}

void event_emitter_free(void *object TSRMLS_DC) {
  event_emitter_t *emitter = (event_emitter_t*) object;
  zend_objects_free_object_storage(&emitter->obj TSRMLS_CC);
  zval_ptr_dtor(&emitter->listeners);
  zval_ptr_dtor(&emitter->once);
}

// private methods
int _node_event_emitter_on(zend_object *self, zval *event, zval *handler) {
  event_emitter_t *emitter = (event_emitter_t*) self;

  return _node_event_emitter_add_to_array( emitter->listeners
                                         , event
                                         , handler
                                         );
}

int _node_event_emitter_once(zend_object *self, zval *event, zval *handler) {
  event_emitter_t *emitter = (event_emitter_t*) self;

  return _node_event_emitter_add_to_array( emitter->once
                                         , event
                                         , handler
                                         );
}

int _node_event_emitter_emit(zend_object *self, zval *event, zval *data) {
    return 0;
}

int _node_event_emitter_add_to_array(zval *ht, zval *event, zval *handler) {
  zval *listeners;
  int index_exists;

  // insure that the event zval is a string
  if (Z_TYPE_P(event) != IS_STRING) {
    return 0;
  }

  // insure that the handler zval is callable
  if (!node_function_is_zval_callable(handler)) {
    return 0;
  }

  // lets find the index for the set of callbacks
  index_exists = zend_hash_find( Z_ARRVAL_P(ht)
                               , Z_STRVAL_P(event)
                               , Z_STRLEN_P(event)
                               , (void**)&listeners
                               );
  
  // if the set of callbacks was not found, lets create one
  if (index_exists == FAILURE) {
    ALLOC_INIT_ZVAL(listeners);
    array_init(listeners);
    add_assoc_zval(ht, Z_STRVAL_P(event), handler);
  }

  // array_push(&listners, handler)
  Z_ADDREF_P(handler);
  add_next_index_zval(listeners, handler);

  return 1;
}

// node_event_emitter object methods
PHP_METHOD(node_event_emitter, addListener) {
  zend_object *self = zend_object_store_get_object(getThis() TSRMLS_CC);
  int argc = ZEND_NUM_ARGS();
  zval *event, *handler;
  int result = zend_parse_parameters(argc TSRMLS_CC, "zz", &event, &handler);
  
  if (result == FAILURE) {
    RETURN_BOOL(0);
  } else {
    RETURN_BOOL(_node_event_emitter_on(self, event, handler));
  }
}

PHP_METHOD(node_event_emitter, on) {
  zend_object *self = zend_object_store_get_object(getThis() TSRMLS_CC);
  int argc = ZEND_NUM_ARGS();
  zval *event, *handler;
  int result = zend_parse_parameters(argc TSRMLS_CC, "zz", &event, &handler);
  
  if (result == FAILURE) {
    RETURN_BOOL(0);
  } else {
    RETURN_BOOL(_node_event_emitter_on(self, event, handler));
  }
}

PHP_METHOD(node_event_emitter, once) {
  zend_object *self = zend_object_store_get_object(getThis() TSRMLS_CC);
  int argc = ZEND_NUM_ARGS();
  zval *event, *handler;
  int result = zend_parse_parameters(argc TSRMLS_CC, "zz", &event, &handler);
  
  if (result == FAILURE) {
    RETURN_BOOL(0);
  } else {
    RETURN_BOOL(_node_event_emitter_once(self, event, handler));
  }
}

PHP_METHOD(node_event_emitter, removeListener) {
  RETURN_NULL();
}

PHP_METHOD(node_event_emitter, removeAllListeners) {
  RETURN_NULL();
}

PHP_METHOD(node_event_emitter, listeners) {
  RETURN_NULL();
}

PHP_METHOD(node_event_emitter, emit) {
    /*
  zend_object *self = zend_object_store_get_object(getThis() TSRMLS_CC);
  int argc = ZEND_NUM_ARGS();
  zval *event, *data;
  int result = zend_parse_parameters(argc TSRMLS_CC, "z|z", &event, &data);

  switch(ZEND_NUM_ARGS()) {
  case 0: RETURN_BOOL(0);
  case 1:
    
    break;
  case 2:  RETURN_BOOL(_node_event_emitter_emit());
  default: RETURN_BOOL(0);
  }
    */
}

