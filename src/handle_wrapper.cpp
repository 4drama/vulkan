#include "handle_wrapper.hpp"

template<>
vk_utils::handle_wrapper<VkInstance, VkAllocationCallbacks>::~handle_wrapper(){
	if(m_handle != VK_NULL_HANDLE){
		vkDestroyInstance(m_handle, m_allocator_ptr);
	}
}

template<>
vk_utils::handle_wrapper<VkDevice, VkAllocationCallbacks>::~handle_wrapper(){
	if(m_handle != VK_NULL_HANDLE){
		vkDeviceWaitIdle(m_handle);
		vkDestroyDevice(m_handle, m_allocator_ptr);
	}
}

#if defined(_WIN32)
template<>
vk_utils::handle_wrapper<HWND, void>::handle_wrapper()
	: m_handle(NULL), m_allocator_ptr(nullptr){

}

template<>
vk_utils::handle_wrapper<HWND, void>::~handle_wrapper(){
	if(m_handle != NULL){
		DestroyWindow(m_handle);
	}
}
#endif
