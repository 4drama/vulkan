#ifndef DEVICE_MEMORY_HPP_
#define DEVICE_MEMORY_HPP_

#include <vulkan/vulkan.h>

#include <map>
#include <vector>

namespace vk_utils{

//====================device_memory declaration======================
class device_memory{
public:
	device_memory() = delete;
	explicit device_memory(VkPhysicalDevice physical_device, VkDevice device,
		VkAllocationCallbacks* allocator_ptr);

	~device_memory() = default;

	VkDeviceMemory allocate_memory(VkMemoryRequirements req,
		VkMemoryPropertyFlags flags);
	void free_memory(VkDeviceMemory memory) noexcept;

	uint32_t get_queue_families_size() const noexcept;
	VkDevice get_device() const noexcept;
	const VkAllocationCallbacks* get_allocator() const noexcept;

	device_memory(device_memory& ) = delete;
	device_memory& operator=(const device_memory& ) = delete;

	device_memory& operator=(device_memory&& ) = default;
	device_memory(device_memory&&) = default;

private:
	class memory_type;
	class memory_heap;

	std::vector<memory_type> m_types;
	std::vector<memory_heap> m_heaps;

	std::map<VkDeviceMemory, uint32_t> m_device_handlers;

	VkDevice m_device;
	VkAllocationCallbacks* m_allocator_ptr;
};
//-------------------------------------------------------------------

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

}

#endif
