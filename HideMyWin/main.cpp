#include <Windows.h>
#define APP_NAME L"Hide my win"

const int SET_EXE_FILE = 1;
const int SET_EXE_ATR = 2;

void WrongArgumentsMessage() {
	MessageBox(NULL, L"Wrong Arguments:\n\t/file 'execute file'\n\t/attribute 'execute attribute'\n\t/minimize - run minimized", APP_NAME, MB_OK);
}

int __stdcall main() {
	PWSTR lpCmdLine = GetCommandLine();
	
	DWORD exitCode = 0;
	STARTUPINFO strInfo;
	PROCESS_INFORMATION prcInfo;

	WORD cmdShow = SW_HIDE;
	DWORD creationFlags = NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW;
	int numArgs;
	LPWSTR *args;
	LPWSTR exeFile = NULL;
	LPWSTR exeAtr = NULL;
	
	args = CommandLineToArgvW(lpCmdLine, &numArgs);
	
	if (numArgs < 2) { // if programm run without parameters
		WrongArgumentsMessage();
		ExitProcess(-1);
	}
	
	int i = 1;
	int setFlag = 0;
	for (; i < numArgs; i++) {
		if (setFlag) {
			switch (setFlag)
			{
			case SET_EXE_FILE:
				exeFile = args[i];
				break;
			case SET_EXE_ATR:
				exeAtr = args[i];
				break;
			}
			setFlag = 0;
			continue;
		}


		if (lstrcmpi(args[i], L"/minimize") == 0) {
			cmdShow = SW_MINIMIZE;
			creationFlags &= ~CREATE_NO_WINDOW;
			continue;
		}

		if (lstrcmpi(args[i], L"/file") == 0) {
			setFlag = SET_EXE_FILE;
			continue;
		}

		if (lstrcmpi(args[i], L"/attribute") == 0) {
			setFlag = SET_EXE_ATR;
			continue;
		}

		break;
	}

	i--;
	while (++i < numArgs)
	{
		if (!exeFile) {
			exeFile = args[i];
			continue;
		}

		if (!exeAtr) {
			exeAtr = args[i];
			break;
		}
	}

	WCHAR exePath[MAX_PATH];
	FindExecutable(exeFile, NULL, exePath);

	GetStartupInfo(&strInfo);
	strInfo.dwFlags |= STARTF_USESHOWWINDOW;
	strInfo.wShowWindow = cmdShow;

	int res = CreateProcess(
		exePath, exeAtr,
		NULL, NULL,
		FALSE,
		creationFlags,
		NULL, NULL,
		&strInfo, &prcInfo);

	WaitForSingleObject(prcInfo.hProcess, INFINITE);

	GetExitCodeProcess(prcInfo.hProcess, &exitCode);
	CloseHandle(prcInfo.hProcess);
	
	ExitProcess(exitCode);
}

#ifdef _DEBUG
int WINAPI wWinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	PWSTR     lpCmdLine,
	int       nShowCmd
) {
	return main();
}
#endif // DEBUG