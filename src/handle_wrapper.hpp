#ifndef HANDLE_WRAPPER_HPP_
#define HANDLE_WRAPPER_HPP_

#include <vulkan/vulkan.h>

#include <type_traits>

namespace vk_utils{

template<class handle_type, class allocator_callback_type,
	class helper_hendler_type = void>
class handle_wrapper;

using instance_wrapper = handle_wrapper<VkInstance, VkAllocationCallbacks>;
using device_wrapper = handle_wrapper<VkDevice, VkAllocationCallbacks>;
using surface_wrapper = handle_wrapper<VkSurfaceKHR, VkAllocationCallbacks, VkInstance>;

using image_handler_wrapper = handle_wrapper<VkImage, VkAllocationCallbacks, VkDevice>;

template<class addition_handler_type>
class helper_hendler{
protected:
	helper_hendler() = default;
	helper_hendler(addition_handler_type handler);

	addition_handler_type m_addition_handler;
};

template<class H>
helper_hendler<H>::helper_hendler(H handler)
	: m_addition_handler(handler){
}

template<>
class helper_hendler<void>{

};

template<class handle_type, class allocator_callback_type, class helper_hendler_type>
class handle_wrapper : public helper_hendler<helper_hendler_type>{

	using compile_helper_hendler_type = typename std::conditional
		<std::is_void<helper_hendler_type>::value, void*, helper_hendler_type>::type;

public:
	handle_wrapper();


	explicit handle_wrapper(
		handle_type handle,
		allocator_callback_type* allocator = nullptr);

	explicit handle_wrapper(
		handle_type handle,
		compile_helper_hendler_type addition_handler,
		allocator_callback_type* allocator = nullptr);

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

template<class H, class A, class HH>
handle_wrapper<H, A, HH>::handle_wrapper()
	: helper_hendler<HH>(), m_handle(VK_NULL_HANDLE), m_allocator_ptr(nullptr){

}

template<class H, class A, class HH>
handle_wrapper<H, A, HH>::handle_wrapper(H handle, A* allocator)
	: m_handle(handle), m_allocator_ptr(allocator){

	static_assert(std::is_void<HH>::value,
		"not valid constructor");
}

template<class H, class A, class HH>
handle_wrapper<H, A, HH>::handle_wrapper(
	H handle,
	compile_helper_hendler_type addition_handler,
	A* allocator)

	: helper_hendler<HH>(addition_handler),
	m_handle(handle), m_allocator_ptr(allocator){

	static_assert(!std::is_void<HH>::value,
		"not valid constructor");
}

template<class H, class A, class HH>
H handle_wrapper<H, A, HH>::get() const{
	return m_handle;
}

template<class H, class A, class HH>
handle_wrapper<H, A, HH>& handle_wrapper<H, A, HH>::operator=(
	handle_wrapper<H, A, HH>&& other){

    if (this != &other) {
		this->m_handle = other.m_handle;
		other.m_handle = VK_NULL_HANDLE;

		this->m_allocator_ptr = other.m_allocator_ptr;

		if constexpr(!std::is_void<HH>::value){
			this->m_addition_handler = other.m_addition_handler;
		}
    }
    return *this;
}

template<class H, class A, class HH>
handle_wrapper<H, A, HH>::~handle_wrapper(){

}

template<>
instance_wrapper::~handle_wrapper();

template<>
device_wrapper::~handle_wrapper();

template<>
surface_wrapper::~handle_wrapper();

template<>
image_handler_wrapper::~handle_wrapper();

}
#endif
