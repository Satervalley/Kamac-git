
// Kamac.cpp: 定义应用程序的类行为。
//

#include "pch.h"
#include "framework.h"
#include "Kamac.h"
#include "CKamacCmdLineInfo.h"
#include "CUtil.h"


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

}


CKamacApp::~CKamacApp()
{
	if (atomInstance)
		::GlobalDeleteAtom(atomInstance);
}
// 唯一的 CKamacApp 对象

CKamacApp theApp;


// CKamacApp 初始化

BOOL CKamacApp::InitInstance()
{
	if (CheckExist())	// already running
	{
		::AfxMessageBox(_T("Kamac already running!"), MB_OK | MB_ICONINFORMATION);
		return FALSE;
	}

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

	m_pMainWnd = ppsSheet;
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	
#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	return TRUE;
}



BOOL CKamacApp::OnIdle(LONG lCount)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CWinApp::OnIdle(lCount);
}


BOOL CKamacApp::CheckExist(void)
{
	CString str, strHash;
	::GetModuleFileName(theApp.m_hInstance, str.GetBufferSetLength(1025), 1025);
	str.ReleaseBuffer();
/*
	str.MakeLower();
	CUtil::ComputeSHA1(str, strHash);
	BOOL b = (::GlobalFindAtom(strHash) != 0);
	if (!b)
	{
		atomInstance = ::GlobalAddAtom(strHash);
	}
	return b;
*/
	return CUtil::FindRunningProcess(str);
}

