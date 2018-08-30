#ifndef IMAGE_CREATOR_HPP_
#define IMAGE_CREATOR_HPP_

#include <vulkan/vulkan.h>

#include "device_memory.hpp"

#include <initializer_list>
#include <vector>

namespace vk_utils{

class image{
public:

	image() = delete;
	image(device_memory &curr_device_memory, VkImage image,
		VkDeviceMemory memory, VkMemoryPropertyFlags flags) noexcept;

	~image() noexcept;

private:
	device_memory &m_device_memory;

	VkImage m_image;
	VkDeviceMemory m_memory;

	VkMemoryPropertyFlags m_flags;
};

class image_creator{
public:
	image_creator(uint32_t width, uint32_t height) noexcept;
	~image_creator() = default;

	image_creator& set_format(VkFormat format) noexcept;
	image_creator& set_depth(uint32_t depth) noexcept;
	image_creator& set_mip_levels(uint32_t mip_levels) noexcept;

	image_creator& set_concurrent(std::initializer_list<uint32_t> indices);

	image_creator& add_property_flag_bits(VkMemoryPropertyFlagBits bits) noexcept;

	[[nodiscard]] image create(device_memory &curr_device_memory) const;

	image_creator() = delete;
	image_creator(image_creator& ) = delete;
	image_creator& operator=(const image_creator& ) = delete;

	image_creator(image_creator&&) = default;
private:
	VkImageCreateInfo m_image_info;
	std::vector<uint32_t> m_concurrent_indices;

	VkMemoryPropertyFlags m_flags;
};

}
#endif
