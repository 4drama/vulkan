#ifndef COMMAND_POOL_HPP_
#define COMMAND_POOL_HPP_

#include "device.hpp"

namespace vk_utils{

class command_pool{
public:
	command_pool() = delete;
	command_pool(const device& device, VkQueueFlags flags);

	command_pool(command_pool& ) = delete;
	command_pool& operator=(const command_pool& ) = delete;

	command_pool(command_pool&&) = default;
	command_pool& operator=(command_pool&& ) = default;

	~command_pool() = default;

private:
	const device& m_device;
	uint32_t m_family_index;
};

}

#endif
