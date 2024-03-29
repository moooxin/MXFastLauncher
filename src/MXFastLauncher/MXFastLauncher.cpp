//MXFastLauncher.cpp: 定义应用程序的入口点。
//

#include "stdafx.h"
#include "MXFastLauncher.h"
#include <shellapi.h>
#include <shlobj.h>

BOOL TaskbarPin(LPTSTR lpPath, LPTSTR lpFileName, BOOL bIsPin = FALSE)
{
    BOOL bRet = FALSE;
    HMENU hmenu = NULL;
    LPSHELLFOLDER pdf = NULL;
    LPSHELLFOLDER psf = NULL;
    LPITEMIDLIST pidl = NULL;
    LPITEMIDLIST pitm = NULL;
    LPCONTEXTMENU pcm = NULL;

    if (SUCCEEDED(SHGetDesktopFolder(&pdf))
        && SUCCEEDED(pdf->ParseDisplayName(NULL, NULL, lpPath, NULL, &pidl, NULL))
        && SUCCEEDED(pdf->BindToObject(pidl, NULL, IID_IShellFolder, (void **)&psf))
        && SUCCEEDED(psf->ParseDisplayName(NULL, NULL, lpFileName, NULL, &pitm, NULL))
        && SUCCEEDED(psf->GetUIObjectOf(NULL, 1, (LPCITEMIDLIST *)&pitm, IID_IContextMenu, NULL, (void **)&pcm))
        && (hmenu = CreatePopupMenu()) != NULL
        && SUCCEEDED(pcm->QueryContextMenu(hmenu, 0, 1, INT_MAX, CMF_NORMAL)))
    {
        CMINVOKECOMMANDINFO ici = { sizeof(CMINVOKECOMMANDINFO), 0 };
        ici.hwnd = NULL;
        ici.lpVerb = bIsPin ? "taskbarpin" : "taskbarunpin";
        pcm->InvokeCommand(&ici);
        bRet = TRUE;
    }

    if (hmenu)
        DestroyMenu(hmenu);
    if (pcm)
        pcm->Release();
    if (pitm)
        CoTaskMemFree(pitm);
    if (psf)
        psf->Release();
    if (pidl)
        CoTaskMemFree(pidl);
    if (pdf)
        pdf->Release();

    return bRet;
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此放置代码。

    //启动程序
    TCHAR tszModule[MAX_PATH + 1] = { 0 };
    ::GetModuleFileName(NULL, tszModule, MAX_PATH);
    TString sInstancePath = tszModule;

    TString dir, file;
    int pos = sInstancePath.rfind(_T('\\'));
    if (pos < 0)
        return 0;

    dir = sInstancePath.substr(0, pos);
    file = sInstancePath.substr(pos + 1);

    //win7/8
    //::ShellExecute(NULL, L"TaskbarPin", tszModule, NULL, NULL, SW_SHOW);     //锁定到任务栏

    //win10 暂无方法，只能引导了
    //MessageBox(NULL, L"任务栏右键，固定到任务栏", L"", MB_OK);

    //win10 无效果
    //CoInitialize(nullptr);
    //TaskbarPin(_T("F:\\MXFastLauncher\\Bin\\Debug"), _T("111.lnk"), true);
    //TaskbarPin((TCHAR*)dir.c_str(), (TCHAR*)file.c_str(), true);
    //CoUninitialize();

    // 初始化全局字符串
    //LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    HANDLE hEvent = ::CreateEvent(NULL, FALSE, FALSE, _T("MX.Launcher.Run.Event"));
    DWORD last = GetLastError();

    if (hEvent)
    {
        if (last == 0)
        {
            //创建成功，重新关闭
            CloseHandle(hEvent);

            //启动程序
            TCHAR tszModule[MAX_PATH + 1] = { 0 };
            ::GetModuleFileName(NULL, tszModule, MAX_PATH);
            TString sInstancePath = tszModule;
            int pos = sInstancePath.rfind(_T('\\'));
            if (pos >= 0)
                sInstancePath = sInstancePath.substr(0, pos);

            sInstancePath += _T("\\MXLauncher.exe");
            ::ShellExecute(NULL, _T("open"), sInstancePath.c_str(), NULL, NULL, SW_SHOW);
        }
        else
        {
            //创建失败，已经存在MXLauncher.exe 进程
            ::SetEvent(hEvent);
        }
    }

    return 0;
}

