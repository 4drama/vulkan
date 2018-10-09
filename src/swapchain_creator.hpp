#ifndef SWAPCHAIN_CREATOR_HPP_
#define SWAPCHAIN_CREATOR_HPP_

#include "platform.hpp"

#include "device.hpp"
#include "handle_wrapper.hpp"

#include <vector>

namespace vk_utils{

class swapchain_creator{
public:
	swapchain_creator() = delete;
	swapchain_creator(
		const device& device,
		const surface_wrapper& surface,
		const window_wrapper& window);

	swapchain_creator& set_image_count(uint32_t count);
	swapchain_creator& set_format(VkFormat format);
	swapchain_creator& set_usage_flags(VkImageUsageFlags flags);
	swapchain_creator& add_usage_bits(VkImageUsageFlags bits);
	swapchain_creator& set_allocator(VkAllocationCallbacks* allocator_ptr);
	swapchain_creator& set_concurrent(std::initializer_list<uint32_t> indices);

	[[nodiscard]] swapchain_wrapper create();

	swapchain_creator(swapchain_creator& ) = delete;
	swapchain_creator& operator=(const swapchain_creator& ) = delete;

	swapchain_creator(swapchain_creator&&) = default;
	swapchain_creator& operator=(swapchain_creator&& ) = default;

	~swapchain_creator() = default;

private:
	VkAllocationCallbacks* m_allocator_ptr;

	VkDevice m_device;
	uint32_t m_image_count;
	VkFormat m_format;
	VkSurfaceKHR m_surface;
	VkImageUsageFlags m_usage_flags;
	std::vector<uint32_t> m_concurrent_indices;

	window_size m_window_size;

	VkSurfaceCapabilitiesKHR m_surface_capabilities;
	std::vector<VkSurfaceFormatKHR> m_supported_formats;
};

}

#endif
