#pragma once

#include "CStatVisualWnd.h"

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
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
