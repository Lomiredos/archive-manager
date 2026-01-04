#include "MainWindow.h"


#define BTN_CREATE_ID 1
#define BTN_OPEN_ID 2
#define BTN_ADD_FILE_ID 3
#define BTN_SAVE_ID 4
#define BTN_EXTRACT_FILE_ID 5
#define BTN_EXTRACT_ALL_FILE_ID 6
#define BTN_QUIT_ID 7

const wchar_t* CLASS_NAME = L"ArchiveManager";

MainWindow::MainWindow(HINSTANCE hInst) : hInstance(hInst)
{

}

MainWindow::~MainWindow()
{
	if (currentArchive != nullptr)
		delete currentArchive;


	if (hWnd != nullptr)
		DestroyWindow(hWnd);
}

bool MainWindow::Create()
{

	if (!RegisterWindowClass())
		return false;
	hWnd = CreateWindowEx(
		0,
		CLASS_NAME,
		L"Archive Manager",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		800, 600,
		nullptr,
		nullptr,
		hInstance,
		this
	);
	if (hWnd == nullptr)
		return false;
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	return true;
}

int MainWindow::Run()
{
	MSG msg = { 0 };

	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

bool MainWindow::RegisterWindowClass()
{


	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

	return RegisterClassEx(&wc) != 0;
}

void MainWindow::CreateControls()
{
	btnCreateArchive = CreateWindow(
		L"BUTTON",
		L"Créer Archive",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		50, 50,
		150, 30,
		hWnd,
		(HMENU)BTN_CREATE_ID,
		hInstance,
		nullptr
	);

	btnOpenArchive = CreateWindow(
		L"BUTTON",
		L"Ouvrire Archive",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		50, 100,
		150, 30,
		hWnd,
		(HMENU)BTN_OPEN_ID,
		hInstance,
		nullptr
	);

	btnAddFile = CreateWindow(
		L"BUTTON",
		L"Ajouter un fichier",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		50, 150,
		150, 30,
		hWnd,
		(HMENU)BTN_ADD_FILE_ID,
		hInstance,
		nullptr
	);

	btnSaveArchive = CreateWindow(
		L"BUTTON",
		L"Sauvegarder Archive",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		50, 200,
		150, 30,
		hWnd,
		(HMENU)BTN_SAVE_ID,
		hInstance,
		nullptr
	);

	btnExtractFile = CreateWindow(
		L"BUTTON",
		L"Extraire un fichier",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		50, 250,
		150, 30,
		hWnd,
		(HMENU)BTN_EXTRACT_FILE_ID,
		hInstance,
		nullptr
	);
	btnExtracteAllFile = CreateWindow(
		L"BUTTON",
		L"Extraire tous les fichiers",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		50, 300,
		150, 30,
		hWnd,
		(HMENU)BTN_EXTRACT_ALL_FILE_ID,
		hInstance,
		nullptr
	);


	btnQuit = CreateWindow(
		L"BUTTON",
		L"Quitter",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		50, 350,
		150, 30,
		hWnd,
		(HMENU)BTN_QUIT_ID,
		hInstance,
		nullptr
	);

	textOutput = CreateWindow(
		L"EDIT",
		L"",
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_READONLY,
		250, 50,
		500, 400,
		hWnd,
		nullptr,
		hInstance,
		nullptr
	);
}

void MainWindow::HandleButtonClick(int buttonId)
{

	switch (buttonId) {
	case BTN_CREATE_ID:
		OnCreateArchive();
		break;
	case BTN_OPEN_ID:
		OnOpenArchive();
		break;
	case BTN_ADD_FILE_ID:
		onFileAdd();
		break;
	case BTN_SAVE_ID:
		onSaveArchive();
		break;
	case BTN_EXTRACT_FILE_ID:
		onExtractFile();
		break;
	case BTN_EXTRACT_ALL_FILE_ID:
		onExtractAllFile();
		break;
	case BTN_QUIT_ID:
		DestroyWindow(hWnd);
		break;
	}
}

void MainWindow::OnCreateArchive()
{
	wchar_t fileName[MAX_PATH] = L"";
	OPENFILENAME ofn = { 0 };
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = L"Archive Files (*.arc)\0*.arc\0All Files (*.*)\0*.*\0";
	ofn.lpstrDefExt = L"arc";
	ofn.Flags = OFN_OVERWRITEPROMPT;

	if (GetSaveFileName(&ofn)) {
		char fileNameAnsi[MAX_PATH];
		WideCharToMultiByte(CP_ACP, 0, fileName, -1, fileNameAnsi, MAX_PATH, nullptr, nullptr);
		currentArchivePath = fileNameAnsi;

		if (currentArchive != nullptr)
			delete currentArchive;

		currentArchive = new Archive();
		if (currentArchive->create(currentArchivePath))
			SetWindowText(textOutput, L"Archive créée avec succès !");
		else
			SetWindowText(textOutput, L"Erreur lors de la création de l'Archive !");
	}



}

void MainWindow::OnOpenArchive()
{
	wchar_t fileName[MAX_PATH] = L"";
	OPENFILENAME ofn = { 0 };
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = L"Archive Files (*.arc)\0*.arc\0All Files (*.*)\0*.*\0";
	ofn.lpstrDefExt = L"arc";
	ofn.Flags = OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn)) {
		char fileNameAnsi[MAX_PATH];
		WideCharToMultiByte(CP_ACP, 0, fileName, -1, fileNameAnsi, MAX_PATH, nullptr, nullptr);

		if (currentArchive != nullptr)
			delete currentArchive;

		currentArchive = new Archive();


		currentArchivePath = fileNameAnsi;
		if (currentArchive->open(currentArchivePath)) {
			SetWindowText(textOutput, L"Archive ouverte avec succès !");
			UpdateFileList();
		}
		else {
			SetWindowText(textOutput, L"Erreur lors de l'ouverture de l'archive !");
			delete currentArchive;
			currentArchive = nullptr;
		}
	}
}

void MainWindow::onFileAdd()
{
	wchar_t fileName[MAX_PATH] = L"";
	OPENFILENAME ofn = { 0 };
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn)) {
		
		if (currentArchive == nullptr) {
			SetWindowText(textOutput, L"Ouvrez une archive avant d'ajouter un fichier !");
			return;
		}
		char fileNameAnsi[MAX_PATH];
		WideCharToMultiByte(CP_ACP, 0, fileName, -1, fileNameAnsi, MAX_PATH, nullptr, nullptr);

		
		if (currentArchive->addFile(fileNameAnsi))
			UpdateFileList();
		else
			SetWindowText(textOutput, L"Erreur lors de l'ajout du fichier !");

	}

}

void MainWindow::onSaveArchive() {
	if (currentArchive == nullptr) {
		SetWindowText(textOutput, L"Impossible de sauvegarder une archive inexistante !");
		return;
	}

	// DEBUG : Afficher des infos
	std::wstring debugMsg = L"DEBUG:\r\n";

	// Chemin de l'archive
	wchar_t pathW[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, currentArchivePath.c_str(), -1, pathW, MAX_PATH);
	debugMsg += L"Chemin: ";
	debugMsg += pathW;
	debugMsg += L"\r\n";

	// Nombre de fichiers
	debugMsg += L"Fichiers à sauvegarder: ";
	debugMsg += std::to_wstring(currentArchive->listFiles().size());
	debugMsg += L"\r\n\r\n";

	SetWindowText(textOutput, debugMsg.c_str());

	if (currentArchive->save())
		SetWindowText(textOutput, L"Archive sauvegardée avec succès !");
	else
		SetWindowText(textOutput, L"Erreur lors de la sauvegarde de l'archive !");
	
}

void MainWindow::onExtractFile()
{
	if (currentArchive == nullptr) {
		SetWindowText(textOutput, L"Merci d'ouvrire une archive avant d'extraire un fichier !");
		return;
	}
	//TODO POP UP
	//choisir un fichier
}

void MainWindow::onExtractAllFile()
{
	if (currentArchive == nullptr) {
		SetWindowText(textOutput, L"Merci d'ouvrire une archive avant d'extraire des fichiers !");
		return;
	}
	if (currentArchive->extractAll("outPut")) {
		SetWindowText(textOutput, L"Fichiers extrait avec succès vers le dossier : outPut/");
	}
	else {
		SetWindowText(textOutput, L"Erreur lors de l'extraction des fichiers !");

	}

}

void MainWindow::UpdateFileList()
{

	if (currentArchive == nullptr)
		return;
	const std::vector<FileEntry>& files = currentArchive->listFiles();
	
	std::wstring text = L"Fichiers dans l'archive :\r\n\r\n";
	for (int i = 0; i < files.size(); i++) {
		const FileEntry& file = files[i];


		wchar_t fileNameW[256];
		MultiByteToWideChar(CP_ACP, 0, file.getFileName().c_str(), -1, fileNameW, 256);

		text += L"[" + std::to_wstring(i) + L"] " + fileNameW;
		text += L" (" + std::to_wstring(file.getFileSize()) + L" octets)\r\n";

	}
	if (files.size() == 0) {
		text = L" Cette archive est vide !\r\n ajouter des fichier !";
	}

	SetWindowText(textOutput, text.c_str());

}

LRESULT MainWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	MainWindow* pThis = nullptr;

	if (msg == WM_CREATE) {

		CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		pThis = reinterpret_cast<MainWindow*>(pCreate->lpCreateParams);
		pThis->hWnd = hwnd;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
	}
	else {
		pThis = reinterpret_cast<MainWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}


	if (pThis) {
		switch (msg) {
		case WM_CREATE:
			pThis->CreateControls();
			return 0;
		case WM_COMMAND:
			pThis->HandleButtonClick(LOWORD(wParam));
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_PAINT:
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			(void)hdc;
			EndPaint(hwnd, &ps);
			return 0;
		}
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);

}
