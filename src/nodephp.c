#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

// nodephp deps
#include "uv.h"
#include "http_parser.h"

#include "nodephp.h"
#include "node_events.h"
#include "node_function.h"
#include "node_http.h"

// php class entries
zend_class_entry *http_server_ce;
zend_class_entry *http_server_response_ce;
zend_class_entry *event_emitter_ce;

PHP_MINIT_FUNCTION(nodephp) {
  zend_class_entry http_ce, request_ce, events_ce;

  // register the event emitter object with php
  INIT_CLASS_ENTRY( events_ce
                  , "node_event_emitter"
                  , event_emitter_methods
                  );
  events_ce.create_object = event_emitter_new;
  event_emitter_ce = zend_register_internal_class(&events_ce TSRMLS_CC);

  // register the http object with php
  INIT_CLASS_ENTRY(http_ce, "node_http", http_server_methods);
  http_ce.create_object = http_new;
  http_server_ce = zend_register_internal_class(&http_ce TSRMLS_CC);

  // register the response object with php
  INIT_CLASS_ENTRY( request_ce
                  , "node_http_response"
                  , http_server_response_methods
                  );
  request_ce.create_object = http_response_new;
  http_server_response_ce = zend_register_internal_class(&request_ce TSRMLS_CC);


  return SUCCESS;
}


PHP_MSHUTDOWN_FUNCTION(nodephp) {
  return SUCCESS;
}


PHP_MINFO_FUNCTION(nodephp) {
  php_info_print_table_start();
  php_info_print_table_header(2, "nodephp", "enabled");
  php_info_print_table_end();
}

PHP_FUNCTION(nodephp_run)
{
  uv_run(uv_default_loop());
  RETURN_NULL();
}

static zend_function_entry nodephp_functions[] = {
  PHP_FE(nodephp_run, NULL)
  NODEPHP_END_FUNCTIONS
};

zend_module_entry nodephp_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
  STANDARD_MODULE_HEADER,
#endif
  NODEPHP_EXTNAME,
  nodephp_functions,
  PHP_MINIT(nodephp),
  PHP_MSHUTDOWN(nodephp),
  NULL,
  NULL,
  PHP_MINFO(nodephp),
#if ZEND_MODULE_API_NO >= 20010901
  NODEPHP_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};

ZEND_GET_MODULE(nodephp)
