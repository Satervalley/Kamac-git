#pragma once

#include "CGraphMan.h"
#include "CColorPickingWnd.h"

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
protected:
	DECLARE_MESSAGE_MAP()

protected:
	static const int nBackgroundSize{ 32 };
	CBitmap bmpBackground;
	CD2DBitmapBrush* pbbBrush{ nullptr };
	CD2DBitmap* pbmpBackground{ nullptr };
	CString strFontName;
	float fFontSize{ 12. };
	CSize szMarginText, szMarginGraph, szLabelText;	// margin for text and graph and graph label text
	DWORD background[nBackgroundSize * nBackgroundSize];
	D2D1::ColorF clrBack{ 0 }, clrBorder{ 0 }, clrText{ 0 }, clrHighBorder{ 0 };
	D2D1::ColorF clrVolColors[3] = { 0x00ff5252, 0xc853, 0x448aff };
	D2D1::ColorF clrSaved{ 0 };	// for auto preview
	CComPtr<ID2D1StrokeStyle> ssDash;
	CGraphMan gmGraph;
	CRect rectGraph, rectGraphCore;	// all graph, graph core area, excluding bottom labels
	CString strColNames[3] = {_T("Keystrokes"), _T("Mouse clicks"), _T("Distance"), };
	CString strColValues[3];
	Date_Key dkCurrTip{ Date_Key_NULL };
	int nMaxLengendStringWidth{ 1 };	// for legend show
	const CString* strCurrLegend{ strColNames };
	int nCompLines[3] = { 0, 0, 0 };
	const int* pnCurrBaseLines{ nullptr };
	CRect rectLegends[3];	// legends rects, for color selection
	int nCurrClickedLegend{ -1 };	// for legend clicking, if legend clicked, show color selection panel
	CColorPickingWnd cpColorPicker;
	UINT uiTimerID{ 1 };	// timer for column values tip	
	CKamacOptions* pkoOptions{ nullptr };

	BOOL bDragging{ FALSE };
	CPoint ptLast;	// for track dragging

	BOOL PrepareD2DResource(void);
	void DeleteD2DResource(void);
	void GetScaledWindowSize(CSize& sz);
	void DrawAxis(CRenderTarget* prt, float at, float top, float bottom, bool bLeft = true, float lg = 8.f, float sg = 4.f);
	void DrawGraph(CRenderTarget* prt, bool bDrawLabel = true);
	void GetTextSize(CRenderTarget * prt, const CString& str,  CSize& sz);
	void DrawTextOn(CRenderTarget* prt, const CString& str, const CRect& rc, bool bRight = false);
	void DrawDataGroup_3(CRenderTarget* prt, CDataGroup_3_Pointer& pdg, const CRect& rc, bool bDrawLabel = true);
	float GetColumnHeight(float nTop, float nValue, float nTotalHeight);
	void DrawLegend(CRenderTarget* prt, const CRect& rc, const CString* items);
	void UpdateLegend(void);
	void DrawCompLines(CRenderTarget* prt);
	void DrawAll(CRenderTarget* prt);
	int LegendClickBegin(const CPoint & pt);
	int LegendClickEnd(const CPoint& pt);
	BOOL CreateColorPicker(void);
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnDraw2D(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnCaptureChanged(CWnd* pWnd);
	afx_msg LRESULT OnHoverColor(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSelectColor(WPARAM wParam, LPARAM lParam);
};


