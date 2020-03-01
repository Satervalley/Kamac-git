// CStatVisualWnd.cpp: 实现文件
//

#include "pch.h"
//#include <memory>
#include "Kamac.h"
#include "CStatVisualWnd.h"

//extern const UINT WM_USER_HOVER_COLOR;
//extern const UINT WM_USER_SELECT_COLOR;
//extern const UINT WM_USER_SELECT_COLOR_CANCEL;
const UINT WM_USER_COLOR_CHANGED = WM_USER + 6;

// CStatVisualWnd

IMPLEMENT_DYNAMIC(CStatVisualWnd, CWnd)


//----------------------------------------------------------------------------------------------------------------------
CStatVisualWnd::CStatVisualWnd(): cpColorPicker(this)
{

}


//----------------------------------------------------------------------------------------------------------------------
CStatVisualWnd::~CStatVisualWnd()
{
	DeleteD2DResource();
}


BEGIN_MESSAGE_MAP(CStatVisualWnd, CWnd)
	ON_WM_CREATE()
//	ON_WM_ERASEBKGND()
//	ON_WM_PAINT()
//	ON_WM_SIZE()
	ON_REGISTERED_MESSAGE(AFX_WM_DRAW2D, OnDraw2D)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_CAPTURECHANGED()
	ON_MESSAGE(WM_USER_HOVER_COLOR, OnHoverColor)
	ON_MESSAGE(WM_USER_SELECT_COLOR, OnSelectColor)
END_MESSAGE_MAP()


// CStatVisualWnd 消息处理程序
int CStatVisualWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	EnableD2DSupport();

	PrepareD2DResource();

	return 0;
}


BOOL CStatVisualWnd::OnEraseBkgnd(CDC* pDC)
{
	//if (hrtRender.IsValid())
	//	return TRUE;
	//else
		return CWnd::OnEraseBkgnd(pDC);
}


void CStatVisualWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // 不为绘图消息调用 CWnd::OnPaint()
	//hrtRender.BeginDraw();

	//D2D1_COLOR_F color = { .5f, .5f, .5f, 1.f }; // r, g, b, a
	//hrtRender.Clear(color);
	//CD2DRoundedRect rr(CD2DRectF(10., 10., 130., 120.), CD2DSizeF(8., 8.));
	//CD2DSolidColorBrush brush(&hrtRender, { 1., 1., 1., 1. });
	//hrtRender.DrawRoundedRectangle(&rr, &brush);
	//HRESULT hr = hrtRender.EndDraw();
	//// if the render target has been lost, then recreate it
	//if (D2DERR_RECREATE_TARGET == hr)
	//{
	//	hrtRender.ReCreate(m_hWnd);
	//}
}


//----------------------------------------------------------------------------------------------------------------------
void CStatVisualWnd::GetScaledWindowSize(CSize& sz)
{
	CRect rcw;
	GetWindowRect(&rcw);
	//UINT dpi = ::GetDpiForSystem();//::GetDpiForWindow(m_hWnd);
	//sz.cx = rcw.Width() * 96;
	//sz.cy = rcw.Height() * 96;
	//sz.cx = (sz.cx % dpi) ? sz.cx / dpi + 1 : sz.cx / dpi;
	//sz.cy = (sz.cy % dpi) ? sz.cy / dpi + 1 : sz.cy / dpi;
	sz.cx = rcw.Width();
	sz.cy = rcw.Height();
}


//----------------------------------------------------------------------------------------------------------------------
void CStatVisualWnd::DrawAll(CRenderTarget* prt)
{
	bool b = (prt == nullptr);
	if (b)
	{
		prt = GetRenderTarget();
		prt->BeginDraw();
	}
	prt->SetDpi(CD2DSizeF(96.f, 96.f));
	prt->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
	CSize sz;
	GetScaledWindowSize(sz);
	prt->Clear(ccBack);
	DrawAxis(prt, float(szMarginGraph.cx), 4.f, float(sz.cy - 4 - szMarginGraph.cy));
	DrawAxis(prt, float(sz.cx - szMarginGraph.cx), 4.f, float(sz.cy - 4 - szMarginGraph.cy), false);
	DrawGraph(prt);
	if (b)
		prt->EndDraw();
}


//----------------------------------------------------------------------------------------------------------------------
LRESULT CStatVisualWnd::OnDraw2D(WPARAM wParam, LPARAM lParam)
{
	CHwndRenderTarget* pRender = (CHwndRenderTarget*)lParam;
	DrawAll(pRender);
	return 0;
}


//----------------------------------------------------------------------------------------------------------------------
void CStatVisualWnd::DrawAxis(CRenderTarget* prt, float at, float top, float bottom, bool bLeft, float lg, float sg)
{
	ASSERT(prt);
	CD2DPointF ptFrom, ptTo;
	CD2DSolidColorBrush brush(prt, ccBorder);
	float h = bottom - top;
	ptFrom.x = at;
	for (int i = 0; i < 21; i++)
	{
		ptFrom.y = ptTo.y = float(top + (h / 20.) * float(i));
		if(i % 5 == 0)
			ptTo.x = ptFrom.x + (bLeft ? -lg : lg);
		else
			ptTo.x = ptFrom.x + (bLeft ? -sg : sg);
		prt->DrawLine(ptFrom, ptTo, &brush);
	}

	int nct = gmGraph.GetCountTop(), ndt = gmGraph.GetDistanceTop();
	int ntag = bLeft? nct : ndt;
	CString str, strfmt;
	CRect rc;

	if (bLeft)
		strfmt = _T("%d");
	else
		strfmt = _T("%dm");
	str.Format(strfmt, 0);
	if (bLeft)
	{
		rc.left = 0;
		rc.right = (int)at - 12;
	}
	else
	{
		rc.left = (int)at + 12;
		rc.right = rc.left + szMarginText.cx + 4;
	}
	rc.bottom = (int)bottom;
	rc.top = rc.bottom - szMarginText.cy;
	DrawTextOn(prt, str, rc, bLeft);

	str.Format(strfmt, ntag / 4);
	rc.bottom = (int)(bottom - h / 4.) + szMarginText.cy / 2;
	rc.top = rc.bottom - szMarginText.cy;
	DrawTextOn(prt, str, rc, bLeft);

	str.Format(strfmt, ntag / 2);
	rc.bottom = (int)(bottom - h / 2.) + szMarginText.cy / 2;
	rc.top = rc.bottom - szMarginText.cy;
	DrawTextOn(prt, str, rc, bLeft);

	str.Format(strfmt, ntag / 4 * 3);
	rc.bottom = (int)(bottom - h / 4. * 3.) + szMarginText.cy / 2;
	rc.top = rc.bottom - szMarginText.cy;
	DrawTextOn(prt, str, rc, bLeft);

	str.Format(strfmt, ntag);
	rc.top = (int)top;
	rc.bottom = rc.top + szMarginText.cy;
	DrawTextOn(prt, str, rc, bLeft);
}


//----------------------------------------------------------------------------------------------------------------------
void CStatVisualWnd::DrawGraph(CRenderTarget* prt, bool bDrawLabel)
{
	CD2DRectF rcf(rectGraphCore), rc, rcClip(rectGraph), rcLabelAll(rectGraph);
	CD2DSolidColorBrush scb(prt, ccBorder);//, scbb(prt, ccBack);
	
	rc.left = rcf.left;
	rc.top = rcf.top;
	rc.right = rcf.right - 1.f;
	rc.bottom = rcf.bottom - 1.f;
	prt->FillRectangle(rc, pbbBrush);
	if (bDrawLabel)
	{
		rcLabelAll.top = rcf.bottom + 1.f;
		CD2DSolidColorBrush brushLabelAll(prt, ccBack);
		prt->FillRectangle(rcLabelAll, &brushLabelAll);
	}
	prt->PushAxisAlignedClip(rcClip);
	DrawCompLines(prt);
	for (auto it = gmGraph.GetDataGroups().begin(); it != gmGraph.GetDataGroups().end(); it++)
	{
		//DrawDataGroup_3(prt, *it, rcf, bDrawLabel, dkCurrTip == (*it)->dkDate);
		DrawDataGroup_3_Shined(prt, *it, rcf, bDrawLabel, dkCurrTip == (*it)->dkDate);
	}
	//DrawLegend(prt, rcf, strCurrLegend, dkCurrTip != Date_Key_NULL);
	DrawLegend_Shined(prt, rcf, strCurrLegend, dkCurrTip != Date_Key_NULL);
	prt->PopAxisAlignedClip();
	prt->DrawRectangle(rcf, &scb);
}


//----------------------------------------------------------------------------------------------------------------------
void CStatVisualWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

//	hrtRender.Resize(CD2DSizeU(cx, cy));
}


//----------------------------------------------------------------------------------------------------------------------
BOOL CStatVisualWnd::PrepareD2DResource(void)
{
	BOOL bRes = FALSE; 
	CRenderTarget* prt = GetRenderTarget();

	ASSERT(prt);

	prt->SetDpi(CD2DSizeF(96.f, 96.f));

	for (int j = 0; j < nBackgroundSize / 2; j++)
	{
		for (int i = 0; i < nBackgroundSize / 2; i++)
		{
			background[j * nBackgroundSize + i] = 0xffd6f4f8;	// white
		}
		for (int i = nBackgroundSize / 2; i < nBackgroundSize; i++)
		{
			background[j * nBackgroundSize + i] = 0xffeff6f7;	// gray
		}
	}
	for (int j = nBackgroundSize / 2; j < nBackgroundSize; j++)
	{
		for (int i = 0; i < nBackgroundSize / 2; i++)
		{
			background[j * nBackgroundSize + i] = 0xffeff6f7;	// gray
		}
		for (int i = nBackgroundSize / 2; i < nBackgroundSize; i++)
		{
			background[j * nBackgroundSize + i] = 0xffd6f4f8;	// white
		}
	}
	bmpBackground.CreateBitmap(nBackgroundSize, nBackgroundSize, 1, 32, background);
	pbmpBackground = new CD2DBitmap(prt, bmpBackground, CD2DSizeU(0, 0));
	//pbmpBackground->CopyFromMemory(background, nBackgroundSize * sizeof(DWORD));
	D2D1_BITMAP_BRUSH_PROPERTIES bbp;
	CD2DBrushProperties bp;
	bbp.extendModeX = D2D1_EXTEND_MODE_WRAP;
	bbp.extendModeY = D2D1_EXTEND_MODE_WRAP;
	bbp.interpolationMode = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR;
	
	bp.opacity = 1.;
	bp.transform = D2D1::Matrix3x2F::Translation(-8., -8.);
	pbbBrush = new CD2DBitmapBrush(prt, &bbp, nullptr);
	pbbBrush->SetBitmap(pbmpBackground);

	NONCLIENTMETRICS ncm;
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);
	strFontName = ncm.lfMessageFont.lfFaceName;

	GetTextSize(prt, _T("99999m"), szMarginText);
	szMarginGraph.cx = szMarginText.cx + 4 + 4 + 8;
	szMarginGraph.cy = szMarginText.cy + 4 + 4;

	GetTextSize(prt, _T("88.88"), szLabelText);

	ccBack = ::GetSysColor(COLOR_WINDOW);
	ccBorder = ::GetSysColor(COLOR_3DSHADOW);
	ccText = ::GetSysColor(COLOR_WINDOWTEXT);
	ccVolBorder = ::GetSysColor(COLOR_3DHILIGHT);
	ccVolBorderHi = ::GetSysColor(COLOR_3DDKSHADOW);
	
	D2D1_STROKE_STYLE_PROPERTIES ssp;
	ssp.startCap = D2D1_CAP_STYLE_ROUND;
	ssp.endCap = D2D1_CAP_STYLE_ROUND;
	ssp.dashCap = D2D1_CAP_STYLE_ROUND;
	ssp.lineJoin = D2D1_LINE_JOIN_ROUND;
	ssp.miterLimit = 1.f;
	ssp.dashStyle = D2D1_DASH_STYLE_CUSTOM;
	ssp.dashOffset = 0.f;
	float dashes[2] = { /*8.f, 4.f*/4.f, 2.f };
	::AfxGetD2DState()->GetDirect2dFactory()->CreateStrokeStyle(ssp, dashes, 2, &ssDash);

	return bRes;
}


//----------------------------------------------------------------------------------------------------------------------
void CStatVisualWnd::DeleteD2DResource(void)
{
	// the rendertarget will delete resources
	//if (pbbBrush)
	//{
	//	pbbBrush->Destroy();
	//	delete pbbBrush;
	//	pbbBrush = nullptr;
	//}
	//if (pbmpBackground)
	//{
	//	pbmpBackground->Destroy();
	//	delete pbmpBackground;
	//	pbmpBackground = nullptr;
	//}
}


//----------------------------------------------------------------------------------------------------------------------
void CStatVisualWnd::GetTextSize(CRenderTarget* prt, const CString& str, CSize& sz)
{
	CD2DTextFormat tf(prt, strFontName, fFontSize);
	CD2DTextLayout tl(prt, str, tf, CD2DSizeF(200., 50.));
	DWRITE_TEXT_METRICS dtm;
	tl.Get()->GetMetrics(&dtm);
	sz.cx = (LONG)::ceilf(dtm.width);
	sz.cy = (LONG)::ceilf(dtm.height);
}


//----------------------------------------------------------------------------------------------------------------------
void CStatVisualWnd::DrawTextOn(CRenderTarget* prt, const CString& str, const CRect& rc, bool bRight)
{
	CD2DTextFormat tf(prt, strFontName, fFontSize);
	CD2DTextLayout tl(prt, str, tf, CD2DSizeF(200.f, 50.f));
	DWRITE_TEXT_METRICS dtm;
	tl.Get()->GetMetrics(&dtm);
	CSize sz;
	sz.cx = (LONG)::ceilf(dtm.width);
	sz.cy = (LONG)::ceilf(dtm.height);
	CPoint pt;
	pt.x = rc.left;
	pt.y = rc.top;
	if (bRight)
	{
		pt.x = rc.right - sz.cx;
		pt.x = pt.x < 0 ? 0 : pt.x;
	}
	CD2DSolidColorBrush brush(prt, ccText);
	prt->DrawTextLayout(CD2DPointF(pt), &tl, &brush);
}


//----------------------------------------------------------------------------------------------------------------------
void CStatVisualWnd::InitGraph(bool bStartTimer)
{
	CSize sz;
	GetScaledWindowSize(sz);
	rectGraphCore = { szMarginGraph.cx, 4, sz.cx - szMarginGraph.cx, sz.cy - 4 - szMarginGraph.cy };
	rectGraph = rectGraphCore;
	rectGraph.bottom = sz.cy - 4;
	gmGraph.Init(rectGraphCore);
	bDragging = FALSE;
	if(bStartTimer)
		SetTimer(uiTimerID_Tip, 400, nullptr);
}


//----------------------------------------------------------------------------------------------------------------------
void CStatVisualWnd::DrawDataGroup_3(CRenderTarget* prt, CDataGroup_3_Pointer& pdg, const CRect& rc, bool bDrawLabel, bool bHilight)
{
	ASSERT(prt);

	if (pdg)
	{
		int nth = rc.Height();
		CD2DRectF rect(rc);
		CD2DSolidColorBrush brushBorder(prt, bHilight? ccVolBorderHi : ccVolBorder);
		for (int i = 0; i < 3; i++)
		{
			rect.bottom = (float)rc.bottom;
			rect.left = (float)(pdg->nDrawXLeft + i * (CDataGroup_3::nColumnWidth + CDataGroup_3::nGap));
			rect.right = (float)(rect.left + CDataGroup_3::nColumnWidth);
			if(i != 2)	// 0, 1: keystrokes and mouse clicks
				rect.top = rect.bottom - GetColumnHeight((float)gmGraph.GetCountTop(), (float)pdg->nData[i], (float)nth) - 1.f;
			else // 2: distance
				rect.top = rect.bottom - GetColumnHeight((float)gmGraph.GetDistanceTop(), (float)pdg->nData[i], (float)nth) - 1.f;
			if (rect.left > (float)rc.right || rect.right < (float)rc.left)
				continue;
			CD2DSolidColorBrush brushColumn(prt, bHilight? ccVolColorsHi[i] : ccVolColors[i]);
			prt->DrawRectangle(rect, &brushBorder);
			rect.left += .5f;
			rect.right -= .5f;
			rect.top += .5f;
			rect.bottom -= 1.f;
			prt->FillRectangle(rect, &brushColumn);
		}
		if (bDrawLabel)
		{
			CD2DTextFormat tf(prt, strFontName, fFontSize);
			CD2DTextLayout tlLabel(prt, pdg->strDate, tf, CD2DSizeF(200., 100.));
			CD2DSolidColorBrush brushText(prt, ccText);
			CD2DPointF pt(float(pdg->nBorderXLeft + (pdg->nTotalWidth - szLabelText.cx) / 2), float(rectGraphCore.bottom + 2));
			prt->DrawTextLayout(pt, &tlLabel, &brushText);
		}
	}
}


//----------------------------------------------------------------------------------------------------------------------
void CStatVisualWnd::DrawDataGroup_3_Shined(CRenderTarget* prt, CDataGroup_3_Pointer& pdg, const CRect& rc, bool bDrawLabel, bool bHilight)
{
	ASSERT(prt);

	if (pdg)
	{
		int nth = rc.Height();
		CRect rect(rc);
		for (int i = 0; i < 3; i++)
		{
			rect.left = pdg->nDrawXLeft + i * (CDataGroup_3::nColumnWidth + CDataGroup_3::nGap);
			rect.right = rect.left + CDataGroup_3::nColumnWidth;
			if (abdAniBar.bEnable && pdg->dkDate == abdAniBar.dkDate && i == abdAniBar.nVolume)
			{
				if (i != 2)	// 0, 1: keystrokes and mouse clicks
					rect.top = rect.bottom - 
					int(GetColumnHeight((float)gmGraph.GetCountTop(), (float)pdg->nData[i], (float)nth, abdAniBar.nPercent) + 0.5f);
				else // 2: distance
					rect.top = rect.bottom - 
					int(GetColumnHeight((float)gmGraph.GetDistanceTop(), (float)pdg->nData[i], (float)nth, abdAniBar.nPercent) + 0.5f);
			}
			else
			{
				if (i != 2)	// 0, 1: keystrokes and mouse clicks
					rect.top = rect.bottom - int(GetColumnHeight((float)gmGraph.GetCountTop(), (float)pdg->nData[i], (float)nth) + 0.5f);
				else // 2: distance
					rect.top = rect.bottom - int(GetColumnHeight((float)gmGraph.GetDistanceTop(), (float)pdg->nData[i], (float)nth) + 0.5f);
			}
			if (rect.left > rc.right || rect.right < rc.left)
				continue;
			DrawShinedBar(prt, rect, ccVolColors[i], bHilight);
		}
		if (bDrawLabel)
		{
			CD2DTextFormat tf(prt, strFontName, fFontSize);
			CD2DTextLayout tlLabel(prt, pdg->strDate, tf, CD2DSizeF(200., 100.));
			CD2DSolidColorBrush brushText(prt, ccText);
			CD2DPointF pt(float(pdg->nBorderXLeft + (pdg->nTotalWidth - szLabelText.cx) / 2), float(rectGraphCore.bottom + 2));
			prt->DrawTextLayout(pt, &tlLabel, &brushText);
		}
	}
}


//----------------------------------------------------------------------------------------------------------------------
void CStatVisualWnd::DrawShinedBar(CRenderTarget* prt, const CRect& rect, CColorComb cc, bool bHilight)
{
#define BAR_REFLECT_AMOUNT 32
#define BAR_REFLECT_FACTOR 5
#define BAR_BORDER_DARKNESS 64
#define BAR_GRADIENT_DARKNESS 32

	CColorComb ccClr(bHilight ? CUtil::CColorUtil::MakeHiColor(cc) : cc.clrColor);

	CColorComb ccBorder(CUtil::CColorUtil::Diff(ccClr, BAR_BORDER_DARKNESS));
	CColorComb ccEnd(CUtil::CColorUtil::Diff(ccClr, BAR_GRADIENT_DARKNESS));
	CColorComb ccVol;
	int r, g, b;
	CD2DRectF rcf(rect);
	CD2DSolidColorBrush brush(prt, ccBorder);
	rcf.right -= 1.f;
	prt->DrawRectangle(rcf, &brush);
	for (int i = 1; i < CDataGroup_3::nColumnWidth - 1; i++)
	{
		r = ccEnd.R() + (i * (ccClr.R() - ccEnd.R()) / CDataGroup_3::nColumnWidth);
		g = ccEnd.G() + (i * (ccClr.G() - ccEnd.G()) / CDataGroup_3::nColumnWidth);
		b = ccEnd.B() + (i * (ccClr.B() - ccEnd.B()) / CDataGroup_3::nColumnWidth);

		if (i < CDataGroup_3::nColumnWidth / 2) 
		{
			ccVol = CUtil::CColorUtil::Or(RGB(r, g, b), RGB(255, 255, 255), 
				(BAR_REFLECT_AMOUNT + BAR_REFLECT_FACTOR * i) > 100 ? 100 : (BAR_REFLECT_AMOUNT + BAR_REFLECT_FACTOR * i));
		}
		else 
		{
			ccVol = RGB(r, g, b);
		}
		brush.SetColor(ccVol);
		prt->DrawLine(CD2DPointF(rcf.left + (float)i, rcf.top), CD2DPointF(rcf.left + (float)i, rcf.bottom - 1.f), &brush);
	}
}


//----------------------------------------------------------------------------------------------------------------------
float CStatVisualWnd::GetColumnHeight(float nTop, float nValue, float nTotalHeight, int nPercent)
{
	float factor = float(nPercent) / 100.f;
	float h = nValue / nTop * nTotalHeight * factor;
	h = h < 2. ? 2 : h;
	h = h > nTop ? nTop : h;
	return h;
}


//----------------------------------------------------------------------------------------------------------------------
void CStatVisualWnd::DrawLegend(CRenderTarget* prt, const CRect& rc, const CString* items, bool bHilight)
{
	rectLegends[0] = rectLegends[1] = rectLegends[2] = CRect(0, 0, 0, 0);
	if (!items)
		return;
	CSize sz[3];
	for (int i = 0; i < 3; i++)
	{
		GetTextSize(prt, items[i], sz[i]);
		if (nMaxLengendStringWidth < sz[i].cx)
			nMaxLengendStringWidth = sz[i].cx;
	}
	rectLegends[0].top = rectLegends[1].top = rectLegends[2].top = rc.top + 4;
	CD2DTextFormat tfKey(prt, strFontName, fFontSize);
	CD2DTextFormat tfMouse(prt, strFontName, fFontSize);
	CD2DTextFormat tfDistance(prt, strFontName, fFontSize);
	CD2DTextLayout tlKey(prt, items[0], tfKey, CD2DSizeF(200.f, 100.f));
	CD2DTextLayout tlMouse(prt, items[1], tfMouse, CD2DSizeF(200.f, 100.f));
	CD2DTextLayout tlDistance(prt, items[2], tfDistance, CD2DSizeF(200.f, 100.f));
	//DWRITE_TEXT_METRICS dtm;
	//tlKey.Get()->GetMetrics(&dtm);
	rectLegends[0].bottom = rectLegends[1].bottom = rectLegends[2].bottom = rc.top + 4 + sz[0].cy;
	rectLegends[0].left = 4;
	float fside = float(sz[0].cy) - 4.f;
	CD2DRectF rcBorder(rc.left + 6.f, rc.top + 6.f, rc.left + 6.f + fside, rc.top + 6.f + fside);
	CD2DRectF rcFill(rcBorder.left + .5f, rcBorder.top + .5f, rcBorder.right - .5f, rcBorder.bottom - .5f);

	CD2DSolidColorBrush brushBorder(prt, bHilight? ccVolBorderHi : ccVolBorder);
	CD2DSolidColorBrush brushFill(prt, bHilight? ccVolColorsHi[0] : ccVolColors[0]);
	CD2DSolidColorBrush brushText(prt, ccText);
	CD2DPointF pt(rcBorder.right + 4.f, rcBorder.top - 2.f);
	prt->DrawRectangle(rcBorder, &brushBorder);
	prt->FillRectangle(rcFill, &brushFill);
	prt->DrawTextLayout(pt, &tlKey, &brushText);
	rectLegends[0].right = int(pt.x + sz[0].cx);

	rcBorder.left += (12.f + nMaxLengendStringWidth + fside);
	rcBorder.right += (12.f + nMaxLengendStringWidth + fside);
	rcFill.left += (12.f + nMaxLengendStringWidth + fside);
	rcFill.right += (12.f + nMaxLengendStringWidth + fside);
	brushFill.SetColor(bHilight ? ccVolColorsHi[1] : ccVolColors[1]);
	prt->DrawRectangle(rcBorder, &brushBorder);
	prt->FillRectangle(rcFill, &brushFill);
	pt.x = rcBorder.right + 4.f;
	prt->DrawTextLayout(pt, &tlMouse, &brushText);
	//tlMouse.Get()->GetMetrics(&dtm);
	rectLegends[1].left = int(rcBorder.left - 2.f);
	rectLegends[1].right = int(pt.x + sz[1].cx);

	rcBorder.left += (12.f + nMaxLengendStringWidth + fside);
	rcBorder.right += (12.f + nMaxLengendStringWidth + fside);
	rcFill.left += (12.f + nMaxLengendStringWidth + fside);
	rcFill.right += (12.f + nMaxLengendStringWidth + fside);
	brushFill.SetColor(bHilight ? ccVolColorsHi[2] : ccVolColors[2]);
	prt->DrawRectangle(rcBorder, &brushBorder);
	prt->FillRectangle(rcFill, &brushFill);
	pt.x = rcBorder.right + 4.f;
	prt->DrawTextLayout(pt, &tlDistance, &brushText);
	//tlDistance.Get()->GetMetrics(&dtm);
	rectLegends[2].left = int(rcBorder.left - 2.f);
	rectLegends[2].right = int(pt.x + sz[2].cx);

	if (!items[3].IsEmpty())
	{
		pt.x = rcBorder.right + 4.f + nMaxLengendStringWidth + 4.f;
		CD2DTextFormat tfInfo(prt, strFontName, fFontSize);
		CD2DTextLayout tlInfo(prt, items[3], tfInfo, CD2DSizeF(200.f, 100.f));
		prt->DrawTextLayout(pt, &tlInfo, &brushText);
	}
}


//----------------------------------------------------------------------------------------------------------------------
void CStatVisualWnd::DrawLegend_Shined(CRenderTarget* prt, const CRect& rc, const CString* items, bool bHilight)
{
	rectLegends[0] = rectLegends[1] = rectLegends[2] = CRect(0, 0, 0, 0);
	if (!items)
		return;
	CSize sz[3];
	for (int i = 0; i < 3; i++)
	{
		GetTextSize(prt, items[i], sz[i]);
		if (nMaxLengendStringWidth < sz[i].cx)
			nMaxLengendStringWidth = sz[i].cx;
	}
	rectLegends[0].top = rectLegends[1].top = rectLegends[2].top = rc.top + 4;
	CD2DTextFormat tfKey(prt, strFontName, fFontSize);
	CD2DTextFormat tfMouse(prt, strFontName, fFontSize);
	CD2DTextFormat tfDistance(prt, strFontName, fFontSize);
	CD2DTextLayout tlKey(prt, items[0], tfKey, CD2DSizeF(200.f, 100.f));
	CD2DTextLayout tlMouse(prt, items[1], tfMouse, CD2DSizeF(200.f, 100.f));
	CD2DTextLayout tlDistance(prt, items[2], tfDistance, CD2DSizeF(200.f, 100.f));
	rectLegends[0].bottom = rectLegends[1].bottom = rectLegends[2].bottom = rc.top + 4 + sz[0].cy;
	rectLegends[0].left = 4;

	int nSide = sz[0].cy - 4;
	CRect rect(rc.left + 6, rc.top + 6, rc.left + 6 + nSide, rc.top + 6 + nSide);

	CD2DSolidColorBrush brushText(prt, ccText);
	CD2DPointF pt(rect.right + 4.f, rect.top - 2.f);
	DrawShinedBar(prt, rect, ccVolColors[0], bHilight);
	prt->DrawTextLayout(pt, &tlKey, &brushText);
	rectLegends[0].right = int(pt.x + sz[0].cx);

	rect.left += (12 + nMaxLengendStringWidth + nSide);
	rect.right += (12 + nMaxLengendStringWidth + nSide);
	DrawShinedBar(prt, rect, ccVolColors[1], bHilight);
	pt.x = rect.right + 4.f;
	prt->DrawTextLayout(pt, &tlMouse, &brushText);
	rectLegends[1].left = rect.left - 2;
	rectLegends[1].right = int(pt.x + sz[1].cx);

	rect.left += (12 + nMaxLengendStringWidth + nSide);
	rect.right += (12 + nMaxLengendStringWidth + nSide);
	DrawShinedBar(prt, rect, ccVolColors[2], bHilight);
	pt.x = rect.right + 4.f;
	prt->DrawTextLayout(pt, &tlDistance, &brushText);
	rectLegends[2].left = rect.left - 2;
	rectLegends[2].right = int(pt.x + sz[2].cx);

	if (!items[3].IsEmpty())
	{
		pt.x = rect.right + 4.f + nMaxLengendStringWidth + 4.f;
		CD2DTextFormat tfInfo(prt, strFontName, fFontSize);
		CD2DTextLayout tlInfo(prt, items[3], tfInfo, CD2DSizeF(200.f, 100.f));
		prt->DrawTextLayout(pt, &tlInfo, &brushText);
	}
}


//----------------------------------------------------------------------------------------------------------------------
void CStatVisualWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (rectGraphCore.PtInRect(point))
	{
		bDragging = true;
		ptLast = point;
		SetCapture();

		LegendClickBegin(point);
	}
	else
		bDragging = false;

	CWnd::OnLButtonDown(nFlags, point);
}


//----------------------------------------------------------------------------------------------------------------------
void CStatVisualWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
#define MAKE_COLORREF(dw) RGB((((dw) & 0x00ff0000) >> 16), (((dw) & 0x0000ff00) >> 8), ((dw) & 0x000000ff))

	bDragging = false;
	ReleaseCapture();

	if (LegendClickEnd(point) >= 0)	// show color selection panel
	{
		ccSaved = ccVolColors[nCurrClickedLegend];
		ccSavedHi = ccVolColorsHi[nCurrClickedLegend];
		COLORREF clr{ 0 };
		switch (nCurrClickedLegend)
		{
		case 0:
			clr = MAKE_COLORREF(pkoOptions->dwVolColor1);
			break;
		case 1:
			clr = MAKE_COLORREF(pkoOptions->dwVolColor2);
			break;
		case 2:
			clr = MAKE_COLORREF(pkoOptions->dwVolColor3);
			break;
		default:
			break;
		}
		ClientToScreen(&point);
		cpColorPicker.ShowAt(point, clr);
	}
	CWnd::OnLButtonUp(nFlags, point);
}


//----------------------------------------------------------------------------------------------------------------------
void CStatVisualWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if (bDragging)
	{
		if (rectGraphCore.PtInRect(point))
		{
			int dis = point.x - ptLast.x;
			ptLast = point;
			if (dis != 0)
			{
				if (gmGraph.UpdateDrag(dis, rectGraphCore))
					DrawAll();
			}
		}
	}

	CWnd::OnMouseMove(nFlags, point);
}


//----------------------------------------------------------------------------------------------------------------------
void CStatVisualWnd::Hide(void)
{
	ShowWindow(SW_HIDE);
	KillTimer(uiTimerID_Tip);
}


//----------------------------------------------------------------------------------------------------------------------
void CStatVisualWnd::OnTimer(UINT_PTR nIDEvent)
{
	if (bDragging)
		return;
	if(nIDEvent == uiTimerID_Tip)
		UpdateLegend();
	if (nIDEvent == uiTimerID_Navi)
	{

	}
	CWnd::OnTimer(nIDEvent);
}


//----------------------------------------------------------------------------------------------------------------------
void CStatVisualWnd::UpdateLegend(void)
{
	CPoint pt;
	bool bRedraw = false;
	if (::GetCursorPos(&pt))
	{
		ScreenToClient(&pt);
		if (rectGraphCore.PtInRect(pt))
		{
			CDataGroup_3_Pointer pdg = gmGraph.OnWhich(pt);
			if (pdg)
			{
				if (dkCurrTip != pdg->dkDate)
				{
					bRedraw = true;
					dkCurrTip = pdg->dkDate;
					strCurrLegend = strColValues;
					strColValues[0].Format(_T("%d"), pdg->nData[0]);
					strColValues[1].Format(_T("%d"), pdg->nData[1]);
					strColValues[2].Format(_T("%dm"), pdg->nData[2]);
					strColValues[3] = MakeDateString(dkCurrTip);

					nCompLines[0] = pdg->nData[0];
					nCompLines[1] = pdg->nData[1];
					nCompLines[2] = pdg->nData[2];
					pnCurrBaseLines = nCompLines;
				}
			}
			else
			{
				if (dkCurrTip != Date_Key_NULL)
				{
					bRedraw = true;
					dkCurrTip = Date_Key_NULL;
					strCurrLegend = strColNames;
					pnCurrBaseLines = nullptr;
				}
			}
		}
		else
		{
			if (dkCurrTip != Date_Key_NULL)
			{
				bRedraw = true;
				dkCurrTip = Date_Key_NULL;
				strCurrLegend = strColNames;
				pnCurrBaseLines = nullptr;
			}
		}
	}
	if (bRedraw)
		DrawAll();
}


//----------------------------------------------------------------------------------------------------------------------
void CStatVisualWnd::DrawCompLines(CRenderTarget* prt)
{
	if (pnCurrBaseLines)
	{
		CD2DPointF pt1, pt2;
		CD2DBrushProperties bp;
		bp.opacity = .75f;
		CD2DSolidColorBrush brush(prt, ccVolColors[0], &bp);
		
		pt1.x = rectGraphCore.left + 1.f;
		pt1.y = rectGraphCore.bottom -
			GetColumnHeight(float(gmGraph.GetCountTop()), float(pnCurrBaseLines[0]), float(rectGraphCore.Height()));
		pt2.x = rectGraphCore.right - 1.f;
		pt2.y = pt1.y;
		prt->DrawLine(pt1, pt2, &brush, /*.5f*/1.f, ssDash);
		pt1.y = rectGraphCore.bottom -
			GetColumnHeight(float(gmGraph.GetCountTop()), float(pnCurrBaseLines[1]), float(rectGraphCore.Height()));
		pt2.y = pt1.y;
		brush.SetColor(ccVolColors[1]);
		prt->DrawLine(pt1, pt2, &brush, /*.5f*/1.f, ssDash);
		pt1.y = rectGraphCore.bottom -
			GetColumnHeight(float(gmGraph.GetDistanceTop()), float(pnCurrBaseLines[2]), float(rectGraphCore.Height()));
		pt2.y = pt1.y;
		brush.SetColor(ccVolColors[2]);
		prt->DrawLine(pt1, pt2, &brush, /*.5f*/1.f, ssDash);
	}
}


//----------------------------------------------------------------------------------------------------------------------
void CStatVisualWnd::OnCaptureChanged(CWnd* pWnd)
{
	if (!pWnd)
	{
		bDragging = false;
	}
	else
	{
		if (pWnd->m_hWnd != m_hWnd)
			bDragging = false;
	}
	CWnd::OnCaptureChanged(pWnd);
}


//----------------------------------------------------------------------------------------------------------------------
int CStatVisualWnd::LegendClickBegin(const CPoint& pt)
{
	nCurrClickedLegend = -1;
	for (int i = 0; i < 3; i++)
	{
		if (rectLegends[i].PtInRect(pt))
		{
			nCurrClickedLegend = i;
			break;
		}
	}
	return nCurrClickedLegend;
}


//----------------------------------------------------------------------------------------------------------------------
int CStatVisualWnd::LegendClickEnd(const CPoint& pt)
{
	int nf = -1;
	if (nCurrClickedLegend >= 0 && nCurrClickedLegend < 3)
	{
		for (int i = 0; i < 3; i++)
		{
			if (rectLegends[i].PtInRect(pt))
			{
				nf = i;
				break;
			}
		}
	}
	if (nf != nCurrClickedLegend)
		nCurrClickedLegend = -1;
	return nCurrClickedLegend;
}


//----------------------------------------------------------------------------------------------------------------------
LRESULT CStatVisualWnd::OnHoverColor(WPARAM wParam, LPARAM lParam)
{
	if (wParam)	// hover on a color
	{
		ccVolColors[nCurrClickedLegend] = COLORREF(lParam);
		ccVolColorsHi[nCurrClickedLegend] = CUtil::CColorUtil::MakeHiColor(COLORREF(lParam));
	}
	else
	{
		ccVolColors[nCurrClickedLegend] = ccSaved;
		ccVolColorsHi[nCurrClickedLegend] = ccSavedHi;
	}
	DrawAll();
	return TRUE;
}


//----------------------------------------------------------------------------------------------------------------------
LRESULT CStatVisualWnd::OnSelectColor(WPARAM wParam, LPARAM lParam)
{
	OnHoverColor(wParam, lParam);
	if (wParam)	// save selected color to config file
	{
		if (pkoOptions)
		{
			switch (nCurrClickedLegend)
			{
			case 0:
				pkoOptions->dwVolColor1 = lParam;
				break;
			case 1:
				pkoOptions->dwVolColor2 = lParam;
				break;
			case 2:
				pkoOptions->dwVolColor3 = lParam;
				break;
			default:
				break;
			}
			CWnd* ppw = GetParent();
			if (ppw)
				ppw->SendMessage(WM_USER_COLOR_CHANGED);
		}
	}
	return TRUE;
}


//----------------------------------------------------------------------------------------------------------------------
void CStatVisualWnd::SetOptions(CKamacOptions* pko)
{
	pkoOptions = pko;
	if (pkoOptions)
	{
		ccVolColors[0] = pkoOptions->dwVolColor1;
		ccVolColors[1] = pkoOptions->dwVolColor2;
		ccVolColors[2] = pkoOptions->dwVolColor3;
		ccVolColorsHi[0] = CUtil::CColorUtil::MakeHiColor(pkoOptions->dwVolColor1);
		ccVolColorsHi[1] = CUtil::CColorUtil::MakeHiColor(pkoOptions->dwVolColor2);
		ccVolColorsHi[2] = CUtil::CColorUtil::MakeHiColor(pkoOptions->dwVolColor3);
	}
}


//----------------------------------------------------------------------------------------------------------------------
D2D1::ColorF CStatVisualWnd::MakeHiColor2(COLORREF clr)
{
	return D2D1::ColorF(CUtil::CColorUtil::MakeHiColor(clr));
}


//----------------------------------------------------------------------------------------------------------------------
void CStatVisualWnd::BeginNaviPrevNext(bool bNext)
{
	AnimateMove(gmGraph.GetPageDistance(rectGraphCore, bNext));
}


//----------------------------------------------------------------------------------------------------------------------
void CStatVisualWnd::BeginNaviFirstLast(bool bFirst)
{
	AnimateMove(bFirst? gmGraph.PrepareForNaviToFirst(rectGraphCore.Width(), rectGraphCore.left) : 
		gmGraph.PrepareForNaviToLast(rectGraphCore.Width(), rectGraphCore.right - CDataGroup_3::nTotalWidth));
}


//----------------------------------------------------------------------------------------------------------------------
void CStatVisualWnd::BeginNaviToDate(Date_Key dk, int nVol)
{
	int nWidth = rectGraphCore.Width();
	int nPosLeft = rectGraphCore.left + (nWidth - CDataGroup_3::nTotalWidth) / 2;
	AnimateMove(gmGraph.PrepareForNaviToDate(dk, nWidth, nPosLeft));
	AnimateBar(dk, nVol);
}


//----------------------------------------------------------------------------------------------------------------------
void CStatVisualWnd::AnimateMove(int nDis, int nSteps, DWORD dwDelay)
{
	CStepMan smSteps(nSteps);
	if (nDis != 0)
	{
		smSteps.SetSpan_Inc(nDis);
		for (size_t i = 0; i < smSteps.Count(); i++)
		{
			if (smSteps[i] == 0)
				break;
			if (gmGraph.UpdateDrag(smSteps[i], rectGraphCore))
			{
				DrawAll();
				::Sleep(dwDelay);
			}
			else
				break;
		}
	}
}


//----------------------------------------------------------------------------------------------------------------------
void CStatVisualWnd::AnimateBar(Date_Key dk, int nVol, int nSteps, DWORD dwDelay)
{
	ASSERT(nVol >= 0 && nVol < 3);

	CStepMan smSteps(nSteps);
	smSteps.SetSpan_IncDec(100);
	
	abdAniBar.bEnable = true;
	abdAniBar.dkDate = dk;
	abdAniBar.nVolume = nVol;
	abdAniBar.nPercent = 0;
	for (size_t i = 0; i < smSteps.Count(); i++)
	{
		if (smSteps[i] == 0)
			break;
		abdAniBar.nPercent += smSteps[i];
		DrawAll();
		::Sleep(dwDelay);
	}
	abdAniBar.bEnable = false;
	//DrawAll();
}


