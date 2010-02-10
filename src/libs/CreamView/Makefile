libcreamview_a_SOURCES = CreamView.c CreamBackwardForwardList.c
libcreamview_a_OBJECTS = $(libcreamview_a_SOURCES:.c=.o)
libcreamview_a_HEADERS = CreamView.h CreamBackwardForwardList.h

.PHONY: all clean

all: libcreamview.a

clean:
	@rm -vrf libcreamview.a *.o

libcreamview.a: $(libcreamview_a_OBJECTS) $(libcreamview_a_HEADERS)
	$(AR) rcs $@ $(libcreamview_a_OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) $(ALL_CFLAGS) -c $^ -o $@
