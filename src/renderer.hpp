#ifndef RENDERER_HPP_
#define RENDERER_HPP_

namespace vk{

class renderer{
public:
	renderer();
	~renderer();
	
	renderer(renderer& ) = delete;
	renderer& operator=(const renderer& ) = delete;
	
	renderer(renderer&&) = default;
private:
	
};


}

#endif