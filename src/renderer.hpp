#ifndef RENDERER_HPP_
#define RENDERER_HPP_

#if defined(_WIN32)
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <vulkan\vulkan.h>

#include "handle_wrapper.hpp"
#include "device.hpp"

#if defined(_WIN32)
#include "win32_window.hpp"
#endif

namespace vk{

class renderer{
	using instance_wrapper = vk_utils::instance_wrapper;
	using device = vk_utils::device;

#if defined(_WIN32)
	using window_wrapper = vk_utils::win32_window_wrapper;
#endif

public:
	renderer();
	~renderer();

	renderer(renderer& ) = delete;
	renderer& operator=(const renderer& ) = delete;

	renderer(renderer&&) = default;
private:
	instance_wrapper m_instance;
	device m_device;
	window_wrapper m_window;
};

}

#endif
