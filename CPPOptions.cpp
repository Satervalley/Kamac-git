﻿// CPPOptions.cpp: 实现文件
//

#include "pch.h"
#include "Kamac.h"
#include "CPPOptions.h"
#include "afxdialogex.h"

UINT WM_OPTIONS_CHANGED = WM_USER + 2;

// CPPOptions 对话框

IMPLEMENT_DYNAMIC(CPPOptions, CPropertyPage)


//----------------------------------------------------------------------------------------------------------------------
CPPOptions::CPPOptions(CKamacOptions& ko)
	: CPropertyPage(IDD_CPPOptions)
	, bStartWithOS(FALSE), koOPtions(ko)
	, ulMonitorSize(ko.ulMonitorSize)
{
	m_pPSP->pszIcon = MAKEINTRESOURCE(IDI_ICON_OPTIONS);
	m_pPSP->dwFlags |= PSP_USEICONID;
}


//----------------------------------------------------------------------------------------------------------------------
CPPOptions::~CPPOptions()
{
}


//----------------------------------------------------------------------------------------------------------------------
void CPPOptions::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_STARTWITHOS, bStartWithOS);
	DDX_Text(pDX, IDC_EDIT_MONITORSIZE, ulMonitorSize);
	DDV_MinMaxUInt(pDX, ulMonitorSize, 100, 1000000);
}


BEGIN_MESSAGE_MAP(CPPOptions, CPropertyPage)
	ON_BN_CLICKED(IDC_CHECK_STARTWITHOS, &CPPOptions::OnClickedCheckStartWithOS)
	ON_EN_KILLFOCUS(IDC_EDIT_MONITORSIZE, &CPPOptions::OnKillFocusEditMonitorSize)
END_MESSAGE_MAP()


// CPPOptions 消息处理程序

//----------------------------------------------------------------------------------------------------------------------
void CPPOptions::OnClickedCheckStartWithOS()
{
	UpdateData();
	koOPtions.bStartWithOS = bStartWithOS;
	koOPtions.UpdateStartWithOS(strExeFileName);
}


//----------------------------------------------------------------------------------------------------------------------
//BOOL CPPOptions::OnKillActive()
//{
//	// TODO: 在此添加专用代码和/或调用基类
//
//	return CPropertyPage::OnKillActive();
//}


//----------------------------------------------------------------------------------------------------------------------
BOOL CPPOptions::OnSetActive()
{
	bStartWithOS = koOPtions.bStartWithOS;
	ulMonitorSize = koOPtions.ulMonitorSize;
	UpdateData(FALSE);

	return CPropertyPage::OnSetActive();
}


//----------------------------------------------------------------------------------------------------------------------
void CPPOptions::OnKillFocusEditMonitorSize()
{
	UpdateData();
	koOPtions.ulMonitorSize = ulMonitorSize;
	GetParent()->SendMessage(WM_OPTIONS_CHANGED);
}
