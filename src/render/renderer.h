#ifndef RENDERER_H
#define RENDERER_H

namespace game
{

struct quadpass
{
	quadpass(win::roll&, const win::area&);

	void add(const ent::entity&);
	void send();

	std::vector<float> buffer_position_size_rotation;

	win::program program;
	win::vao vao;
	win::vbo triangle, position_size_rotation;
	win::ebo ebo;
};

struct renderer
{
	renderer(win::display&, win::roll&);

	win::area screen;
	win::font_renderer font_renderer;

	game::quadpass quadpass;

	struct
	{
		win::font title;
	} font;
};

}

#endif
