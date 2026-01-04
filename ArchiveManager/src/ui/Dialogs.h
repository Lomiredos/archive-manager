#ifndef DIALOGS_H
#define DIALOGS_H

#include <Windows.h>
#include <string>
#include <vector>
#include "../core/FileEntry.h"


namespace Dialogs {

	int showFileSelectionDialog(HWND parent, const std::vector<FileEntry>& files);

	INT_PTR CALLBACK FileSelectionDialogProc(HWND hWndDlg, UINT message, WPARAM wParam, LPARAM lParam);

};

#endif