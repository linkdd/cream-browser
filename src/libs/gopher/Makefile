libcream_gopher_a_SOURCES = GopherClient.c
libcream_gopher_a_OBJECTS = $(libcream_gopher_a_SOURCES:.c=.o)
libcream_gopher_a_HEADERS = GopherClient.h

.PHONY: all clean

all: libcream-gopher.a

clean:
	@rm -vrf libcream-gopher.a *.o

libcream-gopher.a: $(libcream_gopher_a_OBJECTS) $(libcream_gopher_a_HEADERS)
	$(AR) rcs $@ $(libcream_gopher_a_OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) $(ALL_CFLAGS) -c $^ -o $@
