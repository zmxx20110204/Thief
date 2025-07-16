#include <bits/stdc++.h>
#include <windows.h>
#include <cstring>
HWND hwnd;
int main()
{
	system("taskkill /f /t /im Thief.exe");
	char* username;
    char destinationPath[MAX_PATH];
	username = getenv("USERPROFILE");
	snprintf(destinationPath, sizeof(destinationPath), "%s\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\Thief.exe", username);
	if (remove(destinationPath) == 0)
	{
		MessageBox(hwnd, TEXT("完成"), TEXT("删除完成"), MB_OK);
	}
	else
	{
		MessageBox(hwnd,TEXT("失败"),TEXT("没有找到程序"),MB_OK);
	}
}
