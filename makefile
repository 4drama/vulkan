VK_INCLUDE= -IC:\libraries\VulkanSDK\1.1.73.0\Include
VK_LIB= -LC:\libraries\VulkanSDK\1.1.73.0\Lib

LDFLAG= -lstdc++ -lvulkan-1
CFLAGS= -c -Wall -std=c++17

SRC_DIR= ./src/

all: renderer.o vk_utils.o

renderer.o: $(SRC_DIR)vk_utils.hpp $(SRC_DIR)handle_wrapper.hpp $(SRC_DIR)renderer.cpp
	gcc $(VK_INCLUDE) $(VK_LIB) $(CFLAGS) $(SRC_DIR)renderer.cpp -o renderer.o
	
vk_utils.o: $(SRC_DIR)handle_wrapper.hpp $(SRC_DIR)vk_utils.cpp
	gcc $(VK_INCLUDE) $(VK_LIB) $(CFLAGS) $(SRC_DIR)vk_utils.cpp -o vk_utils.o
	
clean:
	rm -rf *.o