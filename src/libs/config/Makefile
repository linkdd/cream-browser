.PHONY: all

all: libconfig.la

libconfig.la: config.lo config.h
	@$(LIBTOOL) --mode=link $(CC) $(LDFLAGS) $(LIBCONFIG_LDFLAGS) -o $@ $^

%.lo: %.c
	@$(LIBTOOL) --mode=compile $(CC) $(CFLAGS) $(LIBCONFIG_CFLAGS) $^
