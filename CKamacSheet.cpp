﻿// CKamacSheet.cpp: 实现文件
//

#include "pch.h"
#include "Kamac.h"
#include "CKamacSheet.h"



UINT WM_TRAYICONNOTIFY = WM_USER + 1;
// CKamacSheet

IMPLEMENT_DYNAMIC(CKamacSheet, CPropertySheet)


//----------------------------------------------------------------------------------------------------------------------
CKamacSheet::CKamacSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	AddPages();
}


//----------------------------------------------------------------------------------------------------------------------
CKamacSheet::CKamacSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPages();
}


//----------------------------------------------------------------------------------------------------------------------
void CKamacSheet::AddPages(void)
{
	AddPage(ppMain);
	AddPage(ppOptions);
	AddPage(ppAbout);
}


//----------------------------------------------------------------------------------------------------------------------
CKamacSheet::~CKamacSheet()
{
	delete ppMain;
	delete ppOptions;
	delete ppAbout;
}


BEGIN_MESSAGE_MAP(CKamacSheet, CPropertySheet)
	ON_WM_INPUT()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_MESSAGE(WM_TRAYICONNOTIFY, OnTrayIconNotify)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CKamacSheet 消息处理程序


//----------------------------------------------------------------------------------------------------------------------
void CKamacSheet::OnRawInput(UINT nInputcode, HRAWINPUT hRawInput)
{
	// 该功能要求使用 Windows XP 或更高版本。
	// 符号 _WIN32_WINNT 必须 >= 0x0501。
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	static RAWINPUT ri;
	UINT ui = sizeof(RAWINPUT);

	UINT uiRes = ::GetRawInputData(hRawInput, RID_INPUT, &ri, &ui, sizeof(RAWINPUTHEADER));
	if (uiRes > 0 && uiRes != UINT(-1))
	{
		if (ri.header.dwType == RIM_TYPEKEYBOARD)
		{
			ProcessKeyboardInput(ri.data.keyboard);
		}
		if (ri.header.dwType == RIM_TYPEMOUSE)
		{
			ProcessMouseInput(ri.data.mouse);
		}
	}

	CPropertySheet::OnRawInput(nInputcode, hRawInput);
}

void CKamacSheet::OnClose()
{
	SaveConfig();
	EndDialog(IDOK);
}


//----------------------------------------------------------------------------------------------------------------------
void CKamacSheet::ProcessKeyboardInput(RAWKEYBOARD& rk)
{
	//if (rk.Flags == RI_KEY_MAKE || rk.Flags == RI_KEY_E0 || rk.Flags == RI_KEY_E1)
	if((rk.Flags & 0X0001))
	{
		kmdSession.IncKeyStrokes();
		kmdToday.IncKeyStrokes();
		kmdTotal.IncKeyStrokes();

		ppMain->UpdateKeyboard(kmdSession.ulKeyStrokes, kmdToday.ulKeyStrokes, kmdTotal.ulKeyStrokes, rk.MakeCode);

		ppMain->UpdateInfo();
	}
}


//----------------------------------------------------------------------------------------------------------------------
void CKamacSheet::ProcessMouseInput(RAWMOUSE& rm)
{
	if (rm.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
	{
		kmdSession.IncLeftClick();
		kmdToday.IncLeftClick();
		kmdTotal.IncLeftClick();
		ppMain->UpdateMouseLeftClick(kmdSession.ulLeftClick, kmdToday.ulLeftClick, kmdTotal.ulLeftClick);
	}
	if (rm.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
	{
		kmdSession.IncRightClick();
		kmdToday.IncRightClick();
		kmdTotal.IncRightClick();
		ppMain->UpdateMouseRightClick(kmdSession.ulRightClick, kmdToday.ulRightClick, kmdTotal.ulRightClick);
	}
	if (rm.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
	{
		kmdSession.IncMiddleClick();
		kmdToday.IncMiddleClick();
		kmdTotal.IncMiddleClick();
		ppMain->UpdateMouseMiddleClick(kmdSession.ulMiddleClick, kmdToday.ulMiddleClick, kmdTotal.ulMiddleClick);
	}
	USHORT f = 0x0001;
	BOOL bAbs = rm.usFlags & f;
	ULONG32 ulmd = CaculateMoveDistance(rm.lLastX, rm.lLastY, bAbs);
	kmdSession.IncDistance(ulmd);
	kmdToday.IncDistance(ulmd);
	kmdTotal.IncDistance(ulmd);
	ppMain->UpdateMouseDistance(kmdSession.ullDistance, kmdToday.ullDistance, kmdTotal.ullDistance);

	ppMain->UpdateMousePos();
	ppMain->UpdateInfo();
}


//----------------------------------------------------------------------------------------------------------------------
ULONG32 CKamacSheet::CaculateMoveDistance(LONG xc, LONG yc, BOOL bAbs)
{
	int nsw = ::GetSystemMetrics(SM_CXSCREEN);
	int nsh = ::GetSystemMetrics(SM_CYSCREEN);

	int sxc, syc;
	double d1, d2;
	d1 = double(koOptions.ulMonitorSize * koOptions.ulMonitorSize);
	d2 = 1.0 + double(nsw * nsw) / double(nsh * nsh);
	d1 = d1 / d2;
	d1 = sqrt(d1);
	d2 = d1 * double(nsw) / double(nsh);
	syc = int(d1 + 0.5);
	sxc = int(d2 + 0.5);
	if (bAbs)
	{
		d1 = xc - lLastX;
		lLastX = xc;
		d2 = yc - lLastY;
		lLastY = yc;
	}
	else
	{
		lLastX += xc;
		lLastY += yc;
		d1 = xc;
		d2 = yc;
	}
	d1 = sxc * d1 / double(nsw);
	d2 = syc * d2 / double(nsh);
	return ULONG32(sqrt(d1 * d1 + d2 * d2) + 0.5);
}


//----------------------------------------------------------------------------------------------------------------------
BOOL CKamacSheet::OnInitDialog()
{
	BOOL bResult = CPropertySheet::OnInitDialog();

	// TODO:  在此添加您的专用代码
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	tniTray.Create(this, 1, _T("Kamac"), m_hIcon, WM_TRAYICONNOTIFY);
	tniTray.HideIcon();

	GetIniFileName();
	ppOptions->strExeFileName = strExeFileName;
	LoadConfig();
	if (!CheckToday())
		ppMain->UpdateAll(kmdSession, kmdToday, kmdTotal);

	CPoint pt;
	::GetCursorPos(&pt);
	lLastX = pt.x;
	lLastY = pt.y;

	RAWINPUTDEVICE rid[2];
	rid[0].usUsagePage = 1;
	rid[0].usUsage = 2;
	rid[0].dwFlags = RIDEV_INPUTSINK;
	rid[0].hwndTarget = *this;

	rid[1].usUsagePage = 1;
	rid[1].usUsage = 6;
	rid[1].dwFlags = RIDEV_INPUTSINK;
	rid[1].hwndTarget = *this;
	::RegisterRawInputDevices(rid, 2, sizeof(rid[0]));


	uTimerID = SetTimer(uTimerID, 1000, nullptr);

	return bResult;
}


//----------------------------------------------------------------------------------------------------------------------
void CKamacSheet::GetIniFileName(void)
{
	CWinApp* pApp = ::AfxGetApp();
	CString str;
	::GetModuleFileName(pApp->m_hInstance, str.GetBufferSetLength(1025), 1025);
	str.ReleaseBuffer();
	strExeFileName = str;
	int nPos = str.ReverseFind(L'.');
	if (nPos >= 0)
	{
		str = str.Left(nPos + 1);
	}
	strIniFileName = str + _T("ini");
}


//----------------------------------------------------------------------------------------------------------------------
void CKamacSheet::LoadConfig(void)
{
	if (siConfig.LoadFile(strIniFileName) == SI_OK)
	{
		GetKMData(kmdToday, _T("Today"));
		GetKMData(kmdTotal, _T("Total"));
		GetOptions(koOptions);
	}
	ullToday = siConfig.GetULongLongValue(_T("Main"), _T("Today"));
	if (ullToday == 0)
	{
		::GetLocalTime(&stToday);
		SystemtimeToULL(stToday, ullToday);
	}
	else
	{
		ULLToSystemtime(ullToday, stToday);
	}
}


//----------------------------------------------------------------------------------------------------------------------
void CKamacSheet::SaveConfig(void)
{
	SetKMData(kmdToday, _T("Today"));
	SetKMData(kmdTotal, _T("Total"));
	::GetLocalTime(&stToday);
	SystemtimeToULL(stToday, ullToday);
	siConfig.SetULongLongValue(_T("Main"), _T("Today"), ullToday);
	SetOptions(koOptions);
	siConfig.SaveFile(strIniFileName, true);
}


//----------------------------------------------------------------------------------------------------------------------
void CKamacSheet::SetKMData(const CKMData& kmd, LPCTSTR strSecName)
{
	siConfig.SetULongValue(strSecName, L"KeyStrokes", kmd.ulKeyStrokes);
	siConfig.SetULongValue(strSecName, L"LeftClick", kmd.ulLeftClick);
	siConfig.SetULongValue(strSecName, L"MiddleClick", kmd.ulMiddleClick);
	siConfig.SetULongValue(strSecName, L"RightClick", kmd.ulRightClick);
	siConfig.SetULongLongValue(strSecName, L"Distance", kmd.ullDistance);
}


//----------------------------------------------------------------------------------------------------------------------
void CKamacSheet::GetKMData(CKMData& kmd, LPCTSTR strSecName)
{
	kmd.ulKeyStrokes = siConfig.GetULongValue(strSecName, L"KeyStrokes");
	kmd.ulLeftClick = siConfig.GetULongValue(strSecName, L"LeftClick");
	kmd.ulMiddleClick = siConfig.GetULongValue(strSecName, L"MiddleClick");
	kmd.ulRightClick = siConfig.GetULongValue(strSecName, L"RightClick");
	kmd.ullDistance = siConfig.GetULongLongValue(strSecName, L"Distance");
}


//----------------------------------------------------------------------------------------------------------------------
void CKamacSheet::SetOptions(const CKamacOptions& ko)
{
	const TCHAR* SecName = _T("Options");
	siConfig.SetULongValue(SecName, _T("StartWithOS"), ULONG32(ko.bStartWithOS));
	siConfig.SetULongValue(SecName, _T("MonitorSizeConfirmed"), ULONG32(ko.bMonitorSizeConfirmed));
	siConfig.SetULongValue(SecName, _T("MonitorSize"), ko.ulMonitorSize);
}


//----------------------------------------------------------------------------------------------------------------------
void CKamacSheet::GetOptions(CKamacOptions& ko)
{
	const TCHAR* SecName = _T("Options");
	ko.bStartWithOS = (BOOL)(siConfig.GetULongValue(SecName, _T("StartWithOS"), 0));
	ko.bMonitorSizeConfirmed = (BOOL)(siConfig.GetULongValue(SecName, _T("MonitorSizeConfirmed"), 0));
	ko.ulMonitorSize = siConfig.GetULongValue(SecName, _T("MonitorSize"), 35560);
}


//----------------------------------------------------------------------------------------------------------------------
void CKamacSheet::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (bSilent)
	{
		bSilent = FALSE;
		PostMessage(WM_SYSCOMMAND, SC_MINIMIZE);
	}
	static int n = 0;
	n++;
	if (n > nAutoSaveInterval)
	{
		SaveConfig();
		n = 0;
	}
	CheckToday();
	CPropertySheet::OnTimer(nIDEvent);
}


//----------------------------------------------------------------------------------------------------------------------
BOOL CKamacSheet::CheckToday(void)
{
	SYSTEMTIME st;
	::GetLocalTime(&st);
	BOOL bRes = st.wYear != stToday.wYear || st.wMonth != stToday.wMonth || st.wDay != stToday.wDay;
	if (bRes)
	{
		stToday = st;
		kmdToday.Reset();
		SaveConfig();
		ppMain->UpdateAll(kmdSession, kmdToday, kmdTotal);
	}
	return bRes;
}


//----------------------------------------------------------------------------------------------------------------------
void CKamacSheet::SystemtimeToULL(const SYSTEMTIME& st, ULONG64& ull)
{
	FILETIME ft;
	SystemTimeToFileTime(&st, &ft);
	ull = ft.dwHighDateTime;
	ull <<= 32;
	ull |= ft.dwLowDateTime;
}


//----------------------------------------------------------------------------------------------------------------------
void CKamacSheet::ULLToSystemtime(const ULONG64& ull, SYSTEMTIME& st)
{
	FILETIME ft;
	ft.dwHighDateTime = ull >> 32;
	ft.dwLowDateTime = ull & 0x00000000ffffffff;
	::FileTimeToSystemTime(&ft, &st);
}


//----------------------------------------------------------------------------------------------------------------------
HRESULT CKamacSheet::OnTrayIconNotify(WPARAM wParam, LPARAM lParam)
{
	if (lParam == WM_LBUTTONUP)
	{
		ShowWindow(SW_SHOW);
		SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
		tniTray.HideIcon();
	}
	//if (lParam == WM_RBUTTONUP)
	//{
	//	CMenu menu;

	//	if (menu.LoadMenu(IDR_MENUTRAYICON))
	//	{
	//		CMenu* pm;
	//		pm = menu.GetSubMenu(0);
	//		if (pm)
	//		{
	//			CPoint pt;

	//			::GetCursorPos(&pt);
	//			pm->SetDefaultItem(0, TRUE);
	//			SetForegroundWindow();
	//			pm->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);
	//		}
	//		menu.DestroyMenu();
	//	}
	//}

	return 0;
}


//----------------------------------------------------------------------------------------------------------------------
void CKamacSheet::OnSize(UINT nType, int cx, int cy)
{
	CPropertySheet::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (nType == SIZE_MINIMIZED)
	{
		tniTray.ShowIcon();
		ShowWindow(SW_HIDE);
	}
}
