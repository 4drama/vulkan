#ifndef DEVICE_HPP_
#define DEVICE_HPP_

#include <vulkan\vulkan.h>

#include <vector>
#include <string>

namespace vk_utils{

class queue_family{
public:
	explicit queue_family(VkPhysicalDevice physical_device, uint32_t family_index);
	
	[[nodiscard]] bool flags_check(VkQueueFlags flags);
	
private:
	uint32_t queue_create();

	uint32_t m_family_index;
	VkQueueFamilyProperties m_family_properties;
	
	std::vector<VkQueue> m_queues;
};

class device{
public:
	device();
	explicit device(VkPhysicalDevice physical_device, VkDevice logical_device);
	
	device(device& ) = delete;
	device& operator=(const device& ) = delete;
	device& operator=(device&& ) = default;
	
	device(device&&) = default;
	
	~device();
	
private:
	VkPhysicalDevice m_physical_device;
	VkDevice m_logical_device;
	std::vector<queue_family> m_queue_families;
};

class device_creator{
public:
	device_creator(VkInstance instance);
	~device_creator() = default;
	
	device_creator& add_extension(const std::string& extension);
	device_creator& add_layer(const std::string& layer);
	
	device_creator& add_feature(const std::string& feature);
	
	[[nodiscard]] device create();
	
	device_creator(device_creator& ) = delete;
	device_creator& operator=(const device_creator& ) = delete;
	
	device_creator(device_creator&&) = default;
private:	
	std::vector<std::string> m_extensions;
	std::vector<std::string> m_layers;
	
	VkAllocationCallbacks* m_allocator_ptr;
	
	VkDeviceCreateInfo m_device_info;
	VkPhysicalDeviceFeatures m_required_features;
	
	VkPhysicalDevice m_physical_device;
};

}

#endif