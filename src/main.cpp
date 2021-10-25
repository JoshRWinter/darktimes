#include <win.h>
#include <chrono>

#include "Renderer.hpp"
#include "WorldSimulation.hpp"

int main()
{
	win::Display display("Darktimes", 1600, 900, false);
	display.vsync(true);

	bool quit = false;
	std::atomic<bool> simulation_quit = false;
	display.register_button_handler([&quit, &simulation_quit](win::Button button, bool press)
	{
		if (button == win::Button::ESC)
		{
			simulation_quit = true;
			quit = true;
		}
	});

	win::AssetRoll roll("assets");
	Renderer renderer(display.width(), display.height(), -8.0, 8.0f, -4.5f, 4.5, roll);

	SyncObject<SimulationToRendererLevelDataSyncObject> syncobject_str_level_data;

	std::thread simulation_thread(simulation, std::ref(simulation_quit), std::ref(syncobject_str_level_data));

	while(display.process() && !quit)
	{
		renderer.computeframe();
		display.swap();
	}

	simulation_thread.join();

	return 0;
}
