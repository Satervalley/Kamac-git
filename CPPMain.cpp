// CPPMain.cpp: 实现文件
//

#include "pch.h"
#include "Kamac.h"
#include "CNFStatic.h"
#include "CPPMain.h"

#include <PhysicalMonitorEnumerationAPI.h>
#include <HighLevelMonitorConfigurationAPI.h>
//#include "afxdialogex.h"

//#pragma comment(lib, "dxva2.lib")

// CPPMain 对话框

IMPLEMENT_DYNAMIC(CPPMain, CPropertyPage)


//----------------------------------------------------------------------------------------------------------------------
CPPMain::CPPMain(CKamacOptions& ko)
	: CPropertyPage(IDD_CPPMain), koOptions(ko)
{
	
}


//----------------------------------------------------------------------------------------------------------------------
CPPMain::~CPPMain()
{
}


//----------------------------------------------------------------------------------------------------------------------
void CPPMain::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MAIN, lcMain);
	DDX_Control(pDX, IDC_STATIC_INFO, statInfo);
}


BEGIN_MESSAGE_MAP(CPPMain, CPropertyPage)
END_MESSAGE_MAP()


// CPPMain 消息处理程序
BOOL CPPMain::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  在此添加额外的初始化
	::SetWindowTheme(lcMain, L"Explorer", nullptr);
	lcMain.SetExtendedStyle(lcMain.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | 
		LVS_EX_BORDERSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_ONECLICKACTIVATE);
	CRect rect;
	int nw;
	lcMain.GetClientRect(rect);
	lcMain.InsertColumn(0, _T("Device"), 0, 96);
	lcMain.InsertColumn(1, _T("Item"), 0, 128);
	nw = (rect.Width() - 224) / 3;
	lcMain.InsertColumn(2, _T("Total"), 0, nw);
	lcMain.InsertColumn(3, _T("Today"), 0, nw);
	lcMain.InsertColumn(4, _T("Session"), 0, nw);

	lcMain.InsertItem(lcMain.GetItemCount(), _T("Keyboard"));
	lcMain.InsertItem(lcMain.GetItemCount(), _T(""));
	lcMain.SetItemText(lcMain.GetItemCount() - 1, 1, _T("Keystrokes"));

	lcMain.InsertItem(lcMain.GetItemCount(), _T("Mouse"));
	lcMain.InsertItem(lcMain.GetItemCount(), _T(""));
	lcMain.SetItemText(lcMain.GetItemCount() - 1, 1, _T("Left click"));
	lcMain.InsertItem(lcMain.GetItemCount(), _T(""));
	lcMain.SetItemText(lcMain.GetItemCount() - 1, 1, _T("Right click"));
	lcMain.InsertItem(lcMain.GetItemCount(), _T(""));
	lcMain.SetItemText(lcMain.GetItemCount() - 1, 1, _T("Middle click"));
	lcMain.InsertItem(lcMain.GetItemCount(), _T(""));
	lcMain.SetItemText(lcMain.GetItemCount() - 1, 1, _T("Distance"));

//	::SetWindowLongPtr(statInfo, GWL_EXSTYLE, statInfo.GetExStyle() | WS_EX_COMPOSITED);
	int nsw = ::GetSystemMetrics(SM_CXSCREEN);
	int nsh = ::GetSystemMetrics(SM_CYSCREEN);
	strScreenSize.Format(_T("Screen size:\t%u mm\r\n"), (koOptions.ulMonitorSize + 5) / 10); 
	strResolution.Format( _T("Resolution:\t%d x %d\r\n"), nsw, nsh);
	strKeyboard.Format(_T("Last key:\t\t0x%02X\r\n"), 0);
	UpdateMousePos();
	strInfo.Format(_T("%s%s%s%s"), (LPCTSTR)strScreenSize, (LPCTSTR)strResolution, (LPCTSTR)strKeyboard, (LPCTSTR)strMouse);

	statInfo.SetContent(strInfo);
	//	statInfo.SetWindowText(strInfo);


	//HMONITOR hm = ::MonitorFromPoint(CPoint(1, 1), MONITOR_DEFAULTTOPRIMARY);
	//PHYSICAL_MONITOR pms[3];
	//::GetPhysicalMonitorsFromHMONITOR(hm, 1, pms);
	//DWORD dwMin, dwCurr, dwMax;
	//::GetMonitorDisplayAreaSize(pms[0].hPhysicalMonitor, MC_WIDTH, &dwMin, &dwCurr, &dwMax);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


//----------------------------------------------------------------------------------------------------------------------
void CPPMain::UpdateInfo(void)
{
	int nsw = ::GetSystemMetrics(SM_CXSCREEN);
	int nsh = ::GetSystemMetrics(SM_CYSCREEN);
	strScreenSize.Format(_T("Screen size:\t%u mm\r\n"), (koOptions.ulMonitorSize + 5) / 10);
	strResolution.Format(_T("Resolution:\t%d x %d\r\n"), nsw, nsh);
	strInfo.Format(_T("%s%s%s%s"), (LPCTSTR)strScreenSize, (LPCTSTR)strResolution, (LPCTSTR)strKeyboard, (LPCTSTR)strMouse);
	statInfo.SetContent(strInfo);
}


//----------------------------------------------------------------------------------------------------------------------
void CPPMain::UpdateMousePos(void)
{
	CPoint pt;
	::GetCursorPos(&pt);
	strMouse.Format(_T("Mouse pointer:\t%d %d"), pt.x, pt.y);

}


//----------------------------------------------------------------------------------------------------------------------
void CPPMain::UpdateMouseLeftClick(ULONG32 ulLCSession, ULONG32 ulLCToday, ULONG32 ulLCTotal)
{
	CString str;
	str.Format(_T("%u"), ulLCTotal);
	lcMain.SetItemText(3, 2, str);
	str.Format(_T("%u"), ulLCToday);
	lcMain.SetItemText(3, 3, str);
	str.Format(_T("%u"), ulLCSession);
	lcMain.SetItemText(3, 4, str);
}


//----------------------------------------------------------------------------------------------------------------------
void CPPMain::UpdateMouseRightClick(ULONG32 ulLCSession, ULONG32 ulLCToday, ULONG32 ulLCTotal)
{
	CString str;
	str.Format(_T("%u"), ulLCTotal);
	lcMain.SetItemText(4, 2, str);
	str.Format(_T("%u"), ulLCToday);
	lcMain.SetItemText(4, 3, str);
	str.Format(_T("%u"), ulLCSession);
	lcMain.SetItemText(4, 4, str);
}


//----------------------------------------------------------------------------------------------------------------------
void CPPMain::UpdateMouseMiddleClick(ULONG32 ulLCSession, ULONG32 ulLCToday, ULONG32 ulLCTotal)
{
	CString str;
	str.Format(_T("%u"), ulLCTotal);
	lcMain.SetItemText(5, 2, str);
	str.Format(_T("%u"), ulLCToday);
	lcMain.SetItemText(5, 3, str);
	str.Format(_T("%u"), ulLCSession);
	lcMain.SetItemText(5, 4, str);
}


//----------------------------------------------------------------------------------------------------------------------
void CPPMain::UpdateMouseDistance(ULONG64 ulMDSession, ULONG64 ulMDToday, ULONG64 ulMDTotal)
{
	ulMDSession /= 100ull;	// to cm
	ulMDToday /= 100ull;
	ulMDTotal /= 100ull;
	CString str;
	if (ulMDSession < 100ull)
	{
		str.Format(_T("%llu cm"), ulMDSession);
	}
	else
	{
		str.Format(_T("%llu.%02llu m"), ulMDSession / 100ull, ulMDSession % 100ull);
	}
	lcMain.SetItemText(6, 4, str);
	if (ulMDToday < 100ll)
	{
		str.Format(_T("%llu cm"), ulMDToday);
	}
	else
	{
		str.Format(_T("%llu.%02llu m"), ulMDToday / 100ull, ulMDToday % 100ull);
	}
	lcMain.SetItemText(6, 3, str);
	if (ulMDTotal < 100ll)
	{
		str.Format(_T("%llu cm"), ulMDTotal);
	}
	else
	{
		str.Format(_T("%llu.%02llu m"), ulMDTotal / 100ull, ulMDTotal % 100ull);
	}
	lcMain.SetItemText(6, 2, str);
}


//----------------------------------------------------------------------------------------------------------------------
void CPPMain::UpdateKeyboard(ULONG32 ulKSSession, ULONG32 ulKSToday, ULONG32 ulKSTotal, USHORT usCode)
{
	CString str;
	str.Format(_T("%u"), ulKSTotal);
	lcMain.SetItemText(1, 2, str);
	str.Format(_T("%u"), ulKSToday);
	lcMain.SetItemText(1, 3, str);
	str.Format(_T("%u"), ulKSSession);
	lcMain.SetItemText(1, 4, str);
	strKeyboard.Format(_T("Last key:\t\t0x%02X\r\n"), usCode);
}


//----------------------------------------------------------------------------------------------------------------------
void CPPMain::UpdateAll(const CKMData& kmdSession, const CKMData& kmdToday, const CKMData& kmdTotal)
{
	UpdateKeyboard(kmdSession.ulKeyStrokes, kmdToday.ulKeyStrokes, kmdTotal.ulKeyStrokes, 0);
	UpdateMouseLeftClick(kmdSession.ulLeftClick, kmdToday.ulLeftClick, kmdTotal.ulLeftClick);
	UpdateMouseMiddleClick(kmdSession.ulMiddleClick, kmdToday.ulMiddleClick, kmdTotal.ulMiddleClick);
	UpdateMouseRightClick(kmdSession.ulRightClick, kmdToday.ulRightClick, kmdTotal.ulRightClick);
	UpdateMouseDistance(kmdSession.ullDistance, kmdToday.ullDistance, kmdTotal.ullDistance);
}

