#include "renderer.hpp"
#include "instance_creator.hpp"

vk::renderer::renderer(){
	this->m_instance = vk_utils::instance_creator()
		.add_extension(VK_KHR_WIN32_SURFACE_EXTENSION_NAME)
		.add_extension(VK_KHR_SURFACE_EXTENSION_NAME)
//		.add_layer("VK_LAYER_LUNARG_api_dump")
		.create();
		
	m_device = vk_utils::device_creator(m_instance.get())
		.add_extension(VK_KHR_SWAPCHAIN_EXTENSION_NAME)
		.add_extension(VK_KHR_DISPLAY_SWAPCHAIN_EXTENSION_NAME)
		.create();
}

vk::renderer::~renderer(){
	
}