#ifndef RENDERER_HPP_
#define RENDERER_HPP_

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan\vulkan.h>

#include "handle_wrapper.hpp"

namespace vk{

class renderer{
	using instance_wrapper = vk_utils::instance_wrapper;
public:
	renderer();
	~renderer();
	
	renderer(renderer& ) = delete;
	renderer& operator=(const renderer& ) = delete;
	
	renderer(renderer&&) = default;
private:
	instance_wrapper m_instance;
};

}

#endif