﻿
// Kamac.cpp: 定义应用程序的类行为。
//

#include "pch.h"
#include "framework.h"
#include "Kamac.h"
#include "CKamacCmdLineInfo.h"

//#include "KamacDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKamacApp

BEGIN_MESSAGE_MAP(CKamacApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CKamacApp 构造

CKamacApp::CKamacApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的 CKamacApp 对象

CKamacApp theApp;


// CKamacApp 初始化

BOOL CKamacApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	CKamacCmdLineInfo cli;
	ParseCommandLine(cli);
	

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	//SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	ppsSheet = new CKamacSheet(_T("Kamac"));
	ppsSheet->bSilent = cli.bSilent;

	ppsSheet->Create(nullptr, WS_SYSMENU | WS_POPUPWINDOW | WS_CAPTION | WS_VISIBLE | WS_MINIMIZEBOX, 
		0);

	RAWINPUTDEVICE rid[2];
	rid[0].usUsagePage = 1;
	rid[0].usUsage = 2;
	rid[0].dwFlags = RIDEV_INPUTSINK;
	rid[0].hwndTarget = *ppsSheet;

	rid[1].usUsagePage = 1;
	rid[1].usUsage = 6;
	rid[1].dwFlags = RIDEV_INPUTSINK;
	rid[1].hwndTarget = *ppsSheet;


	m_pMainWnd = ppsSheet;
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	
	::RegisterRawInputDevices(rid, 2, sizeof(rid[0]));

	//CKamacDlg dlg;
	//m_pMainWnd = &dlg;
	//INT_PTR nResponse = dlg.DoModal();
	//if (nResponse == IDOK)
	//{
	//	// TODO: 在此放置处理何时用
	//	//  “确定”来关闭对话框的代码
	//}
	//else if (nResponse == IDCANCEL)
	//{
	//	// TODO: 在此放置处理何时用
	//	//  “取消”来关闭对话框的代码
	//}
	//else if (nResponse == -1)
	//{
	//	TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
	//}


#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return TRUE;
}



BOOL CKamacApp::OnIdle(LONG lCount)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CWinApp::OnIdle(lCount);
}