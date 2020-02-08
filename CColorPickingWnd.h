#pragma once


extern const UINT WM_USER_HOVER_COLOR;
extern const UINT WM_USER_SELECT_COLOR;


// CColorPickingWnd

class CColorPickingWnd : public CWnd
{
	DECLARE_DYNAMIC(CColorPickingWnd)

public:
	CColorPickingWnd(CWnd * ptw);
	virtual ~CColorPickingWnd();
	void Init(void);
	BOOL ShowAt(const CPoint& pt, COLORREF clr);

protected:
	DECLARE_MESSAGE_MAP()

protected:
	COLORREF clrOld;
	const int nMargin{ 4 }, nGap{ 1 }, nCellEdge{ 8 }, nGridWidth{ 32 }, nGridHeight{ 16 }, nPreviewWidth{ 32 };
	const int nTriWidth{ 12 }, nTriHeigh{ 12 };
	CRgn rgnWnd, rgnCopy;
	CRect rectGrid{ nMargin, nMargin, nMargin + nGridWidth * nCellEdge + nGap * (nGridWidth - 1), nMargin + nGridHeight * nCellEdge + nGap * (nGridHeight - 1) };
	CRect rectPreview{ rectGrid.right + nMargin, nMargin, rectGrid.right + nMargin + nPreviewWidth, rectGrid.bottom };
	CRect rectWin{ 0, 0, rectPreview.right + nMargin, rectPreview.bottom + nMargin};
	CWnd* pTarget{ nullptr };
	COLORREF clrColors[512];
	DWORD dwRGBs[512];
	COLORREF clrCurr;
	CPoint ptCurrHover{ -1, -1 }, ptCurrSelect{ -1, -1 };
	bool bSelected{ false };

	BOOL Create(void);
	COLORREF GetColor(int x, int y);
	DWORD GetRGB(int x, int y);
	bool GetCurrColorPos(const CPoint& pt, CPoint& pos);
	void DrawPreview(CDC* pDC, COLORREF clr, bool bIncOld);

protected:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
public:
	afx_msg void OnCaptureChanged(CWnd* pWnd);
};


