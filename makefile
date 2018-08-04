VK_INCLUDE= -IC:\libraries\VulkanSDK\1.1.73.0\Include
VK_LIB= -LC:\libraries\VulkanSDK\1.1.73.0\Lib

LDFLAG= -lstdc++ -lvulkan-1
CFLAGS= -c -Wall -std=c++17

SRC_DIR= ./src/
OBJ_DIR= ./obj/
BIN_DIR= ./bin/
TESTS_DIR= ./tests/

all: $(BIN_DIR)renderer.dll

$(OBJ_DIR)renderer.o: $(SRC_DIR)instance_creator.hpp $(SRC_DIR)handle_wrapper.hpp $(SRC_DIR)renderer.hpp $(SRC_DIR)renderer.cpp
	gcc $(VK_INCLUDE) $(CFLAGS) $(SRC_DIR)renderer.cpp -o $(OBJ_DIR)renderer.o
	
$(OBJ_DIR)instance_creator.o: $(SRC_DIR)handle_wrapper.hpp $(SRC_DIR)instance_creator.hpp $(SRC_DIR)instance_creator.cpp
	gcc $(VK_INCLUDE) $(CFLAGS) $(SRC_DIR)instance_creator.cpp -o $(OBJ_DIR)instance_creator.o

$(OBJ_DIR)handle_wrapper.o: $(SRC_DIR)handle_wrapper.hpp $(SRC_DIR)handle_wrapper.cpp
	gcc $(VK_INCLUDE) $(CFLAGS) $(SRC_DIR)handle_wrapper.cpp -o $(OBJ_DIR)handle_wrapper.o

$(OBJ_DIR)device.o: $(SRC_DIR)device.hpp $(SRC_DIR)device.cpp
	gcc $(VK_INCLUDE) $(CFLAGS) $(SRC_DIR)device.cpp -o $(OBJ_DIR)device.o
	
$(BIN_DIR)renderer.dll: $(OBJ_DIR)renderer.o $(OBJ_DIR)instance_creator.o $(OBJ_DIR)handle_wrapper.o $(OBJ_DIR)device.o
	gcc $(VK_LIB) -shared $(OBJ_DIR)renderer.o $(OBJ_DIR)instance_creator.o $(OBJ_DIR)device.o -o  $(BIN_DIR)renderer.dll $(LDFLAG)
	
run:
	
test1: $(BIN_DIR)renderer_test_1.exe
	$(BIN_DIR)renderer_test_1.exe

$(BIN_DIR)renderer_test_1.exe: $(BIN_DIR)renderer.dll
	gcc $(VK_INCLUDE) $(VK_LIB) -I$(SRC_DIR) $(TESTS_DIR)renderer_test_1.cpp -o $(BIN_DIR)renderer_test_1.exe -L$(BIN_DIR) -lrenderer
	
clean:
	rm -rf  $(OBJ_DIR)*.o
	rm -rf  $(BIN_DIR)*.dll
	rm -rf  $(BIN_DIR)*.exe