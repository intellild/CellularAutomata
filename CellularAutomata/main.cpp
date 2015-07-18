#include "GacLib/GacUIWindows.h"
#include "gui.h"
#include <Windows.h>

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int CmdShow)
{
	// SetupWindowsDirect2DRenderer() is required for GuiDirect2DElement
	return SetupWindowsDirect2DRenderer();
}

void GuiMain()
{
	CAWindow *window;
	window = new CAWindow;
	GetApplication()->Run(window);
	delete window;
}
