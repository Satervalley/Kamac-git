// CStatVisualWnd.cpp: 实现文件
//

#include "pch.h"
//#include <memory>
#include "Kamac.h"
#include "CStatVisualWnd.h"


// CStatVisualWnd

IMPLEMENT_DYNAMIC(CStatVisualWnd, CWnd)


//----------------------------------------------------------------------------------------------------------------------
CStatVisualWnd::CStatVisualWnd()
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
	ASSERT(prt);
	ASSERT_VALID(prt);

	CSize sz;
	GetScaledWindowSize(sz);
	prt->Clear(clrBack);
	DrawAxis(prt, float(szMarginGraph.cx), 4.f, float(sz.cy - 4 - szMarginGraph.cy));
	DrawAxis(prt, float(sz.cx - szMarginGraph.cx), 4.f, float(sz.cy - 4 - szMarginGraph.cy), false);
	DrawGraph(prt);

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
	CD2DSolidColorBrush brush(prt, clrBorder);
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
	CD2DSolidColorBrush scb(prt, clrBorder);//, scbb(prt, clrBack);
	
	rc.left = rcf.left + 1.f;
	rc.top = rcf.top + 1.f;
	rc.right = rcf.right - 1.f;
	rc.bottom = rcf.bottom - 1.f;
	prt->FillRectangle(rc, pbbBrush);
	if (bDrawLabel)
	{
		rcLabelAll.top = rcf.bottom + 1.f;
		CD2DSolidColorBrush brushLabelAll(prt, clrBack);
		prt->FillRectangle(rcLabelAll, &brushLabelAll);
	}
	prt->PushAxisAlignedClip(rcClip);
	for (auto it = gmGraph.GetDataGroups().begin(); it != gmGraph.GetDataGroups().end(); it++)
	{
		DrawDataGroup_3(prt, *it, rcf, bDrawLabel);
	}
	DrawLegend(prt, rcf, strCurrLegend);
	DrawCompLines(prt);
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
	pbmpBackground = new CD2DBitmap(prt, bmpBackground);
	//pbmpBackground->CopyFromMemory(background, nBackgroundSize * sizeof(DWORD));
	D2D1_BITMAP_BRUSH_PROPERTIES bbp;
	CD2DBrushProperties bp;
	bbp.extendModeX = D2D1_EXTEND_MODE_WRAP;
	bbp.extendModeY = D2D1_EXTEND_MODE_WRAP;
	bbp.interpolationMode = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR;
	
	bp.opacity = 1.;
	bp.transform = D2D1::Matrix3x2F::Translation(-8., -8.);
	pbbBrush = new CD2DBitmapBrush(prt, &bbp);
	pbbBrush->SetBitmap(pbmpBackground);

	NONCLIENTMETRICS ncm;
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);
	strFontName = ncm.lfMessageFont.lfFaceName;

	GetTextSize(prt, _T("99999m"), szMarginText);
	szMarginGraph.cx = szMarginText.cx + 4 + 4 + 8;
	szMarginGraph.cy = szMarginText.cy + 4 + 4;

	GetTextSize(prt, _T("88.88"), szLabelText);

	clrBack = D2D1::ColorF(::GetSysColor(COLOR_WINDOW));
	clrBorder = D2D1::ColorF(::GetSysColor(COLOR_3DSHADOW));
	clrText = D2D1::ColorF(::GetSysColor(COLOR_WINDOWTEXT));
	clrHighBorder = D2D1::ColorF(::GetSysColor(COLOR_3DHILIGHT));
	
	D2D1_STROKE_STYLE_PROPERTIES ssp;
	ssp.startCap = D2D1_CAP_STYLE_ROUND;
	ssp.endCap = D2D1_CAP_STYLE_ROUND;
	ssp.dashCap = D2D1_CAP_STYLE_ROUND;
	ssp.lineJoin = D2D1_LINE_JOIN_ROUND;
	ssp.miterLimit = 1.f;
	ssp.dashStyle = D2D1_DASH_STYLE_CUSTOM;
	ssp.dashOffset = 0.f;
	float dashes[2] = { 8.f, 4.f };
	::AfxGetD2DState()->GetDirect2dFactory()->CreateStrokeStyle(ssp, dashes, 2, &ssDash);
	return bRes;
}


//----------------------------------------------------------------------------------------------------------------------
void CStatVisualWnd::DeleteD2DResource(void)
{
	if (pbbBrush)
	{
		delete pbbBrush;
		pbbBrush = nullptr;
	}
	if (pbmpBackground)
	{
		delete pbmpBackground;
		pbmpBackground = nullptr;
	}
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
	CD2DSolidColorBrush brush(prt, clrText);
	prt->DrawTextLayout(CD2DPointF(pt), &tl, &brush);
}


//----------------------------------------------------------------------------------------------------------------------
void CStatVisualWnd::InitGraph(void)
{
	CSize sz;
	GetScaledWindowSize(sz);
	rectGraphCore = { szMarginGraph.cx, 4, sz.cx - szMarginGraph.cx, sz.cy - 4 - szMarginGraph.cy };
	rectGraph = rectGraphCore;
	rectGraph.bottom = sz.cy - 4;
	gmGraph.Init(rectGraphCore);

	SetTimer(uiTimerID, 1000, nullptr);
}


//----------------------------------------------------------------------------------------------------------------------
void CStatVisualWnd::DrawDataGroup_3(CRenderTarget* prt, CDataGroup_3_Pointer& pdg, const CRect& rc, bool bDrawLabel)
{
	ASSERT(prt);

	if (pdg)
	{
		int nth = rc.Height();
		CD2DRectF rect(rc);
		D2D1::ColorF clrs[3]{ clrCol1, clrCol2, clrCol3 };
		CD2DSolidColorBrush brushBorder(prt, clrHighBorder);
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
			CD2DSolidColorBrush brushColumn(prt, clrs[i]);
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
			CD2DSolidColorBrush brushText(prt, clrText);
			CD2DPointF pt(float(pdg->nBorderXLeft + (pdg->nTotalWidth - szLabelText.cx) / 2), float(rectGraphCore.bottom + 2));
			prt->DrawTextLayout(pt, &tlLabel, &brushText);
		}
	}
}


//----------------------------------------------------------------------------------------------------------------------
float CStatVisualWnd::GetColumnHeight(float nTop, float nValue, float nTotalHeight)
{
	float h = nValue / nTop * nTotalHeight;
	h = h < 2. ? 2 : h;
	h = h > nTop ? nTop : h;
	return h;
}


//----------------------------------------------------------------------------------------------------------------------
void CStatVisualWnd::DrawLegend(CRenderTarget* prt, const CRect& rc, const CString* items)
{
	if (!items)
		return;
	CD2DTextFormat tfKey(prt, strFontName, fFontSize);
	CD2DTextFormat tfMouse(prt, strFontName, fFontSize);
	CD2DTextFormat tfDistance(prt, strFontName, fFontSize);
	CD2DTextLayout tlKey(prt, items[0], tfKey, CD2DSizeF(200.f, 100.f));
	CD2DTextLayout tlMouse(prt, items[1], tfMouse, CD2DSizeF(200.f, 100.f));
	CD2DTextLayout tlDistance(prt, items[2], tfDistance, CD2DSizeF(200.f, 100.f));
	DWRITE_TEXT_METRICS dtm;
	tlKey.Get()->GetMetrics(&dtm);
	float fside = dtm.height - 4.f;
	CD2DRectF rcBorder(rc.left + 6.f, rc.top + 6.f, rc.left + 6.f + fside, rc.top + 6.f + fside);
	CD2DRectF rcFill(rcBorder.left + .5f, rcBorder.top + .5f, rcBorder.right - .5f, rcBorder.bottom - .5f);
//	CD2DRectF rcFrame(rc.left + 2.f, rc.top + 2.f, rc.left + 4.f, rc.top + 4.f + dtm.height);

	CD2DSolidColorBrush brushBorder(prt, clrHighBorder);
	CD2DSolidColorBrush brushFill(prt, clrCol1);
	CD2DSolidColorBrush brushText(prt, clrText);
	CD2DPointF pt(rcBorder.right + 4.f, rcBorder.top - 2.f);
	prt->DrawRectangle(rcBorder, &brushBorder);
	prt->FillRectangle(rcFill, &brushFill);
	prt->DrawTextLayout(pt, &tlKey, &brushText);
//	rcFrame.right += (fside + dtm.width + 8.f);

	rcBorder.left += (12.f + dtm.width + fside);
	rcBorder.right += (12.f + dtm.width + fside);
	rcFill.left += (12.f + dtm.width + fside);
	rcFill.right += (12.f + dtm.width + fside);
	brushFill.SetColor(clrCol2);
	prt->DrawRectangle(rcBorder, &brushBorder);
	prt->FillRectangle(rcFill, &brushFill);
	pt.x = rcBorder.right + 4.f;
	prt->DrawTextLayout(pt, &tlMouse, &brushText);
	tlMouse.Get()->GetMetrics(&dtm);
//	rcFrame.right += (fside + dtm.width + 8.f);

	rcBorder.left += (12.f + dtm.width + fside);
	rcBorder.right += (12.f + dtm.width + fside);
	rcFill.left += (12.f + dtm.width + fside);
	rcFill.right += (12.f + dtm.width + fside);
	brushFill.SetColor(clrCol3);
	prt->DrawRectangle(rcBorder, &brushBorder);
	prt->FillRectangle(rcFill, &brushFill);
	pt.x = rcBorder.right + 4.f;
	prt->DrawTextLayout(pt, &tlDistance, &brushText);
	tlDistance.Get()->GetMetrics(&dtm);
//	rcFrame.right += (fside + dtm.width);

	//CD2DSolidColorBrush brush(prt, clrText);
	//prt->DrawRoundedRectangle(CD2DRoundedRect(rcFrame, 2.f), &brush, .2f);
}


//----------------------------------------------------------------------------------------------------------------------
void CStatVisualWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (rectGraphCore.PtInRect(point))
	{
		bDragging = true;
		ptLast = point;
		SetCapture();
	}
	else
		bDragging = false;

	CWnd::OnLButtonDown(nFlags, point);
}


//----------------------------------------------------------------------------------------------------------------------
void CStatVisualWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	bDragging = false;
	ReleaseCapture();

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
				{
					CRenderTarget* prt = GetRenderTarget();
					prt->BeginDraw();
					//DrawGraph(prt);
					DrawAll(prt);
					prt->EndDraw();
				}
			}
		}
	}

	CWnd::OnMouseMove(nFlags, point);
}


//----------------------------------------------------------------------------------------------------------------------
void CStatVisualWnd::Hide(void)
{
	ShowWindow(SW_HIDE);
	KillTimer(uiTimerID);
}


//----------------------------------------------------------------------------------------------------------------------
void CStatVisualWnd::OnTimer(UINT_PTR nIDEvent)
{
	if (bDragging)
		return;
	UpdateLegend();
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
	{
		CRenderTarget* prt = GetRenderTarget();
		prt->BeginDraw();
		//DrawGraph(prt, false);
		DrawAll(prt);
		prt->EndDraw();
	}
}


//----------------------------------------------------------------------------------------------------------------------
void CStatVisualWnd::DrawCompLines(CRenderTarget* prt)
{
	if (pnCurrBaseLines)
	{
		CD2DPointF pt1, pt2;
		CD2DBrushProperties bp;
		bp.opacity = 1.f;
		CD2DSolidColorBrush brush(prt, /*D2D1::ColorF(D2D1::ColorF::DarkRed)*/clrCol1, &bp);
		
		pt1.x = rectGraphCore.left + 1.f;
		pt1.y = rectGraphCore.bottom -
			GetColumnHeight(float(gmGraph.GetCountTop()), float(pnCurrBaseLines[0]), float(rectGraphCore.Height()));
		pt2.x = rectGraphCore.right - 1.f;
		pt2.y = pt1.y;
		prt->DrawLine(pt1, pt2, &brush, .5f, ssDash);
		pt1.y = rectGraphCore.bottom -
			GetColumnHeight(float(gmGraph.GetCountTop()), float(pnCurrBaseLines[1]), float(rectGraphCore.Height()));
		pt2.y = pt1.y;
		brush.SetColor(/*D2D1::ColorF(D2D1::ColorF::DarkGreen)*/clrCol2);
		prt->DrawLine(pt1, pt2, &brush, .5f, ssDash);
		pt1.y = rectGraphCore.bottom -
			GetColumnHeight(float(gmGraph.GetDistanceTop()), float(pnCurrBaseLines[2]), float(rectGraphCore.Height()));
		pt2.y = pt1.y;
		brush.SetColor(/*D2D1::ColorF(D2D1::ColorF::DarkBlue)*/clrCol3);
		prt->DrawLine(pt1, pt2, &brush, .5f, ssDash);
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
