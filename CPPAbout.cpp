// CPPAbout.cpp: 实现文件
//

#include "pch.h"
#include "Kamac.h"
#include "CPPAbout.h"
#include "afxdialogex.h"


// CPPAbout 对话框

IMPLEMENT_DYNAMIC(CPPAbout, CPropertyPage)


//----------------------------------------------------------------------------------------------------------------------
CPPAbout::CPPAbout()
	: CPropertyPage(IDD_CPPAbout)
{

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

	// TODO:  在此添加额外的初始化
	htmlContent.ModifyStyleEx(WS_EX_TRANSPARENT, 0);
	htmlContent.SetWindowText(htmlStr);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
