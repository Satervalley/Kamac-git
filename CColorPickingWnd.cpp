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
	ON_WM_CAPTURECHANGED()
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
		DrawPreview(pDC, clrCurr, false);
		ReleaseDC(pDC);
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
	ASSERT(rc == rectWin);

	CBrush brFrame(::GetSysColor(COLOR_ACTIVEBORDER)), brFill(::GetSysColor(COLOR_INFOBK));
	dc.FillRect(&rc, &brFill);
	BOOL b = dc.FrameRgn(&rgnCopy, &brFrame, 1, 1);
	
	//CBrush brush;
	//brush.Attach(::GetSysColorBrush(COLOR_INFOBK));
	////dc.FillRect(&rc, &brush);
	//dc.FillRgn(&rgnWnd, &brush);
	//brush.Detach();
	//brush.Attach(::GetSysColorBrush(COLOR_ACTIVEBORDER));
	//dc.FrameRgn(&rgnWnd, &brush, 1, 1);
	//brush.Detach();
	//CPen pen, *pop;
	//pen.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_WINDOWFRAME));
	//pop = dc.SelectObject(&pen);
	//dc.Rectangle(&rc);
	//dc.SelectObject(pop);
	CPoint pt(2, 2);
	CBrush* pob;
	CPen* pop;
	for (int i = 0; i < nGridHeight; i++)
	{
		for (int j = 0; j < nGridWidth; j++)
		{
			rc.left = rectGrid.left + j * nCellEdge + j * nGap;
			rc.top = rectGrid.top + i * nCellEdge + i * nGap;
			rc.right = rc.left + nCellEdge;
			rc.bottom = rc.top + nCellEdge;
			//dc.FillSolidRect(&rc, clrColors[i * nGridWidth + j]);
			CBrush brush(clrColors[i * nGridWidth + j]);
			CPen pen(PS_SOLID, 1, clrColors[i * nGridWidth + j]);
			pob = dc.SelectObject(&brush);
			pop = dc.SelectObject(&pen);
			dc.RoundRect(&rc, pt);
			dc.SelectObject(pob);
			dc.SelectObject(pop);
		}
	}
	DrawPreview(&dc, clrCurr, true);
}


//----------------------------------------------------------------------------------------------------------------------
void CColorPickingWnd::DrawPreview(CDC* pDC, COLORREF clr, bool bIncOld)
{
	ASSERT(pDC);

	pDC->FillSolidRect(rectPreview.left + 1, rectPreview.top + 1, rectPreview.Width() - 2, rectPreview.Height() / 2 - 1, clr);
	if (bIncOld)
	{
		pDC->FillSolidRect(rectPreview.left + 1, rectPreview.Height() / 2, rectPreview.Width() - 2, rectPreview.bottom - rectPreview.Height() / 2, clrOld);
		CPen pen, * pop;
		CGdiObject* pob;
		pen.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_3DDKSHADOW));
		pop = pDC->SelectObject(&pen);
		pob = pDC->SelectStockObject(NULL_BRUSH);
		pDC->Rectangle(&rectPreview);
		pDC->SelectObject(pop);
		pDC->SelectObject(pob);
	}
}


//----------------------------------------------------------------------------------------------------------------------
void CColorPickingWnd::Init(void)
{
	for (int n = 0; n < 2; n++)
	{
		for (int m = 0; m < 4; m++)
		{
			int i = n * 4 + m;
			for (int j = 0; j < 8; j++)
			{
				for (int k = 0; k < 8; k++)
				{
					int x, y;;
					if (m % 2)
						x = (m + 1) * 8 - 1 - j;
					else
						x = m * 8 + j;
					if (n % 2)
						y = (n + 1) * 8 - 1 - k;
					else
						y = n * 8 + k;
					clrColors[y * 32 + x] = RGB(36 * i, 36 * j, 36 * k);
					dwRGBs[y * 32 + x] = ((36 * i) << 16) | ((36 * j) << 8) | (36 * k);
				}
			}
		}
	}
	//for (int i = 0; i < 8; i++)	// red
	//{
	//	for (int j = 0; j < 8; j++)	// green
	//	{
	//		for (int k = 0; k < 8; k++)	// blue
	//		{
	//			clrColors[i * 64 + j * 8 + k] = RGB(36 * i, 36 * j, 36 * k); 
	//			dwRGBs[i * 64 + j * 8 + k] = ((36 * i) << 16) | ((36 * j) << 8) | (36 * k);
	//		}
	//	}
	//}
	clrCurr = ::GetSysColor(COLOR_WINDOW);

	rgnWnd.CreateRectRgn(0, 0, rectWin.Width(), rectWin.Height());
	rgnCopy.CreateRectRgn(0, 0, rectWin.Width(), rectWin.Height());
	int x = rectWin.Width() / 3, y = rectWin.Height();
	CPoint pts[3] = { {x, y}, {x + nTriWidth, y}, {x, y + nTriHeigh} };
	CRgn rgn;
	rgn.CreatePolygonRgn(pts, 3, ALTERNATE);
	int nRes = rgnWnd.CombineRgn(&rgnWnd, &rgn, RGN_OR);
	nRes = rgnCopy.CombineRgn(&rgnCopy, &rgn, RGN_OR);
	rectWin.bottom += nTriHeigh;
	SetWindowRgn(rgnWnd, FALSE);
}


//----------------------------------------------------------------------------------------------------------------------
bool CColorPickingWnd::GetCurrColorPos(const CPoint& pt, CPoint& pos)
{
	if (rectGrid.PtInRect(pt))
	{
		CRect rc;
		pos.x = (pt.x - rectGrid.left) / (nCellEdge + nGap);
		pos.y = (pt.y - rectGrid.top) / (nCellEdge + nGap);
		rc.left = rectGrid.left + pos.x * (nCellEdge + nGap);
		rc.right = rc.left + nCellEdge;
		rc.top = rectGrid.top + pos.y * (nCellEdge + nGap);
		rc.bottom = rc.top + nCellEdge;
		if (rc.PtInRect(pt))
			return true;
		else
		{
			pos.x = pos.y = -1;
			return false;
		}
	}
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


//----------------------------------------------------------------------------------------------------------------------
BOOL CColorPickingWnd::Create(void)
{
	if (::IsWindow(m_hWnd))
		return TRUE;
	CRect rect(0, 0, 300, 40);
	BOOL b = CreateEx(WS_EX_WINDOWEDGE | WS_EX_TOOLWINDOW,
		::AfxRegisterWndClass(CS_OWNDC | CS_DROPSHADOW | CS_HREDRAW | CS_VREDRAW), nullptr, WS_POPUP | WS_EX_TOOLWINDOW/*WS_CHILD*/, rect, this, 0);
	if (b)
		Init();
	return b;
}


//----------------------------------------------------------------------------------------------------------------------
BOOL CColorPickingWnd::ShowAt(const CPoint& pt, COLORREF clr)
{
	if (Create())
	{
		clrOld = clr;
		clrCurr = ::GetSysColor(COLOR_WINDOW);
		ptCurrHover = { -1, -1 };
		ptCurrSelect = { -1, -1 };
		bSelected = false;

		CWnd* ptp = pTarget->GetParentOwner();
		ptp->SetRedraw(FALSE);

		MoveWindow(pt.x - rectWin.Width() / 3, pt.y - rectWin.Height(), rectWin.Width(), rectWin.Height(), FALSE);
		//ShowWindow(SW_SHOWNOACTIVATE);
		//SetCapture();
		AnimateWindow(200, AW_ACTIVATE | AW_BLEND);
		//Invalidate();
		//UpdateWindow();

//		SetFocus();

		ptp->SendMessage(WM_NCACTIVATE, TRUE);
		ptp->SetRedraw(TRUE);

		return TRUE;
	}
	return FALSE;
}


//----------------------------------------------------------------------------------------------------------------------
void CColorPickingWnd::OnCaptureChanged(CWnd* pWnd)
{
	// TODO: 在此处添加消息处理程序代码

	CWnd::OnCaptureChanged(pWnd);
}
