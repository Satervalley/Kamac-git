// CPPStatistics.cpp: 实现文件
//

#include "pch.h"
#include "Kamac.h"
#include "CPPStatistics.h"
#include "afxdialogex.h"


// CPPStatistics 对话框

IMPLEMENT_DYNAMIC(CPPStatistics, CPropertyPage)

CPPStatistics::CPPStatistics()
	: CPropertyPage(IDD_CPPSTATISTICS)
{
	m_pPSP->pszIcon = MAKEINTRESOURCE(IDI_ICON_STATISTICS);
	m_pPSP->dwFlags |= PSP_USEICONID;
}

CPPStatistics::~CPPStatistics()
{
}

void CPPStatistics::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPPStatistics, CPropertyPage)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CPPStatistics 消息处理程序


BOOL CPPStatistics::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	CRect rect;
	GetClientRect(&rect);
	svStatVisual.EnableD2DSupport();
	if (!svStatVisual.Create(::AfxRegisterWndClass(CS_OWNDC), nullptr, WS_CHILD | WS_VISIBLE, rect, this, 1))
		::AfxMessageBox(_T("Create d2d window failed!"));
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CPPStatistics::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPage::OnSize(nType, cx, cy);

	CRect rect;
	GetClientRect(&rect);
	svStatVisual.MoveWindow(&rect);
}
