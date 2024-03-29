//MXLauncher.cpp: 定义应用程序的入口点。
//

#include "stdafx.h"
#include <thread>
#include "MXLauncher.h"
#include "FLTaskBarIcon.h"
#include "FLMainToast.h"


#pragma comment(lib,"Everything32.lib")

#ifdef _DEBUG
#pragma comment(lib,"DuiLib_d.lib")

#else							 
#pragma comment(lib,"DuiLib.lib")

#endif


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此放置代码。
    HANDLE hEvent = ::CreateEvent(NULL, TRUE, FALSE, _T("MX.Launcher.Run.Event"));

    if (!hEvent)
    {
        MessageBox(NULL, _T("环境事件创建失败"), _T("环境事件创建失败"), MB_OK);
        return 0;
    }

    DWORD lsEr = GetLastError();

    if (lsEr != 0)
    {
        //已经存在的，不是创建的
        ::SetEvent(hEvent);
        return 0;
    }
    
    // 初始化全局字符串
    //LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    mxtoolkit::MXDuiWnd::s_smalIcon = LoadIcon(CPaintManagerUI::GetInstance(), MAKEINTRESOURCE(IDI_SMALL));
    mxtoolkit::MXDuiWnd::s_bigIcon = LoadIcon(CPaintManagerUI::GetInstance(), MAKEINTRESOURCE(IDC_MXLAUNCHER));

    //初始化Duilib
    DuiLib::CPaintManagerUI::SetInstance(hInstance);
    DuiLib::CPaintManagerUI::SetResourceDll(hInstance);    
    DuiLib::CPaintManagerUI::SetResourcePath(DuiLib::CPaintManagerUI::GetInstancePath() + _T("\\Skin\\"));

    FLMainToast mainToast;
    if (mainToast.Init(GetDesktopWindow()) == 0)
        return 0;

    auto trd = std::thread([&]()
    {
        while (hEvent)
        {
            WaitForSingleObject(hEvent, INFINITE);

            if(!hEvent)
                break;

            ResetEvent(hEvent);

            mainToast.Show(true);
        }
    });

    FLTaskBarIcon taskBarIcon;
    taskBarIcon.Init(mainToast.GetHWND());
    taskBarIcon.Show(true);

    mainToast.CenterWindow();
    //mainToast.Show(true);
    mainToast.ModalWindow();
    
    HANDLE ev = hEvent;
    hEvent = NULL;
    ::SetEvent(ev);
    CloseHandle(ev);

//     HWND breakWnd = (HWND)1;
//     // 主消息循环: 
//     MSG msg;
//     while (GetMessage(&msg, nullptr, 0, 0))
//     {
//         if (msg.message == WM_QUIT)
//             break;
// 
//         TranslateMessage(&msg);
//         DispatchMessage(&msg);
//     }

    if (trd.joinable())
        trd.join();

    return 0;
}

