#ifndef DEVICE_HPP_
#define DEVICE_HPP_

#include <vulkan/vulkan.h>

#include <vector>
#include <map>
#include <string>

#include "handle_wrapper.hpp"
#include "platform.hpp"

namespace vk_utils{

class queue_family;
class device;
class device_creator;
class device_memory;

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
	explicit device(VkPhysicalDevice physical_device, VkDevice logical_device,
		VkAllocationCallbacks* allocator_ptr);

	[[nodiscard]] bool presentation_support_check(const window_wrapper& window,
		uint32_t index) const noexcept;
	[[nodiscard]] bool presentation_support_check_any(
		const window_wrapper& window) const noexcept;
	std::vector<uint32_t> get_presentation_support(
		const window_wrapper& window) const;

	device(device& ) = delete;
	device& operator=(const device& ) = delete;
	device& operator=(device&& ) = default;

	device(device&&) = default;

	~device();

private:
	VkPhysicalDevice m_physical_device;
	device_wrapper m_logical_device;
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

class device_memory{
public:
	device_memory() = delete;
	explicit device_memory(VkPhysicalDevice physical_device, VkDevice device,
		VkAllocationCallbacks* allocator_ptr);

	VkDeviceMemory allocate_memory(VkMemoryRequirements req,
		VkMemoryPropertyFlags flags);
	void free_memory(VkDeviceMemory memory);

	device_memory(device_memory& ) = delete;
	device_memory& operator=(const device_memory& ) = delete;

private:
	class memory_type;
	class memory_heap;

	std::vector<memory_type> m_types;
	std::vector<memory_heap> m_heaps;

	std::map<VkDeviceMemory, uint32_t> m_device_handlers;

	VkDevice m_device;
	VkAllocationCallbacks* m_allocator_ptr;
};

}

#endif
