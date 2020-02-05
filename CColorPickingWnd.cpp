// CColorPickingWnd.cpp: 实现文件
//

#include "pch.h"
#include "Kamac.h"
#include "CColorPickingWnd.h"

const UINT WM_USER_HOVER_COLOR = WM_USER + 4; 
const UINT WM_USER_SELECT_COLOR = WM_USER + 5; 


// CColorPickingWnd

IMPLEMENT_DYNAMIC(CColorPickingWnd, CWnd)

CColorPickingWnd::CColorPickingWnd(CWnd* ptw)
{
	pTarget = ptw;
}

CColorPickingWnd::~CColorPickingWnd()
{
}


BEGIN_MESSAGE_MAP(CColorPickingWnd, CWnd)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()



// CColorPickingWnd 消息处理程序
//----------------------------------------------------------------------------------------------------------------------
void CColorPickingWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	GetCurrColorPos(point, ptCurrSelect);

	CWnd::OnLButtonDown(nFlags, point);
}


//----------------------------------------------------------------------------------------------------------------------
void CColorPickingWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	CPoint pos;
	if (GetCurrColorPos(point, pos))
	{
		if (pos == ptCurrSelect)
		{
			bSelected = true;
			AnimateWindow(200, AW_HIDE | AW_BLEND);
			pTarget->SendMessage(WM_USER_SELECT_COLOR, 1, LPARAM(GetRGB(pos.x, pos.y)));	
		}
	}
	CWnd::OnLButtonUp(nFlags, point);
}


//----------------------------------------------------------------------------------------------------------------------
void CColorPickingWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	CPoint pos;
	bool b = GetCurrColorPos(point, pos);
	if (pos != ptCurrHover)
	{
		clrCurr = GetColor(pos.x, pos.y);
		CDC* pDC = GetDC();
		pDC->FillSolidRect(264, 4, 32, 128, clrCurr);
		ptCurrHover = pos;
		if (!b)
			pTarget->SendMessage(WM_USER_HOVER_COLOR);	// restore orig color
		else
			pTarget->SendMessage(WM_USER_HOVER_COLOR, 1, LPARAM(GetRGB(pos.x, pos.y)));	// preview new color
	}

	CWnd::OnMouseMove(nFlags, point);
}


//----------------------------------------------------------------------------------------------------------------------
void CColorPickingWnd::OnPaint()
{
	CPaintDC dc(this); 
	CRect rc;
	GetClientRect(&rc);
	CBrush brush;
	brush.Attach(::GetSysColorBrush(COLOR_3DFACE));
	dc.FillRect(&rc, &brush);
	brush.Detach();
	CPen pen, *pop;
	pen.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_WINDOWFRAME));
	pop = dc.SelectObject(&pen);
	dc.Rectangle(&rc);
	dc.SelectObject(pop);
	for (int i = 0; i < 16; i++)
	{
		for (int j = 0; j < 32; j++)
		{
			rc.left = 4 + j * 8;
			rc.top = 4 + i * 8;
			rc.right = rc.left + 8;
			rc.bottom = rc.top + 8;
			dc.FillSolidRect(&rc, clrColors[i * 32 + j]);
		}
	}
	dc.FillSolidRect(264, 4, 32, 128, clrCurr);
}


//----------------------------------------------------------------------------------------------------------------------
void CColorPickingWnd::Init(void)
{
	//DWORD dwR, dwG, dwB;
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			for (int k = 0; k < 8; k++)
			{
				clrColors[i * 64 + j * 8 + k] = RGB(36 * i, 36 * j, 36 * k); 
				dwRGBs[i * 64 + j * 8 + k] = ((36 * i) << 16) | ((36 * j) << 8) | (36 * k);
			}
		}
	}
	clrCurr = ::GetSysColor(COLOR_WINDOW);
}


//----------------------------------------------------------------------------------------------------------------------
bool CColorPickingWnd::GetCurrColorPos(const CPoint& pt, CPoint& pos)
{
	pos.x = (pt.x - 4) / 8;
	pos.y = (pt.y - 4) / 8;
	if (pos.x >= 0 && pos.x < 32 && pos.y >= 0 && pos.y < 16)
		return true;
	else
	{
		pos.x = pos.y = -1;
		return false;
	}
}


//----------------------------------------------------------------------------------------------------------------------
COLORREF CColorPickingWnd::GetColor(int x, int y)
{
	if(x >= 0 && y >= 0)
		return clrColors[y * 32 + x];
	else
		return ::GetSysColor(COLOR_WINDOW);
}


//----------------------------------------------------------------------------------------------------------------------
DWORD CColorPickingWnd::GetRGB(int x, int y)
{
	if (x >= 0 && y >= 0)
		return dwRGBs[y * 32 + x];
	else
		return 0xffffffff;
}

//----------------------------------------------------------------------------------------------------------------------
void CColorPickingWnd::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);
	if (!bSelected)
	{
		AnimateWindow(200, AW_HIDE | AW_BLEND);
		pTarget->SendMessage(WM_USER_SELECT_COLOR);
	}
	bSelected = false;
}
