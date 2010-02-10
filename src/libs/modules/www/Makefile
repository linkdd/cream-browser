libcream_module_webview_a_SOURCES = WebViewModule.c Favicon.c
libcream_module_webview_a_OBJECTS = $(libcream_module_webview_a_SOURCES:.c=.o)
libcream_module_webview_a_HEADERS = WebViewModule.h Favicon.h

.PHONY: all clean

all: libcream-module-webview.a

clean:
	@rm -vrf libcream-module-webview.a *.o

libcream-module-webview.a: $(libcream_module_webview_a_OBJECTS) $(libcream_module_webview_a_HEADERS)
	$(AR) rcs $@ $(libcream_module_webview_a_OBJECTS)

%.o: %.c
	$(CC) $(CCFLAGS) $(INCLUDES) $(ALL_CFLAGS) -c $^ -o $@
