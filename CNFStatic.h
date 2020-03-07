#pragma once


// CNFStatic

#include "pch.h"
#include "CUtil.h"


class CNFStatic : public CStatic
{
	DECLARE_DYNAMIC(CNFStatic)

public:
	CNFStatic();
	virtual ~CNFStatic();

protected:
	DECLARE_MESSAGE_MAP()

	//CString strContent;
	LPCTSTR titles[4];
	TCHAR contents[4][32];
	CRect rcTitles[4];
	CRect rcConts[4];
	CDC dcMem;
	CBitmap bmp;
	CFont font;

	void DrawAndPut(int which, CDC * pTarget = nullptr, bool bRelease = true)
	{
		dcMem.FillSolidRect(rcConts[which], ::GetSysColor(COLOR_INFOBK));
		dcMem.DrawText(contents[which], _tcslen(contents[which]), &rcConts[which], DT_LEFT | DT_EXPANDTABS);
		if (IsWindowVisible() && pTarget)
		{
			pTarget->BitBlt(rcConts[which].left, rcConts[which].top, rcConts[which].Width(), rcConts[which].Height(), &dcMem,
				rcConts[which].left, rcConts[which].top, SRCCOPY);
		}
		if (pTarget && bRelease)
			ReleaseDC(pTarget);
	}

public:

	void UpdateScreenSize(int sz, BOOL refresh = TRUE)
	{
		LPTSTR p = CUtil::itoa(sz, contents[0], false);
		p[0] = p[1] = _T('m');
		p[2] = 0;
		if (refresh)
		{
			if(dcMem)
				DrawAndPut(0, GetDC());
		}
	}

	void UpdateResolution(int cx, int cy, BOOL refresh = TRUE)
	{
		LPTSTR p = CUtil::itoa(cx, contents[1], false);
		p[0] = _T(' ');
		p[1] = _T('x');
		p[2] = _T(' ');
		CUtil::itoa(cy, p + 3);
		if (refresh)
		{
			ASSERT(dcMem);
			DrawAndPut(1, GetDC());
		}
	}

	void UpdateLastKey(unsigned char key, BOOL refresh = TRUE)
	{
		LPTSTR p = contents[2];
		LONG lParam = key;
		lParam <<= 16;
		int nPos = ::GetKeyNameText(lParam, p, 32);
		p += nPos;
		if (nPos)
		{
			p[0] = _T(' ');
			p[1] = _T('(');
			p[2] = _T('0');
			p[3] = _T('x');
			p += 4;
			p = CUtil::itoax(key, p, false);
			p[0] = _T(')');
			p[1] = _T('\0');
		}
		else
		{
			p[0] = _T('0');
			p[1] = _T('x');
			p += 2;
			CUtil::itoax(key, p);
		}
		if (refresh && dcMem)
		{
			ASSERT(dcMem);
			DrawAndPut(2, GetDC());
		}
	}

	void UpdateMousePoint(int x, int y, BOOL refresh = TRUE)
	{
		LPTSTR p = CUtil::itoa(x, contents[3], false);
		p[0] = _T(' ');
		p[1] = _T('x');
		p[2] = _T(' ');
		CUtil::itoa(y, p + 3);
		if (refresh)
		{
			if(dcMem)
				DrawAndPut(3, GetDC());
		}
	}
	
	void UpdateMousePoint(BOOL refresh = TRUE)
	{
		static CPoint pt;
		::GetCursorPos(&pt);
		UpdateMousePoint(pt.x, pt.y, refresh);
	}


	void Draw(CDC& dc, CRect& rect, CDC* pDCTarget = nullptr);
	void PrepareMemDC(CDC& dc, int nw, int nh);

public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
};


