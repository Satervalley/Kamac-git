#pragma once

#include "CPPMain.h"
#include "CPPOptions.h"
#include "CPPAbout.h"

#include "CKMData.h"
#include "CKamacOptions.h"
#include "SimpleIni.h"
#include "NTRAY.H"


class CDisplayMan
{
public:
	void Update(ULONG32 ulms)
	{
		ulWidth = ::GetSystemMetrics(SM_CXSCREEN);
		ulHeight = ::GetSystemMetrics(SM_CYSCREEN);
		ulMonitorSize = ulms;
		double d1, d2;
		d1 = (double)ulMonitorSize * (double)ulMonitorSize;
		d2 = 1.0 + double(ulWidth) * double(ulWidth) / (double(ulHeight) * double(ulHeight));
		d1 = d1 / d2;
		d1 = sqrt(d1);
		dHeight = d1;
		dWidth = d1 * double(ulWidth) / double(ulHeight);
	}

	void Update(int cx, int cy, ULONG32 ulms)
	{
		ulWidth = cx;
		ulHeight = cy;
		Update(ulms);
	}

	ULONG32 GetDistance(int cx, int cy)
	{
		double d1, d2;
		d1 = double(cx) * dWidth / (double)ulWidth;
		d2 = (double)cy * dHeight / double(ulHeight);
		return ULONG32(sqrt(d1 * d1 + d2 * d2) + 0.5);
	}
protected:
	ULONG32 ulMonitorSize{ 3556 };
	ULONG32 ulWidth, ulHeight;
	double dWidth, dHeight;
};


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
	CDisplayMan dmDisplay;
	CString strTrayTipInfo;

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
	void MakeTrayTipInfo(void);
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnRawInput(UINT nInputcode, HRAWINPUT hRawInput);
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg HRESULT OnTrayIconNotify(WPARAM wParam, LPARAM lParam);
	afx_msg HRESULT OnOptionsChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDisplayChange(UINT nImageDepth, int cxScreen, int cyScreen);
	
	afx_msg void OnTrayMain();
	afx_msg void OnTrayOptions();
	afx_msg void OnTrayAbout();
	afx_msg void OnTrayExit();
};


