#include "device_memory.hpp"

#include "vulkan_exception.hpp"

#include <cassert>

using device_memory = vk_utils::device_memory;

//=====================device_memory implementation==================
device_memory::device_memory(VkPhysicalDevice physical_device, VkDevice device,
	VkAllocationCallbacks* allocator_ptr)
	: m_device(device), m_allocator_ptr(allocator_ptr){

	if(physical_device == VK_NULL_HANDLE)
		return;

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

uint32_t device_memory::get_queue_families_size() const noexcept{
	return m_types.size();
}
//-------------------------------------------------------------------

//==============device_memory::memory_heap implementation============
device_memory::memory_heap::memory_heap(
	VkDeviceSize size,	VkMemoryHeapFlags flags) noexcept
	: m_max_size(size), m_curr_size(0), m_flags(flags){
}

bool device_memory::memory_heap::check_heap_flags(
	VkMemoryHeapFlags flags) const noexcept{

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
