.PHONY: all clean

all:
	@make -C config all
	@make -C creamview all
	@make -C modules all

clean:
	@find -name "*.o" -delete
	@find -name "*.a" -delete
	@find -name "*.lo" -delete
	@find -name "*.la" -delete
	@find -name ".libs" -exec rm -rf {} \;

