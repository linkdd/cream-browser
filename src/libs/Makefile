.PHONY: all clean

all:
	@$(MAKE) -C CreamView
	@$(MAKE) -C curl
	@$(MAKE) -C gopher
	@$(MAKE) -C modules/about
	@$(MAKE) -C modules/ftp
	@$(MAKE) -C modules/www
	@$(MAKE) -C config
	@$(AR) rcs libcream.a *.o

clean:
	@rm -vrf *.o libcream.a
