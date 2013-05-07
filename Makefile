CC?=gcc

UVDIR=deps/libuv
HTTPDIR=deps/http-parser
BUILDDIR=build
EXTDIR=$(shell php-config --extension-dir)

OS_NAME=$(shell uname -s)
MH_NAME=$(shell uname -m)

CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -Werror
CFLAGS += -Wno-unused-parameter
CFLAGS += --std=c99
CFLAGS += -DPHP_ATOM_INC
CFLAGS += -DEV_MULTIPLICITY=1
CFLAGS += -DPIC
CFLAGS += -fno-common
CFLAGS += -fPIC

CFLAGS_FAST = $(CFLAGS) -O0 -g
CFLAGS_DEBUG = $(CFLAGS) -O0 -g

LDFLAGS = -bundle -flat_namespace -undefined suppress

INCLUDES = -I$(UVDIR)/include             \
           -I$(HTTPDIR)                   \
           $(shell php-config --includes)

OBJ = $(BUILDDIR)/node_events.o           \
      $(BUILDDIR)/node_function.o         \
      $(BUILDDIR)/node_http.o             \
      $(BUILDDIR)/nodephp.o

DEPS = $(UVDIR)/uv.a                      \
       $(HTTPDIR)/libhttp_parser.o

$(BUILDDIR)/nodephp.so: $(DEPS) $(OBJ)
	$(CC) $(CFLAGS_FAST) $(LDFLAGS) -o $@ $^

$(UVDIR)/Makefile:
	git submodule update --init $(UVDIR)

$(UVDIR)/uv.a: $(UVDIR)/Makefile
	$(MAKE) -C $(UVDIR) uv.a

$(HTTPDIR)/Makefile:
	git submodule update --init $(HTTPDIR)

$(HTTPDIR)/libhttp_parser.o: $(HTTPDIR)/Makefile
	$(MAKE) -C $(HTTPDIR) libhttp_parser.o

install:
	cp $(BUILDDIR)/nodephp.so $(EXTDIR)/

tag:
	etags *.c *.h

dep:
	$(CC) -MM *.c $(INCLUDES)

clean:
	rm -rf build nodephp.so
	$(MAKE) -C $(UVDIR) clean
	$(MAKE) -C $(HTTPDIR) clean

$(BUILDDIR)/%.o: src/%.c $(DEP)
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS_FAST) $(INCLUDES) -c $< -o $@
