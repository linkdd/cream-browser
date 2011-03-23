SOURCES = marshal.c \
		WebViewClass.c ViewAreaClass.c \
		errors.c modules.c lua.c \
		clipboard.c util.c \
		modules/scheme.c xdg.c socket.c main.c

HEADERS = marshal.h $(TOPDIR)/cream-browser_build.h \
		WebViewClass.h ViewAreaClass.h \
		errors.h modules.h lua.h \
		modules/scheme.h xdg.h structs.h local.h

OBJECTS = $(SOURCES:.c=.o)

.PHONY: all
all: $(EXEC) marshal.txt

$(EXEC): $(OBJECTS)
	$(CC) $(LDFLAGS) $(GLIB_LDFLAGS) $(GTK_LDFLAGS) $(LUA_LDFLAGS) $^ -o $@

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(GLIB_CFLAGS) $(GTK_CFLAGS) $(LUA_CFLAGS) -c $< -o $@

marshal.c: marshal.txt marshal.h
	glib-genmarshal --prefix=cream_marshal --body $< > $@

marshal.h: marshal.txt
	glib-genmarshal --prefix=cream_marshal --header $< > $@

.PHONY: clean
clean:
	@rm -vrf $(OBJECTS) $(EXEC) marshal.c marshal.h
