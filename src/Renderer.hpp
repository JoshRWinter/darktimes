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
	struct { unsigned wall; } shader;

	struct { unsigned wall; } vao;

	struct { unsigned wall; } vbo;

	struct { unsigned wall; } ebo;

	struct
	{
		struct { int projection, rot; } wall;
	} uniform;

	win::FontRenderer font_renderer;

	win::Font font_debug, font_ui;

	std::chrono::time_point<std::chrono::high_resolution_clock> last_fps_calc_time;
	int accumulated_fps;
	char fpsindicator[10];

	const float left, right, bottom, top;
};
#endif
