libcream_curl_a_SOURCES = CurlClient.c
libcream_curl_a_OBJECTS = $(libcream_curl_a_SOURCES:.c=.o)
libcream_curl_a_HEADERS = CurlClient.h

.PHONY: all clean

all: libcream-curl.a

clean:
	@rm -vrf libcream-curl.a *.o

libcream-curl.a: $(libcream_curl_a_OBJECTS) $(libcream_curl_a_HEADERS)
	$(AR) rcs $@ $(libcream_curl_a_OBJECTS)

%.o: %.c
	$(CC) $(CCFLAGS) $(INCLUDES) $(ALL_CFLAGS) -c $^ -o $@
