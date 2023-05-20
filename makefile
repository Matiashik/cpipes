build:
	@if [ ! -d "out" ]; then \
		mkdir out; \
	fi;
	@clang src/main.c -lncurses -o out/cpipes
run:
	@make build
	@./out/cpipes
install:
	@make build
	@mv out/cpipes /usr/bin/