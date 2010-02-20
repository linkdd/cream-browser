SOURCES = WebViewModule.c Favicon.c
OBJECTS = $(SOURCES:.c=.o)
HEADERS = WebViewModule.h Favicon.h

.PHONY: all

all: $(OBJECTS) $(HEADERS)
	@mv $(OBJECTS) $(top_srcdir)/src/libs

%.o: %.c
	$(CC) $(CFLAGS) $(ALL_CFLAGS) -c $^ -o $@
