#pragma once

#include "CKamacOptions.h"

// CPPOptions 对话框

class CPPOptions : public CPropertyPage
{
	DECLARE_DYNAMIC(CPPOptions)

public:
	CPPOptions(CKamacOptions & ko);
	virtual ~CPPOptions();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CPPOptions };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClickedCheckStartWithOS();
	BOOL bStartWithOS;
	CString strExeFileName;
	CKamacOptions & koOPtions;

	//virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();
	afx_msg void OnKillFocusEditMonitorSize();
protected:
	ULONG32 ulMonitorSize;
};
