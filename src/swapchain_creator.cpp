#include "swapchain_creator.hpp"

#include "vulkan_exception.hpp"

#include <string>

using swapchain_creator = vk_utils::swapchain_creator;

namespace{

[[nodiscard]] bool f_check_image_count_bound(
	uint32_t image_count, VkSurfaceCapabilitiesKHR capabilities) noexcept;

[[nodiscard]] bool f_check_usage_flags(
	VkImageUsageFlags flags, VkSurfaceCapabilitiesKHR capabilities) noexcept;

[[nodiscard]] bool f_check_format_support(
	const std::vector<VkSurfaceFormatKHR>& sup_formats, VkFormat format) noexcept;

[[nodiscard]] uint32_t f_get_format_indeces(
	const std::vector<VkSurfaceFormatKHR>& sup_formats, VkFormat format);

};

swapchain_creator::swapchain_creator(
	const device& device,
	const surface_wrapper& surface,
	const window_wrapper& window)
	: m_allocator_ptr(nullptr),
	m_device(device.get_logical_device()),
	m_image_count(2),
	m_format(VK_FORMAT_R8G8B8A8_UNORM),
	m_surface(surface.get()),
	m_usage_flags(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT),
	m_window_size(get_window_size(window)){

	VkPhysicalDevice physical_device = device.get_physical_device();

	VkResult res = VK_SUCCESS;
	res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
		physical_device, m_surface, &m_surface_capabilities);
	if(res != VK_SUCCESS){
		std::string msg = "Can not get surface capabilities.";
		throw vk_utils::vulkan_error(msg);
	}

	uint32_t supported_formats_count = 0;
	res = vkGetPhysicalDeviceSurfaceFormatsKHR(
		physical_device, m_surface, &supported_formats_count, nullptr);
	if(res != VK_SUCCESS){
		std::string msg = "Can not get surface formats counter.";
		throw vk_utils::vulkan_error(msg);
	} else if(supported_formats_count == 0){
		std::string msg = "Surface not support no one formats.";
		throw vk_utils::vulkan_error(msg);
	}

	m_supported_formats.resize(supported_formats_count);
	res = vkGetPhysicalDeviceSurfaceFormatsKHR(
		physical_device, m_surface, &supported_formats_count, m_supported_formats.data());
	if(res != VK_SUCCESS){
		std::string msg = "Can not get surface formats.";
		throw vk_utils::vulkan_error(msg);
	}



	if(!f_check_image_count_bound(m_image_count, m_surface_capabilities)){
		std::string msg = "Swapchain image count out of bound.";
		throw vk_utils::vulkan_error(msg);
	}

	if(!f_check_format_support(m_supported_formats, m_format)){
		std::string msg = "Surface not support this format.";
		throw vk_utils::vulkan_error(msg);
	}
}

swapchain_creator& swapchain_creator::set_image_count(uint32_t count){
	if(!f_check_image_count_bound(count, m_surface_capabilities)){
		std::string msg = "Swapchain image count out of bound.";
		throw vk_utils::vulkan_error(msg);
	}
	m_image_count = count;
	return *this;
}

swapchain_creator& swapchain_creator::set_format(VkFormat format){
	if(!f_check_format_support(m_supported_formats, format)){
		std::string msg = "Surface not support this format.";
		throw vk_utils::vulkan_error(msg);
	}
	m_format = format;
	return *this;
}

swapchain_creator& swapchain_creator::set_usage_flags(VkImageUsageFlags flags){
	if(!f_check_usage_flags(flags, m_surface_capabilities)){
		std::string msg = "Surface not support this usage flags.";
		throw vk_utils::vulkan_error(msg);
	}

	m_usage_flags = flags;
	return *this;
}

swapchain_creator& swapchain_creator::add_usage_bits(VkImageUsageFlags bits){
	if(!f_check_usage_flags(m_usage_flags | bits, m_surface_capabilities)){
		std::string msg = "Surface not support this usage flags.";
		throw vk_utils::vulkan_error(msg);
	}

	m_usage_flags = m_usage_flags | bits;
	return *this;
}

swapchain_creator& swapchain_creator::set_allocator(
	VkAllocationCallbacks* allocator_ptr){

	m_allocator_ptr = allocator_ptr;
	return *this;
}

swapchain_creator& swapchain_creator::set_concurrent(
	std::initializer_list<uint32_t> indices){

	m_concurrent_indices = indices;
	return *this;
}

vk_utils::swapchain_wrapper swapchain_creator::create(){

	uint32_t format_indeces = f_get_format_indeces(m_supported_formats, m_format);

	VkSwapchainCreateInfoKHR swapchain_info{};

	swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchain_info.pNext = nullptr;
	swapchain_info.flags = 0;
	swapchain_info.surface = m_surface;

	swapchain_info.minImageCount = m_image_count;
    swapchain_info.imageFormat = m_supported_formats[format_indeces].format;
    swapchain_info.imageColorSpace = m_supported_formats[format_indeces].colorSpace;

    swapchain_info.imageExtent.width = m_window_size.width;
	swapchain_info.imageExtent.height = m_window_size.height;
    swapchain_info.imageArrayLayers = 1;

    swapchain_info.imageUsage = m_usage_flags;

	if(m_concurrent_indices.size() != 0){
		swapchain_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapchain_info.queueFamilyIndexCount = m_concurrent_indices.size();
		swapchain_info.pQueueFamilyIndices = m_concurrent_indices.data();
	} else {
	    swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	    swapchain_info.queueFamilyIndexCount = 0;
	    swapchain_info.pQueueFamilyIndices = nullptr;
	}

    swapchain_info.preTransform = VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR;
    swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    swapchain_info.clipped = VK_TRUE;
    swapchain_info.oldSwapchain = VK_NULL_HANDLE;


	VkResult res = VK_SUCCESS;
	VkSwapchainKHR swapchain = VK_NULL_HANDLE;

	res = vkCreateSwapchainKHR(m_device, &swapchain_info, m_allocator_ptr, &swapchain);
	if(res != VK_SUCCESS){
		std::string msg = "Create swapchain error.";
		throw vk_utils::vulkan_error(msg);
	}

	return vk_utils::swapchain_wrapper(swapchain, m_device, m_allocator_ptr);
}

namespace{

bool f_check_image_count_bound(
	uint32_t image_count, VkSurfaceCapabilitiesKHR capabilities) noexcept{

	uint32_t min_image_count = capabilities.minImageCount;
	uint32_t max_image_count = capabilities.maxImageCount;

	return (min_image_count < image_count) && (image_count < max_image_count);
}

bool f_check_format_support(
	const std::vector<VkSurfaceFormatKHR>& sup_formats, VkFormat format) noexcept{

	for(auto& curr_sup_format : sup_formats){
		if(curr_sup_format.format == format)
			return true;
	}

	return false;
}

uint32_t f_get_format_indeces(
	const std::vector<VkSurfaceFormatKHR>& sup_formats, VkFormat format){

	uint32_t format_indeces = ~0;
	for(uint32_t i = 0; i < sup_formats.size(); ++i){
		if(sup_formats[i].format == format){
			format_indeces = i;
			break;
		}
	}

	if(format_indeces == ~0u){
		std::string msg = "Surface not support this format.";
		throw vk_utils::vulkan_error(msg);
	}

	return format_indeces;
}

[[nodiscard]] bool f_check_usage_flags(
	VkImageUsageFlags flags, VkSurfaceCapabilitiesKHR capabilities) noexcept{

	return (capabilities.supportedUsageFlags & flags) == flags;
}

};
