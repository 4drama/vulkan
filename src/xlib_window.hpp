#ifndef XLIB_WINDOW_HPP_
#define XLIB_WINDOW_HPP_

#include <string>
#include "handle_wrapper.hpp"

#include <X11/Xlib.h>

namespace vk_utils{

struct xlib_app{
	Display *display;
	Window window;
	XEvent event;
	int screen_number;
	GC gc;
};

using xlib_window_wrapper = handle_wrapper<xlib_app, void>;

template<>
xlib_window_wrapper::handle_wrapper();

template<>
xlib_window_wrapper::~handle_wrapper();

template<>
xlib_window_wrapper& xlib_window_wrapper:: operator=(xlib_window_wrapper&& );

class xlib_window_creator{
public:
	xlib_window_creator();
	~xlib_window_creator() = default;

//	xlib_window_creator& set_class_name(std::string name);
	xlib_window_creator& set_window_name(std::string name);
	xlib_window_creator& set_resolution(uint32_t width, uint32_t height);
//	xlib_window_creator& set_icon(HICON icon);
//	xlib_window_creator& set_cursor(HCURSOR cursor);

	[[nodiscard]] xlib_window_wrapper create();

	xlib_window_creator(xlib_window_creator& ) = delete;
	xlib_window_creator& operator=(const xlib_window_creator& ) = delete;
private:

	std::string m_window_name;
	uint32_t m_width;
	uint32_t m_height;
};

VisualID get_VisualID(const xlib_window_wrapper& window);

}

#endif
