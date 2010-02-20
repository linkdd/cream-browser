SOURCES = AboutModule.c
OBJECTS = $(SOURCES:.c=.o)
HEADERS = AboutModule.h

.PHONY: all

all: $(OBJECTS) $(HEADERS)
	@mv $(OBJECTS) $(top_srcdir)/src/libs

%.o: %.c
	$(CC) $(CFLAGS) $(ALL_CFLAGS) -c $^ -o $@
