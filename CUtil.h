#pragma once

#include "framework.h"

class CUtil
{
public:
    static LPTSTR itoa(int v, LPTSTR des, bool bClose = true)
    {
        static TCHAR buf[12] = { _T('\0') };
        int d = v;
        int m = v;
        int l = 0;
        do
        {
            m = (d % 10);
            d = (d / 10);
            buf[10 - l] = m + 48;
            ++l;
        } while (d > 0);
        buf[11] = 0;
        LPTSTR stt = &buf[11 - l];
        int pos = 0;
        while (stt[pos])
        {
            des[pos++] = stt[pos];
        }
        if (bClose)
            des[pos++] = 0;
        return des + pos;
    }

    static LPCTSTR itoa(int v)
    {
        static TCHAR buf[12] = { _T('\0') };
        int d = v;
        int m = v;
        int l = 0;
        do
        {
            m = (d % 10);
            d = (d / 10);
            buf[10 - l] = m + 48;
            ++l;
        } while (d > 0);
        buf[11] = 0;
        return &buf[11 - l];
    }

    static LPTSTR uitoa(unsigned int v, LPTSTR des, bool bClose = true)
    {
        static TCHAR buf[12] = { _T('\0') };
        int d = v;
        int m = v;
        int l = 0;
        do
        {
            m = (d % 10);
            d = (d / 10);
            buf[10 - l] = m + 48;
            ++l;
        } while (d > 0);
        buf[11] = 0;
        LPTSTR stt = &buf[11 - l];
        int pos = 0;
        while (stt[pos])
        {
            des[pos++] = stt[pos];
        }
        if (bClose)
            des[pos++] = 0;
        return des + pos;
    }

    static LPCTSTR uitoa(unsigned int v)
    {
        static TCHAR buf[12] = { _T('\0') };
        unsigned int d = v;
        int m = v;
        int l = 0;
        do
        {
            m = (d % 10);
            d = (d / 10);
            buf[10 - l] = m + 48;
            ++l;
        } while (d > 0);
        buf[11] = 0;
        return &buf[11 - l];
    }

    static LPTSTR itoa_02d(int v, LPTSTR des, bool bClose = true)
    {
        des[1] = v % 10 + 48;
        v /= 10;
        des[0] = v + 48;
        if (bClose)
        {
            des[2] = 0;
            return des + 3;
        }
        else
            return des + 2;
    }

    static LPCTSTR itoa_02d(int v)
    {
        TCHAR des[3];
        des[1] = v % 10 + 48;
        v /= 10;
        des[0] = v + 48;
        des[2] = 0;
        return des;
    }

    static LPTSTR ulltoa(ULONG64 v, LPTSTR des, bool bClose = true)
    {
        static TCHAR buf[24] = { _T('\0') };
        ULONG64 d = v;
        int m;
        int l = 0;
        do
        {
            m = (d % 10);
            d = (d / 10);
            buf[22 - l] = m + 48;
            ++l;
        } while (d > 0);
        buf[23] = 0;
        LPTSTR stt = &buf[23 - l];
        int pos = 0;
        while (stt[pos])
        {
            des[pos++] = stt[pos];
        }
        if (bClose)
            des[pos++] = 0;
        return des + pos;
    }

    static LPCTSTR ulltoa(ULONG64 v)
    {
        static TCHAR buf[24] = { _T('\0') };
        ULONG64 d = v;
        int m;
        int l = 0;
        do
        {
            m = (d % 10);
            d = (d / 10);
            buf[22 - l] = m + 48;
            ++l;
        } while (d > 0);
        buf[23] = 0;
        return &buf[23 - l];
    }

    static LPTSTR itoax(unsigned char uc, LPTSTR des, bool bClose = true)
    {
        static TCHAR dig[] = { _T('0'), _T('1'), _T('2'), _T('3'), _T('4'), _T('5'), _T('6'), _T('7'),
            _T('8'), _T('9'), _T('A'), _T('B'), _T('C'), _T('D'), _T('E'), _T('F') };
        des[0] = dig[(uc >> 4) & 0x0f];
        des[1] = dig[uc & 0x0f];
        if (bClose)
        {
            des[2] = 0;
            return des + 3;
        }
        else
            return des + 2;
    }

    static LPCTSTR GetDistanceString(ULONG64 ull)
    {
        ull /= 100ull;
        static TCHAR dig[24];
        LPTSTR p;
        if (ull < 100ull)
        {
            p = CUtil::ulltoa(ull, dig, false);
            p[0] = _T('c');
            p[1] = _T('m');
            p[2] = 0;
        }
        else
        {
            p = CUtil::ulltoa(ull / 100ull, dig, false);
            p[0] = _T('.');
            p = CUtil::itoa_02d(int(ull % 100ull), p + 1, false);
            p[0] = _T('m');
            p[1] = 0;
        }
        return dig;
    }

    static void GetDistanceString(ULONG64 ull, LPTSTR dest)
    {
        ull /= 100ull;
        static TCHAR dig[24];
        LPTSTR p;
        if (ull < 100ull)
        {
            p = CUtil::ulltoa(ull, dig, false);
            p[0] = _T('c');
            p[1] = _T('m');
            p[2] = 0;
        }
        else
        {
            p = CUtil::ulltoa(ull / 100ull, dig, false);
            p[0] = _T('.');
            p = CUtil::itoa_02d(int(ull % 100ull), p + 1, false);
            p[0] = _T('m');
            p[1] = 0;
        }
#pragma warning(push)
#pragma warning(disable:4996)
        ::_tcscpy(dest, dig);
#pragma warning(pop)
    }


    class CColorUtil
    {
    public:
        // r, g, b: 0 - 255, h: 0 - 360, l, s: 0 - 1
        static void RGB2HLS(int r, int g, int b, float& h, float& l, float& s)
        {
            unsigned char minval = min(r, min(g, b));
            unsigned char maxval = max(r, max(g, b));
            float mdiff = float(maxval) - float(minval);
            float msum = float(maxval) + float(minval);

            l = msum / 510.0f;

            if (maxval == minval)
            {
                s = 0.0f;
                h = 0.0f;
            }
            else
            {
                float rnorm = (maxval - r) / mdiff;
                float gnorm = (maxval - g) / mdiff;
                float bnorm = (maxval - b) / mdiff;

                s = (l <= 0.5f) ? (mdiff / msum) : (mdiff / (510.0f - msum));

                if (r == maxval) 
                    h = 60.0f * (6.0f + bnorm - gnorm);
                if (g == maxval) 
                    h = 60.0f * (2.0f + rnorm - bnorm);
                if (b == maxval) 
                    h = 60.0f * (4.0f + gnorm - rnorm);
                if (h > 360.0f) 
                    h = h - 360.0f;
            }
        }

        // r, g, b: 0 -255, h: 0 - 360, l, s: 0 - 1
        static void HLS2RGB(float h, float l, float s, int& r, int& g, int& b)
        {
            if (s == 0.0) // Grauton, einfacher Fall
            {
                r = g = b = unsigned char(l * 255.0);
            }
            else
            {
                float rm1, rm2;

                if (l <= 0.5f) 
                    rm2 = l + l * s;
                else                     
                    rm2 = l + s - l * s;
                rm1 = 2.0f * l - rm2;
                r = ToRGB1(rm1, rm2, h + 120.0f);
                g = ToRGB1(rm1, rm2, h);
                b = ToRGB1(rm1, rm2, h - 120.0f);
            }
        }

        // h: 0 - 360, l, s: 0 - 1
        static COLORREF FromHLS(float h, float l, float s)
        {
            int r, g, b;
            HLS2RGB(h, l, s, r, g, b);
            return RGB(r, g, b);
        }


        static void ToHLS(COLORREF c, float& h, float& l, float& s)
        {
            int r = GetRValue(c), g = GetGValue(c), b = GetBValue(c);
            RGB2HLS(r, g, b, h, l, s);
        }


        static COLORREF MakeHiColor(COLORREF clr)
        {
            float fa = 0.08f;
            float h, s, /*s2,*/ l, l2;
            ToHLS(clr, h, l, s);
            //s = color.GetSaturation();
            //s2 = s + fa;
            //if (s2 > 1.0f)
            //	s2 = 1.0f;
            l2 = l + fa;
            if (l2 > 1.0f)
                l2 = l - fa;
            return FromHLS(h, l2, s);
        }

        static COLORREF Diff(COLORREF color, UINT nDist)
        {
            return RGB(GABS(GetRValue(color) - nDist), GABS(GetGValue(color) - nDist), GABS(GetBValue(color) - nDist));
        }


        static COLORREF Or(COLORREF colFore, COLORREF colBK, UINT nForePercent)
        {
            return RGB(
                (GetRValue(colBK) * 100 + (nForePercent * (GetRValue(colFore) - GetRValue(colBK)))) / 100,
                (GetGValue(colBK) * 100 + (nForePercent * (GetGValue(colFore) - GetGValue(colBK)))) / 100,
                (GetBValue(colBK) * 100 + (nForePercent * (GetBValue(colFore) - GetBValue(colBK)))) / 100);
        }


        static COLORREF DarkBorderColor(COLORREF clr, int nDiff = 100)
        {
            return Diff(clr, nDiff);
        }
    private:
        static unsigned char ToRGB1(float rm1, float rm2, float rh)
        {
            if (rh > 360.0f) 
                rh -= 360.0f;
            else if (rh < 0.0f) 
                rh += 360.0f;

            if (rh < 60.0f) 
                rm1 = rm1 + (rm2 - rm1) * rh / 60.0f;
            else if (rh < 180.0f) 
                rm1 = rm2;
            else if (rh < 240.0f) 
                rm1 = rm1 + (rm2 - rm1) * (240.0f - rh) / 60.0f;

            return static_cast<unsigned char>(rm1 * 255);
        }


        static UINT GABS(const int& nmbr)
        {
            return (nmbr < 0) ? -nmbr : nmbr;
        }
    };
};
