#pragma once

#include "XHTMLStatic.h"

// CPPAbout 对话框

class CPPAbout : public CPropertyPage
{
	DECLARE_DYNAMIC(CPPAbout)

public:
	CPPAbout();
	virtual ~CPPAbout();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CPPAbout };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	CXHTMLStatic htmlContent;
	const TCHAR * htmlStr = {
							_T("<b><font size=\"+8\" color=\"#ff8000\"><center>Kamac</center></font></b><br><br>")
							_T("<font size=\"+6\" color=\"#a08060\"><center>Keyboard and mouse activity counter</center></font><br>")
							_T("<font color=\"#C6D6FD\"><hr size=2></font><br>")
							_T("<font color=\"#408010\"><center>%s</center></font><br>")
							_T("<center>Copyright&copy; 2019 - %d Darquid studio</center>")
							_T("<br><b><center>All rights reserved</center></b>")
							_T("<br><br>Kamac is open source software.    ")
		_T("<a href=\"https://github.com/Satervalley/Kamac-git\"><font color=\"blue\"><u>source code</u></font></a>    ")
		_T("<a href=\"mailto:darquid@126.com\"><font color=\"blue\"><u>contact me</u></font></a>    ")
		_T("<a href=\"https://github.com/Satervalley/Kamac-git/blob/master/donate.md\"><font color=\"blue\"><u>donate</u></font></a>")
	};
	 


public:
	virtual BOOL OnInitDialog();
};
