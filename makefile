VK_INCLUDE= -IC:\libraries\VulkanSDK\1.1.73.0\Include
VK_LIB= -LC:\libraries\VulkanSDK\1.1.73.0\Lib

LDFLAG= -lstdc++ -lvulkan-1
CFLAGS= -c -Wall -std=c++17

SRC_DIR= ./src/

ALL:
	gcc $(VK_INCLUDE) $(VK_LIB) $(CFLAGS) $(SRC_DIR)renderer.cpp -o renderer.o
	gcc $(VK_INCLUDE) $(VK_LIB) $(CFLAGS) $(SRC_DIR)vk_utils.cpp -o vk_utils.o
	
clean:
	rm -rf *.o