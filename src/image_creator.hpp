#ifndef IMAGE_CREATOR_HPP_
#define IMAGE_CREATOR_HPP_

#include <vulkan/vulkan.h>
#include "handle_wrapper.hpp"

#include <initializer_list>
#include <vector>

namespace vk_utils{

class image_handler_creator{
	using image_handler_wrapper = vk_utils::image_handler_wrapper;
public:
	image_handler_creator(uint32_t width, uint32_t height) noexcept;
	~image_handler_creator() = default;

	image_handler_creator& set_format(VkFormat format) noexcept;
	image_handler_creator& set_depth(uint32_t depth) noexcept;
	image_handler_creator& set_mip_levels(uint32_t mip_levels) noexcept;

	image_handler_creator& set_concurrent(std::initializer_list<uint32_t> indices);

	[[nodiscard]] image_handler_wrapper create(VkDevice device) const;

	image_handler_creator() = delete;
	image_handler_creator(image_handler_creator& ) = delete;
	image_handler_creator& operator=(const image_handler_creator& ) = delete;

	image_handler_creator(image_handler_creator&&) = default;
private:
	VkImageCreateInfo m_image_info;
	std::vector<uint32_t> m_concurrent_indices;
	VkAllocationCallbacks* m_allocator_ptr;
};

}
#endif
