#pragma once

#include "framework.h"

class CColorComb: public D2D1::ColorF
{
public:
	COLORREF clrColor;

	CColorComb() : CColorComb(0)
	{

	}

	CColorComb(UINT32 rgb, FLOAT a = 1.0) : D2D1::ColorF(rgb, a)
	{
		clrColor = rgb;
	}

	CColorComb(Enum knownColor, FLOAT a = 1.0) : D2D1::ColorF(knownColor, a)
	{
		clrColor = knownColor;
	}

	CColorComb(const CColorComb& cc) : D2D1::ColorF(cc.clrColor)
	{
		clrColor = cc.clrColor;
	}

	CColorComb& operator =(const CColorComb& cc)
	{
		if (this != &cc)
		{
			clrColor = cc.clrColor;
			r = cc.r;
			g = cc.g;
			b = cc.b;
			a = cc.a;
		}
		return *this;
	}

	CColorComb& operator =(COLORREF clr)
	{
		clrColor = clr;
		Init2(clr, 1.0f);
		return *this;
	}

	operator COLORREF() const
	{
		return clrColor;
	}

	void GetRGB(int& R, int& G, int& B) const
	{
		R = GetRValue(clrColor);
		G = GetGValue(clrColor);
		B = GetBValue(clrColor);
	}

	int R(void) const
	{
		return GetRValue(clrColor);
	}

	int G(void) const
	{
		return GetGValue(clrColor);
	}

	int B(void) const
	{
		return GetBValue(clrColor);
	}

private:
	void Init2(UINT32 rgb, FLOAT alpha)
	{
		r = static_cast<FLOAT>((rgb & sc_redMask) >> sc_redShift) / 255.f;
		g = static_cast<FLOAT>((rgb & sc_greenMask) >> sc_greenShift) / 255.f;
		b = static_cast<FLOAT>((rgb & sc_blueMask) >> sc_blueShift) / 255.f;
		a = alpha;
	}
	static const UINT32 sc_redShift = 16;
	static const UINT32 sc_greenShift = 8;
	static const UINT32 sc_blueShift = 0;

	static const UINT32 sc_redMask = 0xff << sc_redShift;
	static const UINT32 sc_greenMask = 0xff << sc_greenShift;
	static const UINT32 sc_blueMask = 0xff << sc_blueShift;

};

