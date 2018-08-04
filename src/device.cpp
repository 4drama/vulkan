#include "device.hpp"

#include <vector>
#include <stdexcept>
#include <string>

#include <cassert>

using device = vk_utils::device;
using queue_family = vk_utils::queue_family;

namespace {
	
[[nodiscard]] std::vector<VkPhysicalDevice> f_get_physical_device(VkInstance instance);
[[nodiscard]] std::vector<VkQueueFamilyProperties> 
	f_get_physical_device_queue_family_properties(VkPhysicalDevice physical_device);
[[nodiscard]] VkPhysicalDeviceProperties 
	f_get_physical_device_properties(VkPhysicalDevice physical_device) noexcept;
	
[[nodiscard]] VkPhysicalDevice 
	f_choose_better_physical_device(const std::vector<VkPhysicalDevice> &devices);
}

queue_family::queue_family(VkPhysicalDevice physical_device, uint32_t family_index)
	:m_family_index(family_index){
	
	uint32_t family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &family_count, NULL);
	
	std::vector<VkQueueFamilyProperties> properties(family_count);
	properties.resize(family_count);
	
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &family_count, properties.data());
	
	m_family_properties = properties[m_family_index];
}

device::device() : m_physical_device(VK_NULL_HANDLE){
	
}

device::device(VkInstance instance){
	auto devices = f_get_physical_device(instance);
	m_physical_device = f_choose_better_physical_device(devices);
	
	uint32_t family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(m_physical_device, &family_count, NULL);
	
	m_queue_families.clear();
	for(uint32_t index = 0; index < family_count; index++){
		m_queue_families.push_back(queue_family(m_physical_device, index));
	}
}

device::~device(){
	
}

namespace {
	
std::vector<VkPhysicalDevice> f_get_physical_device(VkInstance instance){
	VkResult res = VK_SUCCESS;
	uint32_t device_count = 0;
	
	res = vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
	if(res != VK_SUCCESS){
		std::string msg = "vkEnumeratePhysicalDevices get counter";
		throw std::runtime_error(msg);
	}
	
	std::vector<VkPhysicalDevice> physical_devices(device_count);
	physical_devices.resize(device_count);
	
	res = vkEnumeratePhysicalDevices(instance, &device_count, physical_devices.data());
	if(res != VK_SUCCESS){
		std::string msg = "vkEnumeratePhysicalDevices get devices handles";
		throw std::runtime_error(msg);
	}
	
	if(device_count == 0){
		std::string msg = "no physical device supporting VulkanAPI";
		throw std::runtime_error(msg);
	}
	
	return physical_devices;
}

[[nodiscard]] std::vector<VkQueueFamilyProperties> 
	f_get_physical_device_queue_family_properties(VkPhysicalDevice physical_device){
	
	uint32_t property_count = 0;
	
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &property_count, nullptr);	
	assert(property_count != 0);
	
	std::vector<VkQueueFamilyProperties> properties(property_count);
	properties.resize(property_count);
	
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &property_count, properties.data());
	
	return properties;
}

VkPhysicalDeviceProperties 
	f_get_physical_device_properties(VkPhysicalDevice physical_device) noexcept{
	
	VkPhysicalDeviceProperties properties;	
	vkGetPhysicalDeviceProperties(physical_device, &properties);
	
	return properties;
}
	
[[nodiscard]] VkPhysicalDevice 
	f_choose_better_physical_device(const std::vector<VkPhysicalDevice> &devices){
	
	VkPhysicalDevice better_device = VK_NULL_HANDLE;
	
	
	for(auto curr_device : devices){
		if(better_device == VK_NULL_HANDLE){
			better_device = curr_device;
		}
		
		//TO DO
	}
	
	return better_device;
}
	
}