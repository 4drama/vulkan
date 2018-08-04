#ifndef RENDERER_HPP_
#define RENDERER_HPP_

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan\vulkan.h>

#include "handle_wrapper.hpp"
#include "device.hpp"

namespace vk{

class renderer{
	using instance_wrapper = vk_utils::instance_wrapper;
	using device = vk_utils::device;
public:
	renderer();
	~renderer();
	
	renderer(renderer& ) = delete;
	renderer& operator=(const renderer& ) = delete;
	
	renderer(renderer&&) = default;
private:
	instance_wrapper m_instance;
	device m_device;
};

}

#endif