#include "instance_creator.hpp"
#include "vulkan_exception.hpp"

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
}

instance_creator& instance_creator::add_extension(const std::string& extension){
	m_extensions.push_back(extension);
	return *this;
}

instance_creator& instance_creator::add_layer(const std::string& layer){
	m_layers.push_back(layer);
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
	[[nodiscard]] std::vector<VkExtensionProperties> f_get_extensions();
	[[nodiscard]] bool f_find_extensions(const std::vector<VkExtensionProperties> &all_extensions,
		const std::string& search_extension);
	[[nodiscard]] std::vector<const char*> f_extensions_filter(
		const std::vector<VkExtensionProperties> &support_extensions,
		const std::vector<std::string> &required_extensions);

	[[nodiscard]] std::vector<VkLayerProperties> f_get_layers();
	[[nodiscard]] bool f_find_layers(const std::vector<VkLayerProperties> &all_layers,
		const std::string& search_layer);
	[[nodiscard]] std::vector<const char*> f_layers_filter(
		const std::vector<VkLayerProperties> &support_layers,
		const std::vector<std::string> &required_layers);
}

instance_wrapper instance_creator::create(){
	this->m_app_info.pApplicationName = this->m_app_name.c_str();
	this->m_app_info.pEngineName = this->m_engine_name.c_str();

	auto all_extensions = f_get_extensions();
	std::vector<const char*> required_extensions = f_extensions_filter(all_extensions,
		this->m_extensions);

	this->m_inst_info.enabledExtensionCount = required_extensions.size();
	this->m_inst_info.ppEnabledExtensionNames = required_extensions.data();

	auto all_layers = f_get_layers();
	std::vector<const char*> required_layers = f_layers_filter(all_layers, m_layers);

	m_inst_info.enabledLayerCount = required_layers.size();
	m_inst_info.ppEnabledLayerNames = required_layers.data();

	VkResult res = VK_SUCCESS;
	VkInstance instance;

	res = vkCreateInstance(&this->m_inst_info, this->m_allocator_ptr, &instance);
	if(res != VK_SUCCESS){
		std::string msg = "instance create failed";
		throw vk_utils::vulkan_error(msg, res);
	}

	return instance_wrapper(instance, m_allocator_ptr);
}

namespace {
	std::vector<VkExtensionProperties> f_get_extensions(){
		VkResult res = VK_SUCCESS;
		uint32_t extensions_count = 0;

		res = vkEnumerateInstanceExtensionProperties(
			nullptr, &extensions_count, nullptr);
		if(res != VK_SUCCESS){
			std::string msg = "get counter enumerate extension failed";
			throw vk_utils::vulkan_error(msg, res);
		}

		std::vector<VkExtensionProperties> extensions(extensions_count);
		extensions.resize(extensions_count);

		res = vkEnumerateInstanceExtensionProperties(
			nullptr, &extensions_count, extensions.data());
		if(res != VK_SUCCESS){
			std::string msg = "get extensions enumerate extension failed";
			throw vk_utils::vulkan_error(msg, res);
		}

		return std::move(extensions);
	}

	bool f_find_extensions(
		const std::vector<VkExtensionProperties> &all_extensions,
		const std::string& search_extension){

		for(auto &curr : all_extensions){
			if(search_extension == curr.extensionName)
				return true;
		}

		return false;
	}

	 std::vector<const char*> f_extensions_filter(
		const std::vector<VkExtensionProperties> &support_extensions,
		const std::vector<std::string> &required_extensions){

		std::vector<const char*> result;
		for(auto &req : required_extensions){
			if(f_find_extensions(support_extensions, req)){
				result.push_back(req.c_str());
			} else {
				std::string msg = req + " extension not support";
				throw vk_utils::vulkan_error(msg);
			}
		}

		return result;
	}

	std::vector<VkLayerProperties> f_get_layers(){
		VkResult res = VK_SUCCESS;
		uint32_t layers_count = 0;

		res = vkEnumerateInstanceLayerProperties(&layers_count, NULL);
		if(res != VK_SUCCESS){
			std::string msg = "get counter enumerate layer failed";
			throw vk_utils::vulkan_error(msg, res);
		}

		std::vector<VkLayerProperties> layers(layers_count);
		layers.resize(layers_count);

		res = vkEnumerateInstanceLayerProperties(&layers_count, layers.data());
		if(res != VK_SUCCESS){
			std::string msg = "get layers enumerate layer failed";
			throw vk_utils::vulkan_error(msg, res);
		}

		return std::move(layers);
	}

	bool f_find_layers(const std::vector<VkLayerProperties> &all_layers,
		const std::string& search_layer){

		for(auto &curr : all_layers){
			if(search_layer == curr.layerName)
				return true;
		}

		return false;
	}

	std::vector<const char*> f_layers_filter(
		const std::vector<VkLayerProperties> &support_layers,
		const std::vector<std::string> &required_layers){

		std::vector<const char*> result;
		for(auto &req : required_layers){
			if(f_find_layers(support_layers, req)){
				result.push_back(req.c_str());
			} else {
				std::string msg = req + " layer not support";
				throw vk_utils::vulkan_error(msg);
			}
		}

		return result;
	}
}
