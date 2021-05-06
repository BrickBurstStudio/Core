#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <windows.h>

#define HOOK_DLL_NAME                         "VRDx.dll"
#define HOOK_PROC_NAME                        "_DebugHookProc@12"

void LogOutput(PSTR pszParamA, PSTR pszParamB)
{
    FILE* fp;

    if ((fp = fopen("log.txt", "a+")) != NULL)
    {
        fprintf(fp, "%s %s\n", pszParamA, pszParamB);

        fclose(fp);
    }
}

int DllInjectEx(DWORD dwThreadId, PCSTR pcszFileName)
{
    int nResult;
    HINSTANCE hInstLib;
    LPVOID lpvfnHook;
    HHOOK hhookCapture;

    if ((hInstLib = LoadLibrary(pcszFileName)) != NULL)
    {
        if ((lpvfnHook = GetProcAddress(hInstLib, HOOK_PROC_NAME)) != NULL)
        {
            if ((hhookCapture = SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC)lpvfnHook, hInstLib, dwThreadId)) != NULL)
            {
                nResult = 0;

                for (int i = 0; i < 10; i++)
                {
                    Sleep(500);
                    if (PostThreadMessage(dwThreadId, WM_USER + 510, 0, (LPARAM)hhookCapture) == FALSE)
                    {
                    }
                }
            }
            else nResult = -4;
        }
        else nResult = -3;

        FreeLibrary(hInstLib);
    }
    else nResult = -2;

    return nResult;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nShowCmd)
{
    int nResult;
    PSTR pCommandLine = GetCommandLine();
    DWORD dwThreadId;
    UINT uLength;
    PSTR pszFileName;

    uLength = GetCurrentDirectory(0, NULL) + 1 + (DWORD)strlen(HOOK_DLL_NAME) + 1;
    pszFileName = new CHAR[uLength];
    uLength = GetCurrentDirectory(uLength, pszFileName);
    pszFileName[uLength++] = '\\';
    strcpy(pszFileName + uLength, HOOK_DLL_NAME);

    dwThreadId = atoi(pCommandLine);
    nResult = DllInjectEx(dwThreadId, pszFileName);

    delete[] pszFileName;

    return nResult;
}