.PHONY: build
build: all

.PHONY: run
run: run_demo_random

.PHONY: run_demo_random
run_demo_random: all
	build/demo/libdog_random_demo

.PHONY: run_demo_usage
run_demo_usage: all
	build/demo/libdog_usage_demo

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

.PHONY: test_notation
test_notation: debug
	cd build && ctest --verbose --stop-on-failure --tests-regex ^NotationParsing.*$

.PHONY: test_perf
test_perf: release
	cd build && ctest --verbose --stop-on-failure --tests-regex ^Performance.*$

.PHONY: callgrind
callgrind: release
	valgrind --tool=callgrind ./build/libdog_test --gtest_filter=Performance.PossibleSeven
