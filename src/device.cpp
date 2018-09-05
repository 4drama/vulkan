#include "device.hpp"

#include "platform.hpp"
#include "vulkan_exception.hpp"

#include <vector>
#include <string>
#include <sstream>

#include <thread>
#include <mutex>

#include <cassert>

using device = vk_utils::device;
using queue_family = vk_utils::queue_family;
using device_creator = vk_utils::device_creator;

struct once_queue{
	VkQueue queue;
	VkFence fence;
};

class queue_family::queues
	: public std::enable_shared_from_this<queue_family::queues>{
public:
	queues() = delete;
	queues(
		VkDevice device,
		const VkAllocationCallbacks* allocator_ptr,
		uint32_t family_index,
		uint32_t queue_count);
	~queues();

	void queue_submit(std::vector<VkSubmitInfo> &submit_info);

private:
	uint32_t queue_create();

	std::vector<once_queue> m_queues;
	std::mutex m_queues_mtx;

	VkDevice m_device;
	const VkAllocationCallbacks* m_allocator_ptr;
	uint32_t m_family_index;
	uint32_t m_queue_count;
};

//=================queue_family::queues implementation================
queue_family::queues::queues(
	VkDevice device,
	const VkAllocationCallbacks* allocator_ptr,
	uint32_t family_index,
	uint32_t queue_count)
	: m_device(device), m_allocator_ptr(allocator_ptr),
	m_family_index(family_index), m_queue_count(queue_count){
}

queue_family::queues::~queues(){
	vkDeviceWaitIdle(m_device);
	for(auto& queue : m_queues){
		vkDestroyFence(m_device, queue.fence, m_allocator_ptr);
	}
}

uint32_t queue_family::queues::queue_create(){

	VkQueue new_queue = VK_NULL_HANDLE;
	VkFence queue_fence = VK_NULL_HANDLE;

	VkFenceCreateInfo info{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		nullptr,
		VK_FENCE_CREATE_SIGNALED_BIT
	};

	VkResult res = VK_SUCCESS;

	std::lock_guard<std::mutex> lock(m_queues_mtx);

	vkGetDeviceQueue(m_device, m_family_index, m_queues.size(), &new_queue);
	res = vkCreateFence(m_device, &info, m_allocator_ptr, &queue_fence);

	if(res == VK_SUCCESS){
		std::string msg = "vkCreateFence failed.";
		throw vk_utils::vulkan_error(msg, res);
	}

	m_queues.push_back(once_queue{new_queue, queue_fence});
	return  m_queues.size() - 1;
}

namespace{

const once_queue *get_ready_queue(const std::vector<once_queue> &m_queues, VkDevice device){
	VkResult fence_status = VK_NOT_READY;

	for(auto& queue : m_queues){
		fence_status = vkGetFenceStatus(device, queue.fence);
		if(fence_status == VK_SUCCESS){
			return &queue;
		}
	}
	return nullptr;
}

};

void queue_family::queues::queue_submit(std::vector<VkSubmitInfo> &submit_info){
	VkDevice device = m_device;

	auto submit_fun = [device]
	(std::vector<VkSubmitInfo> &submit_info, const once_queue *queue_ptr){
		VkResult res = VK_SUCCESS;
		res = vkResetFences(device, 1, &queue_ptr->fence);
		if(res != VK_SUCCESS){
			std::string msg = "vkResetFences failed.";
			throw vk_utils::vulkan_error(msg, res);
		}

		res = vkQueueSubmit(queue_ptr->queue, submit_info.size(),
			submit_info.data(), queue_ptr->fence);
		if(res != VK_SUCCESS){
			std::string msg = "vkQueueSubmit failed.";
			throw vk_utils::vulkan_error(msg, res);
		}
	};

	std::lock_guard<std::mutex> lock(m_queues_mtx);
	const once_queue *queue_ptr = get_ready_queue(m_queues, device);

	if((queue_ptr == nullptr)
		&& (m_queue_count > m_queues.size())){

		uint32_t index = queue_create();
		queue_ptr = &m_queues[index];

	} else {
		std::vector<once_queue> queues_cpy = m_queues;
		std::mutex *mtx_ptr = &m_queues_mtx;

		std::thread thr([queues_cpy, mtx_ptr, submit_fun, device]
			(std::vector<VkSubmitInfo> submit_info){
			try{
				VkResult res = VK_SUCCESS;
				std::vector<VkFence> fences{};
				for(auto& curr : queues_cpy){
					fences.push_back(curr.fence);
				}
				while(true){
					res = vkWaitForFences(
						device, fences.size(), fences.data(), VK_FALSE, UINT64_MAX);
					if(res != VK_SUCCESS){
							std::string msg = "vkWaitForFences failed.";
							throw vk_utils::vulkan_error(msg, res);
					}
					mtx_ptr->lock();
					const once_queue *queue_ptr = get_ready_queue(queues_cpy, device);
					if(queue_ptr != nullptr){
						submit_fun(submit_info, queue_ptr);
						mtx_ptr->unlock();
						return;
					}
					mtx_ptr->unlock();
				}
			} catch(...){
				//TO DO
			}
		}, submit_info);
     	thr.detach();
		return;
	}

	submit_fun(submit_info, queue_ptr);

	return;
}
//-------------------------------------------------------------------

//=====================queue_family implementation===================
queue_family::queue_family(
	VkPhysicalDevice physical_device,
	VkDevice device,
	const VkAllocationCallbacks* allocator_ptr,
	uint32_t family_index)

	: m_family_index(family_index){

	uint32_t family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &family_count, NULL);

	std::vector<VkQueueFamilyProperties> properties(family_count);
	properties.resize(family_count);

	vkGetPhysicalDeviceQueueFamilyProperties(
		physical_device, &family_count, properties.data());

	m_family_properties = properties[m_family_index];

	m_queues_ptr = std::make_shared<queue_family::queues>(
		device, allocator_ptr, family_index, m_family_properties.queueCount);
}

[[nodiscard]] bool queue_family::flags_check(VkQueueFlags flags) const noexcept{
	return (m_family_properties.queueFlags & flags) == flags;
}
//-------------------------------------------------------------------

//========================device implementation======================
device::device()
	: m_physical_device(VK_NULL_HANDLE),
	m_memory(VK_NULL_HANDLE, VK_NULL_HANDLE, nullptr){

}

device::device(VkPhysicalDevice physical_device, VkDevice logical_device,
	VkAllocationCallbacks* allocator_ptr)
	: m_physical_device(physical_device),
	m_logical_device(device_wrapper(logical_device, allocator_ptr)),
	m_memory(physical_device, logical_device, allocator_ptr){

	uint32_t family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(m_physical_device, &family_count, NULL);

	m_queue_families.clear();
	for(uint32_t index = 0; index < family_count; index++){
		m_queue_families.push_back(
			queue_family(m_physical_device, logical_device, allocator_ptr, index));
	}
}

bool device::presentation_support_check(const window_wrapper& window,
	uint32_t index) const noexcept{

	if(index < m_queue_families.size()){
		bool result = vk_utils::get_physical_device_presentation_support(
			window, m_physical_device, index);
		return result;
	} else
		return false;
}

bool device::presentation_support_check_any(const window_wrapper& window
	) const noexcept{

	bool find = false;
	for(uint32_t i = 0 ; i < m_queue_families.size(); i++){
		find = this->presentation_support_check(window, i);
		if(find == true)
			break;
	}
	return find;
}

std::vector<uint32_t> device::get_presentation_support(
	const window_wrapper& window) const{

	std::vector<uint32_t> result{};
	for(uint32_t i = 0 ; i < m_queue_families.size(); i++){
		if(this->presentation_support_check(window, i))
			result.push_back(i);
	}

	if(result.size() == 0){
		std::string msg = "Presentation not support.";
		throw vk_utils::vulkan_error(msg);
	}

	return result;
}


device::~device(){

}
//-------------------------------------------------------------------

namespace {
[[nodiscard]] std::vector<VkPhysicalDevice>
	f_get_physical_device(VkInstance instance);

[[nodiscard]] std::vector<VkQueueFamilyProperties>
	f_get_physical_device_queue_family_properties(VkPhysicalDevice physical_device);

[[nodiscard]] VkPhysicalDeviceProperties
	f_get_physical_device_properties(VkPhysicalDevice physical_device) noexcept;

[[nodiscard]] std::vector<VkDeviceQueueCreateInfo>
	f_create_queue_create_info(VkPhysicalDevice physical_device);

[[nodiscard]] VkPhysicalDevice
	f_choose_better_physical_device(const std::vector<VkPhysicalDevice> &devices);

[[nodiscard]] std::vector<VkLayerProperties>
	f_get_layer_properties(VkPhysicalDevice physical_device);

[[nodiscard]] std::vector<VkExtensionProperties>
	f_get_extension_properties(VkPhysicalDevice physical_device);
}

//====================device_creator implementation==================
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

device_creator& device_creator::add_extension(const std::string& extension){
	auto extension_properties = f_get_extension_properties(m_physical_device);

	for(auto &property : extension_properties){
		if(extension == property.extensionName){
			m_extensions.push_back(extension);
			return *this;
		}
	}

	std::string msg = "extension " + extension + " not supported";
	throw vk_utils::vulkan_error(msg);
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
	throw vk_utils::vulkan_error(msg);
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
		throw vk_utils::vulkan_error(msg);
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
		throw vk_utils::vulkan_error(msg, res);
	}

	return device(m_physical_device, logic_device, m_allocator_ptr);
}
//-------------------------------------------------------------------

namespace {

std::vector<VkPhysicalDevice> f_get_physical_device(VkInstance instance){
	VkResult res = VK_SUCCESS;
	uint32_t device_count = 0;

	res = vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
	if(res != VK_SUCCESS){
		std::string msg = "vkEnumeratePhysicalDevices get counter";
		throw vk_utils::vulkan_error(msg, res);
	}

	std::vector<VkPhysicalDevice> physical_devices(device_count);
	physical_devices.resize(device_count);

	res = vkEnumeratePhysicalDevices(instance, &device_count, physical_devices.data());
	if(res != VK_SUCCESS){
		std::string msg = "vkEnumeratePhysicalDevices get devices handles";
		throw vk_utils::vulkan_error(msg, res);
	}

	if(device_count == 0){
		std::string msg = "no physical device supporting VulkanAPI";
		throw vk_utils::vulkan_error(msg, res);
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

[[nodiscard]] std::vector<VkLayerProperties>
	f_get_layer_properties(VkPhysicalDevice physical_device){

	uint32_t property_count = 0;
	VkResult res = VK_SUCCESS;

	res = vkEnumerateDeviceLayerProperties(physical_device, &property_count, nullptr);

	std::vector<VkLayerProperties> layers_property(property_count);
	layers_property.resize(property_count);

	if(res != VK_SUCCESS){
		std::string msg = "vkEnumerateDeviceLayerProperties get counter";
		throw vk_utils::vulkan_error(msg, res);
	}

	res = vkEnumerateDeviceLayerProperties(
		physical_device, &property_count, layers_property.data());

	if(res != VK_SUCCESS){
		std::string msg = "vkEnumerateDeviceLayerProperties get propertis";
		throw vk_utils::vulkan_error(msg, res);
	}

	return layers_property;
}

[[nodiscard]] std::vector<VkExtensionProperties>
	f_get_extension_properties(VkPhysicalDevice physical_device){

	uint32_t property_count = 0;
	VkResult res = VK_SUCCESS;

	res = vkEnumerateDeviceExtensionProperties(
		physical_device, nullptr, &property_count, nullptr);

	std::vector<VkExtensionProperties> extension_property(property_count);
	extension_property.resize(property_count);

	if(res != VK_SUCCESS){
		std::string msg = "vkEnumerateDeviceExtensionProperties get counter";
		throw vk_utils::vulkan_error(msg, res);
	}

	res = vkEnumerateDeviceExtensionProperties(
		physical_device, nullptr, &property_count, extension_property.data());

	if(res != VK_SUCCESS){
		std::string msg = "vkEnumerateDeviceExtensionProperties get propertis";
		throw vk_utils::vulkan_error(msg, res);
	}

	return extension_property;
}

}
