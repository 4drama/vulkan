#ifndef DEVICE_HPP_
#define DEVICE_HPP_

#include <vulkan/vulkan.h>

#include <vector>
#include <string>
#include <utility>
#include <memory>

#include "handle_wrapper.hpp"
#include "platform.hpp"
#include "device_memory.hpp"

namespace vk_utils{

class queue_family;
class device;
class device_creator;

class queue_family{
public:
	explicit queue_family(
		VkPhysicalDevice physical_device,
		VkDevice device,
		const VkAllocationCallbacks* allocator_ptr,
		uint32_t family_index);

	[[nodiscard]] bool flags_check(VkQueueFlags flags) const noexcept;

private:
	uint32_t m_family_index;
	VkQueueFamilyProperties m_family_properties;

	class queues;
	std::shared_ptr<queues> m_queues_ptr;
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

	device_memory m_memory;
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
