#include "stdafx.h"
#include "window/Window.h"
//Test
void GameUpdate() {
	g_graphicsEngine->BegineRender();

	g_graphicsEngine->EndRender();
}

INT WINAPI wWinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPWSTR lpCmdLine,
	int nCmdShow) {
	InitWindow(hInstance, hPrevInstance, lpCmdLine, nCmdShow, "Game");
	while (DispatchWindowMessage()) {
		GameUpdate();
	}
}
