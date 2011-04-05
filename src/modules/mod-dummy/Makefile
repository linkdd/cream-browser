include module.mk

SOURCES += mod-dummy.c
HEADERS += mod-dummy.h
OBJECTS = $(SOURCES:.c=.o)

all: $(LIBNAME)

$(LIBNAME): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	@rm -vrf $(OBJECTS) $(LIBNAME)

.PHONY: all clean



