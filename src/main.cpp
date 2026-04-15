#include <win/AssetRoll.hpp>
#include <win/Display.hpp>
#include <win/gl/GL.hpp>

#include "KeyEvent.hpp"
#include "render/Renderer.hpp"
#include "sim/Simulation.hpp"

int main()
{
    win::AssetRoll roll("Darktimes.roll");

    // display setup
    win::DisplayOptions display_options;
#ifndef NDEBUG
    display_options.caption = "debug_window";
    display_options.fullscreen = false;
    display_options.width = 1920;
    display_options.height = 1080;
    display_options.debug = false;
#else
    display_options.caption = "Darktimes";
    display_options.fullscreen = true;
    display_options.width = 1;
    display_options.height = 1;
#endif
    display_options.gl_major = 4;
    display_options.gl_minor = 4;

    win::Display display(display_options);
    display.vsync(true);

    win::gl_load_functions();

    const win::Dimensions dims(display.width(), display.height());
    const win::Area area(-8.0f, 8.0f, -4.5f, 4.5f);

    win::Pair<float> mouse;
    display.register_mouse_handler(
        [&mouse, &dims, &area](int x, int y)
        {
            mouse.x = ((x / (float)dims.width) * (area.right - area.left)) - area.right;
            mouse.y = ((-y / (float)dims.height) * (area.top - area.bottom)) + area.top;
        });

    std::vector<KeyEvent> keys;
    keys.reserve(20);
    bool quit = false;
    display.register_button_handler(
        [&quit, &keys](win::Button button, bool press)
        {
#ifndef NDEBUG
            if (button == win::Button::esc)
                quit = true;
#endif

            keys.emplace_back(button, press);
        });

    display.register_window_handler(
        [&quit](win::WindowEvent event)
        {
            if (event == win::WindowEvent::close)
                quit = true;
        });

    Renderer renderer(dims, area, roll);

    win::SimStateExchanger<Renderables> simexchanger(60.0f);
    Simulation sim(simexchanger);

    while (!quit)
    {
        display.process();

        // Look for new level gen data
        {
            const auto statics = sim.get_statics();
            if (statics != NULL)
            {
                renderer.set_statics(*statics);
                sim.release_statics(statics);
            }
        }

        if (!keys.empty())
        {
            sim.queue_inputs(keys);
            keys.clear();
        }

        sim.set_mouse_input(mouse);

        Renderables *prev, *current;
        const float lerp = simexchanger.get_simstates(&prev, &current, display.refresh_rate());

        renderer.render(*current);

        display.swap();
    }

    return 0;
}
