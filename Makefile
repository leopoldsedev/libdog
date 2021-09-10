.PHONY: build
build: all

.PHONY: run
run: all
	build/demo/libdog_demo

.PHONY: runvalgrind
runvalgrind: all
	valgrind --track-fds=yes --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt build/demo/libdog_demo

.PHONY: all
all: debug

.PHONY: release
release:
	cmake -Bbuild -H. -DCMAKE_BUILD_TYPE=Release
	cmake --build build

.PHONY: debug
debug:
	cmake -Bbuild -H. -DCMAKE_BUILD_TYPE=Debug
	cmake --build build

.PHONY: clean
clean:
	rm -r build

.PHONY: test
test: debug
	cd build && ctest --verbose --stop-on-failure
