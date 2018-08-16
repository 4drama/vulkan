#include "renderer.hpp"
#include "instance_creator.hpp"

vk::renderer::renderer(){
	this->m_instance = vk_utils::instance_creator()
		.add_extension(vk_utils::get_surface_extension_name())
		.add_extension(VK_KHR_SURFACE_EXTENSION_NAME)
//		.add_layer("VK_LAYER_LUNARG_api_dump")
		.create();

	m_device = vk_utils::device_creator(m_instance.get())
		.add_feature("multiDrawIndirect")
		.add_feature("tessellationShader")
		.add_feature("geometryShader")
		.add_extension(VK_KHR_SWAPCHAIN_EXTENSION_NAME)
//		.add_extension(VK_KHR_DISPLAY_SWAPCHAIN_EXTENSION_NAME)
		.create();

	m_window = vk_utils::get_platform_window()
		.set_resolution(640, 480)
		.create();

	m_surface = vk_utils::create_surface(m_window, m_instance.get(), nullptr);
}

vk::renderer::~renderer(){

}
