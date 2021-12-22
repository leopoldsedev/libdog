DEBUG_DIR=build/Debug
RELEASE_DIR=build/Release

.PHONY: build
build: all

.PHONY: run
run: run_demo_random

.PHONY: all
all: debug release

.PHONY: run_demo_random
run_demo_random: all
	$(DEBUG_DIR)/demo/libdog_random_demo

.PHONY: run_demo_usage
run_demo_usage: all
	$(DEBUG_DIR)/demo/libdog_usage_demo

.PHONY: runvalgrind
runvalgrind: all
	valgrind --track-fds=yes --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt $(DEBUG_DIR)/demo/libdog_demo

.PHONY: release
release:
	cmake -B$(RELEASE_DIR) -H. -DCMAKE_BUILD_TYPE=Release
	cmake --build $(RELEASE_DIR)

.PHONY: debug
debug:
	cmake -B$(DEBUG_DIR) -H. -DCMAKE_BUILD_TYPE=Debug
	cmake --build $(DEBUG_DIR)

.PHONY: clean
clean:
	rm -rf $(DEBUG_DIR)
	rm -rf $(RELEASE_DIR)

.PHONY: test
test: debug
	$(DEBUG_DIR)/libdog_test --gtest_break_on_failure --gtest_filter=-Performance.*

.PHONY: test_notation
test_notation: debug
	$(DEBUG_DIR)/libdog_test --gtest_break_on_failure --gtest_filter=NotationParsing.*

.PHONY: test_perf
test_perf: release
	$(RELEASE_DIR)/libdog_test --gtest_break_on_failure --gtest_filter=Performance.*

.PHONY: callgrind
callgrind: release
	valgrind --tool=callgrind $(RELEASE_DIR)/libdog_test --gtest_break_on_failure --gtest_filter=Performance.FullGame
