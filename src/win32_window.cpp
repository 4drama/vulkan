#include "win32_window.hpp"

#include <stdexcept>

using win32_window_creator = vk_utils::win32_window_creator;

win32_window_creator::win32_window_creator(WNDPROC process_func)
	: m_window_name("win32_window"),
	m_width(800), m_height(600){

	HINSTANCE hinstance = GetModuleHandleA(nullptr);

	m_window_class_example = {};

	m_window_class_example.cbSize = sizeof(WNDCLASSEX);
	m_window_class_example.style = CS_HREDRAW | CS_VREDRAW;
	m_window_class_example.lpfnWndProc = process_func;

	m_window_class_example.hInstance = hinstance;
	m_window_class_example.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	m_window_class_example.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	m_window_class_example.hCursor = LoadCursor(nullptr, IDC_ARROW);

	m_window_class_example.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	m_window_class_example.lpszClassName = TEXT("win32_window_class");
}

win32_window_creator& win32_window_creator::set_hinstance(HINSTANCE hinstance){
	m_window_class_example.hInstance = hinstance;
	m_window_class_example.hIcon = LoadIcon(hinstance, IDI_APPLICATION);
	m_window_class_example.hIconSm = LoadIcon(hinstance, IDI_APPLICATION);
	m_window_class_example.hCursor = LoadCursor(hinstance, IDC_ARROW);
	return *this;
}

win32_window_creator& win32_window_creator::set_class_name(std::string name){
	m_window_class_example.lpszClassName = TEXT(name.c_str());
	return *this;
}

win32_window_creator& win32_window_creator::set_window_name(std::string name){
	m_window_name = name;
	return *this;
}

win32_window_creator& win32_window_creator::set_resolution(
	uint32_t width, uint32_t height){

	m_width = width;
	m_height = height;
	return *this;
}

win32_window_creator& win32_window_creator::set_icon(HICON icon){
	m_window_class_example.hIcon = icon;
	m_window_class_example.hIconSm = icon;
	return *this;
}

win32_window_creator& win32_window_creator::set_cursor(HCURSOR cursor){
	m_window_class_example.hCursor = cursor;
	return *this;
}

HWND win32_window_creator::create(){
	if(!RegisterClassEx(&m_window_class_example)){
		std::string msg = "RegisterClassEx failed";
		throw std::runtime_error(msg);
	}

	HWND window = CreateWindow(
		m_window_class_example.lpszClassName,
		TEXT(m_window_name.c_str()),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		m_width,
		m_height,
		nullptr,
		nullptr,
		m_window_class_example.hInstance,
		nullptr);

	if(!window){
		std::string msg = "CreateWindow failed";
		throw std::runtime_error(msg);
	}
	return window;
}
