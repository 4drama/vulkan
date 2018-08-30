#include "buffer_creator.hpp"

#include "vulkan_exception.hpp"
#include <string>

using buffer = vk_utils::buffer;
using buffer_creator = typename vk_utils::buffer_creator;

buffer::buffer(device_memory &curr_device_memory, VkBuffer buffer,
	VkDeviceMemory memory, VkMemoryPropertyFlags flags) noexcept
	: m_device_memory(curr_device_memory), m_buffer(buffer),
	m_memory(memory), m_flags(flags){
}

buffer_creator::buffer_creator(VkDeviceSize size) noexcept{
	m_buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	m_buffer_info.pNext = nullptr;
	m_buffer_info.flags = 0;
	m_buffer_info.size = size;
	m_buffer_info.usage = 0;
	m_buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	m_buffer_info.queueFamilyIndexCount = 0;
	m_buffer_info.pQueueFamilyIndices = nullptr;
}

buffer_creator& buffer_creator::add_flag(VkBufferCreateFlagBits flag) noexcept{
	m_buffer_info.flags |= flag;
	return *this;
}

buffer_creator& buffer_creator::add_usage(VkBufferUsageFlagBits usage) noexcept{
	m_buffer_info.usage |= usage;
	return *this;
}

buffer_creator& buffer_creator::set_concurrent(
	std::initializer_list<uint32_t> indices){

	m_concurrent_indices = indices;

	m_buffer_info.sharingMode = VK_SHARING_MODE_CONCURRENT;
	m_buffer_info.queueFamilyIndexCount = m_concurrent_indices.size();
	m_buffer_info.pQueueFamilyIndices = m_concurrent_indices.data();

	return *this;
}

buffer_creator& buffer_creator::add_property_flag_bits(
	VkMemoryPropertyFlagBits bits) noexcept{

	m_flags |= bits;
	return *this;
}

buffer buffer_creator::create(device_memory &curr_device_memory){
	if(m_buffer_info.usage == 0)
		m_buffer_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

	VkResult error = VK_NOT_READY;
	VkBuffer buffer = VK_NULL_HANDLE;
	VkDevice device = curr_device_memory.get_device();

	const VkAllocationCallbacks* allocator_ptr = curr_device_memory.get_allocator();
	error = vkCreateBuffer(device, &m_buffer_info, allocator_ptr, &buffer);
	if(error != VK_SUCCESS){
		std::string msg = "buffer handler create failed";
		throw vk_utils::vulkan_error(msg, error);
	}

	VkMemoryRequirements mem_req{};
	vkGetBufferMemoryRequirements(device, buffer, &mem_req);

	VkDeviceMemory memory = curr_device_memory.allocate_memory(mem_req, m_flags);

	error = vkBindBufferMemory(device, buffer, memory, 0);
	if(error != VK_SUCCESS){
		std::string msg = "image bind failed";
		throw vk_utils::vulkan_error(msg, error);
	}

	return typename vk_utils::buffer(curr_device_memory, buffer, memory, m_flags);
}
