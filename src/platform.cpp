#include "platform.hpp"
#include "vulkan_exception.hpp"

#include <string>

#if defined(_WIN32)
	namespace{
	LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	}
#endif //_WIN32

vk_utils::platform_window_creator vk_utils::get_platform_window(){
#if defined(_WIN32)
	return vk_utils::platform_window_creator(WndProc);
#elif defined(__linux__)
	return vk_utils::platform_window_creator();
#endif
}

const char* vk_utils::get_surface_extension_name(){
#if defined(_WIN32)
	return VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
#elif defined(__linux__)
	return VK_KHR_SURFACE_EXTENSION_NAME;
#endif
}

vk_utils::surface_wrapper vk_utils::create_surface(
	const vk_utils::window_wrapper& window,
	VkInstance instance,
	VkAllocationCallbacks *allocator_ptr){

	VkResult res = VK_SUCCESS;
	VkSurfaceKHR surface = VK_NULL_HANDLE;

#if defined(_WIN32)
	VkWin32SurfaceCreateInfoKHR surface_info;

	surface_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surface_info.pNext = NULL;
	surface_info.flags = 0;
	surface_info.hinstance = window.get().hInstance;
	surface_info.hwnd =  window.get().window;

	res = vkCreateWin32SurfaceKHR(instance, &surface_info, allocator_ptr, &surface);
#elif defined(__linux__)
	VkXlibSurfaceCreateInfoKHR surface_info;

	surface_info.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
	surface_info.pNext = NULL;
	surface_info.flags = 0;
	surface_info.dpy = window.get().display;
	surface_info.window = window.get().window;

	res = vkCreateXlibSurfaceKHR(instance, &surface_info, allocator_ptr, &surface);
#endif
	if(res != VK_SUCCESS){
		std::string msg = "surface create failed";
		throw vk_utils::vulkan_error(msg, res);
	}

	return vk_utils::surface_wrapper(surface, instance, allocator_ptr);
}

bool vk_utils::get_physical_device_presentation_support(
	const vk_utils::window_wrapper& window,
	VkPhysicalDevice physical_device,
	uint32_t family_index) noexcept{

	VkBool32 result = VK_FALSE;

#if defined(_WIN32)
	result = vkGetPhysicalDeviceWin32PresentationSupportKHR(
		physical_device, family_index);
#elif defined(__linux__)

	result = vkGetPhysicalDeviceXlibPresentationSupportKHR(
		physical_device, family_index,
		window.get().display, window.get().visual_info.visualid);
#endif

	if(result == VK_TRUE)
		return true;
	else
		return false;
}

void show_window(const vk_utils::window_wrapper& window) noexcept{
#if defined(_WIN32)
	ShowWindow(window.get().window, SW_SHOW);
	UpdateWindow(window.get().window);
#elif defined(__linux__)
	XMapWindow(window.get().display, window.get().window);
	XFlush(window.get().display);
#endif
}

#if defined(_WIN32)
	namespace{
		LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
			WPARAM wParam, LPARAM lParam){

			switch(message){
			case WM_DESTROY:
				PostQuitMessage(0);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
				break;
			}

			return 0;
		}
	}
#endif //_WIN32
