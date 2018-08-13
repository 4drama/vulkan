#ifndef RENDERER_HPP_
#define RENDERER_HPP_

#include "platform.hpp"

#include <vulkan\vulkan.h>

#include "handle_wrapper.hpp"
#include "device.hpp"

namespace vk{

class renderer{
	using instance_wrapper = vk_utils::instance_wrapper;
	using device = vk_utils::device;
	using window_wrapper = vk_utils::window_wrapper;

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
