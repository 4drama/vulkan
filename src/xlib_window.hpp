#ifndef XLIB_WINDOW_HPP_
#define XLIB_WINDOW_HPP_

#include <string>
#include "handle_wrapper.hpp"

#include <X11/Xlib.h>

namespace vk_utils{

struct win_app{

};

using xlib_window_wrapper = handle_wrapper</**/>;

template<>
xlib_window_wrapper::handle_wrapper();

template<>
xlib_window_wrapper::~handle_wrapper();

template<>
xlib_window_wrapper& xlib_window_wrapper:: operator=(xlib_window_wrapper&& );

class xlib_window_creator{
public:
	xlib_window_creator() = delete;
	explicit xlib_window_creator(/*WNDPROC process_func*/);
	~xlib_window_creator() = default;

//	xlib_window_creator& set_hinstance(HINSTANCE hinstance);
//	xlib_window_creator& set_class_name(std::string name);
	xlib_window_creator& set_window_name(std::string name);
	xlib_window_creator& set_resolution(uint32_t width, uint32_t height);
//	xlib_window_creator& set_icon(HICON icon);
//	xlib_window_creator& set_cursor(HCURSOR cursor);

	[[nodiscard]] xlib_window_wrapper create();

	xlib_window_creator(win32_window_creator& ) = delete;
	xlib_window_creator& operator=(const xlib_window_creator& ) = delete;
private:
//	WNDCLASSEX m_window_class_example;

	std::string m_window_name;
	uint32_t m_width;
	uint32_t m_height;
};

}

#endif
