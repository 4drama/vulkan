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
