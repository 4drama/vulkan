#include "image_creator.hpp"

#include "vulkan_exception.hpp"

#include <string>

using image_handler_creator = vk_utils::image_handler_creator;
using image_handler_wrapper = vk_utils::image_handler_wrapper;

image_handler_creator::image_handler_creator(uint32_t width, uint32_t height) noexcept
	: m_allocator_ptr(nullptr){

	m_image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	m_image_info.pNext = NULL;
	m_image_info.flags = 0;
	m_image_info.imageType = VK_IMAGE_TYPE_2D;
	m_image_info.format = VK_FORMAT_R8G8B8A8_UNORM;
	m_image_info.extent.width = width;
	m_image_info.extent.height = height;
	m_image_info.extent.depth = 1;
	m_image_info.mipLevels = 1;
	m_image_info.arrayLayers = 1;
	m_image_info.samples = VK_SAMPLE_COUNT_1_BIT;
	m_image_info.tiling = VK_IMAGE_TILING_LINEAR;
	m_image_info.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	m_image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	m_image_info.queueFamilyIndexCount = 0;
	m_image_info.pQueueFamilyIndices = NULL;
	m_image_info.initialLayout =  VK_IMAGE_LAYOUT_UNDEFINED;
}

image_handler_creator& image_handler_creator::set_format(VkFormat format) noexcept{
	m_image_info.format = format;
	return *this;
}

image_handler_creator& image_handler_creator::set_depth(uint32_t depth) noexcept{
	m_image_info.extent.depth = depth;
	return *this;
}

image_handler_creator& image_handler_creator::set_mip_levels(
	uint32_t mip_levels) noexcept{

	m_image_info.mipLevels = mip_levels;
	return *this;
}

image_handler_creator& image_handler_creator::set_concurrent(
	std::initializer_list<uint32_t> indices){

	m_concurrent_indices = indices;

	m_image_info.sharingMode = VK_SHARING_MODE_CONCURRENT;
	m_image_info.queueFamilyIndexCount = m_concurrent_indices.size();
	m_image_info.pQueueFamilyIndices = m_concurrent_indices.data();

	return *this;
}

image_handler_wrapper image_handler_creator::create(VkDevice device) const{
	VkResult err = VK_NOT_READY;
	VkImage img = VK_NULL_HANDLE;

	err = vkCreateImage(device, &m_image_info, m_allocator_ptr, &img);
	if(err != VK_SUCCESS){
		std::string msg = "image handler create failed";
		throw vk_utils::vulkan_error(msg, err);
	}

	return image_handler_wrapper(img, device, m_allocator_ptr);
}
