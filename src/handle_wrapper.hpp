#ifndef HANDLE_WRAPPER_HPP_
#define HANDLE_WRAPPER_HPP_


namespace vk_utils{

template<class H>
class handle_wrapper;

using instance_wrapper = handle_wrapper<VkInstance>;

template<class H>
class handle_wrapper{
public:
	handle_wrapper();
	explicit handle_wrapper(H handle, VkAllocationCallbacks* allocator);
	
	H get() const;
	
	handle_wrapper(handle_wrapper& ) = delete;
	handle_wrapper& operator=(const handle_wrapper& ) = delete;
	
	handle_wrapper& operator=(handle_wrapper&& ) = default;
	
	~handle_wrapper();
private:
	H m_handle;
	VkAllocationCallbacks* m_allocator_ptr;
};

template<class H>
handle_wrapper<H>::handle_wrapper()
	: m_handle(VK_NULL_HANDLE), m_allocator_ptr(nullptr){
	
}

template<class H>
handle_wrapper<H>::handle_wrapper(H handle, VkAllocationCallbacks* allocator)
	: m_handle(handle), m_allocator_ptr(allocator){
	
}

template<class H>
H handle_wrapper<H>::get() const{
	return m_handle;
}

template<class H>
handle_wrapper<H>::~handle_wrapper(){
	
}

}
#endif