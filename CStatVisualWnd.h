#pragma once

#include "CGraphMan.h"
#include "CColorPickingWnd.h"
#include "CUtil.h"
#include "CColorComb.h"
#include "CStepMan.h"


extern const UINT WM_USER_COLOR_CHANGED;

// CStatVisualWnd
class CStatVisualWnd : public CWnd
{
	DECLARE_DYNAMIC(CStatVisualWnd)

public:
	CStatVisualWnd();
	virtual ~CStatVisualWnd();

	void SetDS(CKamacDS_Man* pds) { gmGraph.SetDS(pds); }
	void SetOptions(CKamacOptions* pko);
	void InitGraph(bool bStartTimer = true);
	void Hide(void);
	void BeginNaviPrevNext(bool bNext = false);
	void BeginNaviFirstLast(bool bFirst = true);
	void BeginNaviToDate(Date_Key dk, int nVol = -1);
	void BeginNaviToDate(Date_Key dk, bool bVols[3]);
protected:
	DECLARE_MESSAGE_MAP()

	struct SAnimateBarData
	{
		bool bEnable{ false };
		Date_Key dkDate{ Date_Key_NULL };
		bool bVolumes[3]{ false };
		int nPercent{ 0 };

		void Reset(void)
		{
			bEnable = false;
			dkDate = Date_Key_NULL;
			bVolumes[0] = bVolumes[1] = bVolumes[2] = false;
			nPercent = 0;
		}
	};

	static const int nBackgroundSize{ 32 };
	CBitmap bmpBackground;
	CD2DBitmapBrush* pbbBrush{ nullptr };
	CD2DBitmap* pbmpBackground{ nullptr };
	CString strFontName;
	float fFontSize{ 12.f };
	CSize szMarginText, szMarginGraph, szLabelText;	// margin for text and graph and graph label text
	DWORD background[nBackgroundSize * nBackgroundSize];
	CColorComb ccBack{ 0 }, ccBorder{ 0 }, ccText{ 0 }, ccVolBorder{ 0 }, ccVolBorderHi{ 0 };
	CColorComb ccVolColors[3] = { 0x00ff5252, 0xc853, 0x448aff };
	CColorComb ccVolColorsHi[3] = {CUtil::CColorUtil::MakeHiColor(0x00ff5252), CUtil::CColorUtil::MakeHiColor(0xc853), 
		CUtil::CColorUtil::MakeHiColor(0x448aff) };
	CColorComb ccSaved{ 0 }, ccSavedHi{ 0 };	// for auto preview
	CComPtr<ID2D1StrokeStyle> ssDash;
	CGraphMan gmGraph;
	CRect rectGraph, rectGraphCore;	// all graph, graph core area, excluding bottom labels
	CString strColNames[4] = {_T("Keystrokes"), _T("Mouse clicks"), _T("Distance"), _T("")};
	CString strColValues[4];
	Date_Key dkCurrTip{ Date_Key_NULL };
	int nMaxLengendStringWidth{ 1 };	// for legend show
	const CString* strCurrLegend{ strColNames };
	int nCompLines[3] = { 0, 0, 0 };
	const int* pnCurrBaseLines{ nullptr };
	CRect rectLegends[3];	// legends rects, for color selection
	int nCurrClickedLegend{ -1 };	// for legend clicking, if legend clicked, show color selection panel
	CColorPickingWnd cpColorPicker;
	UINT uiTimerID_Tip{ 1 }, uiTimerID_Navi{ 2 };	// timer for column values tip, navigation animation
	CKamacOptions* pkoOptions{ nullptr };

	BOOL bDragging{ FALSE };
	CPoint ptLast;	// for track dragging

	SAnimateBarData abdAniBar;

	BOOL PrepareD2DResource(void);
	void DeleteD2DResource(void);
	void GetScaledWindowSize(CSize& sz);
	void DrawAxis(CRenderTarget* prt, float at, float top, float bottom, bool bLeft = true, float lg = 8.f, float sg = 4.f);
	void DrawGraph(CRenderTarget* prt, bool bDrawLabel = true);
	void GetTextSize(CRenderTarget * prt, const CString& str,  CSize& sz);
	void DrawTextOn(CRenderTarget* prt, const CString& str, const CRect& rc, bool bRight = false);
	void DrawDataGroup_3(CRenderTarget* prt, CDataGroup_3_Pointer& pdg, const CRect& rc, bool bDrawLabel = true, bool bHilight = false);
	void DrawDataGroup_3_Shined(CRenderTarget* prt, CDataGroup_3_Pointer& pdg, const CRect& rc, bool bDrawLabel = true, bool bHilight = false);
	void DrawShinedBar(CRenderTarget* prt, const CRect& rect, CColorComb cc, bool bHilight = false);
	float GetColumnHeight(float nTop, float nValue, float nTotalHeight, int nPercent = 100);
	void DrawLegend(CRenderTarget* prt, const CRect& rc, const CString* items, bool bHilight = false);
	void DrawLegend_Shined(CRenderTarget* prt, const CRect& rc, const CString* items, bool bHilight = false);
	void UpdateLegend(void);
	void DrawCompLines(CRenderTarget* prt);
	void DrawAll(CRenderTarget* prt = nullptr);
	int LegendClickBegin(const CPoint & pt);
	int LegendClickEnd(const CPoint& pt);
	void AnimateMove(int nDis, int nSteps = 12, DWORD dwDelay = 30);
	void AnimateBar(Date_Key dk, int nVol, int nSteps = 12, DWORD dwDelay = 30);
	void AnimateBars(Date_Key dk, bool bVols[3], int nSteps = 12, DWORD dwDelay = 30);
	static D2D1::ColorF MakeHiColor2(COLORREF clr);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnDraw2D(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnCaptureChanged(CWnd* pWnd);
	afx_msg LRESULT OnHoverColor(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSelectColor(WPARAM wParam, LPARAM lParam);
};


