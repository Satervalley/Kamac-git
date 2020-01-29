// CNFStatic.cpp: 实现文件
//

#include "pch.h"
#include "Kamac.h"
#include "CNFStatic.h"


// CNFStatic

IMPLEMENT_DYNAMIC(CNFStatic, CStatic)

CNFStatic::CNFStatic()
{
	titles[0] = _T(" Screen size:    ");
	titles[1] = _T(" Resolution:    ");
	titles[2] = _T(" Last key:    ");
	titles[3] = _T(" Mouse pointer:    ");
}

CNFStatic::~CNFStatic()
{
}


BEGIN_MESSAGE_MAP(CNFStatic, CStatic)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CNFStatic 消息处理程序


//----------------------------------------------------------------------------------------------------------------------
BOOL CNFStatic::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

//	return CStatic::OnEraseBkgnd(pDC);
	return TRUE;
}


//----------------------------------------------------------------------------------------------------------------------
void CNFStatic::OnPaint()
{
	CPaintDC dc(this); 
	CRect rect;
	GetClientRect(&rect);
	if (!dcMem)
		PrepareMemDC(dc, rect.Width(), rect.Height());
	Draw(dcMem, rect, &dc);
}

//----------------------------------------------------------------------------------------------------------------------
void CNFStatic::Draw(CDC& dc, CRect& rect, CDC * pDCTarget)
{
	if (dc)
	{
		CRect rt = rect;
		dc.FillSolidRect(rect, ::GetSysColor(COLOR_INFOBK));
		dc.DrawFocusRect(&rt);
//		rt.DeflateRect(2, 2);
//		dc.DrawText(strContent, &rt, DT_LEFT | DT_EXPANDTABS);
		for (int i = 0; i < 4; i++)
		{
			dc.DrawText(titles[i], _tcslen(titles[i]), &rcTitles[i], DT_LEFT | DT_EXPANDTABS);
			dc.DrawText(contents[i], _tcslen(contents[i]), &rcConts[i], DT_LEFT | DT_EXPANDTABS);
		}
		if(pDCTarget)
		{
			pDCTarget->BitBlt(0, 0, rect.Width(), rect.Height(), &dc, 0, 0, SRCCOPY);
		}
	}
}


//----------------------------------------------------------------------------------------------------------------------
void CNFStatic::PrepareMemDC(CDC& dc, int nw, int nh)
{
	if (font.operator HFONT())
		font.DeleteObject();
	if (bmp.operator HBITMAP())
		bmp.DeleteObject();
	if (dcMem)
		dcMem.DeleteDC();

	dcMem.CreateCompatibleDC(&dc);
	bmp.CreateCompatibleBitmap(&dc, nw, nh);
	dcMem.SelectObject(bmp);
	NONCLIENTMETRICS ncm;
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	if (::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0))
	{
		font.CreateFontIndirect(&(ncm.lfMessageFont));
		dcMem.SelectObject(&font);
	}
	else
		dcMem.SelectStockObject(SYSTEM_FONT);
	dcMem.SetTextColor(::GetSysColor(COLOR_INFOTEXT));
	CSize szs[4];
	int mw = 0;
	int th = 0;
	for (int i = 0; i < 4; i++)
	{
		szs[i] = dcMem.GetTextExtent(titles[i], _tcslen(titles[i]));
		if (szs[i].cx > mw)
			mw = szs[i].cx;
		th += szs[i].cy;
	}
	int gap = (nh - th - 2) / 5;
	int pos = 1;
	for (int i = 0; i < 4; i++)
	{
		rcTitles[i].left = 1;
		rcTitles[i].top = i * gap + gap + pos;
		rcTitles[i].right = rcTitles[i].left + szs[i].cx;
		rcTitles[i].bottom = rcTitles[i].top + szs[i].cy;
		rcConts[i].left = mw;
		rcConts[i].top = i * gap + gap + pos;
		rcConts[i].right = nw - 2;
		rcConts[i].bottom = rcConts[i].top + szs[i].cy;
		pos += szs[i].cy;
	}
}

