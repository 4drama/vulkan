#include "vk_utils.hpp"

#include <stdexcept>

using instance_creator = vk_utils::instance_creator;
using instance_wrapper = vk_utils::instance_wrapper;

instance_creator::instance_creator() 
	: m_app_name("Application name"), m_engine_name("Engine name"), m_allocator_ptr(nullptr){
	this->m_app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	this->m_app_info.pNext = nullptr;

	this->m_app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	this->m_app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	this->m_app_info.apiVersion = VK_API_VERSION_1_0;
	
	this->m_inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	this->m_inst_info.pNext = nullptr;
	this->m_inst_info.flags = 0;
	this->m_inst_info.pApplicationInfo = &this->m_app_info;
	
	this->m_inst_info.enabledLayerCount = 0;			//	TODO: take out
	this->m_inst_info.ppEnabledLayerNames = nullptr;	//	TODO: take out
}

instance_creator& instance_creator::add_extension(const std::string& extension){
	m_extensions.push_back(extension);
	return *this;
}

instance_creator& instance_creator::set_app_name(const std::string& app_name){
	this->m_app_name = app_name;
	return *this;
}

instance_creator& instance_creator::set_engine_name(const std::string& engine_name){
	this->m_engine_name = engine_name;
	return *this;
}

namespace {
	[[nodiscard]] std::vector<VkExtensionProperties> get_extensions();
	[[nodiscard]] bool find_extensions(const std::vector<VkExtensionProperties> &all_extensions,
		const std::string& search_extension);
	[[nodiscard]] std::vector<const char*> extensions_filter(
		const std::vector<VkExtensionProperties> &support_extensions,
		const std::vector<std::string> &required_extensions);
}

instance_wrapper instance_creator::create(){	
	this->m_app_info.pApplicationName = this->m_app_name.c_str();
	this->m_app_info.pEngineName = this->m_engine_name.c_str();
	
	auto all_extensions = get_extensions();
	
	std::vector<const char*> required_extensions = extensions_filter(all_extensions, 
		this->m_extensions);
	
	this->m_inst_info.enabledExtensionCount = required_extensions.size();
	this->m_inst_info.ppEnabledExtensionNames = required_extensions.data();
	
	VkResult res = VK_SUCCESS;
	VkInstance instance;
	
	res = vkCreateInstance(&this->m_inst_info, this->m_allocator_ptr, &instance);
	if(res != VK_SUCCESS){
		throw std::runtime_error("vkCreateInstance");
	}
	
	return instance_wrapper(instance, m_allocator_ptr);
}

namespace {
	std::vector<VkExtensionProperties> get_extensions(){
		VkResult res = VK_SUCCESS;		
		uint32_t extensions_count = 0;
		
		res = vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, nullptr);
		if(res != VK_SUCCESS){
			throw std::runtime_error("vkEnumerateInstanceExtensionProperties get counter");
		}
		
		std::vector<VkExtensionProperties> extensions(extensions_count);
		extensions.resize(extensions_count);
		
		res = vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, extensions.data());
		if(res != VK_SUCCESS){
			throw std::runtime_error("vkEnumerateInstanceExtensionProperties get extensions");
		}
		
		return std::move(extensions);
	}
	
	bool find_extensions(
		const std::vector<VkExtensionProperties> &all_extensions,
		const std::string& search_extension){
			
		for(auto &curr : all_extensions){
			if(search_extension == curr.extensionName)
				return true;
		}
		
		return false;
	}
	
	 std::vector<const char*> extensions_filter(
		const std::vector<VkExtensionProperties> &support_extensions,
		const std::vector<std::string> &required_extensions){
		
		std::vector<const char*> result;
		for(auto &req : required_extensions){
			if(find_extensions(support_extensions, req)){
				result.push_back(req.c_str());
			} else {
				throw std::runtime_error("required extensions not support");
			}				
		}
		
		return result;
	}
}