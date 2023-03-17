#pragma once

#include <win/Win.hpp>

#ifdef WIN_USE_OPENGL

#include <GL/gl.h>

#include <win/Atlas.hpp>

namespace win
{

class GLAtlas
{
	WIN_NO_COPY_MOVE(GLAtlas);

public:
	enum class Mode { linear, nearest };

	GLAtlas(Stream, Mode);
	~GLAtlas();

	GLuint texture() const;

	int count() const;
	const AtlasItem &item(int) const;

private:
	GLuint gltex;

	int num;
	std::unique_ptr<AtlasItem[]> items;
};

}

#endif