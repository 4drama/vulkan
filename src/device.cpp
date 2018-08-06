#include "device.hpp"

#include <vector>
#include <stdexcept>
#include <string>

#include <cassert>

using device = vk_utils::device;
using queue_family = vk_utils::queue_family;
using device_creator = vk_utils::device_creator;

queue_family::queue_family(VkPhysicalDevice physical_device, uint32_t family_index)
	:m_family_index(family_index){

	uint32_t family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &family_count, NULL);

	std::vector<VkQueueFamilyProperties> properties(family_count);
	properties.resize(family_count);

	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &family_count, properties.data());

	m_family_properties = properties[m_family_index];
}

[[nodiscard]] bool queue_family::flags_check(VkQueueFlags flags){
	// TO DO
}

uint32_t queue_family::queue_create(){
	VkQueue new_queue;
	// TO DO
}

device::device() : m_physical_device(VK_NULL_HANDLE){

}

device::device(VkPhysicalDevice physical_device, VkDevice logical_device,
	VkAllocationCallbacks* allocator_ptr)
	: m_physical_device(physical_device),
	m_logical_device(device_wrapper(logical_device, allocator_ptr)){

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

[[nodiscard]] std::vector<VkPhysicalDevice> f_get_physical_device(VkInstance instance);
[[nodiscard]] std::vector<VkQueueFamilyProperties>
	f_get_physical_device_queue_family_properties(VkPhysicalDevice physical_device);
[[nodiscard]] VkPhysicalDeviceProperties
	f_get_physical_device_properties(VkPhysicalDevice physical_device) noexcept;
[[nodiscard]] std::vector<VkDeviceQueueCreateInfo>
	f_create_queue_create_info(VkPhysicalDevice physical_device);

[[nodiscard]] VkPhysicalDevice
	f_choose_better_physical_device(const std::vector<VkPhysicalDevice> &devices);
}

device_creator::device_creator(VkInstance instance)
	: m_allocator_ptr(nullptr), m_physical_device(VK_NULL_HANDLE){

	m_required_features = {};

	assert(instance != VK_NULL_HANDLE);

	auto devices = f_get_physical_device(instance);
	m_physical_device = f_choose_better_physical_device(devices);

	assert(m_physical_device != VK_NULL_HANDLE);

	m_device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	m_device_info.pNext = NULL;
	m_device_info.flags = 0;
}

namespace {
	[[nodiscard]] std::vector<VkLayerProperties> f_get_layer_properties
		(VkPhysicalDevice physical_device);

	[[nodiscard]] std::vector<VkExtensionProperties> f_get_extension_properties
		(VkPhysicalDevice physical_device);
}

device_creator& device_creator::add_extension(const std::string& extension){
	auto extension_properties = f_get_extension_properties(m_physical_device);

	for(auto &property : extension_properties){
		if(extension == property.extensionName){
			m_extensions.push_back(extension);
			return *this;
		}
	}

	std::string msg = "extension " + extension + " not supported";
	throw std::runtime_error(msg);
}

device_creator& device_creator::add_layer(const std::string& layer){
	auto layers_property = f_get_layer_properties(m_physical_device);

	for(auto &property : layers_property){
		if(layer == property.layerName){
			m_layers.push_back(layer);
			return *this;
		}
	}

	std::string msg = "layer " + layer + " not supported";
	throw std::runtime_error(msg);
}

namespace {
	[[nodiscard]] std::vector<VkLayerProperties> f_get_layer_properties
		(VkPhysicalDevice physical_device){

		uint32_t property_count = 0;
		VkResult res = VK_SUCCESS;

		res = vkEnumerateDeviceLayerProperties(physical_device, &property_count, nullptr);

		std::vector<VkLayerProperties> layers_property(property_count);
		layers_property.resize(property_count);

		if(res != VK_SUCCESS){
			std::string msg = "vkEnumerateDeviceLayerProperties get counter";
			throw std::runtime_error(msg);
		}

		res = vkEnumerateDeviceLayerProperties(
			physical_device, &property_count, layers_property.data());

		if(res != VK_SUCCESS){
			std::string msg = "vkEnumerateDeviceLayerProperties get propertis";
			throw std::runtime_error(msg);
		}

		return layers_property;
	}

	[[nodiscard]] std::vector<VkExtensionProperties> f_get_extension_properties
		(VkPhysicalDevice physical_device){

		uint32_t property_count = 0;
		VkResult res = VK_SUCCESS;

		res = vkEnumerateDeviceExtensionProperties(
			physical_device, nullptr, &property_count, nullptr);

		std::vector<VkExtensionProperties> extension_property(property_count);
		extension_property.resize(property_count);

		if(res != VK_SUCCESS){
			std::string msg = "vkEnumerateDeviceExtensionProperties get counter";
			throw std::runtime_error(msg);
		}

		res = vkEnumerateDeviceExtensionProperties(
			physical_device, nullptr, &property_count, extension_property.data());

		if(res != VK_SUCCESS){
			std::string msg = "vkEnumerateDeviceExtensionProperties get propertis";
			throw std::runtime_error(msg);
		}

		return extension_property;
	}

}

device_creator& device_creator::add_feature(const std::string& feature){
	VkPhysicalDeviceFeatures supported_features;
	vkGetPhysicalDeviceFeatures(m_physical_device, &supported_features);

	if(feature == "multiDrawIndirect"){
		m_required_features.multiDrawIndirect = supported_features.multiDrawIndirect;
	} else if(feature == "tessellationShader") {
		m_required_features.tessellationShader = VK_TRUE;
	} else if(feature == "geometryShader"){
		m_required_features.geometryShader = VK_TRUE;
	} else {
		std::string msg = "Feature " + feature + "not found.";
		throw std::runtime_error(msg);
	}

	return *this;
}

device device_creator::create(){
	auto queue_create_info = f_create_queue_create_info(m_physical_device);

	m_device_info.queueCreateInfoCount = queue_create_info.size();
	m_device_info.pQueueCreateInfos = queue_create_info.data();

	std::vector<const char*> layers{};
	for(auto &layer : m_layers)
		layers.push_back(layer.c_str());
	m_device_info.enabledLayerCount = layers.size();
	m_device_info.ppEnabledLayerNames = layers.data();

	std::vector<const char*> extensions{};
	for(auto &extension : m_extensions)
		extensions.push_back(extension.c_str());
	m_device_info.enabledExtensionCount = extensions.size();
	m_device_info.ppEnabledExtensionNames = extensions.data();

	m_device_info.pEnabledFeatures = &m_required_features;

	VkResult res = VK_SUCCESS;
	VkDevice logic_device = VK_NULL_HANDLE;

	res = vkCreateDevice(m_physical_device, &m_device_info, m_allocator_ptr, &logic_device);
	if(res != VK_SUCCESS){
		std::string msg = "vkCreateDevice failed";
		throw std::runtime_error(msg);
	}

	return device(m_physical_device, logic_device, m_allocator_ptr);
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


[[nodiscard]] std::vector<VkDeviceQueueCreateInfo>
	f_create_queue_create_info(VkPhysicalDevice physical_device){

	uint32_t family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &family_count, NULL);

	std::vector<VkQueueFamilyProperties> properties(family_count);
	properties.resize(family_count);

	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &family_count, properties.data());

	std::vector<VkDeviceQueueCreateInfo> result{};

	for(uint32_t i = 0; i < family_count; i++){
		VkDeviceQueueCreateInfo info;

		info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		info.pNext = NULL;
		info.flags = 0;
		info.queueFamilyIndex = i;
		info.queueCount = properties[i].queueCount;
		info.pQueuePriorities = NULL;

		result.push_back(info);
	}

	assert(result.size() == family_count);
	assert(result.size() != 0);
	return result;
}

}
