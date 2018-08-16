#ifndef WIN32_WINDOW_HPP_
#define WIN32_WINDOW_HPP_

#include "windows.h"

#include <string>
#include "handle_wrapper.hpp"

namespace vk_utils{

struct win_app{
	HWND window;
	HINSTANCE hInstance;
};

using win32_window_wrapper = handle_wrapper<win_app, void>;

template<>
win32_window_wrapper::handle_wrapper();

template<>
win32_window_wrapper::~handle_wrapper();

template<>
win32_window_wrapper& win32_window_wrapper:: operator=(win32_window_wrapper&& );

class win32_window_creator{
public:
	win32_window_creator() = delete;
	explicit win32_window_creator(WNDPROC process_func);
	~win32_window_creator() = default;

	win32_window_creator& set_hinstance(HINSTANCE hinstance);
	win32_window_creator& set_class_name(std::string name);
	win32_window_creator& set_window_name(std::string name);
	win32_window_creator& set_resolution(uint32_t width, uint32_t height);
	win32_window_creator& set_icon(HICON icon);
	win32_window_creator& set_cursor(HCURSOR cursor);

	[[nodiscard]] win32_window_wrapper create();

	win32_window_creator(win32_window_creator& ) = delete;
	win32_window_creator& operator=(const win32_window_creator& ) = delete;
private:
	WNDCLASSEX m_window_class_example;

	std::string m_window_name;
	uint32_t m_width;
	uint32_t m_height;
};

}
#endif
