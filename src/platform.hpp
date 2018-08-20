#ifndef PLATFORM_HPP_
#define PLATFORM_HPP_

#if defined(_WIN32)
	#define VK_USE_PLATFORM_WIN32_KHR
#elif defined(__linux__)
	#define VK_USE_PLATFORM_XLIB_KHR
#endif

#include "handle_wrapper.hpp"

#if defined(_WIN32)
	#include "win32_window.hpp"

	namespace vk_utils{
		using window_wrapper = win32_window_wrapper;
		using platform_window_creator = win32_window_creator;
	}
#elif defined(__linux__)
	#include "xlib_window.hpp"

	namespace vk_utils{
		using window_wrapper = xlib_window_wrapper;
		using platform_window_creator = xlib_window_creator;
	}
#endif

namespace vk_utils{

	platform_window_creator get_platform_window();

	const char* get_surface_extension_name();

	surface_wrapper create_surface(
		const window_wrapper& window,
		VkInstance instance,
		VkAllocationCallbacks *allocator_ptr);

	bool get_physical_device_presentation_support(
		const window_wrapper& window,
		VkPhysicalDevice physical_device,
		uint32_t family_index) noexcept;

	void show_window(const window_wrapper& window) noexcept;

}

#endif //PLATFORM_HPP_
