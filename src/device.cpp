#include "device.hpp"

#include "platform.hpp"
#include "vulkan_exception.hpp"

#include <vector>
#include <string>

#include <cassert>

using device = vk_utils::device;
using queue_family = vk_utils::queue_family;
using device_creator = vk_utils::device_creator;
using device_memory = vk_utils::device_memory;

//=====================queue_family implementation===================
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
//-------------------------------------------------------------------

//========================device implementation======================
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

//================device_memory::memory_type declaration=============
class device_memory::memory_type{
public:
	memory_type() = delete;
	explicit memory_type(VkMemoryPropertyFlags flags, uint32_t heap_index) noexcept;

	bool check_type_flags(VkMemoryPropertyFlags flags) const noexcept;
	uint32_t get_index() const noexcept;
	double get_priority() const noexcept;

private:
	VkMemoryPropertyFlags m_flags;
	uint32_t m_heap_index;
	double m_priority;
};
//-------------------------------------------------------------------

//================device_memory::memory_heap declaration=============
class device_memory::memory_heap{
public:
	memory_heap() = delete;
	explicit memory_heap(VkDeviceSize size, VkMemoryHeapFlags flags) noexcept;

	[[nodiscard]] bool check_heap_flags(VkMemoryHeapFlags flags) const noexcept;

	[[nodiscard]] VkDeviceSize get_max_mem() const noexcept;
	[[nodiscard]] VkDeviceSize get_busy_mem() const noexcept;
	[[nodiscard]] VkDeviceSize get_remaining_mem() const noexcept;

	void calc_allocate(VkDeviceSize size) noexcept;
	void calc_free(VkDeviceSize size) noexcept;
private:
	VkDeviceSize m_max_size;
	VkDeviceSize m_curr_size;
	VkMemoryHeapFlags m_flags;
};
//-------------------------------------------------------------------

//=====================device_memory implementation==================
device_memory::device_memory(VkPhysicalDevice physical_device, VkDevice device,
	VkAllocationCallbacks* allocator_ptr)
	: m_device(device), m_allocator_ptr(allocator_ptr){

	VkPhysicalDeviceMemoryProperties device_memory_property = {};
	vkGetPhysicalDeviceMemoryProperties(physical_device, &device_memory_property);

	m_types.reserve(device_memory_property.memoryTypeCount);
	m_heaps.reserve(device_memory_property.memoryHeapCount);

	for(uint32_t i = 0; i < device_memory_property.memoryTypeCount; i++){
		const VkMemoryType *type = &device_memory_property.memoryTypes[i];
		m_types.push_back(memory_type(type->propertyFlags, type->heapIndex));
	}

	for(uint32_t i = 0; i < device_memory_property.memoryHeapCount; i++){
		const VkMemoryHeap *heap = &device_memory_property.memoryHeaps[i];
		m_heaps.push_back(memory_heap(heap->size, heap->flags));
	}
}

VkDeviceMemory device_memory::allocate_memory(VkMemoryRequirements req,
	VkMemoryPropertyFlags flags){

	struct{
		uint32_t index;
		double priority;
	} heap = {0, 0};

	for (uint32_t memoryType = 0; memoryType < 32; ++memoryType){
		if (req.memoryTypeBits & (1 << memoryType)){

			if(m_types[memoryType].check_type_flags(flags)){
				double curr_priority = m_types[memoryType].get_priority();
				if((curr_priority > heap.priority) &&
					(m_heaps[memoryType].get_remaining_mem() >= req.size)){

					heap.index = memoryType;
					heap.priority = curr_priority;
				}
			}
		}
	}

	VkMemoryAllocateInfo allocate_info = {
		VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		NULL,
		req.size,
		heap.index
	};

	VkResult res = VK_SUCCESS;
	VkDeviceMemory memory = VK_NULL_HANDLE;

	res = vkAllocateMemory(m_device, &allocate_info, m_allocator_ptr, &memory);
	if(res != VK_SUCCESS){
		std::string msg = "vkAllocateMemory";
		throw vk_utils::vulkan_error(msg, res);
	}

	m_device_handlers[memory] = heap.index;

	m_heaps[heap.index].calc_allocate(req.size);
	return memory;
}

void device_memory::free_memory(VkDeviceMemory memory){
	assert(memory != VK_NULL_HANDLE);

	VkDeviceSize size = 0;
	vkGetDeviceMemoryCommitment(m_device, memory, &size);

	vkFreeMemory(m_device, memory, m_allocator_ptr);

	uint32_t index = m_device_handlers[memory];
	m_heaps[index].calc_free(size);
	m_device_handlers.erase(memory);
}
//-------------------------------------------------------------------

//==============device_memory::memory_heap implementation============
device_memory::memory_heap::memory_heap(
	VkDeviceSize size,	VkMemoryHeapFlags flags) noexcept
	: m_max_size(size), m_curr_size(0), m_flags(flags){
}

bool device_memory::memory_heap::check_heap_flags(VkMemoryHeapFlags flags) const noexcept{
	return (m_flags & flags) == flags;
}

VkDeviceSize device_memory::memory_heap::get_max_mem() const noexcept{
	return m_max_size;
}

VkDeviceSize device_memory::memory_heap::get_busy_mem() const noexcept{
	return m_curr_size;
}

VkDeviceSize device_memory::memory_heap::get_remaining_mem() const noexcept{
	return this->get_max_mem() - this->get_busy_mem();
}

void device_memory::memory_heap::calc_allocate(VkDeviceSize size) noexcept{
	m_curr_size += size;
	assert(m_curr_size <= m_max_size);
}

void device_memory::memory_heap::calc_free(VkDeviceSize size) noexcept{
	m_curr_size -= size;
	assert(m_curr_size >= 0);
}
//-------------------------------------------------------------------

//==============device_memory::memory_type implementation============
device_memory::memory_type::memory_type(
	VkMemoryPropertyFlags flags, uint32_t heap_index) noexcept
	: m_flags(flags), m_heap_index(heap_index){

	uint32_t flags_counter = 0;
	for(uint32_t offset = 0; offset < sizeof(VkMemoryPropertyFlags) * 8; offset++){
		if(flags & (1 << offset))
			flags_counter++;
	}

	m_priority = 1 / flags_counter;
}

bool device_memory::memory_type::check_type_flags(
	VkMemoryPropertyFlags flags) const noexcept{
	return (m_flags & flags) == flags;
}

uint32_t device_memory::memory_type::get_index() const noexcept{
	return m_heap_index;
}

double device_memory::memory_type::get_priority() const noexcept{
	return m_priority;
}
//-------------------------------------------------------------------
