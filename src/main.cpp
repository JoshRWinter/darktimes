#include <win.h>
#include <chrono>

#include "assetmanager.hpp"
#include "render/renderer.hpp"
#include "sim/simulation.hpp"

int main()
{
	win::Display display("Darktimes", 1600, 900, false);
	display.vsync(true);

	bool quit = false;
	std::atomic<bool> simulation_quit = false;
	display.register_button_handler([&quit, &simulation_quit](win::Button button, bool press)
	{
		if (button == win::Button::ESC)
			quit = true;
	});

	win::AssetRoll roll("darktimes.bin");
	AssetManager assetmanager(roll);
	Renderer renderer(display.width(), display.height(), -8.0, 8.0f, -4.5f, 4.5, assetmanager);

	LargeSyncObject<STR_LevelDataSyncObject> str_level_data_sync;
	std::thread simulation_thread(simulation, std::ref(simulation_quit), std::ref(str_level_data_sync));

	while(display.process() && !quit)
	{
		if (str_level_data_sync.dirty())
		{
			std::unique_ptr<STR_LevelDataSyncObject> leveldata;
			str_level_data_sync.get(leveldata);
			renderer.set_level_data(leveldata->floors, leveldata->walls);
		}

		renderer.computeframe();
		display.swap();
	}

	simulation_quit = true;
	simulation_thread.join();

	return 0;
}
