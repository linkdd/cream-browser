libcream_module_ftp_a_SOURCES = FTPModule.c $(top_srcdir)/src/libs/curl/CurlClient.c
libcream_module_ftp_a_OBJECTS = $(libcream_module_ftp_a_SOURCES:.c=.o)
libcream_module_ftp_a_HEADERS = FTPModule.h

.PHONY: all clean

all: libcream-module-ftp.a

clean:
	@rm -vrf libcream-module-ftp.a *.o

libcream-module-ftp.a: $(libcream_module_ftp_a_OBJECTS) $(libcream_module_ftp_a_HEADERS)
	$(AR) rcs $@ $(libcream_module_ftp_a_OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) $(ALL_CFLAGS) -c $^ -o $@
