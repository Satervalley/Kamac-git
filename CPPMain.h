#pragma once


// CPPMain 对话框
#include "CNFStatic.h"
#include "CKMData.h"
#include "CKamacOptions.h"


class CPPMain : public CPropertyPage
{
	DECLARE_DYNAMIC(CPPMain)

public:
	CPPMain(CKamacOptions & ko);
	virtual ~CPPMain();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CPPMain };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void UpdateInfo(void);
	void UpdateMousePos(void);
	void UpdateMousePos(int cx, int cy);
	void UpdateMouseLeftClick(ULONG32 ulLCSession, ULONG32 ulLCToday, ULONG32 ulLCTotal);
	void UpdateMouseRightClick(ULONG32 ulLCSession, ULONG32 ulLCToday, ULONG32 ulLCTotal);
	void UpdateMouseMiddleClick(ULONG32 ulLCSession, ULONG32 ulLCToday, ULONG32 ulLCTotal);
	void UpdateMouseDistance(ULONG64 ulMDSession, ULONG64 ulMDToday, ULONG64 ulMDTotal);
	void UpdateKeyboard(ULONG32 ulKSSession, ULONG32 ulKSToday, ULONG32 ulKSTotal, USHORT usCode);
	void UpdateAll(const CKMData& kmdSession, const CKMData& kmdToday, const CKMData& kmdTotal);
protected:
	CListCtrl lcMain;
	CString strInfo, strScreenSize, strResolution, strKeyboard, strMouse;
	//CStatic statInfo;
	CNFStatic statInfo;
	CKamacOptions& koOptions;
};
