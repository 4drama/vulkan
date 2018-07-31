#ifndef RENDERER_HPP_
#define RENDERER_HPP_

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan\vulkan.h>

namespace vk{

class renderer{
public:
	renderer();
	~renderer();
	
	renderer(renderer& ) = delete;
	renderer& operator=(const renderer& ) = delete;
	
	renderer(renderer&&) = default;
private:
	VkInstance m_instance;
};

}

#endif