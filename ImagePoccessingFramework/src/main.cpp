
#include "ui/ui.h"
#include "core.h"
#include "renderer.h"

int main(int arc, char** argv)
{
	Bubble::Window window;
	Bubble::UI ui(&window);
	Bubble::Input::SetWindow(&window);
	Bubble::Timer timer;

	while (window.IsOpen())
	{
		SDL_Event event;
		while (window.PollEvent(event))
		{
			window.OnEvent(event);
		}
		ui.Draw(timer.GetDeltaTime());

		timer.Update();
		window.OnUpdate();
	}

	return 0;
}