#pragma once

#include <win/MappedRingBuffer.hpp>

namespace win
{

struct GLMappedRingBufferReservation
{
	GLMappedRingBufferReservation(int start, int length)
		: start(start)
		, length(length)
	{}

	bool conflicts(const GLMappedRingBufferReservation &rhs) const
	{
		return start < rhs.start + rhs.length && rhs.start < start + length;
	}

	int start;
	int length;
};

template<typename T> class GLMappedRingBuffer
{
	WIN_NO_COPY_MOVE(GLMappedRingBuffer);

public:
	GLMappedRingBuffer(void *mem, int length_elements)
		: inner(mem, length_elements)
	{}

	MappedRingBufferRange<T> reserve(int len)
	{
		return inner.reserve(len);
	}

private:
	MappedRingBuffer<T> inner;
};

}
