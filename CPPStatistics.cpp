// CPPStatistics.cpp: 实现文件
//

#include "pch.h"
#include "Kamac.h"
#include "CPPStatistics.h"


constexpr UINT WM_USER_VIEW_CHART = WM_USER + 3;
constexpr UINT WM_USER_NAVI_NEXT = WM_USER + 11;
constexpr UINT WM_USER_NAVI_PREV = WM_USER + 12;
constexpr UINT WM_USER_NAVI_NEWEST = WM_USER + 13;
constexpr UINT WM_USER_NAVI_OLDEST = WM_USER + 14;
constexpr UINT WM_USER_NAVI_TOP_KBD = WM_USER + 15;
constexpr UINT WM_USER_NAVI_TOP_MCC = WM_USER + 16;
constexpr UINT WM_USER_NAVI_TOP_DIS = WM_USER + 17;
constexpr UINT WM_USER_NAVI_TOP_ACT = WM_USER + 18;

extern const UINT WM_USER_COLOR_CHANGED;

// CPPStatistics 对话框

IMPLEMENT_DYNAMIC(CPPStatistics, CPropertyPage)


//----------------------------------------------------------------------------------------------------------------------
CPPStatistics::CPPStatistics(CKamacOptions& ko)
	: CPropertyPage(IDD_CPPSTATISTICS), koOPtions(ko)
{
	m_pPSP->pszIcon = MAKEINTRESOURCE(IDI_ICON_STATISTICS);
	m_pPSP->dwFlags |= PSP_USEICONID;
}


//----------------------------------------------------------------------------------------------------------------------
CPPStatistics::~CPPStatistics()
{
}


//----------------------------------------------------------------------------------------------------------------------
void CPPStatistics::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_STATINFO, htmlInfo);
}


BEGIN_MESSAGE_MAP(CPPStatistics, CPropertyPage)
	ON_WM_SIZE()
	ON_MESSAGE(WM_USER_VIEW_CHART, &CPPStatistics::OnUserShowGraph)
	ON_MESSAGE(WM_USER_COLOR_CHANGED, OnColorChanged)
	ON_MESSAGE(WM_USER_NAVI_PREV, &CPPStatistics::OnUserNaviPrev)
	ON_MESSAGE(WM_USER_NAVI_NEXT, &CPPStatistics::OnUserNaviNext)
	ON_MESSAGE(WM_USER_NAVI_NEWEST, &CPPStatistics::OnUserNaviNewest)
	ON_MESSAGE(WM_USER_NAVI_OLDEST, &CPPStatistics::OnUserNaviOldest)
	ON_MESSAGE(WM_USER_NAVI_TOP_KBD, &CPPStatistics::OnUserNaviTopKBD)
	ON_MESSAGE(WM_USER_NAVI_TOP_MCC, &CPPStatistics::OnUserNaviTopMCC)
	ON_MESSAGE(WM_USER_NAVI_TOP_DIS, &CPPStatistics::OnUserNaviTopDIS)
	ON_MESSAGE(WM_USER_NAVI_TOP_ACT, &CPPStatistics::OnUserNaviTopACT)
END_MESSAGE_MAP()


// CPPStatistics 消息处理程序
//----------------------------------------------------------------------------------------------------------------------
BOOL CPPStatistics::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	
	XHTMLSTATIC_APP_COMMAND appCommands[] = 
	{
		{m_hWnd, WM_USER_VIEW_CHART, 0, _T("CMD_VIEW_CHART")},
		{m_hWnd, WM_USER_NAVI_NEXT, 0, _T("CMD_NAVI_NEXT")},
		{m_hWnd, WM_USER_NAVI_PREV, 0, _T("CMD_NAVI_PREV")},
		{m_hWnd, WM_USER_NAVI_NEWEST, 0, _T("CMD_NAVI_NEWEST")},
		{m_hWnd, WM_USER_NAVI_OLDEST, 0, _T("CMD_NAVI_OLDEST")},
		{m_hWnd, WM_USER_NAVI_TOP_KBD, 0, _T("CMD_NAVI_TOP_KBD")},
		{m_hWnd, WM_USER_NAVI_TOP_MCC, 0, _T("CMD_NAVI_TOP_MCC")},
		{m_hWnd, WM_USER_NAVI_TOP_DIS, 0, _T("CMD_NAVI_TOP_DIS")},
		{m_hWnd, WM_USER_NAVI_TOP_ACT, 0, _T("CMD_NAVI_TOP_ACT")}
	};
	
	htmlInfo.ModifyStyleEx(WS_EX_TRANSPARENT, 0);
	htmlInfo.SetAppCommands(appCommands, sizeof(appCommands) / sizeof(XHTMLSTATIC_APP_COMMAND));
	 
	CRect rect;
	GetClientRect(&rect);
	rect.DeflateRect(8, 8, 8, 8);
	svStatVisual.SetDS(&dsMan);
	svStatVisual.SetOptions(&koOPtions);

	if (!svStatVisual.Create(::AfxRegisterWndClass(CS_OWNDC), nullptr, WS_CHILD, rect, this, 1))
		::AfxMessageBox(_T("Create d2d window failed!"));
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


//----------------------------------------------------------------------------------------------------------------------
void CPPStatistics::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPage::OnSize(nType, cx, cy);

	if (::IsWindow(svStatVisual.m_hWnd) && ::IsWindow(htmlInfo.m_hWnd))
	{
		CRect rect, rc1;
		GetClientRect(&rect);
		htmlInfo.GetWindowRect(&rc1);
		ScreenToClient(&rc1);
		rect.left = rc1.left;
		rect.top = rc1.bottom + 4;//rc1.top;
		rect.right = rc1.right;
		rect.bottom -= rc1.top;
		svStatVisual.MoveWindow(&rect);
	}
}


//----------------------------------------------------------------------------------------------------------------------
afx_msg LRESULT CPPStatistics::OnUserShowGraph(WPARAM wParam, LPARAM lParam)
{
	MakeInfoString(dsMan, TRUE);
	htmlInfo.SetWindowText(strHtml);
	svStatVisual.InitGraph();
	svStatVisual.ShowWindow(SW_SHOW);
	bGraphShowed = true;
	return 0;
}


//----------------------------------------------------------------------------------------------------------------------
void CPPStatistics::MakeInfoString(CKamacDS_Man& dsMan, BOOL bChartShowed)
{
	Date_Key dkToday = DateKeyToday();
	CTime tmFirst(DateKeyYear(dsMan.dkFirstDay), DateKeyMonth(dsMan.dkFirstDay), DateKeyDay(dsMan.dkFirstDay), 1, 0, 0);
	CTime tmToday(DateKeyYear(dkToday), DateKeyMonth(dkToday), DateKeyDay(dkToday), 1, 0, 0);
	CTimeSpan ts = tmToday - tmFirst;
	CString strColor1, strColor2, strColor3;
	strColor1.Format(_T("#%06x"), koOPtions.dwVolColor1);
	strColor2.Format(_T("#%06x"), koOPtions.dwVolColor2);
	strColor3.Format(_T("#%06x"), koOPtions.dwVolColor3);
	strHtml.Format(htmlStr, int(ts.GetDays()), MakeDateString(dsMan.dkFirstDay), dsMan.ulRecCount,
		strColor1, MakeDateString(dsMan.recKeyMost.dkDate), strColor1, dsMan.recKeyMost.kmdDay.ulKeyStrokes,
		strColor2, MakeDateString(dsMan.recMouseMost.dkDate), strColor2, dsMan.recMouseMost.MouseTotal(),
		MakeDateString(dsMan.recActivityMost.dkDate), dsMan.recActivityMost.ActivityTotal(),
		strColor3, MakeDateString(dsMan.recMoveMost.dkDate), strColor3, (dsMan.recMoveMost.kmdDay.ullDistance + 5000) / 10000);
	strNavigation.Format(htmlNavigation, strColor1, strColor2, strColor3);
	if (!bChartShowed)
		strHtml += htmlShowGraph;
	else
		strHtml += strNavigation;
}


//----------------------------------------------------------------------------------------------------------------------
BOOL CPPStatistics::LoadHistory(void)
{
	BOOL bRes = false;
	dsMan.Close(true);
	if (dsMan.Open(strHisFileName))
		bRes = dsMan.LoadAll();
	dsMan.Close(false);
	return bRes;
}


//----------------------------------------------------------------------------------------------------------------------
BOOL CPPStatistics::OnSetActive()
{
	if (LoadHistory())
	{
		MakeInfoString(dsMan, FALSE);
		htmlInfo.SetWindowText(strHtml);
		//OnUserShowGraph(0, 0);
	}
	else
	{
		strError.Format(htmlError, _T("load history file failed"));
		htmlInfo.SetWindowText(strError);
	}

	return CPropertyPage::OnSetActive();
}


//----------------------------------------------------------------------------------------------------------------------
BOOL CPPStatistics::OnKillActive()
{
	svStatVisual.Hide();
	bGraphShowed = false;
	dsMan.Close(true);

	return CPropertyPage::OnKillActive();
}


//----------------------------------------------------------------------------------------------------------------------
void CPPStatistics::DayPassed(void)
{
	if (!::IsWindow(m_hWnd))
		return;

	if (LoadHistory())
	{
		MakeInfoString(dsMan, bGraphShowed);
		htmlInfo.SetWindowText(strHtml);
	}
	else
	{
		strError.Format(htmlError, _T("load history file failed"));
		htmlInfo.SetWindowText(strError);
		svStatVisual.Hide();
		bGraphShowed = false;
		dsMan.Close(true);
	}
	if (bGraphShowed)
	{
		svStatVisual.InitGraph(false);
		svStatVisual.Invalidate();
		svStatVisual.UpdateWindow();
	}
}


//----------------------------------------------------------------------------------------------------------------------
LRESULT CPPStatistics::OnColorChanged(WPARAM wParam, LPARAM lParam)
{
	// update statistics string
	MakeInfoString(dsMan, bGraphShowed);
	htmlInfo.SetWindowText(strHtml);

	CWnd* ppw = GetParent();
	if (ppw)
		ppw->SendMessage(WM_USER_COLOR_CHANGED);
	return TRUE;
}


//----------------------------------------------------------------------------------------------------------------------
LRESULT CPPStatistics::OnUserNaviPrev(WPARAM wParam, LPARAM lParam)
{
	if (!bGraphShowed)
		OnUserShowGraph(0, 0);
	svStatVisual.BeginNaviPrevNext();
	return LRESULT(0);
}


//----------------------------------------------------------------------------------------------------------------------
LRESULT CPPStatistics::OnUserNaviNext(WPARAM wParam, LPARAM lParam)
{
	if (!bGraphShowed)
		OnUserShowGraph(0, 0);
	svStatVisual.BeginNaviPrevNext(true);
	return LRESULT(0);
}


//----------------------------------------------------------------------------------------------------------------------
LRESULT CPPStatistics::OnUserNaviNewest(WPARAM wParam, LPARAM lParam)
{
	if (!bGraphShowed)
		OnUserShowGraph(0, 0);
	svStatVisual.BeginNaviFirstLast(false);
	return LRESULT(0);
}


//----------------------------------------------------------------------------------------------------------------------
LRESULT CPPStatistics::OnUserNaviOldest(WPARAM wParam, LPARAM lParam)
{
	if (!bGraphShowed)
		OnUserShowGraph(0, 0);
	svStatVisual.BeginNaviFirstLast();
	return LRESULT(0);
}


//----------------------------------------------------------------------------------------------------------------------
LRESULT CPPStatistics::OnUserNaviTopKBD(WPARAM wParam, LPARAM lParam)
{
	if (!bGraphShowed)
		OnUserShowGraph(0, 0);
	svStatVisual.BeginNaviToDate(dsMan.recKeyMost.dkDate, 0);
	return LRESULT(0);
}


//----------------------------------------------------------------------------------------------------------------------
LRESULT CPPStatistics::OnUserNaviTopMCC(WPARAM wParam, LPARAM lParam)
{
	if (!bGraphShowed)
		OnUserShowGraph(0, 0);
	svStatVisual.BeginNaviToDate(dsMan.recMouseMost.dkDate, 1);
	return LRESULT(0);
}


//----------------------------------------------------------------------------------------------------------------------
LRESULT CPPStatistics::OnUserNaviTopDIS(WPARAM wParam, LPARAM lParam)
{
	if (!bGraphShowed)
		OnUserShowGraph(0, 0);
	svStatVisual.BeginNaviToDate(dsMan.recMouseMost.dkDate, 2);
	return LRESULT(0);
}


//----------------------------------------------------------------------------------------------------------------------
LRESULT CPPStatistics::OnUserNaviTopACT(WPARAM wParam, LPARAM lParam)
{
	if (!bGraphShowed)
		OnUserShowGraph(0, 0);
	bool bVols[3]{ true, true, false };
	svStatVisual.BeginNaviToDate(dsMan.recActivityMost.dkDate, bVols);
	return LRESULT(0);
}

