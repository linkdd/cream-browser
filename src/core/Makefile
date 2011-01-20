SOURCES=main.c
OBJECTS=$(SOURCES:.c=.lo)

.PHONY: all

all: $(EXEC) local.h structs.h

$(EXEC): $(OBJECTS)
	@$(LIBTOOL) --mode=link $(CC) $(LDFLAGS) $(GTK_LDFLAGS) $(WEBKIT_LDFLAGS) -lcream $^ -o $@

%.lo: %.c
	@$(LIBTOOL) --mode=compile $(CC) $(CFLAGS) $(GTK_CFLAGS) $(WEBKIT_CFLAGS) $^
