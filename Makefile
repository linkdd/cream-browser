include common.mk

all:
	@make -C $(SRCDIR)/libs all
	@make -C $(SRCDIR)/core all

clean:
	@echo "-- Cleaning"
	@make -C $(SRCDIR)/libs clean
	@make -C $(SRCDIR)/core clean

install:
	@echo "pass"

uninstall:
	@echo "pass"

.PHONY: all clean install uninstall
