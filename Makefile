.PHONY: build
build: all

.PHONY: run
run: run_demo_random

.PHONY: all
all: debug release

.PHONY: run_demo_random
run_demo_random: all
	build_debug/demo/libdog_random_demo

.PHONY: run_demo_usage
run_demo_usage: all
	build_debug/demo/libdog_usage_demo

.PHONY: runvalgrind
runvalgrind: all
	valgrind --track-fds=yes --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt build_debug/demo/libdog_demo

.PHONY: release
release:
	cmake -Bbuild_release -H. -DCMAKE_BUILD_TYPE=Release
	cmake --build build_release

.PHONY: debug
debug:
	cmake -Bbuild_debug -H. -DCMAKE_BUILD_TYPE=Debug
	cmake --build build_debug

.PHONY: clean
clean:
	rm -r build_debug
	rm -r build_release

.PHONY: test
test: debug
	./build_debug/libdog_test --gtest_break_on_failure --gtest_filter=-Performance.*

.PHONY: test_notation
test_notation: debug
	./build_debug/libdog_test --gtest_break_on_failure --gtest_filter=NotationParsing.*

.PHONY: test_perf
test_perf: release
	./build_release/libdog_test --gtest_break_on_failure --gtest_filter=Performance.*

.PHONY: callgrind
callgrind: release
	valgrind --tool=callgrind ./build_release/libdog_test --gtest_break_on_failure --gtest_filter=Performance.FullGame
