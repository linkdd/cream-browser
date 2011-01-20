SOURCES=errors.c config.c
OBJECTS=$(SOURCES:.c=.lo)

.PHONY: all

all: libcream.la

libcream.la: $(OBJECTS)
	@$(LIBTOOL) --mode=link $(CC) -shared -rpath $(DESTDIR)$(PREFIX)/lib/ $(LDFLAGS) $(LIBCREAM_LDFLAGS) $^ -o $@

%.lo: %.c
	@$(LIBTOOL) --mode=compile $(CC) -shared $(CFLAGS) $(LIBCREAM_CFLAGS) $^

