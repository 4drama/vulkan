
PLATFORM=
PLATFORM_DEPENDENCE_OBJ=
PLATFORM_DEPENDENCE_SRC=

VK_INCLUDE=
VK_LIB=

LDFLAG= -lstdc++
CFLAGS= -c -Wall -std=c++17 -g

SHARED_FORM=
EXEC_FORM=

LIB_PRE=

ifeq ($(OS),Windows_NT)
	PLATFORM += WIN32

	PLATFORM_DEPENDENCE_OBJ += $(OBJ_DIR)win32_window.o
	PLATFORM_DEPENDENCE_SRC += $(SRC_DIR)win32_window.hpp

	VK_INCLUDE += -IC:\libraries\VulkanSDK\1.1.73.0\Include
	VK_LIB += -LC:\libraries\VulkanSDK\1.1.73.0\Lib

	LDFLAG += -lvulkan-1

	SHARED_FORM =.dll
	EXEC_FORM =.exe
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		PLATFORM += LINUX

		PLATFORM_DEPENDENCE_OBJ += $(OBJ_DIR)xlib_window.o
		PLATFORM_DEPENDENCE_SRC += $(SRC_DIR)xlib_window.hpp

		VK_INCLUDE += -I/home/evgeny/VulkanAPI/1.1.82.0/x86_64/include
		VK_LIB += -L/home/evgeny/VulkanAPI/1.1.82.0/x86_64/lib

		LDFLAG += -lvulkan -lX11
		CFLAGS += -fPIC

		LIB_PRE =lib
		SHARED_FORM =.so
		EXEC_FORM =.out
	endif
endif

SRC_DIR= ./src/
OBJ_DIR= ./obj/
BIN_DIR= ./bin/
TESTS_DIR= ./tests/

all: $(BIN_DIR)renderer$(SHARED_FORM)


$(OBJ_DIR)win32_window.o: $(SRC_DIR)handle_wrapper.hpp $(SRC_DIR)win32_window.hpp $(SRC_DIR)win32_window.cpp
	gcc $(VK_INCLUDE) $(CFLAGS) $(SRC_DIR)win32_window.cpp -o $(OBJ_DIR)win32_window.o

$(OBJ_DIR)xlib_window.o: $(SRC_DIR)handle_wrapper.hpp $(SRC_DIR)xlib_window.hpp $(SRC_DIR)xlib_window.cpp
	gcc $(VK_INCLUDE) $(CFLAGS) $(SRC_DIR)xlib_window.cpp -o $(OBJ_DIR)xlib_window.o


$(OBJ_DIR)renderer.o: $(SRC_DIR)vulkan_exception.hpp $(PLATFORM_DEPENDENCE_SRC) $(SRC_DIR)device.hpp $(SRC_DIR)instance_creator.hpp $(SRC_DIR)handle_wrapper.hpp $(SRC_DIR)renderer.hpp $(SRC_DIR)renderer.cpp
	gcc $(VK_INCLUDE) $(CFLAGS) $(SRC_DIR)renderer.cpp -o $(OBJ_DIR)renderer.o

$(OBJ_DIR)instance_creator.o: $(SRC_DIR)vulkan_exception.hpp $(SRC_DIR)handle_wrapper.hpp $(SRC_DIR)instance_creator.hpp $(SRC_DIR)instance_creator.cpp
	gcc $(VK_INCLUDE) $(CFLAGS) $(SRC_DIR)instance_creator.cpp -o $(OBJ_DIR)instance_creator.o

$(OBJ_DIR)handle_wrapper.o: $(SRC_DIR)handle_wrapper.hpp $(SRC_DIR)handle_wrapper.cpp
	gcc $(VK_INCLUDE) $(CFLAGS) $(SRC_DIR)handle_wrapper.cpp -o $(OBJ_DIR)handle_wrapper.o

$(OBJ_DIR)device.o: $(SRC_DIR)vulkan_exception.hpp $(SRC_DIR)handle_wrapper.hpp $(SRC_DIR)device.hpp $(SRC_DIR)device.cpp
	gcc $(VK_INCLUDE) $(CFLAGS) $(SRC_DIR)device.cpp -o $(OBJ_DIR)device.o

$(OBJ_DIR)platform.o: $(SRC_DIR)vulkan_exception.hpp $(PLATFORM_DEPENDENCE_SRC) $(SRC_DIR)handle_wrapper.hpp $(SRC_DIR)platform.hpp $(SRC_DIR)platform.cpp
	gcc $(VK_INCLUDE) $(CFLAGS) $(SRC_DIR)platform.cpp -o $(OBJ_DIR)platform.o

$(OBJ_DIR)vulkan_exception.o: $(SRC_DIR)vulkan_exception.hpp $(SRC_DIR)vulkan_exception.cpp
	gcc $(VK_INCLUDE) $(CFLAGS) $(SRC_DIR)vulkan_exception.cpp -o $(OBJ_DIR)vulkan_exception.o

$(BIN_DIR)renderer$(SHARED_FORM): $(OBJ_DIR)renderer.o $(OBJ_DIR)instance_creator.o $(OBJ_DIR)handle_wrapper.o $(OBJ_DIR)device.o $(PLATFORM_DEPENDENCE_OBJ) $(OBJ_DIR)platform.o $(OBJ_DIR)vulkan_exception.o
	gcc $(VK_LIB) -shared $(OBJ_DIR)renderer.o $(OBJ_DIR)instance_creator.o $(OBJ_DIR)handle_wrapper.o $(OBJ_DIR)device.o $(PLATFORM_DEPENDENCE_OBJ) $(OBJ_DIR)platform.o $(OBJ_DIR)vulkan_exception.o -o  $(BIN_DIR)$(LIB_PRE)renderer$(SHARED_FORM) $(LDFLAG)

run:

test1: $(BIN_DIR)renderer_test_1$(EXEC_FORM)
	$(BIN_DIR)renderer_test_1$(EXEC_FORM)

$(BIN_DIR)renderer_test_1$(EXEC_FORM): $(BIN_DIR)renderer$(SHARED_FORM)
	gcc $(VK_INCLUDE) $(VK_LIB) -O0 -I$(SRC_DIR) $(TESTS_DIR)renderer_test_1.cpp $(BIN_DIR)$(LIB_PRE)renderer$(SHARED_FORM) -o $(BIN_DIR)renderer_test_1$(EXEC_FORM)

clean:
	rm -rf  $(OBJ_DIR)*.o

	rm -rf  $(BIN_DIR)*.dll
	rm -rf  $(BIN_DIR)*.so

	rm -rf  $(BIN_DIR)*.exe
	rm -rf  $(BIN_DIR)*.out
