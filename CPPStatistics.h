#pragma once

#include "CStatVisualWnd.h"
#include "XHTMLStatic.h"
#include "CKamacDS.h"


// CPPStatistics 对话框

class CPPStatistics : public CPropertyPage
{
	DECLARE_DYNAMIC(CPPStatistics)

public:
	CPPStatistics(CKamacOptions& ko);
	virtual ~CPPStatistics();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CPPSTATISTICS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

protected:
	CKamacOptions& koOPtions; 
	CStatVisualWnd svStatVisual;
	CXHTMLStatic htmlInfo;
	CString strHtml, strViewChart, strError;
	CString strHisFileName;
	CKamacDS_Man dsMan;
	bool bGraphShowed{ false };
	
	const TCHAR* htmlStr = {
		_T("&nbsp;&nbsp;&nbsp;&nbsp;It's been <font color=\"#aa00ff\">%d</font> day(s) since you first ran Kamac on <font color=\"#aa00ff\">%s</font>, ")
		_T("including <font color=\"#aa00ff\">%d</font> day(s) of records! ")
		_T("The most keystroke is on <font color=\"%s\">%s</font>, <font color=\"%s\">%d</font> keystrokes; ")
		_T("the most mouse clicking is on <font color=\"%s\">%s</font>, <font color=\"%s\">%d</font> mouse clicks; ")
		_T("the most activity is on <font color=\"#aa00ff\">%s</font>, <font color=\"#aa00ff\">%d</font> keyboard and mouse activities; ")
		_T("the longest moving is on <font color=\"%s\">%s</font>, <font color=\"%s\">%dm</font>.<br>")
	};
	const TCHAR* htmlViewChart = _T("<a href=\"app:CMD_VIEW_CHART\"><b><font color=\"blue\"><u>View Chart</u></font></b></a>");
	const TCHAR* htmlError = _T("<center><font color=\"red\" size=\"+6\">Error: %s!</font></center>");

	void MakeInfoString(CKamacDS_Man & dsMan, BOOL bChartShowed);
	BOOL LoadHistory(void);
public:
	void SetHisFileName(LPCTSTR hisfn) { strHisFileName = hisfn; }
	virtual BOOL OnInitDialog();
	void DayPassed(void);
protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnUserViewChart(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnColorChanged(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
};
