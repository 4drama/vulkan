#include "xlib_window.hpp"

#include <stdexcept>

using xlib_window_creator = vk_utils::xlib_window_creator;

using xlib_window_wrapper = vk_utils::xlib_window_wrapper;

template<>
xlib_window_wrapper::handle_wrapper(){
	m_handle.display = nullptr;
	m_handle.window = 0;
	m_handle.event.type = 0;
	m_handle.screen_number = 0;
	XVisualInfo visual_info = {0};
	m_handle.gc = 0;

	m_allocator_ptr = nullptr;
}

template<>
xlib_window_wrapper::~handle_wrapper(){
	if(m_handle.display != nullptr){
		XDestroyWindow(m_handle.display, m_handle.window);
	 	XCloseDisplay(m_handle.display);
	}
}

template<>
xlib_window_wrapper& xlib_window_wrapper:: operator=(xlib_window_wrapper&& other){
	if (this != &other) {
		this->m_handle = other.m_handle;
		other.m_handle = vk_utils::xlib_app{nullptr, 0, 0, 0, {0}, 0};
    }
    return *this;
}

xlib_window_creator::xlib_window_creator()
	: m_window_name("xlib_window"), m_width(800), m_height(600){

}

xlib_window_creator& xlib_window_creator::set_window_name(std::string name){
	m_window_name = name;
	return *this;
}

xlib_window_creator& xlib_window_creator::set_resolution(
	uint32_t width, uint32_t height){

	m_width = width;
	m_height = height;
	return *this;
}

namespace{
	void set_title(const char *title_ptr, Display *display, Window window){
		XTextProperty windowname;

		XStringListToTextProperty((char**)&title_ptr, 1, &windowname);

		XSetWMProperties(display, window, &windowname,
	  		NULL, NULL, NULL, NULL, NULL,
	  		NULL );
	}
}

xlib_window_wrapper xlib_window_creator::create(){
	xlib_app window{0};

	window.display = XOpenDisplay(NULL);
	if(!window.display){
		std::string msg = "XOpenDisplay failed";
		throw std::runtime_error(msg);
	}

	window.screen_number = DefaultScreen(window.display);

	int depth = 32;

	if (!XMatchVisualInfo(window.display, XDefaultScreen(window.display),
	 	depth, TrueColor, &window.visual_info)){

		std::string msg = "XMatchVisualInfo failed";
		throw std::runtime_error(msg);
	}

	Window parent = XDefaultRootWindow(window.display);
	XSetWindowAttributes attrs{0};

	attrs.colormap = XCreateColormap(window.display,
		parent, window.visual_info.visual, AllocNone);
	attrs.background_pixel = 0;
	attrs.border_pixel = 0;

	window.window = XCreateWindow(window.display,
		parent, 10, 10, m_width, m_height, 1,
		depth, InputOutput, window.visual_info.visual,
		CWBackPixel | CWColormap | CWBorderPixel, &attrs);

	set_title(m_window_name.c_str(), window.display, window.window);

	XSelectInput(window.display, window.window, KeyPressMask | KeyReleaseMask );
	XMapWindow(window.display, window.window);

	window.gc = XCreateGC ( window.display, window.window, 0 , NULL );

	return xlib_window_wrapper(window);
}
