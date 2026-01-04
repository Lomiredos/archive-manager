#include "src/ui/MainWindow.h"
#include "Windows.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	MainWindow window(hInstance);
	if (!window.Create()) {
		MessageBox(nullptr, L"Erreur lors de la création de la fenêtre !", L"Erreur", MB_ICONERROR);
		return 1;
	}


	return window.Run();
}