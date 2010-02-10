libcream_module_about_a_SOURCES = AboutModule.c
libcream_module_about_a_OBJECTS = $(libcream_module_about_a_SOURCES:.c=.o)
libcream_module_about_a_HEADERS = AboutModule.h

.PHONY: all clean

all: libcream-module-about.a

clean:
	@rm -vrf libcream-module-about.a *.o

libcream-module-about.a: $(libcream_module_about_a_OBJECTS) $(libcream_module_about_a_HEADERS)
	$(AR) rcs $@ $(libcream_module_about_a_OBJECTS)

%.o: %.c
	$(CC) $(CCFLAGS) $(INCLUDES) $(ALL_CFLAGS) -c $^ -o $@
