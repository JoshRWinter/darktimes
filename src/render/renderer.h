#ifndef RENDERER_H
#define RENDERER_H

namespace game
{

struct renderer
{
	renderer(win::display&);

	win::area screen;
};

}

#endif
