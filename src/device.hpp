#ifndef DEVICE_HPP_
#define DEVICE_HPP_

#include <vulkan\vulkan.h>

#include <vector>

namespace vk_utils{

class queue_family{
public:
	explicit queue_family(VkPhysicalDevice physical_device, uint32_t family_index);
	
private:
	uint32_t m_family_index;
	VkQueueFamilyProperties m_family_properties;
	
	std::vector<VkQueue> m_queues;
};

class device{
public:
	device();
	explicit device(VkInstance instance);
	
	device(device& ) = delete;
	device& operator=(const device& ) = delete;
	device& operator=(device&& ) = default;
	
	device(device&&) = default;
	
	~device();
	
private:
	VkPhysicalDevice m_physical_device;
	std::vector<queue_family> m_queue_families;
};


}

#endif