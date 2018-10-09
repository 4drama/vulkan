#include "handle_wrapper.hpp"

template<>
vk_utils::instance_wrapper::~handle_wrapper(){
	if(m_handle != VK_NULL_HANDLE){
		vkDestroyInstance(m_handle, m_allocator_ptr);
	}
}

template<>
vk_utils::device_wrapper::~handle_wrapper(){
	if(m_handle != VK_NULL_HANDLE){
		vkDeviceWaitIdle(m_handle);
		vkDestroyDevice(m_handle, m_allocator_ptr);
	}
}

template<>
vk_utils::surface_wrapper::~handle_wrapper(){
	if(m_handle != VK_NULL_HANDLE){
		vkDestroySurfaceKHR(m_addition_handler, m_handle, m_allocator_ptr);
	}
}

template<>
vk_utils::swapchain_wrapper::~swapchain_wrapper(){
	if(m_handle != VK_NULL_HANDLE){
		vkDestroySwapchainKHR(m_addition_handler, m_handle, m_allocator_ptr);
	}
}

/*
template<>
vk_utils::image_handler_wrapper::~handle_wrapper(){
	if(m_handle != VK_NULL_HANDLE){
		vkDestroyImage(m_addition_handler, m_handle, m_allocator_ptr);
	}
}
*/
