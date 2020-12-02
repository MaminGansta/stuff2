
#include "core/core.h"
#include "ui/ui.h"

int main(int argc, char* args[])
{
	Bubble::Window window;
	Bubble::Input::SetWindow(&window);
	Bubble::UI ui(&window);
	Bubble::Timer timer;

	while (window.IsOpen())
	{
		SDL_Event event;
		while (window.PollEvent(event))
		{
			ui.mImGuiControll.OnEvent(event);
			window.OnEvent(event);
		}
		ui.Draw(timer.GetDeltaTime());

		timer.Update();
		window.OnUpdate();
	}

	return 0;
}