#ifndef INSTANCE_CREATOR_HPP_
#define INSTANCE_CREATOR_HPP_

#include <vulkan\vulkan.h>

#include <string>
#include <vector>

#include "handle_wrapper.hpp"

namespace vk_utils{

class instance_creator{
	using instance_wrapper = vk_utils::instance_wrapper;
	
public:
	instance_creator();
	~instance_creator() = default;
	
	instance_creator& add_extension(const std::string& extension);
	instance_creator& add_layer(const std::string& layer);
	instance_creator& set_app_name(const std::string& app_name);
	instance_creator& set_engine_name(const std::string& engine_name);
	
	[[nodiscard]] instance_wrapper create();
	
	instance_creator(instance_creator& ) = delete;
	instance_creator& operator=(const instance_creator& ) = delete;
	
	instance_creator(instance_creator&&) = default;
private:
	std::string m_app_name;
	std::string m_engine_name;
	
	std::vector<std::string> m_extensions;
	std::vector<std::string> m_layers;
	
	VkAllocationCallbacks* m_allocator_ptr;
	
	VkApplicationInfo m_app_info;
	VkInstanceCreateInfo m_inst_info;
};

}
#endif