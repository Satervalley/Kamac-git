// CPPOptions.cpp: 实现文件
//

#include "pch.h"
#include "Kamac.h"
#include "CPPOptions.h"
#include "afxdialogex.h"
#include "Registry.h"


LPCTSTR STR_REG_KEY_AUTO_START =
_T("Software\\microsoft\\Windows\\CurrentVersion\\Run");

// CPPOptions 对话框

IMPLEMENT_DYNAMIC(CPPOptions, CPropertyPage)

CPPOptions::CPPOptions()
	: CPropertyPage(IDD_CPPOptions)
	, bStartWithOS(FALSE)
{

}

CPPOptions::~CPPOptions()
{
}

void CPPOptions::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_STARTWITHOS, bStartWithOS);
}


BEGIN_MESSAGE_MAP(CPPOptions, CPropertyPage)
	ON_BN_CLICKED(IDC_CHECK_STARTWITHOS, &CPPOptions::OnClickedCheckStartWithOS)
END_MESSAGE_MAP()


// CPPOptions 消息处理程序

//----------------------------------------------------------------------------------------------------------------------
void CPPOptions::OnClickedCheckStartWithOS()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	CRegistry reg;
	reg.OpenKey(CRegistry::currentUser, STR_REG_KEY_AUTO_START);
	if (bStartWithOS)
	{
		CString str;
		str.Format(_T("\"%s\" /s"), strExeFileName);
		reg.SetValue(_T("Kamac"), str);
	}
	else
	{
		reg.DeleteValue(_T("Kamac"));
	}
	reg.CloseKey();
}
