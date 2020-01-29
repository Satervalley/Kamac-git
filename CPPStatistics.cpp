// CPPStatistics.cpp: 实现文件
//

#include "pch.h"
#include "Kamac.h"
#include "CPPStatistics.h"
#include "afxdialogex.h"

constexpr UINT WM_USER_VIEW_CHART = WM_USER + 3;

// CPPStatistics 对话框

IMPLEMENT_DYNAMIC(CPPStatistics, CPropertyPage)


//----------------------------------------------------------------------------------------------------------------------
CPPStatistics::CPPStatistics()
	: CPropertyPage(IDD_CPPSTATISTICS)
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
	ON_MESSAGE(WM_USER_VIEW_CHART, &CPPStatistics::OnUserViewChart)
END_MESSAGE_MAP()


// CPPStatistics 消息处理程序
//----------------------------------------------------------------------------------------------------------------------
BOOL CPPStatistics::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	
	XHTMLSTATIC_APP_COMMAND appCommands[] = 
	{
		{m_hWnd, WM_USER_VIEW_CHART, 0, _T("CMD_VIEW_CHART")}
	};
	
	htmlInfo.ModifyStyleEx(WS_EX_TRANSPARENT, 0);
	htmlInfo.SetAppCommands(appCommands, 1);
	 
	CRect rect;
	GetClientRect(&rect);
	rect.DeflateRect(8, 8, 8, 8);
	svStatVisual.SetDS(&dsMan);

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
afx_msg LRESULT CPPStatistics::OnUserViewChart(WPARAM wParam, LPARAM lParam)
{
	svStatVisual.ShowWindow(SW_SHOW);
	MakeInfoString(dsMan, TRUE);
	htmlInfo.SetWindowText(strHtml);
	svStatVisual.InitGraph();
	return 0;
}


//----------------------------------------------------------------------------------------------------------------------
void CPPStatistics::MakeInfoString(CKamacDS_Man& dsMan, BOOL bChartShowed)
{
	Date_Key dkToday = DateKeyToday();
	CTime tmFirst(DateKeyYear(dsMan.dkFirstDay), DateKeyMonth(dsMan.dkFirstDay), DateKeyDay(dsMan.dkFirstDay), 1, 0, 0);
	CTime tmToday(DateKeyYear(dkToday), DateKeyMonth(dkToday), DateKeyDay(dkToday), 1, 0, 0);
	CTimeSpan ts = tmToday - tmFirst;
	strHtml.Format(htmlStr, int(ts.GetDays()), MakeDateString(dsMan.dkFirstDay), dsMan.ulRecCount, 
		MakeDateString(dsMan.recKeyMost.dkDate), dsMan.recKeyMost.kmdDay.ulKeyStrokes,
		MakeDateString(dsMan.recMouseMost.dkDate), dsMan.recMouseMost.MouseTotal(),
		MakeDateString(dsMan.recActivityMost.dkDate), dsMan.recActivityMost.ActivityTotal(),
		MakeDateString(dsMan.recMoveMost.dkDate), (dsMan.recMoveMost.kmdDay.ullDistance + 5000) / 10000);
	if (!bChartShowed)
		strHtml += htmlViewChart;
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
	dsMan.Close(true);

	return CPropertyPage::OnKillActive();
}
