#include "renderer.hpp"
#include "instance_creator.hpp"

namespace{

#if defined(_WIN32)
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
#endif

}

vk::renderer::renderer(){
	this->m_instance = vk_utils::instance_creator()
#if defined(_WIN32)
		.add_extension(VK_KHR_WIN32_SURFACE_EXTENSION_NAME)
#endif
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

#if defined(_WIN32)
	m_window = vk_utils::win32_window_creator(WndProc)
		.set_resolution(640, 480)
		.create();
#endif

}

vk::renderer::~renderer(){

}

namespace{

#if defined(_WIN32)
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	switch(message){
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}
#endif

}
