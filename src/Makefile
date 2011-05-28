SOURCES = marshal.c \
		WebViewClass.c NotebookClass.c SplittedWindow.c \
		modules.c lua.c \
		clipboard.c util.c \
		theme.c interface.c \
		modules/scheme.c socket.c main.c

HEADERS = marshal.h \
		WebViewClass.h NotebookClass.h SplittedWindow.h \
		modules.h lua.h \
		theme.h interface.h \
		modules/scheme.h structs.h local.h

OBJECTS = $(SOURCES:.c=.o)

.PHONY: all
all: $(EXEC) marshal.txt

$(EXEC): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

marshal.c: marshal.txt marshal.h
	glib-genmarshal --prefix=cream_marshal --body $< > $@

marshal.h: marshal.txt
	glib-genmarshal --prefix=cream_marshal --header $< > $@

.PHONY: clean
clean:
	@rm -vrf $(OBJECTS) $(EXEC) marshal.c marshal.h
