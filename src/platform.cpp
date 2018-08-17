#include "platform.hpp"

#include <stdexcept>
#include <string>

#if defined(_WIN32)

namespace{
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
}

vk_utils::platform_window_creator vk_utils::get_platform_window(){
	return vk_utils::platform_window_creator(WndProc);
}

const char* vk_utils::get_surface_extension_name(){
	return VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
}

vk_utils::surface_wrapper vk_utils::create_surface(
	vk_utils::window_wrapper& window,
	VkInstance instance,
	VkAllocationCallbacks *allocator_ptr){

	VkWin32SurfaceCreateInfoKHR surface_info;

	surface_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surface_info.pNext = NULL;
	surface_info.flags = 0;
	surface_info.hinstance = window.get().hInstance;
	surface_info.hwnd =  window.get().window;

	VkResult res = VK_SUCCESS;
	VkSurfaceKHR surface = VK_NULL_HANDLE;

	res = vkCreateWin32SurfaceKHR(instance, &surface_info, allocator_ptr, &surface);

	if(res != VK_SUCCESS){
		std::string msg = "surface create failed";
		throw std::runtime_error(msg);
	}

	return vk_utils::surface_wrapper(surface, instance, allocator_ptr);
}

bool vk_utils::get_physical_device_presentation_support(
	VkPhysicalDevice physical_device, uint32_t family_index) noexcept{

	VkBool32 result = vkGetPhysicalDeviceWin32PresentationSupportKHR(
		physical_device, family_index);

	if(result == VK_TRUE)
		return true;
	else
		return false;
}

namespace{
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
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
