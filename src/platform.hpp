#ifndef PLATFORM_HPP_
#define PLATFORM_HPP_

#if defined(_WIN32)
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include "handle_wrapper.hpp"

#if defined(_WIN32)

#include "win32_window.hpp"

namespace vk_utils{

using window_wrapper = win32_window_wrapper;
using platform_window_creator = win32_window_creator;

}

#endif //_WIN32

namespace vk_utils{

platform_window_creator get_platform_window();
const char* get_surface_extension_name();
surface_wrapper create_surface(
	window_wrapper& window, VkInstance instance, VkAllocationCallbacks *allocator_ptr);

}

#endif //PLATFORM_HPP_