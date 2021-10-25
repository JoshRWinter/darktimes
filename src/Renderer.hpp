#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "Darktimes.hpp"

class Renderer
{
public:
	NOCOPYMOVE(Renderer);

	Renderer(int, int, float, float, float, float, win::AssetRoll&);
	~Renderer();

	void computeframe();

private:
	unsigned shader_wall;
	unsigned vbo_wall;
	unsigned vao_wall;

	win::FontRenderer font_renderer;
	win::Font font_debug, font_ui;

	std::chrono::time_point<std::chrono::high_resolution_clock> last_fps_calc_time;
	int accumulated_fps;
	char fpsindicator[10];

	const float left, right, bottom, top;
};
#endif
