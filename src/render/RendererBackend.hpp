#pragma once

#include <vector>

#include <win/Font.hpp>

#include "../Renderable.hpp"

class RendererBackend
{
public:
	virtual ~RendererBackend() = default;

	virtual const win::Font &create_font(win::Stream data, float size) = 0;
	virtual void draw_text(const win::Font &font, const char *text, float x, float y, bool centered = false) = 0;
	virtual void set_view(float x, float y, float zoom) = 0;
	virtual void begin() = 0;
	virtual void end() = 0;
	virtual void render(const std::vector<Renderable> &objects) = 0;
};
