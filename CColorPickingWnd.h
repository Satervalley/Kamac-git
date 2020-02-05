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

protected:
	DECLARE_MESSAGE_MAP()
public:
	void Init(void);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
protected:
	CWnd* pTarget{ nullptr };
	COLORREF clrColors[512];
	DWORD dwRGBs[512];
	COLORREF clrCurr;
	CPoint ptCurrHover{ -1, -1 }, ptCurrSelect{ -1, -1 };
	bool bSelected{ false };

	COLORREF GetColor(int x, int y);
	DWORD GetRGB(int x, int y);
	bool GetCurrColorPos(const CPoint& pt, CPoint& pos);
public:
	afx_msg void OnKillFocus(CWnd* pNewWnd);
};


