#pragma once

#include "../../Darktimes.hpp"

class GLSubRenderer
{
public:
	virtual void flush() = 0;

protected:
	~GLSubRenderer() = default;
};
