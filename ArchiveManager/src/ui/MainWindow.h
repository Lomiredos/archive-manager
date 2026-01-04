#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <Windows.h>
#include <string>
#include "../core/Archive.h"
#include "Dialogs.h"

class MainWindow {

private:
	HWND hWnd = nullptr;
	HINSTANCE hInstance;

	HWND btnCreateArchive = nullptr;
	HWND btnOpenArchive = nullptr;
	HWND btnAddFile = nullptr;
	HWND btnSaveArchive = nullptr;
	HWND btnQuit = nullptr;
	HWND btnExtractFile = nullptr;
	HWND btnExtracteAllFile = nullptr;
	HWND textOutput = nullptr;

	Archive* currentArchive = nullptr;
	std::string currentArchivePath;




public:


	MainWindow(HINSTANCE hInst);
	~MainWindow();


	bool Create();
	int Run();

private:

	bool RegisterWindowClass();
	void CreateControls();
	void HandleButtonClick(int buttonId);

	void OnCreateArchive();
	void OnOpenArchive();
	void onFileAdd();
	void onSaveArchive();
	void onExtractFile();
	void onExtractAllFile();


	void UpdateFileList();




	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
#endif