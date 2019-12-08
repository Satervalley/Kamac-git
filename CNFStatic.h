#pragma once


// CNFStatic

#include "pch.h"


class CNFStatic : public CStatic
{
	DECLARE_DYNAMIC(CNFStatic)

public:
	CNFStatic();
	virtual ~CNFStatic();

protected:
	DECLARE_MESSAGE_MAP()

	CString strContent;
	CDC dcMem;
	CBitmap bmp;


public:
	//----------------------------------------------------------------------------------------------------------------------
	void SetContent(const CString& str, BOOL refresh = TRUE)
	{
		strContent = str;
		if (refresh)
		{
			Invalidate(FALSE);
			UpdateWindow();
		}
	}

	void Draw(CDC& dc, CRect& rect, CDC* pDCTarget = nullptr);
	void PrepareMemDC(CDC& dc, int nw, int nh);

public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
};


