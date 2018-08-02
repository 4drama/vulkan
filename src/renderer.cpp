#include "renderer.hpp"
#include "vk_utils.cpp"

vk::renderer::renderer(){
	this->m_instance = instance_creator()
		.add_extension(VK_KHR_WIN32_SURFACE_EXTENSION_NAME)
		.add_extension(VK_KHR_SURFACE_EXTENSION_NAME)
		.add_extension(VK_KHR_DISPLAY_EXTENSION_NAME)
		.create();
}

vk::renderer::~renderer(){
	
}