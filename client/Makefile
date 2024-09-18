CMAKE_BUILD_DIR ?= build
NPROCS ?= $(shell nproc)

.PHONY: all
all: build

.PHONY: build
build:
	cmake -B $(CMAKE_BUILD_DIR)
	cmake --build $(CMAKE_BUILD_DIR) -j $(NPROCS)

.PHONY: run
run: build
	./$(CMAKE_BUILD_DIR)/Client

.PHONY: clean
clean:
	rm -rf $(CMAKE_BUILD_DIR)