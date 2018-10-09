#include "renderer.hpp"
#include "instance_creator.hpp"
#include "vulkan_exception.hpp"

#include <string>

#include <chrono>
#include "swapchain_creator.hpp"

vk::renderer::renderer(){
	this->m_instance = vk_utils::instance_creator()
		.add_extension(vk_utils::get_surface_extension_name())
		.add_extension(VK_KHR_SURFACE_EXTENSION_NAME)
//		.add_extension(VK_KHR_DISPLAY_EXTENSION_NAME)
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

	if(!m_device.presentation_support_check_any(m_window)){
		std::string msg = "Presentation not support.";
		throw std::runtime_error(msg);
	}

	m_surface = vk_utils::create_surface(m_window, m_instance.get(), nullptr);

	//=========REMOVE=========
	using high_resolution_clock = std::chrono::high_resolution_clock;
	high_resolution_clock::time_point time_point = high_resolution_clock::now();
	high_resolution_clock::time_point end_time_point{};
	end_time_point = time_point + std::chrono::seconds(2);

	vk_utils::swapchain_creator(m_device, m_surface, m_window).create();

	vk_utils::show_window(m_window);
	while(time_point < end_time_point){

		time_point = high_resolution_clock::now();
	}
	//------------------------
}

vk::renderer::~renderer(){

}
