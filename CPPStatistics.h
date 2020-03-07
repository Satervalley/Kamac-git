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
	CString strHtml, strViewChart, strError, strNavigation;
	CString strHisFileName;
	CKamacDS_Man dsMan;
	bool bGraphShowed{ false };
	
	const TCHAR* htmlStr = {
		_T("&nbsp;&nbsp;&nbsp;&nbsp;It's been <font color=\"#aa00ff\">%d</font> day(s) since you first ran Kamac on <a href=\"app:CMD_NAVI_OLDEST\"><font color=\"#aa00ff\"><u>%s</u></font></a>, ")
		_T("including <font color=\"#aa00ff\">%d</font> day(s) of records! ")
		_T("The most keystroke is on <a href=\"app:CMD_NAVI_TOP_KBD\"><font color=\"%s\"><u>%s</u></font></a>, <font color=\"%s\">%d</font> keystrokes; ")
		_T("the most mouse clicking is on <a href=\"app:CMD_NAVI_TOP_MCC\"><font color=\"%s\"><u>%s</u></font></a>, <font color=\"%s\">%d</font> mouse clicks; ")
		_T("the most activity is on <a href=\"app:CMD_NAVI_TOP_ACT\"><font color=\"#aa00ff\"><u>%s</u></font></a>, <font color=\"#aa00ff\">%d</font> keyboard and mouse activities; ")
		_T("the longest moving is on <a href=\"app:CMD_NAVI_TOP_DIS\"><font color=\"%s\"><u>%s</u></font></a>, <font color=\"%s\">%dm</font>.<br>")
	};
	const TCHAR* htmlShowGraph = _T("<a href=\"app:CMD_VIEW_CHART\"><b><font color=\"blue\"><u>Show chart</u></font></b></a>");
	const TCHAR* htmlNavigation = { 
		_T("&nbsp;&nbsp;<a href=\"app:CMD_NAVI_OLDEST\"><font color=\"blue\"> ∣← </font></a>&nbsp;&nbsp;") 
		_T("&nbsp;&nbsp;<a href=\"app:CMD_NAVI_PREV\"><font color=\"blue\"> ← </font></a>&nbsp;&nbsp;")
		_T("&nbsp;&nbsp;<a href=\"app:CMD_NAVI_TOP_KBD\"><font color=\"%s\"> ↑ </font></a>&nbsp;")
		_T("&nbsp;<a href=\"app:CMD_NAVI_TOP_MCC\"><font color=\"%s\"> ↑ </font></a>&nbsp;")
		_T("&nbsp;<a href=\"app:CMD_NAVI_TOP_DIS\"><font color=\"%s\"> ↑ </font></a>&nbsp;&nbsp;")
		_T("&nbsp;&nbsp;<a href=\"app:CMD_NAVI_NEXT\"><font color=\"blue\"> → </font></a>&nbsp;&nbsp;")
		_T("&nbsp;&nbsp;<a href=\"app:CMD_NAVI_NEWEST\"><font color=\"blue\"> →∣ </font></a>&nbsp;&nbsp;")
	};

	const TCHAR* htmlError = _T("<center><font color=\"red\" size=\"+6\">Error: %s!</font></center>");

	void MakeInfoString(CKamacDS_Man & dsMan, BOOL bChartShowed);
	BOOL LoadHistory(void);
public:
	void SetHisFileName(LPCTSTR hisfn) { strHisFileName = hisfn; }
	virtual BOOL OnInitDialog();
	void DayPassed(void);
protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnUserShowGraph(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnColorChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserNaviPrev(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserNaviNext(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserNaviNewest(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserNaviOldest(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserNaviTopKBD(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserNaviTopMCC(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserNaviTopDIS(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserNaviTopACT(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
};
