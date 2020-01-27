.PHONY: all
all: undo

.PHONY: clean
clean:
	@rm -rf undo

undo: undo.cpp
	@mkdir -p bin
	clang++ undo.cpp -o bin/undo --std=c++14 -O0 -g -fsanitize=address,undefined
