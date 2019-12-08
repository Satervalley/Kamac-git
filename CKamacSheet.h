#pragma once

#include "CPPMain.h"
#include "CPPOptions.h"
#include "CPPAbout.h"

#include "CKMData.h"
#include "CKamacOptions.h"
#include "SimpleIni.h"
#include "NTRAY.H"

// CKamacSheet

class CKamacSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CKamacSheet)

public:
	BOOL bSilent{ FALSE };

	CKamacSheet(UINT nIDCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);
	CKamacSheet(LPCTSTR pszCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);
	virtual ~CKamacSheet();
protected:

	HICON m_hIcon;
	CPPMain* ppMain{ new CPPMain(koOptions) };
	CPPOptions* ppOptions{ new CPPOptions(koOptions) };
	CPPAbout* ppAbout{ new CPPAbout() };
	CKMData kmdTotal, kmdToday, kmdSession;
	CKamacOptions koOptions;
	CSimpleIni siConfig;
	CString strExeFileName;
	CString strIniFileName;
	UINT uTimerID = 1;
	int nAutoSaveInterval = 100;
	ULONG64 ullToday = 0;
	SYSTEMTIME stToday;
	CTrayNotifyIcon tniTray;
	LONG lLastX{ 0 }, lLastY{ 0 };

	void Init(void);
	void AddPages(void);
	void ProcessKeyboardInput(RAWKEYBOARD & rk);
	void ProcessMouseInput(RAWMOUSE & rm);
	void LoadConfig(void);
	void SaveConfig(void);
	void SetKMData(const CKMData& kmd, LPCTSTR strSecName);
	void GetKMData(CKMData& kmd, LPCTSTR strSecName);
	void SetOptions(const CKamacOptions& ko);
	void GetOptions(CKamacOptions& ko);
	void GetIniFileName(void);
	BOOL CheckToday(void);
	void SystemtimeToULL(const SYSTEMTIME& st, ULONG64 & ull);
	void ULLToSystemtime(const ULONG64& ull, SYSTEMTIME& st);
	ULONG32 CaculateMoveDistance(LONG xc, LONG yc, BOOL bAbs = FALSE);
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnRawInput(UINT nInputcode, HRAWINPUT hRawInput);
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg HRESULT OnTrayIconNotify(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


