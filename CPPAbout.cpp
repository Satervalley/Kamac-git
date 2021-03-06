﻿// CPPAbout.cpp: 实现文件
//

#include "pch.h"
#include "Kamac.h"
#include "CPPAbout.h"
#include "afxdialogex.h"
#include "Kamac_av.h"


// CPPAbout 对话框

IMPLEMENT_DYNAMIC(CPPAbout, CPropertyPage)


//----------------------------------------------------------------------------------------------------------------------
CPPAbout::CPPAbout()
	: CPropertyPage(IDD_CPPAbout)
{
	m_pPSP->pszIcon = MAKEINTRESOURCE(IDI_ICON_ABOUT);
	m_pPSP->dwFlags |= PSP_USEICONID;
}


//----------------------------------------------------------------------------------------------------------------------
CPPAbout::~CPPAbout()
{
}


//----------------------------------------------------------------------------------------------------------------------
void CPPAbout::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_CONTENT, htmlContent);
}


BEGIN_MESSAGE_MAP(CPPAbout, CPropertyPage)
END_MESSAGE_MAP()


// CPPAbout 消息处理程序

//----------------------------------------------------------------------------------------------------------------------
BOOL CPPAbout::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	htmlContent.ModifyStyleEx(WS_EX_TRANSPARENT, 0);
	CString str, strVer;
	strVer.Format(_T("Version %d.%02d Build %d.%d on %d.%02d.%02d"), AutoVersion::nMajor, AutoVersion::nMinor,
		AutoVersion::nBuild, AutoVersion::nRevision, AutoVersion::nYear, AutoVersion::nMonth, AutoVersion::nDay);
	str.Format(htmlStr, strVer, AutoVersion::nYear);
	htmlContent.SetWindowText(str);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
