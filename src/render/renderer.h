#ifndef RENDERER_H
#define RENDERER_H

namespace game
{

struct renderer
{
	renderer(win::display&, win::roll&);

	win::area screen;
	win::font_renderer font_renderer;

	struct
	{
		win::font title;
	} font;
};

}

#endif
