#ifndef nodephp_http_h
#define nodephp_http_h

#include "php.h"

/**
 * Setup some convenience typedefs for various HTTP types
 */
typedef struct _http_wrap_t http_wrap_t;
typedef struct _http_request_t http_request_t;                     
typedef struct _http_response_t http_response_t;
typedef struct _http_write_t http_write_t;

/**
 * This is the main HTTP object that represents the HTTP server
 * object from PHP and stores the listening tcp socket as well
 * as the connection and close callbacks. This will typically get
 * allocated from PHP everytime a new `node_http` object is
 * instantiated
 */
struct _http_wrap_t {
  zend_object obj;
  uv_tcp_t handle;
  zval *close_cb;
  zval *connection_cb;
};

/**
 * This is the HTTP request handling struct. It is used primarily
 * to store the current parse state of incoming HTTP requests
 */ 
struct _http_request_t {
  uv_tcp_t handle;
  http_parser parser;
  http_wrap_t *parent;
  char *header;
  zval *request;
  zval *headers;
};

/**
 * This struct represents the HTTP response object that gets created
 * for every incoming HTTP request. It's resonsible for storing the
 * state of the response per the request and contains the php object
 * created by the automatic instanstiation of the `node_http_response`
 * object.
 */
struct  _http_response_t {
  zend_object obj;
  zend_object_handle handle;
  uv_tcp_t *socket;
  zval *headers;
  zval *status;
  unsigned int headers_sent : 1;
  unsigned int is_chunked : 1;
  zval *callback;
  zval *string;
};

/**
 * This is an alloc once (FAM) style struct for storing the
 * uv_buf_t alongside the data it points to
 */
struct _http_write_t {
  uv_write_t request;
  uv_buf_t buf;
  char data[1];
};

// object ctors and dtors

/**
 * This is the C constructor for the `node_http` object. It gets called
 * every time a `node_http` object is instanstiated
 *
 * @param {zend_clas_entry *} class_type - The class entry struct
 * @param {TSRMLS_CC}                    - PHPs thread safe resource mngmnt
 * @returns {zend_object_value}
 */
zend_object_value http_new(zend_class_entry *class_type TSRMLS_DC);

/**
 * This is the C destructor for the `node_http` object. It gets called
 * everytime a `node_http` object get's destructed
 *
 * @param {void *} object - The `node_http` object via it's `http_wrap_t`
 * @param {TSRMLS_CC}     - PHPs thread safe resource mngmnt
 * @returns {void}
 */
void http_wrap_free(void *object TSRMLS_DC);

/**
 * This is the C constructor for the `node_http_response` object. It gets
 * called every time a `node_http_response` object is instanstiated
 *
 * @param {zend_class_entry *} class_type - The class entry struct
 * @param {TSRMLS_CC}                     - PHPs thread safe resource mngmnt
 * @returns {zend_object_value}
 */
zend_object_value http_response_new(zend_class_entry *class_type TSRMLS_DC);

/**
 * This is the C destructor for the `node_http_response` object. It gets
 * called every time a `node_http_response` object gets destructed
 *
 * @param {void *} object - The `node_http_response` object via http_response_t
 * @param {TSRMLS_CC}     - PHPs thread safe resource mngmnt
 * @returns {void}
 */
void http_response_free(void *object TSRMLS_DC);

// node_http object methods

/**
 * This is the C code for the `node_http::listen` function. It's responsible
 * for setting up an accept'ing socket on a specified port and passing
 * requests as the come in, to a request handling callback.
 *
 * PHP protocol as follows:
 * @param {Int} $port         - The port to listen on
 * @param {Function} $handler - The request handler callback
 * @returns {void}
 */
PHP_METHOD(node_http, listen);

// node_http_response object methods

/**
 * TODO:
 */
PHP_METHOD(node_http_response, writeContinue);

/**
 * TODO:
 */
PHP_METHOD(node_http_response, writeHead);

/**
 * This is the C code for the `node_http_response::setStatus` method. When
 * called it sets the HTTP status code of the response. This method only works
 * when it's called before the headers are sent.
 *
 * PHP protocol as follows:
 * @param {Int|String} $status - The HTTP response code to set the response to
 * @returns {Boolean}          - True on success false on failure
 */
PHP_METHOD(node_http_response, setStatus);

/**
 * This is the C code for the `node_http_response::getStatus` method. When
 * called it returns the current HTTP status code for the response
 *
 * PHP protocol as follows:
 * @returns {Int|String} - The current HTTP status code for the response 
 */
PHP_METHOD(node_http_response, getStatus);

/**
 * This is the C code for the `node_http_response::setHeader` method. When
 * called it sets the appropriate header key and value pair, over writing
 * any previous definitions if present. This method only works before the
 * headers are sent.
 *
 * PHP protocol as follows:
 * @param {String} $name  - The name of the header to set
 * @param {String} $value - The value to set the header to
 * @returns {Boolean}     - True on success false on failure
 */
PHP_METHOD(node_http_response, setHeader);

/**
 * This is the C code for the `node_http_response::getHeader` method. When
 * called it looks for the value of the header passed in, if found it returns
 * it as a string otherwise it returns boolean false.
 *
 * PHP protocol as follows:
 * @param {String} $header   - The header to look for and retrieve
 * @returns {Boolean|String} - Boolean false on failure string on success
 */
PHP_METHOD(node_http_response, getHeader);

/**
 * This is the C code for the `node_http_response:removeHeader` method. When
 * called looks for the header specified and removes it from the response
 * if it's found. This method only works before the headers are sent.
 *
 * PHP protocol as follows:
 * @params {String} $header - The header to remove from the response
 * @returns {Boolean}       - True on success false on failure
 */
PHP_METHOD(node_http_response, removeHeader);

/**
 * TODO:
 */
PHP_METHOD(node_http_response, addTrailers);

/**
 * This is the C code for the `node_http_response::write` method. When called
 * it sets the response up to respond in chunked encoding and then writes
 * it's payload to the client.
 *
 * PHP protocol as follows:
 * @param {String} $data - Data to write to the client
 * @returns {Boolean}    - True on success false on failure
 */
PHP_METHOD(node_http_response, write);

/**
 * This is the C code for the `node_http_response::end` method. When called
 * it sends any data that was passed to it and takes the appropriate measures
 * to end the response and close the connection.
 *
 * PHP protocol as follows:
 * @param {String} $data - (optional) Data to write before ending the response
 * @returns {Boolean}    - True on success false on failure
 */
PHP_METHOD(node_http_response, end);

/**
 * Thes are the function registries for the various objects we register
 * as php objects
 */
extern zend_function_entry http_server_methods[];
extern zend_function_entry http_server_response_methods[];

#endif
