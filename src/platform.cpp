#include "platform.hpp"

#if defined(_WIN32)

namespace{
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
}

vk_utils::platform_window_creator vk_utils::get_platform_window(){
	return vk_utils::platform_window_creator(WndProc);
}

const char* vk_utils::get_surface_extension_name(){
	return VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
}

namespace{
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
}

#endif //_WIN32
