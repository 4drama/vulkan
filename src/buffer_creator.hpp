#ifndef BUFFER_CREATOR_HPP_
#define BUFFER_CREATOR_HPP_

#include <vulkan/vulkan.h>

#include "device_memory.hpp"

#include <initializer_list>
#include <vector>

namespace vk_utils{

class buffer{
public:
	buffer() = delete;
	buffer(device_memory &curr_device_memory, VkBuffer buffer,
		VkDeviceMemory memory, VkMemoryPropertyFlags flags) noexcept;

	~buffer() noexcept;
private:
	device_memory &m_device_memory;

	VkBuffer m_buffer;
	VkDeviceMemory m_memory;

	VkMemoryPropertyFlags m_flags;
};

class buffer_creator{
public:

	explicit buffer_creator(VkDeviceSize size) noexcept;

	~buffer_creator() = default;

	buffer_creator& add_flag(VkBufferCreateFlagBits flag) noexcept;
	buffer_creator& add_usage(VkBufferUsageFlagBits usage) noexcept;
	buffer_creator& set_concurrent(std::initializer_list<uint32_t> indices);
	buffer_creator& add_property_flag_bits(VkMemoryPropertyFlagBits bits) noexcept;

	[[nodiscard]] buffer create(device_memory &curr_device_memory);

	buffer_creator() = delete;
	buffer_creator(buffer_creator& ) = delete;
	buffer_creator& operator=(const buffer_creator& ) = delete;

	buffer_creator(buffer_creator&&) = default;
private:
	VkBufferCreateInfo m_buffer_info;
	std::vector<uint32_t> m_concurrent_indices;

	VkMemoryPropertyFlags m_flags;
};

template<class T>
buffer_creator buffer_creator(){
		return typename vk_utils::buffer_creator(sizeof(T));
};

}

#endif
