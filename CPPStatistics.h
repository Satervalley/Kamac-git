#pragma once

#include "CStatVisualWnd.h"
#include "XHTMLStatic.h"
#include "CKamacDS.h"


// CPPStatistics 对话框

class CPPStatistics : public CPropertyPage
{
	DECLARE_DYNAMIC(CPPStatistics)

public:
	CPPStatistics();
	virtual ~CPPStatistics();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CPPSTATISTICS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

protected:
	CStatVisualWnd svStatVisual;
	CXHTMLStatic htmlInfo;
	CString strHtml, strViewChart, strError;
	CString strHisFileName;
	CKamacDS_Man dsMan;
	
	const TCHAR* htmlStr = {
		_T("&nbsp;&nbsp;&nbsp;&nbsp;It's been <font color=\"#aa00ff\">%d</font> day(s) since you first ran Kamac on <font color=\"#aa00ff\">%s</font>, ")
		_T("including <font color=\"#aa00ff\">%d</font> day(s) of records! ")
		_T("The most keystroke is on <font color=\"#ff5252\">%s</font>, <font color=\"#ff5252\">%d</font> keystrokes; ")
		_T("the most mouse clicking is on <font color=\"#69f0ae\">%s</font>, <font color=\"#69f0ae\">%d</font> mouse clicks; ")
		_T("the most activity is on <font color=\"#aa00ff\">%s</font>, <font color=\"#aa00ff\">%d</font> keyboard and mouse activities; ")
		_T("the longest moving is on <font color=\"#448aff\">%s</font>, <font color=\"#448aff\">%dm</font>.<br>")
	};
	const TCHAR* htmlViewChart = _T("<a href=\"app:CMD_VIEW_CHART\"><b><font color=\"blue\"><u>View Chart</u></font></b></a>");
	const TCHAR* htmlError = _T("<center><font color=\"red\" size=\"+6\">Error: %s!</font></center>");

	void MakeInfoString(CKamacDS_Man & dsMan, BOOL bChartShowed);
	BOOL LoadHistory(void);
public:
	void SetHisFileName(LPCTSTR hisfn) { strHisFileName = hisfn; }
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	afx_msg LRESULT OnUserViewChart(WPARAM wParam, LPARAM lParam);
public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
};
