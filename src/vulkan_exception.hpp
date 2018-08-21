#ifndef VULKAN_EXEPTION_HPP_
#define VULKAN_EXEPTION_HPP_

#include <vulkan/vulkan.h>
#include <stdexcept>

#include <string>

namespace vk_utils{

class vulkan_error : public std::exception{
public:
	vulkan_error(std::string msg) noexcept;
	vulkan_error(std::string msg, VkResult err) noexcept;
	virtual const char* what() const noexcept override;
private:
	std::string m_msg;
};

}

#endif
