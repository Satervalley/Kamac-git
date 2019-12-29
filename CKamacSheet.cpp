// CKamacSheet.cpp: 实现文件
//

#include "pch.h"
#include "Kamac.h"
#include "CKamacSheet.h"
#include "Kamac_av.h"
#include "HighColorTab.hpp"


UINT WM_TRAYICONNOTIFY = WM_USER + 1;
extern UINT WM_OPTIONS_CHANGED;
// CKamacSheet

IMPLEMENT_DYNAMIC(CKamacSheet, CPropertySheet)


//----------------------------------------------------------------------------------------------------------------------
CKamacSheet::CKamacSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	Init();
	AddPages();
}


//----------------------------------------------------------------------------------------------------------------------
CKamacSheet::CKamacSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	Init();
	AddPages();
}


void CKamacSheet::Init(void)
{
	GetIniFileName();
	LoadConfig();
	dmDisplay.Update(koOptions.ulMonitorSize);
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
	ON_MESSAGE(WM_OPTIONS_CHANGED, OnOptionsChanged)
	ON_WM_SIZE()
	ON_WM_DISPLAYCHANGE()
	ON_COMMAND(ID_TRAY_MAIN, &CKamacSheet::OnTrayMain)
	ON_COMMAND(ID_TRAY_OPTIONS, &CKamacSheet::OnTrayOptions)
	ON_COMMAND(ID_TRAY_ABOUT, &CKamacSheet::OnTrayAbout)
	ON_COMMAND(ID_TRAY_EXIT, &CKamacSheet::OnTrayExit)
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
		if (IsIconic())
		{
			int i = 0;
 			i++; 
		}
		if (!IsIconic())
		{
			ppMain->UpdateKeyboard(kmdSession.ulKeyStrokes, kmdToday.ulKeyStrokes, kmdTotal.ulKeyStrokes, rk.MakeCode);
			ppMain->UpdateInfo();
		}
	}
}


//----------------------------------------------------------------------------------------------------------------------
void CKamacSheet::ProcessMouseInput(RAWMOUSE& rm)
{
	BOOL b = IsIconic();
	if (rm.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
	{
		kmdSession.IncLeftClick();
		kmdToday.IncLeftClick();
		kmdTotal.IncLeftClick();
		if (!b)
			ppMain->UpdateMouseLeftClick(kmdSession.ulLeftClick, kmdToday.ulLeftClick, kmdTotal.ulLeftClick);
	}
	if (rm.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
	{
		kmdSession.IncRightClick();
		kmdToday.IncRightClick();
		kmdTotal.IncRightClick();
		if (!b)
			ppMain->UpdateMouseRightClick(kmdSession.ulRightClick, kmdToday.ulRightClick, kmdTotal.ulRightClick);
	}
	if (rm.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
	{
		kmdSession.IncMiddleClick();
		kmdToday.IncMiddleClick();
		kmdTotal.IncMiddleClick();
		if (!b)
			ppMain->UpdateMouseMiddleClick(kmdSession.ulMiddleClick, kmdToday.ulMiddleClick, kmdTotal.ulMiddleClick);
	}
	// 下面代码因win鼠标加速，计算不准确
	/*
	USHORT f = 0x0001;
	BOOL bAbs = rm.usFlags & f;
	ULONG32 ulmd = CaculateMoveDistance(rm.lLastX, rm.lLastY, bAbs);
	*/
	CPoint pt;
	::GetCursorPos(&pt);
	ULONG32 ulmd = CaculateMoveDistance(pt.x, pt.y, TRUE);
	kmdSession.IncDistance(ulmd);
	kmdToday.IncDistance(ulmd);
	kmdTotal.IncDistance(ulmd);
	if (!b)
	{
		ppMain->UpdateMouseDistance(kmdSession.ullDistance, kmdToday.ullDistance, kmdTotal.ullDistance);
		ppMain->UpdateMousePos(pt.x, pt.y);
		ppMain->UpdateInfo();
	}
}


//----------------------------------------------------------------------------------------------------------------------
ULONG32 CKamacSheet::CaculateMoveDistance(LONG xc, LONG yc, BOOL bAbs)
{
	int dx, dy;
	if (bAbs)
	{
		dx = xc - lLastX;
		lLastX = xc;
		dy = yc - lLastY;
		lLastY = yc;
	}
	else
	{
		lLastX += xc;
		lLastY += yc;
		dx = xc;
		dy = yc;
	}
	return dmDisplay.GetDistance(dx, dy);
}


//----------------------------------------------------------------------------------------------------------------------
BOOL CKamacSheet::OnInitDialog()
{
	BOOL bResult = CPropertySheet::OnInitDialog();

//	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hIcon = (HICON)::LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON,
		::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	m_hIconSmall = (HICON)::LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON,
		::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	SetIcon(m_hIconSmall, FALSE);		// 设置小图标

	tniTray.Create(this, 1, _T("Kamac"), m_hIconSmall, WM_TRAYICONNOTIFY);
	tniTray.HideIcon();

	ppOptions->strExeFileName = strExeFileName;
	if (!CheckToday())
		ppMain->UpdateAll(kmdSession, kmdToday, kmdTotal);

	if (!koOptions.bMonitorSizeConfirmed)
	{
		koOptions.bMonitorSizeConfirmed = TRUE;
		::AfxMessageBox(_T("You'd better input your monitor size before we start!"), MB_OK | MB_ICONINFORMATION);
		SetActivePage(1);
	}

	HighColorTab::UpdateImageList(*this);

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
	else
		str += _T(".");
	strIniFileName = str + _T("ini");
	strHistoryFileName = CT2CA(str);
	strHistoryFileName += "his";
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
	ko.ulMonitorSize = siConfig.GetULongValue(SecName, _T("MonitorSize"), 3556);
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
//	bRes = true;	// todo: delete this line when release
	if (bRes)
	{
		SaveDayDate(stToday);
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
	if (lParam == WM_RBUTTONUP)
	{
		CMenu menu;
		if (menu.LoadMenu(IDR_MENU_TRAY))
		{
			CMenu* pm;
			pm = menu.GetSubMenu(0);
			if (pm)
			{
				CPoint pt;

				::GetCursorPos(&pt);
				//pm->SetDefaultItem(0, TRUE);
				SetForegroundWindow();
				pm->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);
			}
			menu.DestroyMenu();
		}
	}
	if (lParam == WM_MOUSEMOVE)
	{
		MakeTrayTipInfo();
		tniTray.SetTooltipText(strTrayTipInfo);
	}
	return 0;
}


//----------------------------------------------------------------------------------------------------------------------
void CKamacSheet::MakeTrayTipInfo(void)
{
	ULONG64 ulMDSession = kmdSession.ullDistance;
	ulMDSession /= 100ull;	// to cm
	ULONG64 ulMDToday = kmdToday.ullDistance;
	ulMDToday /= 100ull;
	ULONG64 ulMDTotal = kmdTotal.ullDistance;
	ulMDTotal /= 100ull;
	CString strDistSession, strDistToday, strDistTotal;
	if (ulMDSession < 100ull)
	{
		strDistSession.Format(_T("%llucm"), ulMDSession);
	}
	else
	{
		strDistSession.Format(_T("%llu.%02llum"), ulMDSession / 100ull, ulMDSession % 100ull);
	}
	if (ulMDToday < 100ll)
	{
		strDistToday.Format(_T("%llucm"), ulMDToday);
	}
	else
	{
		strDistToday.Format(_T("%llu.%02llum"), ulMDToday / 100ull, ulMDToday % 100ull);
	}
	if (ulMDTotal < 100ll)
	{
		strDistTotal.Format(_T("%llucm"), ulMDTotal);
	}
	else
	{
		strDistTotal.Format(_T("%llu.%02llum"), ulMDTotal / 100ull, ulMDTotal % 100ull);
	}
	strTrayTipInfo.Format(_T("Kamac V%d.%02d Build %d.%d\n\nKBD: %u  %u  %u\nMLC: %u  %u  %u\nDIS: %s  %s  %s"), 
		AutoVersion::nMajor, AutoVersion::nMinor, AutoVersion::nBuild, AutoVersion::nRevision,
		kmdTotal.ulKeyStrokes, kmdToday.ulKeyStrokes, kmdSession.ulKeyStrokes,
		kmdTotal.ulLeftClick, kmdToday.ulLeftClick, kmdSession.ulLeftClick,
		strDistTotal, strDistToday, strDistSession);
}


//----------------------------------------------------------------------------------------------------------------------
HRESULT CKamacSheet::OnOptionsChanged(WPARAM wParam, LPARAM lParam)
{
	dmDisplay.Update(koOptions.ulMonitorSize);
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
	if (nType == SIZE_RESTORED)
	{
		if (::IsWindow(ppMain->m_hWnd))
		{
			ppMain->UpdateAll(kmdSession, kmdToday, kmdTotal);
			ppMain->UpdateInfo();
		}
	}
}


//----------------------------------------------------------------------------------------------------------------------
void CKamacSheet::OnDisplayChange(UINT nImageDepth, int cxScreen, int cyScreen)
{
	CPropertySheet::OnDisplayChange(nImageDepth, cxScreen, cyScreen);

	dmDisplay.Update(cxScreen, cyScreen, koOptions.ulMonitorSize);
}


//----------------------------------------------------------------------------------------------------------------------
void CKamacSheet::OnTrayMain()
{
	// TODO: 在此添加命令处理程序代码
	SetActivePage(0);
	ShowWindow(SW_SHOW);
	SendMessage(WM_SYSCOMMAND, SC_RESTORE);
	tniTray.HideIcon();
}


//----------------------------------------------------------------------------------------------------------------------
void CKamacSheet::OnTrayOptions()
{
	// TODO: 在此添加命令处理程序代码
	SetActivePage(1);
	ShowWindow(SW_SHOW);
	SendMessage(WM_SYSCOMMAND, SC_RESTORE);
	tniTray.HideIcon();
}


//----------------------------------------------------------------------------------------------------------------------
void CKamacSheet::OnTrayAbout()
{
	// TODO: 在此添加命令处理程序代码
	SetActivePage(2);
	ShowWindow(SW_SHOW);
	SendMessage(WM_SYSCOMMAND, SC_RESTORE);
	tniTray.HideIcon();
}


//----------------------------------------------------------------------------------------------------------------------
void CKamacSheet::OnTrayExit()
{
	// TODO: 在此添加命令处理程序代码
	tniTray.HideIcon();
	SendMessage(WM_CLOSE);
}


//----------------------------------------------------------------------------------------------------------------------
BOOL CKamacSheet::SaveDayDate(const SYSTEMTIME& st)
{
	BOOL bRes = false;
	//UnqliteDatabase db;
	//if (db.open(strHistoryFileName))
	//{
	//	std::string strEngineName;
	//	db.getEngineName(strEngineName);

	//	UnqliteQuery query(db);
	//	std::string strKey;
	//	char buf[10];
	//	unsigned char data[sizeof(CKMData) * 2];
	//	int pos = 0;
	//	::snprintf(buf, 10, "%04d%02d%02d", st.wYear, st.wMonth, st.wDay);
	//	strKey = buf;
	//	pos = kmdToday.PutToBuffer(data, pos);
	//	kmdTotal.PutToBuffer(data, pos);
	//	bRes = query.storeData(strKey, data, sizeof(CKMData) * 2);
	//	db.close();
	//}
	return bRes;
}

