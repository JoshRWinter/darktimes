#pragma once

#include <memory>

#include <win/win.hpp>
#include <win/stream.hpp>

#ifdef WIN_USE_OPENGL
#include <GL/gl.h>
#endif

namespace win
{

struct AtlasItem
{
	float x1;
	float y1;
	float x2;
	float y2;
};

class Atlas
{
public:
	enum class Mode {LINEAR, NEAREST};

	Atlas(Stream, Mode = Mode::LINEAR);
	Atlas(const Atlas&) = delete;
	Atlas(Atlas&&) = delete;
	~Atlas();

	void operator=(const Atlas&) = delete;
	Atlas &operator=(Atlas&&) = delete;

	unsigned texture() const;
	const AtlasItem item(int) const;

	static void corrupt();

private:
	int count;
	std::unique_ptr<AtlasItem[]> textures;
#ifdef WIN_USE_OPENGL
	GLuint object;
#endif
};

}
