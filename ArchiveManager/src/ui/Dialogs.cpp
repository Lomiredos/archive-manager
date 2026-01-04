#include "Dialogs.h"
#include <CommCtrl.h>


struct FileSelectionData {
    const std::vector<FileEntry>* files;
    int selectedIndex;
};


namespace Dialogs {

    INT_PTR CALLBACK FileSelectionDialogProc(HWND hWndDlg, UINT message, WPARAM wParam, LPARAM lParam) {

        static FileSelectionData* pData = nullptr;
        static HWND hListBox = nullptr;

        switch (message) {
        case WM_INITDIALOG:
            pData = (FileSelectionData*)lParam;

            hListBox = CreateWindow(
                L"LISTBOX",
                L"",
                WS_CHILD | WS_VISIBLE | WS_BORDER,
                12, 12, 360, 200,
                hWndDlg,
                (HMENU)1001,
                GetModuleHandle(nullptr),
                nullptr
            );


            for (size_t i = 0; i < pData->files->size(); i++) {
                const FileEntry& file = (*pData->files)[i];

                wchar_t fileNameW[256];
                MultiByteToWideChar(CP_ACP, 0, file.getFileName().c_str(), -1, fileNameW, 256);
                std::wstring itemText = L"[" + std::to_wstring(i) + L"] " + fileNameW;
                itemText += L"(" + std::to_wstring(file.getFileCompressedSize()) + L" octets)";

                SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)itemText.c_str());
            }

            CreateWindow(
                L"BUTTON",
                L"Extraire",
                WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                100, 220, 80, 25,
                hWndDlg,
                (HMENU)IDOK,
                GetModuleHandle(nullptr),
                nullptr
            );

            CreateWindow(
                L"BUTTON",
                L"Annuler",
                WS_CHILD | WS_VISIBLE,
                200, 220, 80, 25,
                hWndDlg,
                (HMENU)IDCANCEL,
                GetModuleHandle(nullptr),
                nullptr
            );

            return TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK) {
                int selectedIndex = (int)SendMessage(hListBox, LB_GETCURSEL, 0, 0);

                if (selectedIndex == LB_ERR) {
                    MessageBox(hWndDlg, L"Veuillez sélectionner un fichier !", L"Erreur", MB_OK | MB_ICONWARNING);
                    return TRUE;
                }
                pData->selectedIndex = selectedIndex;
                EndDialog(hWndDlg, IDOK);
                return TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL) {
                pData->selectedIndex = -1;
                EndDialog(hWndDlg, IDCANCEL);
                return TRUE;
            }

            if (LOWORD(wParam) == 1001 && HIWORD(wParam) == LBN_DBLCLK) {
                SendMessage(hWndDlg, WM_COMMAND, IDOK, 0);
                return TRUE;
            }
            break;

        }
        return FALSE;



    }

    int ShowFileSelectionDialog(HWND parent, const std::vector<FileEntry>& files) {
        FileSelectionData data;
        data.files = &files;
        data.selectedIndex = -1;

        HWND hwndDlg = CreateWindowEx(
            WS_EX_DLGMODALFRAME | WS_EX_TOPMOST,
            L"#32770", //Classe Dialog system
            L"Sélectionner un fichier a extraire",
            WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
            CW_USEDEFAULT, CW_USEDEFAULT, 400, 300,
            parent,
            nullptr,
            GetModuleHandle(nullptr),
            nullptr
        );

        if (hwndDlg == nullptr)
            return -1;

        SendMessage(hwndDlg, WM_INITDIALOG, 0, (LPARAM)&data);

        MSG msg;
        bool dialogActive = true;



        while (dialogActive && GetMessage(&msg, nullptr, 0, 0)) {
            if (msg.hwnd == hwndDlg || IsChild(hwndDlg, msg.hwnd)) {
                if (msg.message == WM_COMMAND) {
                    FileSelectionDialogProc(hwndDlg, msg.message, msg.wParam, msg.lParam);

                    if (LOWORD(msg.wParam) == IDOK || LOWORD(msg.wParam) == IDCANCEL) {
                        DestroyWindow(hwndDlg);
                        dialogActive = false;
                    }
                    else if (!IsDialogMessage(hwndDlg, &msg)) {
                        TranslateMessage(&msg);
                        DispatchMessage(&msg);
                    }

                }
                else {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);

                }

                if (!IsWindow(hwndDlg)) {
                    dialogActive = false;
                }
            }
            return data.selectedIndex;

        }
    
    }



}


