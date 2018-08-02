#include "renderer.hpp"
#include "instance_creator.hpp"

vk::renderer::renderer(){
	this->m_instance = vk_utils::instance_creator()
		.add_extension(VK_KHR_WIN32_SURFACE_EXTENSION_NAME)
		.add_extension(VK_KHR_SURFACE_EXTENSION_NAME)
		.add_layer("VK_LAYER_LUNARG_api_dump")
		.create();
}

vk::renderer::~renderer(){
	
}