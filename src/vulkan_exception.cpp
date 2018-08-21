#include "vulkan_exception.hpp"

using vulkan_error = vk_utils::vulkan_error;

vulkan_error::vulkan_error(std::string msg) noexcept
	: m_msg(msg){
}

vulkan_error::vulkan_error(std::string msg, VkResult err) noexcept{
	m_msg += "ERROR: ";
	switch(err){
		case VK_SUCCESS :
			m_msg += "VK_SUCCESS ";
		break;
		case VK_NOT_READY :
			m_msg += "VK_NOT_READY ";
		break;
		case VK_TIMEOUT :
			m_msg += "VK_TIMEOUT ";
		break;
		case VK_EVENT_SET :
			m_msg += "VK_EVENT_SET ";
		break;
		case VK_EVENT_RESET :
			m_msg += "VK_EVENT_RESET ";
		break;
		case VK_INCOMPLETE :
			m_msg += "VK_INCOMPLETE ";
		break;
		case VK_ERROR_OUT_OF_HOST_MEMORY :
			m_msg += "VK_ERROR_OUT_OF_HOST_MEMORY ";
		break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY :
			m_msg += "VK_ERROR_OUT_OF_DEVICE_MEMORY ";
		break;
		case VK_ERROR_INITIALIZATION_FAILED :
			m_msg += "VK_ERROR_INITIALIZATION_FAILED ";
		break;
		case VK_ERROR_DEVICE_LOST :
			m_msg += "VK_ERROR_DEVICE_LOST ";
		break;
		case VK_ERROR_MEMORY_MAP_FAILED :
			m_msg += "VK_ERROR_MEMORY_MAP_FAILED ";
		break;
		case VK_ERROR_LAYER_NOT_PRESENT :
			m_msg += "VK_ERROR_LAYER_NOT_PRESENT ";
		break;
		case VK_ERROR_EXTENSION_NOT_PRESENT :
			m_msg += "VK_ERROR_EXTENSION_NOT_PRESENT ";
		break;
		case VK_ERROR_FEATURE_NOT_PRESENT :
			m_msg += "VK_ERROR_FEATURE_NOT_PRESENT ";
		break;
		case VK_ERROR_INCOMPATIBLE_DRIVER :
			m_msg += "VK_ERROR_INCOMPATIBLE_DRIVER ";
		break;
		case VK_ERROR_TOO_MANY_OBJECTS :
			m_msg += "VK_ERROR_TOO_MANY_OBJECTS ";
		break;
		case VK_ERROR_FORMAT_NOT_SUPPORTED :
			m_msg += "VK_ERROR_FORMAT_NOT_SUPPORTED ";
		break;
		case VK_ERROR_FRAGMENTED_POOL :
			m_msg += "VK_ERROR_FRAGMENTED_POOL ";
		break;
		case VK_ERROR_OUT_OF_POOL_MEMORY :
			m_msg += "VK_ERROR_OUT_OF_POOL_MEMORY ";
		break;
		case VK_ERROR_INVALID_EXTERNAL_HANDLE :
			m_msg += "VK_ERROR_INVALID_EXTERNAL_HANDLE ";
		break;
		case VK_ERROR_SURFACE_LOST_KHR :
			m_msg += "VK_ERROR_SURFACE_LOST_KHR ";
		break;
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR :
			m_msg += "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR ";
		break;
		case VK_SUBOPTIMAL_KHR :
			m_msg += "VK_SUBOPTIMAL_KHR ";
		break;
		case VK_ERROR_OUT_OF_DATE_KHR :
			m_msg += "VK_ERROR_OUT_OF_DATE_KHR ";
		break;
		case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR :
			m_msg += "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR ";
		break;
		case VK_ERROR_VALIDATION_FAILED_EXT :
			m_msg += "VK_ERROR_VALIDATION_FAILED_EXT ";
		break;
		case VK_ERROR_INVALID_SHADER_NV :
			m_msg += "VK_ERROR_INVALID_SHADER_NV ";
		break;
		case VK_ERROR_FRAGMENTATION_EXT :
			m_msg += "VK_ERROR_FRAGMENTATION_EXT ";
		break;
		case VK_ERROR_NOT_PERMITTED_EXT :
			m_msg += "VK_ERROR_NOT_PERMITTED_EXT ";
		break;

		default:
			m_msg += "unknown error";
	}
	m_msg += "\n   ";
	m_msg += msg;
}

const char* vulkan_error::what() const noexcept{
	return m_msg.c_str();
}
