#include "guiAlexandrov/include.h"

#include <array>
#include "main_window.cpp"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE old, LPSTR lpCmdLine, int nShowCmd)
{
	al_init(hInstance);

	MainWindow window;

	Window::wait_msg_proc();
	return 0;
}