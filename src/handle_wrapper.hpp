#ifndef HANDLE_WRAPPER_HPP_
#define HANDLE_WRAPPER_HPP_

#include <vulkan\vulkan.h>

namespace vk_utils{

template<class handle_type, class allocator_callback_type>
class handle_wrapper;

using instance_wrapper = handle_wrapper<VkInstance, VkAllocationCallbacks>;
using device_wrapper = handle_wrapper<VkDevice, VkAllocationCallbacks>;

template<class handle_type, class allocator_callback_type>
class handle_wrapper{
public:
	handle_wrapper();
	explicit handle_wrapper(handle_type handle, allocator_callback_type* allocator = nullptr);

	handle_type get() const;

	handle_wrapper(handle_wrapper& ) = delete;
	handle_wrapper& operator=(const handle_wrapper& ) = delete;

	handle_wrapper(handle_wrapper&&) = delete;
	handle_wrapper& operator=(handle_wrapper&& );

	~handle_wrapper();
private:
	handle_type m_handle;
	allocator_callback_type* m_allocator_ptr;
};

template<class H, class A>
handle_wrapper<H, A>::handle_wrapper()
	: m_handle(VK_NULL_HANDLE), m_allocator_ptr(nullptr){

}

template<class H, class A>
handle_wrapper<H, A>::handle_wrapper(H handle, A* allocator)
	: m_handle(handle), m_allocator_ptr(allocator){

}

template<class H, class A>
H handle_wrapper<H, A>::get() const{
	return m_handle;
}

template<class H, class A>
handle_wrapper<H, A>& handle_wrapper<H, A>::operator=(handle_wrapper<H, A>&& other){
    if (this != &other) {
		this->m_handle = other.m_handle;
		other.m_handle = VK_NULL_HANDLE;

		this->m_allocator_ptr = other.m_allocator_ptr;
    }
    return *this;
}

template<>
handle_wrapper<VkInstance, VkAllocationCallbacks>::~handle_wrapper();

template<>
handle_wrapper<VkDevice, VkAllocationCallbacks>::~handle_wrapper();

template<class H, class A>
handle_wrapper<H, A>::~handle_wrapper(){

}

}
#endif
